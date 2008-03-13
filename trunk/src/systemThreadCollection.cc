/***************************************************************************
 *            systemThreadCollection.cc
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


#include <libdodo/systemThreadCollection.h>

#ifdef PTHREAD_EXT

using namespace dodo;

__threadInfo::__threadInfo() : thread(0),
							   isRunning(false),
							   executed(0),
							   executeLimit(0)
{
}

//-------------------------------------------------------------------

systemThreadCollection::systemThreadCollection(systemThreadCollection &st)
{
}

//-------------------------------------------------------------------

systemThreadCollection::systemThreadCollection() : threadNum(0)
{
	pthread_attr_init(&attr);
}

//-------------------------------------------------------------------

systemThreadCollection::~systemThreadCollection()
{
	pthread_attr_destroy(&attr);

	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());

#ifdef DL_EXT
	deinitSystemThreadsModule deinit;
#endif

	for (; i != j; ++i)
	{
		if (!_isRunning(i) || i->detached)
			continue;

		switch (i->action)
		{
			case SYSTEMTHREADCOLLECTION_KEEP_ALIVE:

				pthread_detach(i->thread);

				break;

			case SYSTEMTHREADCOLLECTION_STOP:

				pthread_cancel(i->thread);

				break;

			case SYSTEMTHREADCOLLECTION_WAIT:
			default:

				pthread_join(i->thread, NULL);
		}

#ifdef DL_EXT

		if (i->handle != NULL)
		{
			deinit = (deinitSystemThreadsModule)dlsym(i->handle, "deinitSystemThreadsModule");
			if (deinit != NULL)
				deinit();

			dlclose(i->handle);
		}

#endif
	}
}

//-------------------------------------------------------------------

unsigned long
systemThreadCollection::add(jobFunc func,
				   void    *data)
{
	return add(func, data, false, SYSTEMTHREADCOLLECTION_WAIT, 2097152);
}

//-------------------------------------------------------------------

unsigned long
systemThreadCollection::add(threadFunc func,
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
systemThreadCollection::getThread(unsigned long position) const
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
systemThreadCollection::del(unsigned long position,
				   bool force)
{
	if (getThread(position))
	{
		if (_isRunning(current))
		{
			if (!force)
				throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_DEL, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_ISALREADYRUNNING, SYSTEMTHREADCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else
			{
				errno = pthread_cancel(current->thread);
				if (errno != 0)
					throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_DEL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}

#ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitSystemThreadsModule deinit;

			deinit = (deinitSystemThreadsModule)dlsym(current->handle, "deinitSystemThreadsModule");
			if (deinit != NULL)
				deinit();

			if (dlclose(current->handle) != 0)
				throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_DEL, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
		}

#endif

		threads.erase(current);
	}
	else
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_DEL, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_NOTFOUND, SYSTEMTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemThreadCollection::replace(unsigned long position,
					   threadFunc func,
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
				throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_REPLACE, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_ISALREADYRUNNING, SYSTEMTHREADCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else
			{
				errno = pthread_cancel(current->thread);
				if (errno != 0)
					throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_REPLACE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}


#ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitSystemThreadsModule deinit;

			deinit = (deinitSystemThreadsModule)dlsym(current->handle, "deinitSystemThreadsModule");
			if (deinit != NULL)
				deinit();

			if (dlclose(current->handle) != 0)
				throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_REPLACE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

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
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_REPLACE, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_NOTFOUND, SYSTEMTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemThreadCollection::run(unsigned long position,
				   bool force)
{
	if (getThread(position))
	{
		if (current->executeLimit > 0 && (current->executeLimit <= current->executed))
		{
			threads.erase(current);

			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_RUN, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_SWEPT, SYSTEMTHREADCOLLECTIONEX_SWEPT_STR, __LINE__, __FILE__);
		}

		if (_isRunning(current) && !force)
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_RUN, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_ISALREADYRUNNING, SYSTEMTHREADCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);

		if (current->detached)
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		else
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		errno = pthread_attr_setstacksize(&attr, current->stackSize);
		if (errno != 0)
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_RUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		errno = pthread_create(&(current->thread), &attr, current->func, current->data);
		if (errno != 0)
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_RUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		current->isRunning = true;
		++ (current->executed);
	}
	else
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_RUN, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_NOTFOUND, SYSTEMTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemThreadCollection::wait(unsigned long position)
{
	if (getThread(position))
	{
		if (!_isRunning(current))
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_WAIT, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_ISNOTRUNNING, SYSTEMTHREADCOLLECTIONEX_ISNOTRUNNING_STR, __LINE__, __FILE__);

		if (current->detached)
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_WAIT, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_ISDETACHED, SYSTEMTHREADCOLLECTIONEX_ISDETACHED_STR, __LINE__, __FILE__);

		errno = pthread_join(current->thread, NULL);
		if (errno != 0)
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_WAIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		current->isRunning = false;
	}
	else
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_WAIT, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_NOTFOUND, SYSTEMTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemThreadCollection::wait()
{
	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i) || i->detached)
			continue;

		errno = pthread_join(i->thread, NULL);
		if (errno != 0)
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_WAIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		i->isRunning = false;
	}
}

//-------------------------------------------------------------------

void
systemThreadCollection::stop(unsigned long position)
{
	if (getThread(position))
	{
		if (!_isRunning(current))
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_STOP, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_ISNOTRUNNING, SYSTEMTHREADCOLLECTIONEX_ISNOTRUNNING_STR, __LINE__, __FILE__);

		errno = pthread_cancel(current->thread);
		if (errno != 0)
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_STOP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		current->isRunning = false;
	}
	else
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_STOP, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_NOTFOUND, SYSTEMTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemThreadCollection::stop()
{
	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
	{
		if (!_isRunning(i))
			continue;

		errno = pthread_cancel(i->thread);
		if (errno != 0)
			throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_STOP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		i->isRunning = false;
	}
}

//-------------------------------------------------------------------


bool
systemThreadCollection::isRunning(unsigned long position) const
{
	if (getThread(position))
		return _isRunning(current);
	else
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_ISRUNNING, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_NOTFOUND, SYSTEMTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
systemThreadCollection::_isRunning(dodoList<__threadInfo>::iterator &position) const
{
	if (!position->isRunning)
		return false;

	errno = pthread_kill(position->thread, 0);
	if (errno != 0)
	{
		if (errno == ESRCH || errno == EAGAIN)
		{
			position->isRunning = false;

			return false;
		}

		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX__ISRUNNING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	return true;
}

//-------------------------------------------------------------------

void
systemThreadCollection::sweepTrash()
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
systemThreadCollection::setExecutionLimit(unsigned long position,
								 unsigned long limit)
{
	if (getThread(position))
		current->executeLimit = limit;
	else
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_SETEXECUTIONLIMIT, ERR_LIBDODO, SYSTEMTHREADCOLLECTIONEX_NOTFOUND, SYSTEMTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
systemThreadCollection::running() const
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

__systemThreadCollectionMod
systemThreadCollection::getModuleInfo(const dodoString &module,
							 void             *toInit)
{
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
	if (handle == NULL)
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initSystemThreadsModule init = (initSystemThreadsModule)dlsym(handle, "initSystemThreadsModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__systemThreadCollectionMod mod = init(toInit);

	if (dlclose(handle) != 0)
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	return mod;
}

//-------------------------------------------------------------------

unsigned long
systemThreadCollection::add(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	__threadInfo thread;

	thread.data = data;
	thread.position = ++threadNum;

	thread.handle = dlopen(module.c_str(), RTLD_LAZY);
	if (thread.handle == NULL)
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initSystemThreadsModule init = (initSystemThreadsModule)dlsym(thread.handle, "initSystemThreadsModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__systemThreadCollectionMod temp = init(toInit);

	threadFunc in = (threadFunc)dlsym(thread.handle, temp.hook);
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_ADD, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

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
systemThreadCollection::blockSignal(int signals,
						   bool block)
{
	sigset_t signal_mask;
	sigemptyset(&signal_mask);

	systemTools::sigMask(&signal_mask, signals);

	if (block)
		pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
	else
		pthread_sigmask(SIG_UNBLOCK, &signal_mask, NULL);
}

//-------------------------------------------------------------------

unsigned long
systemThreadCollection::addNRun(jobFunc func,
					   void    *data)
{
	return addNRun(func, data, 1, false, SYSTEMTHREADCOLLECTION_WAIT, 2097152);
}

//-------------------------------------------------------------------

unsigned long
systemThreadCollection::addNRun(threadFunc func,
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

	if (detached)
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	else
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	errno = pthread_attr_setstacksize(&attr, stackSize);
	if (errno != 0)
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_ADDNRUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	errno = pthread_create(&(thread.thread), &attr, func, data);
	if (errno != 0)
		throw baseEx(ERRMODULE_SYSTEMTHREADCOLLECTION, SYSTEMTHREADCOLLECTIONEX_ADDNRUN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	thread.isRunning = true;
	++ (thread.executed);

	threads.push_back(thread);

	return thread.position;
}

//-------------------------------------------------------------------

dodoList<unsigned long>
systemThreadCollection::getJobsIds()
{
	dodoList<unsigned long> ids;

	dodoList<__threadInfo>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
		ids.push_back(i->position);

	return ids;

}

#endif


//-------------------------------------------------------------------

