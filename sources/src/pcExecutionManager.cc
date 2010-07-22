/***************************************************************************
 *            pcExecutionManager.cc
 *
 *  Sun Oct  30 2007
 *  Copyright  2007  Dmytro Milinevskyy
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

#include <libdodo/pcExecutionManager.h>
#include <libdodo/pcExecutionJob.h>
#include <libdodo/pcExecutionThread.h>
#include <libdodo/pcExecutionProcess.h>
#include <libdodo/pcExecutionManagerEx.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncThread.h>
#include <libdodo/pcSyncStack.h>

using namespace dodo::pc::execution;

manager::manager() : counter(0),
                     keeper(new pc::sync::thread)
{
}

//-------------------------------------------------------------------

manager::~manager()
{
    dodoMap<unsigned long, execution::job *>::const_iterator i = handles.begin(), j = handles.end();

    for (; i != j; ++i)
        delete i->second;

    delete keeper;
}

//-------------------------------------------------------------------

unsigned long
manager::add(const execution::job &job)
{
    pc::sync::stack tg(keeper);

    execution::job *j;

    execution::job *orig = const_cast<execution::job *>(&job);

    switch (job.type) {
        case execution::job::TYPE_PROCESS:
            j = new process(*dynamic_cast<process *>(orig));

            break;

        case execution::job::TYPE_THREAD:
            j = new thread(*dynamic_cast<thread *>(orig));

            break;

        default:
            dodo_throw exception::basic(exception::MODULE_PCEXECUTIONMANAGER, MANAGEREX_ADD, exception::ERRNO_LIBDODO, MANAGEREX_UNKNOWNJOB, PCEXECUTIONMANAGEREX_UNKNOWNJOB_STR, __LINE__, __FILE__);
    }

    handles.insert(std::make_pair(counter, j));

    return counter++;
}

//-------------------------------------------------------------------

void
manager::remove(unsigned long id,
                bool          terminate)
{
    pc::sync::stack tg(keeper);

    dodoMap<unsigned long, execution::job *>::iterator job = handles.find(id);

    if (job == handles.end())
        return;

    if (terminate && job->second->isRunning())
        job->second->stop();

    delete job->second;

    handles.erase(job);
}

//-------------------------------------------------------------------

void
manager::run(unsigned long id)
{
    pc::sync::stack tg(keeper);

    dodoMap<unsigned long, execution::job *>::iterator job = handles.find(id);

    if (job == handles.end())
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONMANAGER, MANAGEREX_RUN, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCEXECUTIONMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);

    job->second->run();
}

//-------------------------------------------------------------------

void
manager::stop(unsigned long id)
{
    pc::sync::stack tg(keeper);

    dodoMap<unsigned long, execution::job *>::iterator job = handles.find(id);

    if (job == handles.end())
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONMANAGER, MANAGEREX_STOP, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCEXECUTIONMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);

    job->second->stop();
}

//-------------------------------------------------------------------

void
manager::stop()
{
    pc::sync::stack tg(keeper);

    dodoMap<unsigned long, execution::job *>::iterator i = handles.begin(), j = handles.end();

    for (; i != j; ++i)
        i->second->stop();
}

//-------------------------------------------------------------------

int
manager::wait(unsigned long id)
{
    pc::sync::stack tg(keeper);

    dodoMap<unsigned long, execution::job *>::iterator job = handles.find(id);

    if (job == handles.end())
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONMANAGER, MANAGEREX_WAIT, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCEXECUTIONMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);

    return job->second->wait();
}

//-------------------------------------------------------------------

void
manager::wait()
{
    pc::sync::stack tg(keeper);

    dodoMap<unsigned long, execution::job *>::iterator i = handles.begin(), j = handles.end();

    for (; i != j; ++i)
        i->second->wait();
}

//-------------------------------------------------------------------

bool
manager::isRunning(unsigned long id) const
{
    pc::sync::stack tg(keeper);

    dodoMap<unsigned long, execution::job *>::const_iterator job = handles.find(id);

    if (job == handles.end())
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONMANAGER, MANAGEREX_ISRUNNING, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCEXECUTIONMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);

    return job->second->isRunning();
}

//-------------------------------------------------------------------

unsigned long
manager::running() const
{
    pc::sync::stack tg(keeper);

    unsigned long jobs;

    dodoMap<unsigned long, execution::job *>::const_iterator i = handles.begin(), j = handles.end();

    for (; i != j; ++i)
        if (i->second->isRunning())
            ++jobs;

    return jobs;
}

//-------------------------------------------------------------------

dodoList<unsigned long>
manager::jobs()
{
    pc::sync::stack tg(keeper);

    dodoList<unsigned long> jobs;

    dodoMap<unsigned long, execution::job *>::const_iterator i = handles.begin(), j = handles.end();

    for (; i != j; ++i)
        jobs.push_back(i->first);

    return jobs;
}

//-------------------------------------------------------------------

dodo::pc::execution::job *
manager::job(unsigned long id)
{
    pc::sync::stack tg(keeper);

    dodoMap<unsigned long, execution::job *>::const_iterator job = handles.find(id);

    if (job == handles.end())
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONMANAGER, MANAGEREX_JOB, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCEXECUTIONMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);

    return job->second;
}

//-------------------------------------------------------------------
