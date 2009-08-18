/***************************************************************************
 *            pcThreadCollection.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#ifdef DL_EXT
#include <dlfcn.h>
#endif
#ifdef PTHREAD_EXT
#include <pthread.h>
#endif
#include <signal.h>
#include <errno.h>
#include <string.h>

#include <libdodo/pcThreadCollection.h>
#include <libdodo/pcJobCollection.h>
#include <libdodo/toolsOs.h>
#include <libdodo/pcThreadCollectionEx.h>
#include <libdodo/types.h>

namespace dodo {
	namespace pc {
		namespace thread {
			/**
			 * @struct __thread__
			 * @brief defines process information
			 */
			struct __thread__ {
				/**
				 * contructor
				 */
				__thread__();

#ifdef PTHREAD_EXT
				pthread_t     thread;           ///< thread descriptor

				/**
				 * @return thread exit status
				 * @param data defines user data
				 */
				static void   *routine(void *data);
#endif

				void          *data;            ///< thread data
				bool          isRunning;        ///< true if thread is running
				bool          joined;           ///< true if the thread was joined
				int           status;           ///< thread exit status
				bool          detached;         ///< true if thread is detached
				unsigned long position;         ///< identificator
				job::routine  func;             ///< function to execute
				int           stackSize;        ///< size of stack for thread[in bytes]
				short         action;           ///< action on object destruction[see collectionOnDestructEnum]
				unsigned long executed;         ///< amount of times thread was executed
				unsigned long executeLimit;     ///< if greater than one will be a atomatically deleted or deleted with `sweepTrash` method; default is 0(unlimit);

#ifdef DL_EXT
				void          *handle;          ///< handle to library
#endif
			};
		};
	};
};

using namespace dodo::pc::thread;

__thread__::__thread__() :
#ifdef PTHREAD_EXT
	thread(0),
#endif
	isRunning(false),
	joined(false),
	status(0),
	executed(0),
	executeLimit(0)
{
}

//-------------------------------------------------------------------

#ifdef PTHREAD_EXT
void *
__thread__::routine(void *data)
{
	__thread__ *ti = (__thread__ *)data;

	ti->status = ti->func(ti->data);

	return NULL;
}
#endif

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

	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());

#ifdef DL_EXT
	deinitIpcThreadCollectionModule deinit;
#endif

	for (; i != j; ++i) {
		if (!_isRunning(i) || (*i)->detached) {
			delete *i;

			continue;
		}

		switch ((*i)->action) {
			case COLLECTION_ONDESTRUCT_KEEP_ALIVE:

#ifdef PTHREAD_EXT
				pthread_detach((*i)->thread);
#endif

				break;

			case COLLECTION_ONDESTRUCT_STOP:

#ifdef PTHREAD_EXT
				pthread_cancel((*i)->thread);
#endif

				break;

			case COLLECTION_ONDESTRUCT_WAIT:
			default:

#ifdef PTHREAD_EXT
				pthread_join((*i)->thread, NULL);
#endif

				break;
		}

#ifdef DL_EXT
		if ((*i)->handle != NULL) {
			deinit = (deinitIpcThreadCollectionModule)dlsym((*i)->handle, "deinitIpcThreadCollectionModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			dlclose((*i)->handle);
#endif
		}
#endif

		delete *i;
	}
}

//-------------------------------------------------------------------

unsigned long
collection::add(job::routine func,
				void         *data)
{
	return add(func, data, false, COLLECTION_ONDESTRUCT_WAIT, 2097152);
}

//-------------------------------------------------------------------

unsigned long
collection::add(job::routine func,
				void         *data,
				bool         detached,
				short        action,
				int          stackSize)
{
	__thread__ *thread = new __thread__;

	thread->detached = detached;
	thread->data = data;
	thread->func = func;
	thread->position = ++threadNum;
	thread->stackSize = stackSize;
	thread->action = action;
	thread->executeLimit = 0;

#ifdef DL_EXT
	thread->handle = NULL;
#endif

	threads.push_back(thread);

	return thread->position;
}

//-------------------------------------------------------------------

