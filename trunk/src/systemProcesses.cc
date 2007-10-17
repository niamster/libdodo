/***************************************************************************
 *            systemProcesses.cc
 *
 *  Tue Feb 27 08:47:16 2007
 *  Copyright  2007  Ni@m
 *  niam.niam@gmail.com
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


#include <systemProcesses.h>

using namespace dodo;

__processInfo::__processInfo() : isRunning(false),
								 executed(0),
								 executeLimit(0)
{
}

//-------------------------------------------------------------------

systemProcesses::systemProcesses(systemProcesses &sp)
{
}

//-------------------------------------------------------------------

systemProcesses::systemProcesses() : processNum(0)
{
}

//-------------------------------------------------------------------

systemProcesses::~systemProcesses()
{
	std::list<__processInfo>::iterator i(processes.begin()), j(processes.end());

    #ifdef DL_EXT
	deinitSystemProcessesModule deinit;
    #endif

	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;

		switch (i->action)
		{
			case PROCESS_KEEP_ALIVE:

				waitpid(i->pid, NULL, WNOHANG);

				break;

			case PROCESS_STOP:

				kill(i->pid, 2);

				break;

			case PROCESS_WAIT:
			default:

				waitpid(i->pid, NULL, 0);
		}

        #ifdef DL_EXT

		if (i->handle != NULL)
		{
			deinit = (deinitSystemProcessesModule)dlsym(i->handle, "deinitSystemProcessesModule");
			if (deinit != NULL)
				deinit();

			dlclose(i->handle);
		}

        #endif
	}
}

//-------------------------------------------------------------------

unsigned long
systemProcesses::add(processFunc func,
					 void        *data,
					 short action)
{
	__processInfo process;

	process.data = data;
	process.func = func;
	process.position = ++processNum;
	process.action = action;
	process.executeLimit = 0;

    #ifdef DL_EXT
	process.handle = NULL;
    #endif

	processes.push_back(process);

	return process.position;
}

//-------------------------------------------------------------------

unsigned long
systemProcesses::add(jobFunc func,
					 void    *data)
{
	return add(func, data, PROCESS_WAIT);
}

//-------------------------------------------------------------------

unsigned long
systemProcesses::addNRun(jobFunc func,
						 void    *data)
{
	return addNRun(func, data, 1, PROCESS_WAIT);
}

//-------------------------------------------------------------------

unsigned long
systemProcesses::addNRun(processFunc func,
						 void          *data,
						 unsigned long limit,
						 short action)
{
	__processInfo process;

	process.data = data;
	process.func = func;
	process.position = ++processNum;
	process.action = action;
	process.executeLimit = 0;

    #ifdef DL_EXT
	process.handle = NULL;
    #endif

	pid_t pid = fork();

	if (pid == 0)
	{
		func(data);

		_exit(0);
	}
	else
	{
		if (pid == -1)
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ADDNRUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		else
			process.pid = pid;
	}

	process.isRunning = true;
	++ (process.executed);

	processes.push_back(process);

	return process.position;
}

//-------------------------------------------------------------------

void
systemProcesses::del(unsigned long position,
					 bool force)
{
	if (getProcess(position))
	{
		if (_isRunning(current))
		{
			if (!force)
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_DEL, ERR_LIBDODO, SYSTEMPROCESSES_ISALREADYRUNNING, SYSTEMPROCESSES_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else
			{
				if (kill(current->pid, 2) == -1)
					throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_DEL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}

        #ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitSystemProcessesModule deinit;

			deinit = (deinitSystemProcessesModule)dlsym(current->handle, "deinitSystemProcessesModule");
			if (deinit != NULL)
				deinit();

			if (dlclose(current->handle) != 0)
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_DEL, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
		}

        #endif

		processes.erase(current);
	}
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_DEL, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
systemProcesses::getProcess(unsigned long position) const
{
	std::list<__processInfo>::const_iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
		if (i->position == position)
		{
			current = *((std::list<__processInfo>::iterator *) & i);
			return true;
		}

	return false;
}

//-------------------------------------------------------------------

bool
systemProcesses::_isRunning(std::list<__processInfo>::iterator &position) const
{
	if (!position->isRunning)
		return false;

	int res = kill(position->pid, 0);
	if (res != 0)
	{
		if (errno == ESRCH)
		{
			position->isRunning = false;

			return false;
		}

		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES__ISRUNNING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	return true;
}

//-------------------------------------------------------------------

void
systemProcesses::replace(unsigned long position,
						 processFunc func,
						 void          *data,
						 bool force,
						 short action)
{
	if (getProcess(position))
	{
		if (_isRunning(current))
		{
			if (!force)
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_REPLACE, ERR_LIBDODO, SYSTEMPROCESSES_ISALREADYRUNNING, SYSTEMPROCESSES_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else
			{
				if (kill(current->pid, 2) == -1)
					throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_REPLACE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}


        #ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitSystemProcessesModule deinit;

			deinit = (deinitSystemProcessesModule)dlsym(current->handle, "deinitSystemProcessesModule");
			if (deinit != NULL)
				deinit();

			if (dlclose(current->handle) != 0)
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_REPLACE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
		}

        #endif

		current->data = data;
		current->func = func;
		current->isRunning = false;
		current->action = action;
	}
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_REPLACE, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemProcesses::run(unsigned long position,
					 bool force)
{
	if (getProcess(position))
	{
		if (current->executeLimit > 0 && (current->executeLimit <= current->executed))
		{
			processes.erase(current);

			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_RUN, ERR_LIBDODO, SYSTEMPROCESSES_SWEPT, SYSTEMPROCESSES_SWEPT_STR, __LINE__, __FILE__);
		}

		if (_isRunning(current) && !force)
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_RUN, ERR_LIBDODO, SYSTEMPROCESSES_ISALREADYRUNNING, SYSTEMPROCESSES_ISALREADYRUNNING_STR, __LINE__, __FILE__);

		pid_t pid = fork();

		if (pid == 0)
		{
			current->func(current->data);

			_exit(0);
		}
		else
		{
			if (pid == -1)
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_RUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			else
				current->pid = pid;
		}

		current->isRunning = true;
		++ (current->executed);
	}
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_RUN, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemProcesses::stop(unsigned long position)
{
	if (getProcess(position))
	{
		if (!_isRunning(current))
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_STOP, ERR_LIBDODO, SYSTEMPROCESSES_ISNOTRUNNING, SYSTEMPROCESSES_ISNOTRUNNING_STR, __LINE__, __FILE__);

		if (kill(current->pid, 9) == -1)
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_STOP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		current->isRunning = false;
	}
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_STOP, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemProcesses::stop()
{
	std::list<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;


		if (kill(i->pid, 9) == -1)
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_STOP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		i->isRunning = false;
	}
}

//-------------------------------------------------------------------

void
systemProcesses::wait(unsigned long position)
{
	if (getProcess(position))
	{
		if (!_isRunning(current))
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_WAIT, ERR_LIBDODO, SYSTEMPROCESSES_ISNOTRUNNING, SYSTEMPROCESSES_ISNOTRUNNING_STR, __LINE__, __FILE__);

		if (waitpid(current->pid, NULL, 0) == -1)
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_WAIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		current->isRunning = false;
	}
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_WAIT, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemProcesses::wait()
{
	std::list<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;

		if (waitpid(i->pid, NULL, 0) == -1)
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_WAIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		i->isRunning = false;
	}
}

//-------------------------------------------------------------------

bool
systemProcesses::isRunning(unsigned long position) const
{
	if (getProcess(position))
		return _isRunning(current);
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ISRUNNING, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
systemProcesses::running() const
{
	unsigned long amount(0);

	std::list<__processInfo>::const_iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
		if (_isRunning(*((std::list<__processInfo>::iterator *) & i)))
			++amount;

	return amount;
}

//-------------------------------------------------------------------

void
systemProcesses::sweepTrash()
{
	std::list<__processInfo>::iterator i(processes.begin()), j(processes.end());
	while (i != j)
	{
		if (_isRunning(i))
		{
			++i;

			continue;
		}

		if (i->executeLimit > 0 && (i->executeLimit <= i->executed))
		{
			i = processes.erase(i);

			continue;
		}

		++i;
	}
}

//-------------------------------------------------------------------

void
systemProcesses::setExecutionLimit(unsigned long position,
								   unsigned long limit)
{
	if (getProcess(position))
		current->executeLimit = limit;
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_SETEXECUTIONLIMIT, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
}
//-------------------------------------------------------------------

    #ifdef DL_EXT

systemProcessesMod
systemProcesses::getModuleInfo(const dodoString &module,
							   void             *toInit)
{
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
	if (handle == NULL)
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initSystemProcessesModule init = (initSystemProcessesModule)dlsym(handle, "initSystemProcessesModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	systemProcessesMod mod = init(toInit);

	if (dlclose(handle) != 0)
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	return mod;
}

//-------------------------------------------------------------------

unsigned long
systemProcesses::add(const dodoString &module,
					 void             *data,
					 void             *toInit)
{
	__processInfo process;

	process.data = data;
	process.position = ++processNum;

	process.handle = dlopen(module.c_str(), RTLD_LAZY);
	if (process.handle == NULL)
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initSystemProcessesModule init = (initSystemProcessesModule)dlsym(process.handle, "initSystemProcessesModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	systemProcessesMod temp = init(toInit);

	processFunc in = (processFunc)dlsym(process.handle, temp.hook);
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	process.executeLimit = temp.executeLimit;
	process.action = temp.action;
	process.func = in;

	processes.push_back(process);

	return process.position;
}

    #endif

//-------------------------------------------------------------------
