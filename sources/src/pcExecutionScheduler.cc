/***************************************************************************
 *            pcExecutionScheduler.cc
 *
 *  Sun Nov 08 2009
 *  Copyright  2009  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WIjobHOUjob ANY WARRANjobY; without even the implied warranty of
 *  MERCHANjobABILIjobY or FIjobNESS FOR A PARjobICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 jobemple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#ifdef PTHREAD_EXT
#include <pthread.h>
#endif
#include <errno.h>
#include <string.h>

namespace dodo {
    namespace pc {
        namespace execution {
            /**
             * @struct __manager__
             * @brief defines process information
             */
            struct __manager__ {
                /**
                 * contructor
                 */
                __manager__();

                /**
                 * destructor
                 */
                ~__manager__();

                bool exit; ///< true if the scheduler thread should exit

#ifdef PTHREAD_EXT
                pthread_mutex_t     mutex;          ///< event mutex
                pthread_cond_t      condition;      ///< condition lock
#endif
            };
        };
    };
};

#include <libdodo/pcExecutionScheduler.h>
#include <libdodo/pcExecutionSchedulerEx.h>
#include <libdodo/pcExecutionJob.h>
#include <libdodo/pcExecutionThread.h>
#include <libdodo/pcExecutionProcess.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncThread.h>
#include <libdodo/pcSyncStack.h>
#include <libdodo/toolsTime.h>

using namespace dodo::pc::execution;

__manager__::__manager__() : exit(false)
{
#ifdef PTHREAD_EXT
    pthread_mutexattr_t attr;

    errno = pthread_mutexattr_init(&attr);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONSCHEDULER, SCHEDULEREX___MANAGER__CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    errno = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONSCHEDULER, SCHEDULEREX___MANAGER__CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    errno = pthread_mutex_init(&mutex, &attr);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONSCHEDULER, SCHEDULEREX___MANAGER__CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    errno = pthread_mutexattr_destroy(&attr);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONSCHEDULER, SCHEDULEREX___MANAGER__CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    errno = pthread_cond_init(&condition, NULL);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONSCHEDULER, SCHEDULEREX___MANAGER__CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#endif
}

//-------------------------------------------------------------------

__manager__::~__manager__()
{
#ifdef PTHREAD_EXT
    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&condition);
#endif
}

//-------------------------------------------------------------------

scheduler::scheduler() : counter(0),
                         keeper(NULL),
                         thread(NULL),
                         handle(NULL)
{
    dodo_try {
        keeper = new pc::sync::thread;
        handle = new __manager__;
        thread = new execution::thread(scheduler::manager, this, execution::ON_DESTRUCTION_STOP, false);

        thread->run();
    } dodo_catch (exception::basic *e UNUSED) {
        delete thread;
        delete handle;
        delete keeper;

        dodo_rethrow;
    }
}
//-------------------------------------------------------------------

scheduler::~scheduler()
{
#ifdef PTHREAD_EXT
    pthread_mutex_lock(&handle->mutex);

    handle->exit = true;

    pthread_cond_signal(&handle->condition);
    pthread_mutex_unlock(&handle->mutex);
#endif

    thread->wait();

    delete thread;
    delete handle;
    delete keeper;
}

//-------------------------------------------------------------------

int
scheduler::manager(void *data)
{
    scheduler *parent = (scheduler *)data;

    unsigned long idle = 0;

    while (true) {
        if (idle != 0) {
            pthread_mutex_lock(&parent->handle->mutex);
            if (idle == ~0UL) {
                pthread_cond_wait(&parent->handle->condition, &parent->handle->mutex);
            } else {
                timespec ts;

                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += idle/1000;
                ts.tv_nsec += (idle % 1000) * 1000000;
                if (ts.tv_nsec > 999999999) {
                    ts.tv_sec += 1;
                    ts.tv_nsec -= 999999999;
                }

                pthread_cond_timedwait(&parent->handle->condition, &parent->handle->mutex, &ts);
            }

            if (parent->handle->exit) {
                pthread_mutex_unlock(&parent->handle->mutex);

                return 0;
            }
            pthread_mutex_unlock(&parent->handle->mutex);

            idle = 0;
        } else {
            pc::sync::stack tg(parent->keeper);

            idle = ~0UL;

            dodoMap<unsigned long, scheduler::__job__>::iterator i = parent->handles.begin(), j = parent->handles.end();
            while (i!=j) {
                pthread_mutex_lock(&parent->handle->mutex);
                if (parent->handle->exit) {
                    pthread_mutex_unlock(&parent->handle->mutex);

                    return 0;
                }
                pthread_mutex_unlock(&parent->handle->mutex);

                scheduler::__job__ &j = i->second;
                unsigned long ts = tools::time::nowMs();
                if (ts - j.ts >= j.timeout) {
                    if (!j.job->isRunning()) {
                        j.job->run();
                        if (j.repeat) {
                            j.ts = ts;

                            if (idle > j.timeout)
                                idle = j.timeout;
                        } else {
                            parent->handles.erase(i++);

                            continue;
                        }
                    } else {
                        idle = 0;
                    }
                } else {
                    ts = j.ts + j.timeout - ts;
                    if (idle > ts)
                        idle = ts;
                }

                ++i;
            }
        }
    }

    return 0;
}

//-------------------------------------------------------------------

unsigned long
scheduler::schedule(const execution::job &job,
                    unsigned long timeout,
                    bool repeat)
{
    pc::sync::stack tg(keeper);

    execution::job *_job;

    execution::job *orig = const_cast<execution::job *>(&job);

    switch (job.type) {
        case execution::job::TYPE_PROCESS:
            _job = new process(*dynamic_cast<execution::process *>(orig));

            break;

        case execution::job::TYPE_THREAD:
            _job = new execution::thread(*dynamic_cast<execution::thread *>(orig));

            break;

        default:
            dodo_throw exception::basic(exception::MODULE_PCEXECUTIONSCHEDULER, SCHEDULEREX_SCHEDULE, exception::ERRNO_LIBDODO, SCHEDULEREX_UNKNOWNJOB, PCEXECUTIONSCHEDULEREX_UNKNOWNJOB_STR, __LINE__, __FILE__);
    }

    __job__ j = {_job, timeout, tools::time::nowMs(), repeat};

    handles.insert(std::make_pair(counter, j));

    pthread_mutex_lock(&handle->mutex);
    pthread_cond_signal(&handle->condition);
    pthread_mutex_unlock(&handle->mutex);

    return counter++;
}

//-------------------------------------------------------------------

void
scheduler::remove(unsigned long id,
                  bool          terminate)
{
    pc::sync::stack tg(keeper);

    dodoMap<unsigned long, __job__>::iterator job = handles.find(id);

    if (job == handles.end())
        return;

    if (terminate && job->second.job->isRunning())
        job->second.job->stop();

    handles.erase(job);
}

//-------------------------------------------------------------------
