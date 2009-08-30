/***************************************************************************
 *            pcProcessManager.cc
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

#include <libdodo/directives.h>

#ifdef DL_EXT
#include <dlfcn.h>
#endif

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include <libdodo/pcJobManager.h>

namespace dodo {
	namespace pc {
		namespace process {
			/**
			 * @struct __process__
			 * @brief defines process information
			 */
			struct __process__ {
				/**
				 * constuctor
				 */
				__process__();

				pid_t         pid;              ///< process pid
				void          *data;            ///< process data
				bool          isRunning;        ///< true if the process is running
				bool          joined;           ///< true if the process was joined
				int           status;           ///< process exit status
				unsigned long position;         ///< identificator
				job::routine  func;             ///< function to execute
				short         action;           ///< action on object destruction[@see job::onDestructionEnum]
#ifdef DL_EXT
				void          *handle;          ///< handle to library
#endif
			};
		};
	};
};

#include <libdodo/pcProcessManager.h>
#include <libdodo/types.h>
#include <libdodo/pcJobManager.h>
#include <libdodo/pcProcessManagerEx.h>

using namespace dodo::pc::process;

__process__::__process__() : isRunning(false),
							 joined(false),
							 status(0)
{
}

//-------------------------------------------------------------------

manager::manager(manager &sp)
{
}

//-------------------------------------------------------------------

manager::manager() : processNum(0)
{
}

//-------------------------------------------------------------------

