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
                pthread_t      thread;              ///< thread descriptor

                pthread_mutex_t     mutex;          ///< event mutex
                pthread_cond_t      condition;      ///< condition lock

                /**
                 * schedule manager
                 * @return thread exit status
                 * @param data defines user data
                 */
                static void *manager(void *data);
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
#ifdef PTHREAD_EXT
                           ,
                             thread(0)
#endif
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

#ifdef PTHREAD_EXT
void *
__manager__::manager(void *data)
{
    scheduler *parent = (scheduler *)data;

    unsigned long idle = 0;

    while (true) {
        if (idle != 0) {
            pthread_mutex_lock(&parent->manager->mutex);
            if (idle == ~0UL) {
                pthread_cond_wait(&parent->manager->condition, &parent->manager->mutex);
            } else {
                timespec ts;

                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += idle/1000;
                ts.tv_nsec += (idle % 1000) * 1000000;

                pthread_cond_timedwait(&parent->manager->condition, &parent->manager->mutex, &ts);
            }

            if (parent->manager->exit) {
                pthread_mutex_unlock(&parent->manager->mutex);

                return NULL;
            }
            pthread_mutex_unlock(&parent->manager->mutex);

            idle = 0;
        } else {
            pc::sync::stack tg(parent->keeper);

            idle = ~0UL;

            dodoMap<unsigned long, scheduler::__job__>::iterator i = parent->handles.begin(), j = parent->handles.end();
            while (i!=j) {
                pthread_mutex_lock(&parent->manager->mutex);
                if (parent->manager->exit) {
                    pthread_mutex_unlock(&parent->manager->mutex);

                    return NULL;
                }
                pthread_mutex_unlock(&parent->manager->mutex);

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

    return NULL;
}
#endif

//-------------------------------------------------------------------

scheduler::scheduler() : counter(0),
      keeper(NULL),
      manager(NULL)
{
    dodo_try {
        keeper = new pc::sync::thread;
        manager = new __manager__;
    } dodo_catch (exception::basic *e UNUSED) {
        delete manager;
        delete keeper;

        dodo_rethrow;
    }

#ifdef PTHREAD_EXT
    errno = pthread_create(&manager->thread, NULL, __manager__::manager, this);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONSCHEDULER, SCHEDULEREX_CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#endif
}
//-------------------------------------------------------------------

scheduler::~scheduler()
{
#ifdef PTHREAD_EXT
    pthread_mutex_lock(&manager->mutex);

    manager->exit = true;

    pthread_cond_signal(&manager->condition);
    pthread_mutex_unlock(&manager->mutex);

    pthread_join(manager->thread, NULL);
#endif

    delete manager;
    delete keeper;
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
            _job = new process(*dynamic_cast<process *>(orig));

            break;

        case execution::job::TYPE_THREAD:
            _job = new thread(*dynamic_cast<thread *>(orig));

            break;

        default:
            dodo_throw exception::basic(exception::MODULE_PCEXECUTIONSCHEDULER, SCHEDULEREX_SCHEDULE, exception::ERRNO_LIBDODO, SCHEDULEREX_UNKNOWNJOB, PCEXECUTIONSCHEDULEREX_UNKNOWNJOB_STR, __LINE__, __FILE__);
    }

    __job__ j = {_job, timeout, tools::time::nowMs(), repeat};

    handles.insert(std::make_pair(counter, j));

    pthread_mutex_lock(&manager->mutex);
    pthread_cond_signal(&manager->condition);
    pthread_mutex_unlock(&manager->mutex);

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
