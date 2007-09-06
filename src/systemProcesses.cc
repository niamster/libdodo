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
            #ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ADDNRUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            #else
			return false;
            #endif
		else
			process.pid = pid;
	}

	process.isRunning = true;
	++ (process.executed);

	processes.push_back(process);

	return process.position;
}

//-------------------------------------------------------------------

#ifndef NO_EX
void
#else
bool
#endif
systemProcesses::del(unsigned long position,
					 bool force)
{
	if (getProcess(position))
	{
		if (_isRunning(current))
		{
			if (!force)
                #ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_DEL, ERR_LIBDODO, SYSTEMPROCESSES_ISALREADYRUNNING, SYSTEMPROCESSES_ISALREADYRUNNING_STR, __LINE__, __FILE__);
                #else
				return false;
                #endif
			else
			{
				if (kill(current->pid, 2) == -1)
                #ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_DEL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
                #else
					return false;
                #endif
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
                #ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_DEL, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
                #endif

				current->handle = NULL;
		}

        #endif

		processes.erase(current);

        #ifdef NO_EX
		return true;
        #endif
	}
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_DEL, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return false;
        #endif
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

        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES__ISRUNNING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        #else
		return false;
        #endif
	}

	return true;
}

//-------------------------------------------------------------------

#ifndef NO_EX
void
#else
bool
#endif
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
                #ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_REPLACE, ERR_LIBDODO, SYSTEMPROCESSES_ISALREADYRUNNING, SYSTEMPROCESSES_ISALREADYRUNNING_STR, __LINE__, __FILE__);
                #else
				return false;
                #endif
			else
			{
				if (kill(current->pid, 2) == -1)
                #ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_REPLACE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
                #else
					return false;
                #endif
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
                #ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_REPLACE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
                #endif

				current->handle = NULL;
		}

        #endif

		current->data = data;
		current->func = func;
		current->isRunning = false;
		current->action = action;

        #ifdef NO_EX
		return true;
        #endif
	}
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_REPLACE, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return false;
        #endif

}

//-------------------------------------------------------------------

#ifndef NO_EX
void
#else
bool
#endif
systemProcesses::run(unsigned long position,
					 bool force)
{
	if (getProcess(position))
	{
		if (current->executeLimit > 0 && (current->executeLimit <= current->executed))
		{
			processes.erase(current);

            #ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_RUN, ERR_LIBDODO, SYSTEMPROCESSES_SWEPT, SYSTEMPROCESSES_SWEPT_STR, __LINE__, __FILE__);
            #else
			return false;
            #endif
		}

		if (_isRunning(current) && !force)
            #ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_RUN, ERR_LIBDODO, SYSTEMPROCESSES_ISALREADYRUNNING, SYSTEMPROCESSES_ISALREADYRUNNING_STR, __LINE__, __FILE__);
            #else
			return false;
            #endif

		pid_t pid = fork();

		if (pid == 0)
		{
			current->func(current->data);

			_exit(0);
		}
		else
		{
			if (pid == -1)
                #ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_RUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
                #else
				return false;
                #endif
			else
				current->pid = pid;
		}

		current->isRunning = true;
		++ (current->executed);

        #ifdef NO_EX
		return true;
        #endif
	}
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_RUN, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return false;
        #endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
void
#else
bool
#endif
systemProcesses::stop(unsigned long position)
{
	if (getProcess(position))
	{
		if (!_isRunning(current))
            #ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_STOP, ERR_LIBDODO, SYSTEMPROCESSES_ISNOTRUNNING, SYSTEMPROCESSES_ISNOTRUNNING_STR, __LINE__, __FILE__);
            #else
			return false;
            #endif

		if (kill(current->pid, 9) == -1)
            #ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_STOP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            #else
			return false;
            #endif

		current->isRunning = false;

        #ifdef NO_EX
		return true;
        #endif
	}
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_STOP, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return false;
        #endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
void
#else
bool
#endif
systemProcesses::stop()
{
	std::list<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;


		if (kill(i->pid, 9) == -1)
            #ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_STOP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            #else
			return false;
            #endif

		i->isRunning = false;
	}

    #ifdef NO_EX
	return true;
    #endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
void
#else
bool
#endif
systemProcesses::wait(unsigned long position)
{
	if (getProcess(position))
	{
		if (!_isRunning(current))
            #ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_WAIT, ERR_LIBDODO, SYSTEMPROCESSES_ISNOTRUNNING, SYSTEMPROCESSES_ISNOTRUNNING_STR, __LINE__, __FILE__);
            #else
			return false;
            #endif

		if (waitpid(current->pid, NULL, 0) == -1)
            #ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_WAIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            #else
			return false;
            #endif

		current->isRunning = false;

        #ifdef NO_EX
		return true;
        #endif
	}
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_WAIT, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return false;
        #endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
void
#else
bool
#endif
systemProcesses::wait()
{
	std::list<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;

		if (waitpid(i->pid, NULL, 0) == -1)
            #ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_WAIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            #else
			return false;
            #endif

		i->isRunning = false;
	}

    #ifdef NO_EX
	return true;
    #endif
}

//-------------------------------------------------------------------

bool
systemProcesses::isRunning(unsigned long position) const
{
	if (getProcess(position))
		return _isRunning(current);
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ISRUNNING, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return false;
        #endif
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

#ifndef NO_EX
void
#else
bool
#endif
systemProcesses::setExecutionLimit(unsigned long position,
								   unsigned long limit)
{
	if (getProcess(position))
		current->executeLimit = limit;
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_SETEXECUTIONLIMIT, ERR_LIBDODO, SYSTEMPROCESSES_NOTFOUND, SYSTEMPROCESSES_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return false;
        #endif

    #ifdef NO_EX
	return true;
    #endif
}
//-------------------------------------------------------------------

    #ifdef DL_EXT

systemProcessesMod
systemProcesses::getModuleInfo(const dodoString &module,
							   void             *toInit)
{
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
	if (handle == NULL)
                #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
                #else
		return systemThreadsMod();
                #endif

	initSystemProcessesModule init = (initSystemProcessesModule)dlsym(handle, "initSystemProcessesModule");
	if (init == NULL)
                #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
                #else
		return systemThreadsMod();
                #endif

	systemProcessesMod mod = init(toInit);

	if (dlclose(handle) != 0)
                #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
                #else
		return mod;
                #endif

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
                #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
                #else
		return 0;
                #endif

	initSystemProcessesModule init = (initSystemProcessesModule)dlsym(process.handle, "initSystemProcessesModule");
	if (init == NULL)
                #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
                #else
		return 0;
                #endif

	systemProcessesMod temp = init(toInit);

	processFunc in = (processFunc)dlsym(process.handle, temp.hook);
	if (in == NULL)
                #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMPROCESSES, SYSTEMPROCESSES_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
                #else
		return 0;
                #endif

	process.executeLimit = temp.executeLimit;
	process.action = temp.action;
	process.func = in;

	processes.push_back(process);

	return process.position;
}

    #endif

//-------------------------------------------------------------------
