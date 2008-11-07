/***************************************************************************
 *            pcProcessCollection.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/pcProcessCollection.h>

using namespace dodo::pc::process;

__processInfo::__processInfo() : isRunning(false),
								 executed(0),
								 executeLimit(0)
{
}

//-------------------------------------------------------------------

collection::collection(collection &sp)
{
}

//-------------------------------------------------------------------

collection::collection() : processNum(0)
{
}

//-------------------------------------------------------------------

collection::~collection()
{
	dodoList<__processInfo>::iterator i(processes.begin()), j(processes.end());

#ifdef DL_EXT
	deinitIpcProcessCollectionModule deinit;
#endif

	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;

		switch (i->action)
		{
			case COLLECTION_ONDESTRUCT_KEEP_ALIVE:

				waitpid(i->pid, NULL, WNOHANG);

				break;

			case COLLECTION_ONDESTRUCT_STOP:

				kill(i->pid, 2);

				break;

			case COLLECTION_ONDESTRUCT_WAIT:
			default:

				waitpid(i->pid, NULL, 0);
		}

#ifdef DL_EXT

		if (i->handle != NULL)
		{
			deinit = (deinitIpcProcessCollectionModule)dlsym(i->handle, "deinitIpcProcessCollectionModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			dlclose(i->handle);
#endif

		}

#endif
	}
}

//-------------------------------------------------------------------

unsigned long
collection::add(job::routine func,
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
collection::add(job::routine func,
				void    *data)
{
	return add(func, data, COLLECTION_ONDESTRUCT_WAIT);
}

//-------------------------------------------------------------------

unsigned long
collection::addNRun(job::routine func,
					void    *data)
{
	return addNRun(func, data, 1, COLLECTION_ONDESTRUCT_WAIT);
}

//-------------------------------------------------------------------

unsigned long
collection::addNRun(job::routine func,
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
			throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_ADDNRUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		else
			process.pid = pid;
	}

	process.isRunning = true;
	++(process.executed);

	processes.push_back(process);

	return process.position;
}

//-------------------------------------------------------------------

void
collection::del(unsigned long position,
				bool force)
{
	if (getProcess(position))
	{
		if (_isRunning(current))
		{
			if (!force)
				throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_DEL, exception::ERRNO_LIBDODO, COLLECTIONEX_ISALREADYRUNNING, PCPROCESSCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else
			{
				if (kill(current->pid, 2) == -1)
					throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_DEL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}

#ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitIpcProcessCollectionModule deinit;

			deinit = (deinitIpcProcessCollectionModule)dlsym(current->handle, "deinitIpcProcessCollectionModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose(current->handle) != 0)
				throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_DEL, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif
		}

#endif

		processes.erase(current);
	}
	else
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_DEL, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCPROCESSCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
collection::getProcess(unsigned long position) const
{
	dodoList<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
		if (i->position == position)
		{
			current = i;

			return true;
		}

	return false;
}

//-------------------------------------------------------------------

bool
collection::_isRunning(dodoList<__processInfo>::iterator &position) const
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

		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX__ISRUNNING, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	return true;
}

//-------------------------------------------------------------------

void
collection::replace(unsigned long position,
					job::routine func,
					void          *data,
					bool force,
					short action)
{
	if (getProcess(position))
	{
		if (_isRunning(current))
		{
			if (!force)
				throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_REPLACE, exception::ERRNO_LIBDODO, COLLECTIONEX_ISALREADYRUNNING, PCPROCESSCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else
			{
				if (kill(current->pid, 2) == -1)
					throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_REPLACE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}


#ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitIpcProcessCollectionModule deinit;

			deinit = (deinitIpcProcessCollectionModule)dlsym(current->handle, "deinitIpcProcessCollectionModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose(current->handle) != 0)
				throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_REPLACE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif
		}

#endif

		current->data = data;
		current->func = func;
		current->isRunning = false;
		current->action = action;
	}
	else
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_REPLACE, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCPROCESSCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::run(unsigned long position,
				bool force)
{
	if (getProcess(position))
	{
		if (current->executeLimit > 0 && (current->executeLimit <= current->executed))
		{
			processes.erase(current);

			throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_RUN, exception::ERRNO_LIBDODO, COLLECTIONEX_SWEPT, PCPROCESSCOLLECTIONEX_SWEPT_STR, __LINE__, __FILE__);
		}

		if (_isRunning(current) && !force)
			throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_RUN, exception::ERRNO_LIBDODO, COLLECTIONEX_ISALREADYRUNNING, PCPROCESSCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);

		pid_t pid = fork();

		if (pid == 0)
		{
			current->func(current->data);

			_exit(0);
		}
		else
		{
			if (pid == -1)
				throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_RUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			else
				current->pid = pid;
		}

		current->isRunning = true;
		++(current->executed);
	}
	else
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_RUN, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCPROCESSCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::stop(unsigned long position)
{
	if (getProcess(position))
	{
		if (!_isRunning(current))
			throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_STOP, exception::ERRNO_LIBDODO, COLLECTIONEX_ISNOTRUNNING, PCPROCESSCOLLECTIONEX_ISNOTRUNNING_STR, __LINE__, __FILE__);

		if (kill(current->pid, 9) == -1)
			throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		current->isRunning = false;
	}
	else
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_STOP, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCPROCESSCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::stop()
{
	dodoList<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;

		if (kill(i->pid, 9) == -1)
			throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		i->isRunning = false;
	}
}

//-------------------------------------------------------------------

void
collection::wait(unsigned long position)
{
	if (getProcess(position))
	{
		if (!_isRunning(current))
			throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_WAIT, exception::ERRNO_LIBDODO, COLLECTIONEX_ISNOTRUNNING, PCPROCESSCOLLECTIONEX_ISNOTRUNNING_STR, __LINE__, __FILE__);

		if (waitpid(current->pid, NULL, 0) == -1)
			throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		current->isRunning = false;
	}
	else
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_WAIT, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCPROCESSCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::wait()
{
	dodoList<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;

		if (waitpid(i->pid, NULL, 0) == -1)
			throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		i->isRunning = false;
	}
}

//-------------------------------------------------------------------

bool
collection::isRunning(unsigned long position) const
{
	if (getProcess(position))
		return _isRunning(current);
	else
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_ISRUNNING, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCPROCESSCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
collection::running() const
{
	unsigned long amount(0);

	dodoList<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
		if (_isRunning(i))
			++amount;

	return amount;
}

//-------------------------------------------------------------------

void
collection::sweepTrash()
{
	dodoList<__processInfo>::iterator i(processes.begin()), j(processes.end());
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
collection::setExecutionLimit(unsigned long position,
							  unsigned long limit)
{
	if (getProcess(position))
		current->executeLimit = limit;
	else
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_SETEXECUTIONLIMIT, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCPROCESSCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

__processMod
collection::getModuleInfo(const dodoString &module,
						  void             *toInit)
{
#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initIpcProcessCollectionModule init = (initIpcProcessCollectionModule)dlsym(handle, "initIpcProcessCollectionModule");
	if (init == NULL)
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__processMod mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif

	return mod;
}

//-------------------------------------------------------------------

unsigned long
collection::add(const dodoString &module,
				void             *data,
				void             *toInit)
{
	__processInfo process;

	process.data = data;
	process.position = ++processNum;

#ifdef DL_FAST
	process.handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	process.handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (process.handle == NULL)
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initIpcProcessCollectionModule init = (initIpcProcessCollectionModule)dlsym(process.handle, "initIpcProcessCollectionModule");
	if (init == NULL)
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__processMod temp = init(toInit);

	job::routine in = (job::routine)dlsym(process.handle, temp.hook);
	if (in == NULL)
		throw exception::basic(exception::ERRMODULE_PCPROCESSCOLLECTION, COLLECTIONEX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	process.executeLimit = temp.executeLimit;
	process.action = temp.action;
	process.func = in;

	processes.push_back(process);

	return process.position;
}

#endif

//-------------------------------------------------------------------

dodoList<unsigned long>
collection::getIds()
{
	dodoList<unsigned long> ids;

	dodoList<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
		ids.push_back(i->position);

	return ids;

}

//-------------------------------------------------------------------

