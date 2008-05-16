/***************************************************************************
 *            ipcThreadCollection.cc
 *
 *  Wed Nov 30 22:02:16 2005
 *  Copyright  2005  Ni@m
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


#include <libdodo/ipcThreadCollection.h>

using namespace dodo::ipc::thread;

__threadInfo::__threadInfo() :

#ifdef PTHREAD_EXT

	thread(0),

#endif

	isRunning(false),
	executed(0),
	executeLimit(0)
{
}

//-------------------------------------------------------------------

collection::collection(collection &st)
{
}

//-------------------------------------------------------------------

collection::collection() : threadNum(0)
{
#ifdef PTHREAD_EXT

	pthread_attr_init(&attr);

#endif
}

//-------------------------------------------------------------------

collection::~collection()
{
#ifdef PTHREAD_EXT

	pthread_attr_destroy(&attr);

#endif

	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());

#ifdef DL_EXT
	deinitIpcThreadCollectionModule deinit;
#endif

	for (; i != j; ++i)
	{
		if (!_isRunning(i) || i->detached)
			continue;

		switch (i->action)
		{
			case COLLECTION_ONDESTRUCT_KEEP_ALIVE:

#ifdef PTHREAD_EXT

				pthread_detach(i->thread);

#endif

				break;

			case COLLECTION_ONDESTRUCT_STOP:

#ifdef PTHREAD_EXT

				pthread_cancel(i->thread);

#endif

				break;

			case COLLECTION_ONDESTRUCT_WAIT:
			default:

#ifdef PTHREAD_EXT

				pthread_join(i->thread, NULL);

#endif

				break;
		}

#ifdef DL_EXT

		if (i->handle != NULL)
		{
			deinit = (deinitIpcThreadCollectionModule)dlsym(i->handle, "deinitIpcThreadCollectionModule");
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
				void    *data)
{
	return add(func, data, false, COLLECTION_ONDESTRUCT_WAIT, 2097152);
}

//-------------------------------------------------------------------

unsigned long
collection::add(job::routine func,
				void       *data,
				bool detached,
				short action,
				int stackSize)
{
	__threadInfo thread;

	thread.detached = detached;
	thread.data = data;
	thread.func = func;
	thread.position = ++threadNum;
	thread.stackSize = stackSize;
	thread.action = action;
	thread.executeLimit = 0;

#ifdef DL_EXT
	thread.handle = NULL;
#endif

	threads.push_back(thread);

	return thread.position;
}

//-------------------------------------------------------------------

bool
collection::getThread(unsigned long position) const
{
	dodoList<__threadInfo>::const_iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
		if (i->position == position)
		{
			current = *((dodoList<__threadInfo>::iterator *) & i);
			return true;
		}

	return false;
}

//-------------------------------------------------------------------

void
collection::del(unsigned long position,
				bool force)
{
	if (getThread(position))
	{
		if (_isRunning(current))
		{
			if (!force)
				throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_DEL, ERR_LIBDODO, COLLECTIONEX_ISALREADYRUNNING, IPCTHREADCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else
			{
#ifdef PTHREAD_EXT

				errno = pthread_cancel(current->thread);
				if (errno != 0)
					throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_DEL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
			}
		}

#ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitIpcThreadCollectionModule deinit;

			deinit = (deinitIpcThreadCollectionModule)dlsym(current->handle, "deinitIpcThreadCollectionModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose(current->handle) != 0)
				throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_DEL, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif
		}

#endif

		threads.erase(current);
	}
	else
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_DEL, ERR_LIBDODO, COLLECTIONEX_NOTFOUND, IPCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::replace(unsigned long position,
					job::routine func,
					void          *data,
					bool force,
					bool detached,
					short action,
					int stackSize)
{
	if (getThread(position))
	{
		if (_isRunning(current))
		{
			if (!force)
				throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_REPLACE, ERR_LIBDODO, COLLECTIONEX_ISALREADYRUNNING, IPCTHREADCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else
			{
#ifdef PTHREAD_EXT

				errno = pthread_cancel(current->thread);
				if (errno != 0)
					throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_REPLACE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
			}
		}


#ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitIpcThreadCollectionModule deinit;

			deinit = (deinitIpcThreadCollectionModule)dlsym(current->handle, "deinitIpcThreadCollectionModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose(current->handle) != 0)
				throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_REPLACE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif

			current->handle = NULL;
		}

#endif

		current->data = data;
		current->func = func;
		current->isRunning = false;
		current->detached = detached;
		current->stackSize = stackSize;
		current->action = action;
	}
	else
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_REPLACE, ERR_LIBDODO, COLLECTIONEX_NOTFOUND, IPCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::run(unsigned long position,
				bool force)
{
	if (getThread(position))
	{
		if (current->executeLimit > 0 && (current->executeLimit <= current->executed))
		{
			threads.erase(current);

			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_RUN, ERR_LIBDODO, COLLECTIONEX_SWEPT, IPCTHREADCOLLECTIONEX_SWEPT_STR, __LINE__, __FILE__);
		}

		if (_isRunning(current) && !force)
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_RUN, ERR_LIBDODO, COLLECTIONEX_ISALREADYRUNNING, IPCTHREADCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);

#ifdef PTHREAD_EXT

		if (current->detached)
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		else
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		errno = pthread_attr_setstacksize(&attr, current->stackSize);
		if (errno != 0)
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_RUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		errno = pthread_create(&(current->thread), &attr, current->func, current->data);
		if (errno != 0)
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_RUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		current->isRunning = true;
		++(current->executed);
	}
	else
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_RUN, ERR_LIBDODO, COLLECTIONEX_NOTFOUND, IPCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::wait(unsigned long position)
{
	if (getThread(position))
	{
		if (!_isRunning(current))
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_WAIT, ERR_LIBDODO, COLLECTIONEX_ISNOTRUNNING, IPCTHREADCOLLECTIONEX_ISNOTRUNNING_STR, __LINE__, __FILE__);

		if (current->detached)
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_WAIT, ERR_LIBDODO, COLLECTIONEX_ISDETACHED, IPCTHREADCOLLECTIONEX_ISDETACHED_STR, __LINE__, __FILE__);

#ifdef PTHREAD_EXT

		errno = pthread_join(current->thread, NULL);
		if (errno != 0)
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_WAIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		current->isRunning = false;
	}
	else
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_WAIT, ERR_LIBDODO, COLLECTIONEX_NOTFOUND, IPCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::wait()
{
	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i) || i->detached)
			continue;

#ifdef PTHREAD_EXT

		errno = pthread_join(i->thread, NULL);
		if (errno != 0)
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_WAIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		i->isRunning = false;
	}
}

//-------------------------------------------------------------------

void
collection::stop(unsigned long position)
{
	if (getThread(position))
	{
		if (!_isRunning(current))
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_STOP, ERR_LIBDODO, COLLECTIONEX_ISNOTRUNNING, IPCTHREADCOLLECTIONEX_ISNOTRUNNING_STR, __LINE__, __FILE__);

#ifdef PTHREAD_EXT

		errno = pthread_cancel(current->thread);
		if (errno != 0)
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_STOP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		current->isRunning = false;
	}
	else
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_STOP, ERR_LIBDODO, COLLECTIONEX_NOTFOUND, IPCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::stop()
{
	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;

#ifdef PTHREAD_EXT

		errno = pthread_cancel(i->thread);
		if (errno != 0)
			throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_STOP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		i->isRunning = false;
	}
}

//-------------------------------------------------------------------


bool
collection::isRunning(unsigned long position) const
{
	if (getThread(position))
		return _isRunning(current);
	else
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_ISRUNNING, ERR_LIBDODO, COLLECTIONEX_NOTFOUND, IPCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
collection::_isRunning(dodoList<__threadInfo>::iterator &position) const
{
	if (!position->isRunning)
		return false;

#ifdef PTHREAD_EXT

	errno = pthread_kill(position->thread, 0);
	if (errno != 0)
	{
		if (errno == ESRCH || errno == EAGAIN)
		{
			position->isRunning = false;

			return false;
		}

		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX__ISRUNNING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

#endif

	return true;
}

//-------------------------------------------------------------------

void
collection::sweepTrash()
{
	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());
	while (i != j)
	{
		if (_isRunning(i))
		{
			++i;

			continue;
		}

		if (i->executeLimit > 0 && (i->executeLimit <= i->executed))
		{
			i = threads.erase(i);

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
	if (getThread(position))
		current->executeLimit = limit;
	else
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_SETEXECUTIONLIMIT, ERR_LIBDODO, COLLECTIONEX_NOTFOUND, IPCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
collection::running() const
{
	unsigned long amount(0);

	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
		if (_isRunning(i))
			++amount;

	return amount;
}

//-------------------------------------------------------------------

#ifdef DL_EXT

__threadMod
collection::getModuleInfo(const dodoString &module,
						  void             *toInit)
{
#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initIpcThreadCollectionModule init = (initIpcThreadCollectionModule)dlsym(handle, "initIpcThreadCollectionModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__threadMod mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif

	return mod;
}

//-------------------------------------------------------------------

unsigned long
collection::add(const dodoString &module,
				void             *data,
				void             *toInit)
{
	__threadInfo thread;

	thread.data = data;
	thread.position = ++threadNum;

#ifdef DL_FAST
	thread.handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	thread.handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (thread.handle == NULL)
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initIpcThreadCollectionModule init = (initIpcThreadCollectionModule)dlsym(thread.handle, "initIpcThreadCollectionModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__threadMod temp = init(toInit);

	job::routine in = (job::routine)dlsym(thread.handle, temp.hook);
	if (in == NULL)
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	thread.executeLimit = temp.executeLimit;
	thread.detached = temp.detached;
	thread.stackSize = temp.stackSize;
	thread.action = temp.action;
	thread.func = in;

	threads.push_back(thread);

	return thread.position;
}

#endif

//-------------------------------------------------------------------

void
collection::blockSignal(int signals,
						bool block)
{
	sigset_t signal_mask;
	sigemptyset(&signal_mask);

	tools::os::sigMask(&signal_mask, signals);

#ifdef PTHREAD_EXT

	if (block)
		pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
	else
		pthread_sigmask(SIG_UNBLOCK, &signal_mask, NULL);

#endif
}

//-------------------------------------------------------------------

unsigned long
collection::addNRun(job::routine func,
					void    *data)
{
	return addNRun(func, data, 1, false, COLLECTION_ONDESTRUCT_WAIT, 2097152);
}

//-------------------------------------------------------------------

unsigned long
collection::addNRun(job::routine func,
					void          *data,
					unsigned long limit,
					bool detached,
					short action,
					int stackSize)
{
	__threadInfo thread;

	thread.detached = detached;
	thread.data = data;
	thread.func = func;
	thread.position = ++threadNum;
	thread.stackSize = stackSize;
	thread.action = action;
	thread.executeLimit = limit;

#ifdef DL_EXT
	thread.handle = NULL;
#endif

#ifdef PTHREAD_EXT

	if (detached)
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	else
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	errno = pthread_attr_setstacksize(&attr, stackSize);
	if (errno != 0)
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_ADDNRUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	errno = pthread_create(&(thread.thread), &attr, func, data);
	if (errno != 0)
		throw baseEx(ERRMODULE_IPCTHREADCOLLECTION, COLLECTIONEX_ADDNRUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

	thread.isRunning = true;
	++(thread.executed);

	threads.push_back(thread);

	return thread.position;
}

//-------------------------------------------------------------------

dodoList<unsigned long>
collection::getJobsIds()
{
	dodoList<unsigned long> ids;

	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
		ids.push_back(i->position);

	return ids;

}

//-------------------------------------------------------------------

