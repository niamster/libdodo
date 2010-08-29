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
#include <libdodo/types.h>
#include <libdodo/pcSyncThread.h>
#include <libdodo/pcSyncStack.h>

namespace dodo {
    namespace pc {
        namespace execution {
            /**
             * @struct __work__
             * @brief defines workqueue task
             */
            struct __work__ {
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

            /**
             * @struct __wake__
             * @brief defines wake mechanism
             */
            struct __wake__ {
                /**
                 * constructor
                 */
                __wake__()
                {
#ifdef PTHREAD_EXT
                    pthread_mutex_init(&mutex, NULL);
                    pthread_cond_init(&cond, NULL);
#endif
                }

                /**
                 * wait for event occurency
                 * @return true if event occured
                 * @param timeout defines time to wait for the event
                 */
                bool
                wait(timespec *timeout)
                {
#ifdef PTHREAD_EXT
                    int rc;
                    timespec now;

                    clock_gettime(CLOCK_REALTIME, &now);
                    now.tv_sec += timeout->tv_sec;
                    now.tv_nsec += timeout->tv_nsec;
                    if (now.tv_nsec > 999999999) {
                        now.tv_sec += 1;
                        now.tv_nsec -= 999999999;
                    }

                    pthread_mutex_lock(&mutex);
                    rc = pthread_cond_timedwait(&cond, &mutex, &now);
                    pthread_mutex_unlock(&mutex);

                    return (rc != ETIMEDOUT);
#else
                    return true;
#endif
                }

                /**
                 * raise an event
                 */
                void
                raise()
                {
#ifdef PTHREAD_EXT
                    pthread_cond_signal(&cond);
#endif
                }

#ifdef PTHREAD_EXT
                pthread_cond_t cond; ///< condition handler
                pthread_mutex_t mutex; ///< lock mutex
#endif
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
                                     tasksProtector(new pc::sync::thread),
                                     threadsProtector(new pc::sync::thread),
                                     wake(new __wake__)
{
    if (maxThreads < minThreads)
        maxThreads = minThreads;

    thread *t;
    for (unsigned int i=0; i<minThreads; ++i) {
        t = new thread((routine)workqueue::worker, this, ON_DESTRUCTION_STOP);
        inactive.push_back(t);
        t->run();
    }
}

//-------------------------------------------------------------------

workqueue::~workqueue()
{
    dodoList<thread *>::iterator i = inactive.begin(), j = inactive.end();

    for (; i != j; ++i) {
        /* (*i)->stop(); */
        delete *i;
    }

    delete tasksProtector;
    delete threadsProtector;
    delete wake;
}

//-------------------------------------------------------------------

int
workqueue::worker(workqueue *queue)
{
    __work__ *work;
    thread *self = NULL;
    timespec ts = {0, 0};
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

    for (;;) {
        tasksProtector->acquire();
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
        } else {
            if (active) {
                threadsProtector->acquire();
                activeQueue.remove(self);
                inactiveQueue.push_back(self);
                threadsProtector->release();

                active = false;
            }

            unsigned long delta = queue->maxThreads - queue->minThreads;
            if (delta > 0) {
                unsigned long k;
                threadsProtector->acquire();
                k = activeQueue.size() + inactiveQueue.size();
                threadsProtector->release();
                k = ((k - queue->minThreads)*100)/delta;
                ts.tv_sec = (queue->minIdleTime*(100 + k))/100;
            } else {
                ts.tv_sec = 0x00FFFFFF; /* 194 days, enough as for indefinite sleep */
            }

          sleep:
            if (!queue->wake->wait(&ts)) {
                unsigned long queueSize = 0;
                tasksProtector->acquire();
                queueSize = tasks.size();
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
        wake->raise();

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