manager::~manager()
{
	dodoList<__process__ *>::iterator i(processes.begin()), j(processes.end());

#ifdef DL_EXT
	deinitModule deinit;
#endif

	for (; i != j; ++i) {
		if (!_isRunning(i)) {
			delete *i;

			continue;
		}

		switch ((*i)->action) {
			case job::ON_DESTRUCTION_KEEP_ALIVE:

				waitpid((*i)->pid, NULL, WNOHANG);

				break;

			case job::ON_DESTRUCTION_STOP:

				kill((*i)->pid, 2);

				break;

			case job::ON_DESTRUCTION_WAIT:
			default:

				waitpid((*i)->pid, NULL, 0);
		}

#ifdef DL_EXT
		if ((*i)->handle != NULL) {
			deinit = (deinitModule)dlsym((*i)->handle, "deinitPcProcessModule");
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
	__process__ *process = new __process__;

	process->data = data;
	process->func = func;
	process->position = ++processNum;
	process->action = action;

#ifdef DL_EXT
	process->handle = NULL;
#endif

	processes.push_back(process);

	return process->position;
}

//-------------------------------------------------------------------

unsigned long
manager::addNRun(job::routine  func,
					void          *data,
					short         action)
{
	__process__ *process = new __process__;

	process->data = data;
	process->func = func;
	process->position = ++processNum;
	process->action = action;

#ifdef DL_EXT
	process->handle = NULL;
#endif

	processes.push_back(process);

	pid_t pid = fork();

	if (pid == 0)
		_exit(func(data));
	else {
		if (pid == -1)
			throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_ADDNRUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		else
			process->pid = pid;
	}

	process->isRunning = true;

	return process->position;
}

//-------------------------------------------------------------------

void
manager::remove(unsigned long position,
				bool          force)
{
	if (getProcess(position)) {
		if (_isRunning(current)) {
			if (!force)
				throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_REMOVE, exception::ERRNO_LIBDODO, MANAGEREX_ISALREADYRUNNING, PCPROCESSMANAGEREX_ISALREADYRUNNING_STR, __LINE__, __FILE__);
			else if (kill((*current)->pid, 2) == -1)
				throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		}

#ifdef DL_EXT
		if ((*current)->handle != NULL) {
			deinitModule deinit;

			deinit = (deinitModule)dlsym((*current)->handle, "deinitPcProcessModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose((*current)->handle) != 0)
				throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_REMOVE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif
		}
#endif

		delete *current;

		processes.erase(current);
	} else
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_REMOVE, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCPROCESSMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
manager::getProcess(unsigned long position) const
{
	dodoList<__process__ *>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i) {
		if ((*i)->position == position) {
			current = i;

			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------

bool
manager::_isRunning(dodoList<__process__ *>::iterator &position) const
{
	if (!(*position)->isRunning)
		return false;

	int res = kill((*position)->pid, 0);
	if (res != 0) {
		if (errno == ESRCH) {
			(*position)->isRunning = false;

			return false;
		}

		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX__ISRUNNING, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	return true;
}

//-------------------------------------------------------------------

void
manager::run(unsigned long position,
				bool          force)
{
	if (getProcess(position)) {
		if (_isRunning(current) && !force)
			throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_RUN, exception::ERRNO_LIBDODO, MANAGEREX_ISALREADYRUNNING, PCPROCESSMANAGEREX_ISALREADYRUNNING_STR, __LINE__, __FILE__);

		pid_t pid = fork();

		if (pid == 0)
			_exit((*current)->func((*current)->data));
		else {
			if (pid == -1)
				throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_RUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			else
				(*current)->pid = pid;
		}

		(*current)->isRunning = true;
	} else
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_RUN, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCPROCESSMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
manager::stop(unsigned long position)
{
	if (getProcess(position)) {
		if (kill((*current)->pid, 9) == -1)
			throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		(*current)->isRunning = false;
	} else
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_STOP, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCPROCESSMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
manager::stop()
{
	dodoList<__process__ *>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i) {
		if (!_isRunning(i))
			continue;

		if (kill((*i)->pid, 9) == -1)
			throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		(*i)->isRunning = false;
	}
}

//-------------------------------------------------------------------

int
manager::wait(unsigned long position)
{
	if (getProcess(position)) {
		if ((*current)->joined)
			return (*current)->status;

		int status;

		if (waitpid((*current)->pid, &status, 0) == -1)
			throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (WIFEXITED(status))
			(*current)->status = WEXITSTATUS(status);
		(*current)->isRunning = false;
		(*current)->joined = true;

		return (*current)->status;
	} else
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_WAIT, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCPROCESSMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
manager::wait()
{
	int status;

	dodoList<__process__ *>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i) {
		if ((*i)->joined)
			continue;

		if (waitpid((*i)->pid, &status, 0) == -1)
			throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (WIFEXITED(status))
			(*i)->status = WEXITSTATUS(status);
		(*i)->isRunning = false;
		(*i)->joined = true;
	}
}

//-------------------------------------------------------------------

bool
manager::isRunning(unsigned long position) const
{
	if (getProcess(position))
		return _isRunning(current);
	else
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_ISRUNNING, exception::ERRNO_LIBDODO, MANAGEREX_NOTFOUND, PCPROCESSMANAGEREX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
manager::running() const
{
	unsigned long amount(0);

	dodoList<__process__ *>::iterator i(processes.begin()), j(processes.end());
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
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initModule init = (initModule)dlsym(handle, "initPcProcessModule");
	if (init == NULL)
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__module__ mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

	return mod;
}

//-------------------------------------------------------------------

unsigned long
manager::add(const dodoString &module,
				void             *data,
				void             *toInit)
{
	__process__ *process = new __process__;

	process->data = data;
	process->position = ++processNum;

#ifdef DL_FAST
	process->handle = dlopen(module.data(), RTLD_LAZY | RTLD_NODELETE);
#else
	process->handle = dlopen(module.data(), RTLD_LAZY);
#endif
	if (process->handle == NULL)
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initModule init = (initModule)dlsym(process->handle, "initPcProcessModule");
	if (init == NULL)
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__module__ temp = init(toInit);

	job::routine in = (job::routine)dlsym(process->handle, temp.hook);
	if (in == NULL)
		throw exception::basic(exception::MODULE_PCPROCESSMANAGER, MANAGEREX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	process->action = temp.action;
	process->func = in;

	processes.push_back(process);

	return process->position;
}
#endif

//-------------------------------------------------------------------

dodoList<unsigned long>
manager::jobs()
{
	dodoList<unsigned long> ids;

	dodoList<__process__ *>::iterator i(processes.begin()), j(processes.end());
	for (; i != j; ++i)
		ids.push_back((*i)->position);

	return ids;
}

//-------------------------------------------------------------------

