/***************************************************************************
 *            pcThreadManager.cc
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

#include <libdodo/pcThreadManager.h>
#include <libdodo/pcJobManager.h>
#include <libdodo/toolsOs.h>
#include <libdodo/pcThreadManagerEx.h>
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
				unsigned long id;         ///< identificator
				job::routine  func;             ///< function to execute
				int           stackSize;        ///< size of stack for thread[in bytes]
				short         action;           ///< action on object destruction[@see job::onDestructionEnum]

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
	status(0)
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

manager::manager(manager &st)
{
}

//-------------------------------------------------------------------

manager::manager() : threadNum(0)
{
#ifdef PTHREAD_EXT
	pthread_attr_init(&attr);
#endif
}

//-------------------------------------------------------------------

manager::~manager()
{
#ifdef PTHREAD_EXT
	pthread_attr_destroy(&attr);
#endif

	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());

#ifdef DL_EXT
	deinitModule deinit;
#endif

	for (; i != j; ++i) {
		if (!_isRunning(i) || (*i)->detached) {
			delete *i;

			continue;
		}

		switch ((*i)->action) {
			case job::ON_DESTRUCTION_KEEP_ALIVE:

#ifdef PTHREAD_EXT
				pthread_detach((*i)->thread);
#endif

				break;

			case job::ON_DESTRUCTION_STOP:

#ifdef PTHREAD_EXT
				pthread_cancel((*i)->thread);
#endif

				break;

			case job::ON_DESTRUCTION_WAIT:
			default:

#ifdef PTHREAD_EXT
				pthread_join((*i)->thread, NULL);
#endif

				break;
		}

#ifdef DL_EXT
		if ((*i)->handle != NULL) {
			deinit = (deinitModule)dlsym((*i)->handle, "deinitPcThreadModule");
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
manager::add(job::routine func,
				void         *data,
				short        action)
{
	__thread__ *thread = new __thread__;

	thread->detached = false;
	thread->data = data;
	thread->func = func;
	thread->id = ++threadNum;
	thread->stackSize = 8388608;
	thread->action = action;

#ifdef DL_EXT
	thread->handle = NULL;
#endif

	threads.push_back(thread);

	return thread->id;
}

//-------------------------------------------------------------------

bool
manager::getThread(unsigned long id) const
{
	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i) {
		if ((*i)->id == id) {
			current = i;

			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------

void
manager::remove(unsigned long id,
				bool          force)
{
	if (getThread(id)) {
		if (_isRunning(current)) {
			if (!force)
				throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_REMOVE, exception::ERRNO_LIBDODO, MANAGEREX_ISALREADYRUNNING, PCTHREADMANAGEREX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else {
#ifdef PTHREAD_EXT
				errno = pthread_cancel((*current)->thread);
				if (errno != 0)
					throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
			}
		}

#ifdef DL_EXT
		if ((*current)->handle != NULL) {
			deinitModule deinit;

			deinit = (deinitModule)dlsym((*current)->handle, "deinitPcThreadModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose((*current)->handle) != 0)
				throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_REMOVE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif
		}
#endif

		delete *current;

		threads.erase(current);
	} else
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_REMOVE, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCTHREADMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
manager::run(unsigned long id,
				bool          force)
{
	if (getThread(id)) {
		if (_isRunning(current) && !force)
			throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_RUN, exception::ERRNO_LIBDODO, MANAGEREX_ISALREADYRUNNING, PCTHREADMANAGEREX_ISALREADYRUNNING_STR, __LINE__, __FILE__);

#ifdef PTHREAD_EXT
		if ((*current)->detached)
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		else
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		errno = pthread_attr_setstacksize(&attr, (*current)->stackSize);
		if (errno != 0)
			throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_RUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		errno = pthread_create(&((*current)->thread), &attr, __thread__::routine, *current);
		if (errno != 0)
			throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_RUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		(*current)->isRunning = true;
	} else
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_RUN, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCTHREADMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
manager::wait(unsigned long id)
{
	if (getThread(id)) {
		if ((*current)->detached)
			throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_WAIT, exception::ERRNO_LIBDODO, MANAGEREX_ISDETACHED, PCTHREADMANAGEREX_ISDETACHED_STR, __LINE__, __FILE__);

		if ((*current)->joined)
			return (*current)->status;

		if (!(*current)->isRunning)
			throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_WAIT, exception::ERRNO_LIBDODO, MANAGEREX_ISNOTLAUNCHED, PCTHREADMANAGEREX_ISNOTLAUNCHED_STR, __LINE__, __FILE__);

		int status = 0;

#ifdef PTHREAD_EXT
		errno = pthread_join((*current)->thread, NULL);
		if (errno != 0)
			throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		status = (*current)->status;
#endif

		(*current)->joined = true;
		(*current)->isRunning = false;

		return status;
	} else
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_WAIT, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCTHREADMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
manager::wait()
{
	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i) {
		if ((*i)->joined || (*i)->detached)
			continue;

		if (!(*current)->isRunning)
			continue;

#ifdef PTHREAD_EXT
		errno = pthread_join((*i)->thread, NULL);
		if (errno != 0)
			throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		(*i)->joined = true;
		(*i)->isRunning = false;
	}
}

//-------------------------------------------------------------------

void
manager::stop(unsigned long id)
{
	if (getThread(id)) {
#ifdef PTHREAD_EXT
		errno = pthread_cancel((*current)->thread);
		if (errno != 0)
			throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		(*current)->isRunning = false;
	} else
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_STOP, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCTHREADMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
manager::stop()
{
	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i) {
		if (!_isRunning(i))
			continue;

#ifdef PTHREAD_EXT
		errno = pthread_cancel((*i)->thread);
		if (errno != 0)
			throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

		(*i)->isRunning = false;
	}
}

//-------------------------------------------------------------------

bool
manager::isRunning(unsigned long id) const
{
	if (getThread(id))
		return _isRunning(current);
	else
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_ISRUNNING, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCTHREADMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
manager::_isRunning(dodoList<__thread__ *>::iterator &id) const
{
	if (!(*id)->isRunning)
		return false;

#ifdef PTHREAD_EXT
	errno = pthread_kill((*id)->thread, 0);
	if (errno != 0) {
		if (errno == ESRCH || errno == EAGAIN) {
			(*id)->isRunning = false;

			return false;
		}

		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX__ISRUNNING, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif

	return true;
}

//-------------------------------------------------------------------

void
manager::setStackSize(unsigned long id,
						 unsigned long size)
{
	if (getThread(id))
		(*current)->stackSize = size;
	else
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_SETSTACKSIZE, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCTHREADMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
manager::running() const
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
__module__
manager::module(const dodoString &module,
						  void             *toInit)
{
#ifdef DL_FAST
	void *handle = dlopen(module.data(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.data(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initModule init = (initModule)dlsym(handle, "initPcThreadModule");
	if (init == NULL)
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__module__ mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

	return mod;
}

//-------------------------------------------------------------------

unsigned long
manager::add(const dodoString &module,
				void             *data,
				void             *toInit)
{
	__thread__ *thread = new __thread__;

	thread->data = data;
	thread->id = ++threadNum;

#ifdef DL_FAST
	thread->handle = dlopen(module.data(), RTLD_LAZY | RTLD_NODELETE);
#else
	thread->handle = dlopen(module.data(), RTLD_LAZY);
#endif
	if (thread->handle == NULL)
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initModule init = (initModule)dlsym(thread->handle, "initPcThreadModule");
	if (init == NULL)
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__module__ temp = init(toInit);

	job::routine in = (job::routine)dlsym(thread->handle, temp.hook);
	if (in == NULL)
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	thread->detached = temp.detached;
	thread->stackSize = temp.stackSize;
	thread->action = temp.action;
	thread->func = in;

	threads.push_back(thread);

	return thread->id;
}
#endif

//-------------------------------------------------------------------

void
manager::blockSignal(int  signals,
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
manager::addNRun(job::routine  func,
					void          *data,
					short         action)
{
	return addNRun(func, data, action, false);
}

//-------------------------------------------------------------------

unsigned long
manager::addNRunDetached(job::routine  func,
							void          *data,
							short         action)
{
	return addNRun(func, data, action, true);
}

//-------------------------------------------------------------------

unsigned long
manager::addNRun(job::routine  func,
					void          *data,
					short         action,
					bool detached)
{
	__thread__ *thread = new __thread__;

	thread->detached = detached;
	thread->data = data;
	thread->func = func;
	thread->id = ++threadNum;
	thread->stackSize = 8388608;
	thread->action = action;

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

	errno = pthread_attr_setstacksize(&attr, thread->stackSize);
	if (errno != 0)
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_ADDNRUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	errno = pthread_create(&ti->thread, &attr, __thread__::routine, ti);
	if (errno != 0)
		throw exception::basic(exception::MODULE_PCTHREADMANAGER, MANAGEREX_ADDNRUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

	thread->isRunning = true;

	return thread->id;
}

//-------------------------------------------------------------------

dodoList<unsigned long>
manager::jobs()
{
	dodoList<unsigned long> ids;

	dodoList<__thread__ *>::iterator i(threads.begin()), j(threads.end());
	for (; i != j; ++i)
		ids.push_back((*i)->id);

	return ids;
}

//-------------------------------------------------------------------

