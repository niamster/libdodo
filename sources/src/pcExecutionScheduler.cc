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

#include <libdodo/pcExecutionScheduler.h>
#include <libdodo/pcExecutionSchedulerEx.h>
#include <libdodo/pcExecutionJob.h>
#include <libdodo/pcExecutionThread.h>
#include <libdodo/pcExecutionProcess.h>
#include <libdodo/pcNotificationThread.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncThread.h>
#include <libdodo/pcSyncStack.h>
#include <libdodo/toolsTime.h>

using namespace dodo::pc::execution;

scheduler::scheduler() : counter(0),
                         keeper(NULL),
                         thread(NULL),
                         notification(NULL),
                         closing(false)
{
    dodo_try {
        keeper = new pc::sync::thread;
        notification = new pc::notification::thread(*keeper);
        thread = new execution::thread(scheduler::manager, this, execution::ON_DESTRUCTION_STOP, false);

        thread->run();
    } dodo_catch (exception::basic *e UNUSED) {
        delete thread;
        delete notification;
        delete keeper;

        dodo_rethrow;
    }
}
//-------------------------------------------------------------------

scheduler::~scheduler()
{
    keeper->acquire();
    closing = true;
    keeper->release();

    notification->notify();

    /* thread->wait(); */

    delete thread;
    delete notification;
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
            parent->keeper->acquire();
            parent->notification->wait(idle);

            if (parent->closing) {
                parent->keeper->release();

                return 0;
            }
            parent->keeper->release();

            idle = 0;
        } else {
            pc::sync::stack tg(parent->keeper);

            idle = ~0UL;

            dodoMap<unsigned long, scheduler::__job__>::iterator i = parent->handles.begin(), j = parent->handles.end();
            while (i!=j) {
                if (parent->closing)
                    return 0;

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

    notification->notify();

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
