/***************************************************************************
 *            pcExecutionWorkqueue.cc
 *
 *  Sun Jul 25 2010
 *  Copyright  2010  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <time.h>
#ifdef PTHREAD_EXT
#include <pthread.h>
#include <errno.h>
#endif

#include <libdodo/pcExecutionWorkqueue.h>
#include <libdodo/pcExecutionThread.h>
#include <libdodo/pcExecutionJob.h>
#include <libdodo/pcSyncThread.h>
#include <libdodo/pcNotificationThread.h>
#include <libdodo/exceptionBasic.h>
#include <libdodo/pcSyncStack.h>
#include <libdodo/types.h>

namespace dodo {
    namespace pc {
        namespace execution {
            /**
             * @struct __work__
             * @brief defines workqueue task
             */
            struct workqueue::__work__ {
                /**
                 * constructor
                 * @param routine defines routine for execution
                 * @param data defines data passed to the routine
                 */
                __work__(pc::execution::routine routine,
                        void *data) : routine(routine),
                                      data(data)
                {
                }

                pc::execution::routine routine; ///< task to be exeuted
                void *data; ///< task data
            };
        };
    };
};

using namespace dodo::pc::execution;

workqueue::workqueue(unsigned int maxThreads,
        unsigned int minThreads,
        unsigned long minIdleTime) : maxThreads(maxThreads),
                                     minThreads(minThreads),
                                     minIdleTime(minIdleTime),
                                     tasksProtector(NULL),
                                     threadsProtector(NULL),
                                     notification(NULL),
                                     closing(false)
{
    dodo_try {
        tasksProtector = new pc::sync::thread;
        threadsProtector = new pc::sync::thread;
        notification = new pc::notification::thread(*tasksProtector);

        if (maxThreads < minThreads)
            maxThreads = minThreads;

        thread *t;
        for (unsigned int i=0; i<minThreads; ++i) {
            t = new thread((routine)workqueue::worker, this, ON_DESTRUCTION_STOP);
            inactive.push_back(t);
            t->run();
        }
    } dodo_catch (exception::basic *e UNUSED) {
        delete notification;
        delete tasksProtector;
        delete threadsProtector;

        dodo_rethrow;
    }
}

//-------------------------------------------------------------------

workqueue::~workqueue()
{
    dodoList<thread *>::iterator i, j;
    dodoList<__work__ *>::iterator o, p;

    tasksProtector->acquire();
    closing = true;

    o = tasks.begin();
    p = tasks.end();
    for (; o != p; ++o)
        delete *o;
    tasksProtector->release();

    notification->notify(true);

    threadsProtector->acquire();
    i = inactive.begin();
    j = inactive.end();
    for (; i != j; ++i) {
        /* (*i)->wait(); */
        delete *i;
    }
    threadsProtector->release();

    delete notification;
    delete tasksProtector;
    delete threadsProtector;
}

//-------------------------------------------------------------------

int
workqueue::worker(workqueue *queue)
{
    __work__ *work;
    thread *self = NULL;
    unsigned long timeout;
    bool active = false;

    dodoList<thread *> &inactiveQueue = queue->inactive,
        &activeQueue = queue->active;
    sync::thread *tasksProtector = queue->tasksProtector,
        *threadsProtector = queue->threadsProtector;
    dodoList<__work__ *> &tasks = queue->tasks;

    threadsProtector->acquire();
    for (dodoList<thread *>::iterator i = inactiveQueue.begin(), j = inactiveQueue.end();
         i != j;
         ++i) {
        if ((*i)->self()) {
            self = *i;
            break;
        }
    }
    threadsProtector->release();

    /* task protector is aquired in a strange manner
       to decrease lock contention
       due to using notification mecanism */
    tasksProtector->acquire();
    for (;;) {
        if (tasks.size()) {
            work = *tasks.begin();
            tasks.pop_front();
        } else {
            work = NULL;
        }
        tasksProtector->release();

        if (work) {
            if (!active) {
                threadsProtector->acquire();
                inactiveQueue.remove(self);
                activeQueue.push_back(self);
                threadsProtector->release();

                active = true;
            }

            work->routine(work->data);
            delete work;

            tasksProtector->acquire();
        } else {
            if (active) {
                threadsProtector->acquire();
                activeQueue.remove(self);
                inactiveQueue.push_back(self);
                threadsProtector->release();

                active = false;
            }

            tasksProtector->acquire();
            if (queue->closing) {
                tasksProtector->release();

                return 0;
            }
            tasksProtector->release();

            unsigned long delta = queue->maxThreads - queue->minThreads;
            if (delta > 0) {
                unsigned long k;
                threadsProtector->acquire();
                k = activeQueue.size() + inactiveQueue.size();
                threadsProtector->release();
                k = ((k - queue->minThreads)*100)/delta;
                timeout = (queue->minIdleTime*(100 + k))*10000;
            } else {
                timeout = 0;
            }

          sleep:
            tasksProtector->acquire();
            if (!queue->notification->wait(timeout)) {
                unsigned long queueSize = tasks.size();

                if (queue->closing) {
                    tasksProtector->release();

                    return 0;
                }
                tasksProtector->release();

                if (queueSize == 0) {
                    threadsProtector->acquire();
                    if (inactiveQueue.size() + activeQueue.size() > queue->minThreads) {
                        inactiveQueue.remove(self);
                        threadsProtector->release();

                        return 0;
                    }
                    threadsProtector->release();

                    goto sleep;
                }

                tasksProtector->acquire();
            }
        }
    }

    return 0;
}

//-------------------------------------------------------------------

void
workqueue::add(routine routine,
        void *data)
{
    struct __work__ *work = new __work__(routine, data);
    unsigned long activeSize, inactiveSize;

    tasksProtector->acquire();
    tasks.push_back(work);
    tasksProtector->release();

    threadsProtector->acquire();
    inactiveSize = inactive.size();
    activeSize = active.size();
    threadsProtector->release();

    if (inactiveSize) {
        notification->notify();

        return;
    }
    if (inactiveSize + activeSize == maxThreads)
        return;

    thread *t = new thread((execution::routine)workqueue::worker, this, ON_DESTRUCTION_STOP);

    threadsProtector->acquire();
    inactive.push_back(t);
    threadsProtector->release();

    t->run();
}

//-------------------------------------------------------------------