bool
collection::getThread(unsigned long position) const
{
	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i) {
		if ((*i)->position == position) {
			current = i;

			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------

void
collection::del(unsigned long position,
				bool          force)
{
	if (getThread(position)) {
		if (_isRunning(current)) {
			if (!force)
				throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_DEL, exception::ERRNO_LIBDODO, COLLECTIONEX_ISALREADYRUNNING, PCTHREADCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else {
#ifdef PTHREAD_EXT
				errno = pthread_cancel((*current)->thread);
				if (errno != 0)
					throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_DEL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
			}
		}

#ifdef DL_EXT
		if ((*current)->handle != NULL) {
			deinitIpcThreadCollectionModule deinit;

			deinit = (deinitIpcThreadCollectionModule)dlsym((*current)->handle, "deinitIpcThreadCollectionModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose((*current)->handle) != 0)
				throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_DEL, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif
		}
#endif

		delete *current;

		threads.erase(current);
	} else
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_DEL, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::replace(unsigned long position,
					job::routine  func,
					void          *data,
					bool          force,
					bool          detached,
					short         action,
					int           stackSize)
{
	if (getThread(position)) {
		if (_isRunning(current)) {
			if (!force)
				throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_REPLACE, exception::ERRNO_LIBDODO, COLLECTIONEX_ISALREADYRUNNING, PCTHREADCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else {
#ifdef PTHREAD_EXT
				errno = pthread_cancel((*current)->thread);
				if (errno != 0)
					throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_REPLACE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
			}
		}

#ifdef DL_EXT
		if ((*current)->handle != NULL) {
			deinitIpcThreadCollectionModule deinit;

			deinit = (deinitIpcThreadCollectionModule)dlsym((*current)->handle, "deinitIpcThreadCollectionModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose((*current)->handle) != 0)
				throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_REPLACE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

			(*current)->handle = NULL;
		}
#endif

		(*current)->data = data;
		(*current)->func = func;
		(*current)->isRunning = false;
		(*current)->detached = detached;
		(*current)->stackSize = stackSize;
		(*current)->action = action;
	} else
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_REPLACE, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::run(unsigned long position,
				bool          force)
{
	if (getThread(position)) {
		if ((*current)->executeLimit > 0 && ((*current)->executeLimit <= (*current)->executed)) {
			threads.erase(current);

			throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_RUN, exception::ERRNO_LIBDODO, COLLECTIONEX_SWEPT, PCTHREADCOLLECTIONEX_SWEPT_STR, __LINE__, __FILE__);
		}

		if (_isRunning(current) && !force)
			throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_RUN, exception::ERRNO_LIBDODO, COLLECTIONEX_ISALREADYRUNNING, PCTHREADCOLLECTIONEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);

#ifdef PTHREAD_EXT
		if ((*current)->detached)
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		else
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		errno = pthread_attr_setstacksize(&attr, (*current)->stackSize);
		if (errno != 0)
			throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_RUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		errno = pthread_create(&((*current)->thread), &attr, __thread__::routine, *current);
		if (errno != 0)
			throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_RUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		(*current)->isRunning = true;
		++((*current)->executed);
	} else
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_RUN, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
collection::wait(unsigned long position)
{
	if (getThread(position)) {
		if ((*current)->detached)
			throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_WAIT, exception::ERRNO_LIBDODO, COLLECTIONEX_ISDETACHED, PCTHREADCOLLECTIONEX_ISDETACHED_STR, __LINE__, __FILE__);

		if ((*current)->joined)
			return (*current)->status;

		int status = 0;

#ifdef PTHREAD_EXT
		errno = pthread_join((*current)->thread, NULL);
		if (errno != 0)
			throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		status = (*current)->status;
#endif

		(*current)->joined = true;
		(*current)->isRunning = false;

		return status;
	} else
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_WAIT, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::wait()
{
	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i) {
		if ((*i)->joined || (*i)->detached)
			continue;

#ifdef PTHREAD_EXT
		errno = pthread_join((*i)->thread, NULL);
		if (errno != 0)
			throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		(*i)->joined = true;
		(*i)->isRunning = false;
	}
}

//-------------------------------------------------------------------

void
collection::stop(unsigned long position)
{
	if (getThread(position)) {
#ifdef PTHREAD_EXT
		errno = pthread_cancel((*current)->thread);
		if (errno != 0)
			throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		(*current)->isRunning = false;
	} else
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_STOP, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::stop()
{
	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i) {
		if (!_isRunning(i))
			continue;

#ifdef PTHREAD_EXT
		errno = pthread_cancel((*i)->thread);
		if (errno != 0)
			throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		(*i)->isRunning = false;
	}
}

//-------------------------------------------------------------------

bool
collection::isRunning(unsigned long position) const
{
	if (getThread(position))
		return _isRunning(current);
	else
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_ISRUNNING, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
collection::_isRunning(dodoList<__thread__ *>::iterator &position) const
{
	if (!(*position)->isRunning)
		return false;

#ifdef PTHREAD_EXT
	errno = pthread_kill((*position)->thread, 0);
	if (errno != 0) {
		if (errno == ESRCH || errno == EAGAIN) {
			(*position)->isRunning = false;

			return false;
		}

		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX__ISRUNNING, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif

	return true;
}

//-------------------------------------------------------------------

void
collection::sweepTrash()
{
	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	while (i != j) {
		if (_isRunning(i)) {
			++i;

			continue;
		}

		if ((*i)->executeLimit > 0 && ((*i)->executeLimit <= (*i)->executed)) {
			delete *i;

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
		(*current)->executeLimit = limit;
	else
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_SETEXECUTIONLIMIT, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCTHREADCOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
collection::running() const
{
	unsigned long amount(0);

	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
		if (_isRunning(i))
			++amount;

	return amount;
}

//-------------------------------------------------------------------

#ifdef DL_EXT
__threadMod__
collection::getModuleInfo(const dodoString &module,
						  void             *toInit)
{
#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initIpcThreadCollectionModule init = (initIpcThreadCollectionModule)dlsym(handle, "initIpcThreadCollectionModule");
	if (init == NULL)
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__threadMod__ mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

	return mod;
}

//-------------------------------------------------------------------

unsigned long
collection::add(const dodoString &module,
				void             *data,
				void             *toInit)
{
	__thread__ *thread = new __thread__;

	thread->data = data;
	thread->position = ++threadNum;

#ifdef DL_FAST
	thread->handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	thread->handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (thread->handle == NULL)
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initIpcThreadCollectionModule init = (initIpcThreadCollectionModule)dlsym(thread->handle, "initIpcThreadCollectionModule");
	if (init == NULL)
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__threadMod__ temp = init(toInit);

	job::routine in = (job::routine)dlsym(thread->handle, temp.hook);
	if (in == NULL)
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	thread->executeLimit = temp.executeLimit;
	thread->detached = temp.detached;
	thread->stackSize = temp.stackSize;
	thread->action = temp.action;
	thread->func = in;

	threads.push_back(thread);

	return thread->position;
}
#endif

//-------------------------------------------------------------------

void
collection::blockSignal(int  signals,
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
					void         *data)
{
	return addNRun(func, data, 1, false, COLLECTION_ONDESTRUCT_WAIT, 2097152);
}

//-------------------------------------------------------------------

unsigned long
collection::addNRun(job::routine  func,
					void          *data,
					unsigned long limit,
					bool          detached,
					short         action,
					int           stackSize)
{
	__thread__ *thread = new __thread__;

	thread->detached = detached;
	thread->data = data;
	thread->func = func;
	thread->position = ++threadNum;
	thread->stackSize = stackSize;
	thread->action = action;
	thread->executeLimit = limit;

#ifdef DL_EXT
	thread->handle = NULL;
#endif

	threads.push_back(thread);
	__thread__ *ti = threads.back();

#ifdef PTHREAD_EXT
	if (detached)
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	else
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	errno = pthread_attr_setstacksize(&attr, stackSize);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_ADDNRUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	errno = pthread_create(&ti->thread, &attr, __thread__::routine, ti);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCTHREADCOLLECTION, COLLECTIONEX_ADDNRUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

	thread->isRunning = true;
	++(thread->executed);

	return thread->position;
}

//-------------------------------------------------------------------

dodoList<unsigned long>
collection::getIds()
{
	dodoList<unsigned long> ids;

	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
		ids.push_back((*i)->position);

	return ids;
}

//-------------------------------------------------------------------

