/***************************************************************************
 *            toolsOs.cc
 *
 *  Sat Nov 19 15:19:57 2005
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

#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#ifdef PTHREAD_EXT
#include <pthread.h>
#endif
#ifdef DL_EXT
#include <dlfcn.h>
#endif

#include "pcSyncThreadLock.inline"

#include <libdodo/toolsOs.h>
#include <libdodo/toolsOsEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>

using namespace dodo::tools;

#ifdef DL_EXT
void *os::handlesSig[] =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

//-------------------------------------------------------------------

bool os::handlesOpenedSig[] =
{
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false
};
#endif

//-------------------------------------------------------------------

dodo::pc::sync::thread::__lock__ os::syncThreadSection::keeper;

//-------------------------------------------------------------------

os::syncThreadSection os::keeper;

//-------------------------------------------------------------------

os::syncThreadSection::syncThreadSection()
{
#ifdef PTHREAD_EXT
	pthread_mutexattr_t attr;
	errno = pthread_mutexattr_init(&attr);
	if (errno != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOSSYNCTHREADSECTION, SYNCTHREADSECTION_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	errno = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	if (errno != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOSSYNCTHREADSECTION, SYNCTHREADSECTION_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	errno = pthread_mutex_init(&keeper.keeper, &attr);
	if (errno != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOSSYNCTHREADSECTION, SYNCTHREADSECTION_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	errno = pthread_mutexattr_destroy(&attr);
	if (errno != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOSSYNCTHREADSECTION, SYNCTHREADSECTION_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif
}

//-------------------------------------------------------------------

os::syncThreadSection::~syncThreadSection()
{
#ifdef PTHREAD_EXT
	pthread_mutex_destroy(&keeper.keeper);
#endif
}

//-------------------------------------------------------------------

void
os::syncThreadSection::acquire()
{
#ifdef PTHREAD_EXT
	errno = pthread_mutex_lock(&keeper.keeper);
	if (errno != 0 && errno != EDEADLK)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOSSYNCTHREADSECTION, SYNCTHREADSECTION_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif
}

//-------------------------------------------------------------------

void
os::syncThreadSection::release()
{
#ifdef PTHREAD_EXT
	errno = pthread_mutex_unlock(&keeper.keeper);
	if (errno != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOSSYNCTHREADSECTION, SYNCTHREADSECTION_RELEASE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif
}

//-------------------------------------------------------------------

os::syncThreadStack::syncThreadStack()
{
	keeper.acquire();
}

//-------------------------------------------------------------------

os::syncThreadStack::~syncThreadStack()
{
	try
	{
		keeper.release();
	}
	catch (...)
	{
	}
}

//-------------------------------------------------------------------

dodoString
os::getWorkingDir()
{
	char wd[MAXPATHLEN];

	if (getcwd(wd, MAXPATHLEN) == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETWORKINGDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	return wd;
}

//-------------------------------------------------------------------

void
os::setWorkingDir(const dodoString &path)
{
	if (chdir(path.c_str()) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETWORKINGDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------


__usage__
os::getUsageInfo()
{
	rusage use;
	if (getrusage(RUSAGE_SELF, &use) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETUSAGEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	__usage__ info;
	info.time = use.ru_utime.tv_sec * 100 + use.ru_utime.tv_usec;
	info.mem = use.ru_maxrss * 1024;

	return info;
}

//-------------------------------------------------------------------

void
os::changeRoot(const dodoString &path)
{
	setWorkingDir(path);

	if (chroot(path.c_str()) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_CHANGEROOT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

__limits__
os::getLimit(short type)
{
	rlimit limit;

	int realRes(0);

	switch (type)
	{
		case OS_CPUTIME:

			realRes = RLIMIT_CPU;

			break;

		case OS_MAXFILESIZE:

			realRes = RLIMIT_FSIZE;

			break;

		case OS_MAXMEMUSAGE:

			realRes = RLIMIT_DATA;

			break;

		case OS_MAXSTACK:

			realRes = RLIMIT_STACK;

			break;

		case OS_MAXPROC:

			realRes = RLIMIT_NPROC;

			break;

		case OS_MAXOPENFILES:

#ifdef __FreeBSD____
			realRes = RLIMIT_NOFILE;
#else
			realRes = RLIMIT_OFILE;
#endif

			break;

		default:

			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETLIMIT, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (getrlimit(realRes, &limit) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETLIMIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	__limits__ lim;

	lim.current = limit.rlim_cur;
	lim.max = limit.rlim_max;

	return lim;
}

//-------------------------------------------------------------------

void
os::setLimit(short          type,
			 const __limits__ &lim)
{
	rlimit limit;

	int realRes(0);

	switch (type)
	{
		case OS_CPUTIME:

			realRes = RLIMIT_CPU;

			break;

		case OS_MAXFILESIZE:

			realRes = RLIMIT_FSIZE;

			break;

		case OS_MAXMEMUSAGE:

			realRes = RLIMIT_DATA;

			break;

		case OS_MAXSTACK:

			realRes = RLIMIT_STACK;

			break;

		case OS_MAXPROC:

			realRes = RLIMIT_NPROC;

			break;

		case OS_MAXOPENFILES:

#ifdef __FreeBSD____
			realRes = RLIMIT_NOFILE;
#else
			realRes = RLIMIT_OFILE;
#endif

			break;

		default:

			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETLIMIT, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	limit.rlim_cur = lim.current;
	limit.rlim_max = lim.max;

	if (setrlimit(realRes, &limit) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETLIMIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

int
os::getPriority(short type)
{
	int prio = getpriority(PRIO_PROCESS, getUID(type));
	if (prio == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETPRIORITY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	return prio;
}

//-------------------------------------------------------------------

void
os::setPriority(short type,
				int   prio)
{
	if (setpriority(PRIO_PROCESS, getUID(type), prio) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETPRIORITY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

int
os::getUID(short type)
{
	switch (type)
	{
		case OS_UID:

			return getuid();

		case OS_EUID:

			return geteuid();

		default:

			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETUID, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
os::setUID(short type,
		   int   uid)
{
	int res(0);

	switch (type)
	{
		case OS_UID:

			res = setuid(uid);

			break;

		case OS_EUID:

			res = seteuid(uid);

			break;

		default:

			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETUID, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (res == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETUID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

int
os::getGID(short type)
{
	switch (type)
	{
		case OS_UID:

			return getgid();

		case OS_EUID:

			return getegid();

		default:

			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETGID, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
os::setGID(short type,
		   int   uid)
{
	int res(0);

	switch (type)
	{
		case OS_UID:

			res = setgid(uid);

			break;

		case OS_EUID:

			res = setegid(uid);

			break;

		default:

			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETGID, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (res == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETGID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

__userInfo__
os::getUserInfo(int uid)
{
	passwd *in = getpwuid(uid);
	if (in == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETUSERINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	__userInfo__ info;

	return fillUserInfo(info, in);
}

//-------------------------------------------------------------------

__userInfo__
os::getUserInfo(const dodoString &uid)
{
	passwd *in = getpwnam(uid.c_str());
	if (in == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETUSERINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	__userInfo__ info;

	return fillUserInfo(info, in);
}

//-------------------------------------------------------------------

dodoArray<__userInfo__>
os::getUsers()
{
	dodoArray<__userInfo__> users;

	passwd *in;

	__userInfo__ info;

	while ((in = getpwent()) != NULL)
	{
		users.push_back(fillUserInfo(info, in));
	}

	switch (errno)
	{
		case EIO:
		case EMFILE:
		case ENFILE:
		case ENOMEM:

			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETUSERS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	endpwent();

	return users;
}

//-------------------------------------------------------------------

__userInfo__ &
os::fillUserInfo(__userInfo__ &info,
				 void     *in)
{
	info.gid = ((passwd *)in)->pw_gid;
	info.home = ((passwd *)in)->pw_dir;
	info.name = ((passwd *)in)->pw_name;
	info.pass = ((passwd *)in)->pw_passwd;
	info.realName = ((passwd *)in)->pw_gecos;
	info.shell = ((passwd *)in)->pw_shell;
	info.uid = ((passwd *)in)->pw_uid;

	return info;
}

//-------------------------------------------------------------------

__groupInfo__ &
os::fillGroupInfo(__groupInfo__ &info,
				  void       *pw)
{
	info.gid = ((group *)pw)->gr_gid;
	info.name = ((group *)pw)->gr_name;

	info.members.clear();

	int i(0);

	while (((group *)pw)->gr_mem[i] != NULL)
	{
		info.members.push_back(((group *)pw)->gr_mem[i++]);
	}

	return info;
}

//-------------------------------------------------------------------

__groupInfo__
os::getGroupInfo(int uid)
{
	group *in = getgrgid(uid);
	if (in == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETGROUPINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	__groupInfo__ info;

	return fillGroupInfo(info, in);
}

//-------------------------------------------------------------------

__groupInfo__
os::getGroupInfo(const dodoString &uid)
{
	group *in = getgrnam(uid.c_str());
	if (in == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETGROUPINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	__groupInfo__ info;

	return fillGroupInfo(info, in);
}

//-------------------------------------------------------------------

dodoArray<__groupInfo__>
os::getGroups()
{
	dodoArray<__groupInfo__> groups;

	group *in;

	__groupInfo__ info;

	while ((in = getgrent()) != NULL)
	{
		groups.push_back(fillGroupInfo(info, in));
	}

	switch (errno)
	{
		case EIO:
		case EMFILE:
		case ENFILE:
		case EINTR:
		case ENOMEM:

			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETGROUPS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	endgrent();

	return groups;
}

//-------------------------------------------------------------------

void
os::die(const dodoString &message,
		int              status)
{
	fwrite(message.c_str(), message.size(), 1, stderr);
	fflush(stderr);

	kill(0, SIGTERM);

	wait(NULL);

	exit(status);
}

//-------------------------------------------------------------------

void
os::microSleep(unsigned long period)
{
	if (period < 1000000)
	{
		::usleep(period);
	}
	else
	{
		::sleep(period / 1000000);
	}
}

//-------------------------------------------------------------------

void
os::sleep(long period)
{
	::sleep(period);
}

//-------------------------------------------------------------------

void
os::atExit(void (*func)())
{
	if (atexit(func) != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_ATEXIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

int
os::getPID()
{
	return getpid();
}

//-------------------------------------------------------------------

int
os::getParentPID()
{
	return getppid();
}

//-------------------------------------------------------------------

int
os::getGroupPID()
{
	return getpgrp();
}

//-------------------------------------------------------------------

int
os::getGroupPID(int pid)
{
	int pgid = getpgid(pid);
	if (pgid == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETGROUPPID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	return pgid;
}

//-------------------------------------------------------------------

void
os::setGroupPID(int gpid)
{
	if (setpgid(0, gpid) == 1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETGROUPPID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
os::setGroupPID(int pid,
				int gpid)
{
	if (setpgid(pid, gpid) == 1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETGROUPPID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
os::sigMask(sigset_t *set,
			long     blockSignals)
{
	if (blockSignals != -1)
	{
		if (isSetFlag(blockSignals, OS_SIGNAL_HANGUP))
		{
			sigaddset(set, SIGHUP);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_INTERRUPT))
		{
			sigaddset(set, SIGINT);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_QUIT))
		{
			sigaddset(set, SIGQUIT);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_ILLEGAL_INSTRUCTION))
		{
			sigaddset(set, SIGILL);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_ABORT))
		{
			sigaddset(set, SIGABRT);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_BUS_FAULT))
		{
			sigaddset(set, SIGBUS);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_FLOATINGPOINT_FAULT))
		{
			sigaddset(set, SIGFPE);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_USER_DEFINED1))
		{
			sigaddset(set, SIGUSR1);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_SEGMENTATION_FAULT))
		{
			sigaddset(set, SIGSEGV);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_USER_DEFINED2))
		{
			sigaddset(set, SIGUSR2);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_PIPE_FAULT))
		{
			sigaddset(set, SIGPIPE);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_ALARM))
		{
			sigaddset(set, SIGALRM);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_TERMINATION))
		{
			sigaddset(set, SIGTERM);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_CHILD_CHANGED))
		{
			sigaddset(set, SIGCHLD);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_CONTINUE))
		{
			sigaddset(set, SIGCONT);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_KEYBOARD_STOP))
		{
			sigaddset(set, SIGTSTP);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_CPULIMIT_EXCEEDED))
		{
			sigaddset(set, SIGXCPU);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_FILESIZE_EXCEEDED))
		{
			sigaddset(set, SIGXFSZ);
		}

		if (isSetFlag(blockSignals, OS_SIGNAL_BAD_SYSCALL))
		{
			sigaddset(set, SIGSYS);
		}
	}
}

//-------------------------------------------------------------------

void
os::setSignalHandler(long          signal,
					 signalHandler handler,
					 int           blockSignals)
{
	syncThreadStack tg;

#ifdef DL_EXT
	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(signal);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
		{
			deinit();
		}

#ifndef DL_FAST
		dlclose(handlesSig[handleSignal]);
#endif

		handlesOpenedSig[handleSignal] = false;
		handlesSig[handleSignal] = NULL;
	}
#endif

	struct sigaction act;
	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	sigMask(&act.sa_mask, blockSignals);

	if (sigaction(os::toRealSignal(signal), &act, NULL) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
os::setMicroTimer(unsigned long timeout,
				  signalHandler handler,
				  int           blockSignals)
{
	syncThreadStack tg;

#ifdef DL_EXT
	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(OS_SIGNAL_ALARM);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
		{
			deinit();
		}

#ifndef DL_FAST
		dlclose(handlesSig[handleSignal]);
#endif

		handlesOpenedSig[handleSignal] = false;
		handlesSig[handleSignal] = NULL;
	}
#endif

	struct sigaction act;
	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETMICROTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	sigMask(&act.sa_mask, blockSignals);

	long tSec = 0, tMicrosec = timeout;

	if (timeout > 1000000)
	{
		tSec = timeout / 1000000;
		tMicrosec = timeout % 1000000;
	}

	itimerval value;
	value.it_interval.tv_sec = tSec;
	value.it_interval.tv_usec = tMicrosec;
	value.it_value.tv_sec = tSec;
	value.it_value.tv_usec = tMicrosec;

	if (sigaction(SIGALRM, &act, NULL) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETMICROTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	if (setitimer(ITIMER_REAL, &value, NULL) != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETMICROTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
os::setTimer(long          timeout,
			 signalHandler handler,
			 int           blockSignals)
{
	syncThreadStack tg;

#ifdef DL_EXT
	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(OS_SIGNAL_ALARM);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
		{
			deinit();
		}

#ifndef DL_FAST
		dlclose(handlesSig[handleSignal]);
#endif

		handlesOpenedSig[handleSignal] = false;
		handlesSig[handleSignal] = NULL;
	}
#endif

	struct sigaction act;
	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	sigMask(&act.sa_mask, blockSignals);

	itimerval value;
	value.it_interval.tv_sec = timeout;
	value.it_interval.tv_usec = 0;
	value.it_value.tv_sec = timeout;
	value.it_value.tv_usec = 0;

	if (sigaction(SIGALRM, &act, NULL) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	if (setitimer(ITIMER_REAL, &value, NULL) != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

bool
os::isSignalHandled(long signal)
{
	struct sigaction act;
	if (sigaction(os::toRealSignal(signal), NULL, &act) == 1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	if (act.sa_sigaction != NULL || act.sa_handler != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------

void
os::sendSignal(int  pid,
			   long signal)
{
	if (kill(pid, os::toRealSignal(signal)) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SENDSIGNAL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
os::unsetSignalHandler(long signal)
{
	syncThreadStack tg;

#ifdef DL_EXT
	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(signal);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
		{
			deinit();
		}

#ifndef DL_FAST
		dlclose(handlesSig[handleSignal]);
#endif

		handlesOpenedSig[handleSignal] = false;
		handlesSig[handleSignal] = NULL;
	}
#endif

	struct sigaction act;
	act.sa_sigaction = NULL;

	if (sigaction(os::toRealSignal(signal), &act, NULL) == 1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_UNSETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

#ifdef DL_EXT
__signalMod__
os::getModuleInfo(const dodoString &module,
				  void             *toInit)
{
#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
	}

	initOsSignalModule init = (initOsSignalModule)dlsym(handle, "initOsSignalModule");
	if (init == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
	}

	__signalMod__ mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
	}
#endif

	return mod;
}

//-------------------------------------------------------------------

void
os::setSignalHandler(const dodoString &path,
					 void             *toInit)
{
	syncThreadStack tg;

#ifdef DL_FAST
	void *handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(path.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	initOsSignalModule init = (initOsSignalModule)dlsym(handle, "initOsSignalModule");
	if (init == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	__signalMod__ mod = init(toInit);

	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(mod.signal);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
		{
			deinit();
		}

#ifndef DL_FAST
		dlclose(handlesSig[handleSignal]);
#endif

		handlesOpenedSig[handleSignal] = false;
		handlesSig[handleSignal] = NULL;
	}

	handlesSig[handleSignal] = handle;

	signalHandler in = (signalHandler)dlsym(handlesSig[mod.signal], mod.hook);
	if (in == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	struct sigaction act;
	act.sa_sigaction = in;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	sigMask(&act.sa_mask, mod.blockSignals);

	if (sigaction(os::toRealSignal(mod.signal), &act, NULL) == 1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	handlesOpenedSig[handleSignal] = true;
}
#endif

//-------------------------------------------------------------------

int
os::toSignalNumber(long signal)
{
	switch (signal)
	{
		case OS_SIGNAL_HANGUP:

			return 0;

		case OS_SIGNAL_INTERRUPT:

			return 1;

		case OS_SIGNAL_QUIT:

			return 2;

		case OS_SIGNAL_ILLEGAL_INSTRUCTION:

			return 3;

		case OS_SIGNAL_ABORT:

			return 4;

		case OS_SIGNAL_BUS_FAULT:

			return 5;

		case OS_SIGNAL_FLOATINGPOINT_FAULT:

			return 6;

		case OS_SIGNAL_USER_DEFINED1:

			return 7;

		case OS_SIGNAL_SEGMENTATION_FAULT:

			return 8;

		case OS_SIGNAL_USER_DEFINED2:

			return 9;

		case OS_SIGNAL_PIPE_FAULT:

			return 10;

		case OS_SIGNAL_ALARM:

			return 11;

		case OS_SIGNAL_TERMINATION:

			return 12;

		case OS_SIGNAL_CHILD_CHANGED:

			return 13;

		case OS_SIGNAL_CONTINUE:

			return 14;

		case OS_SIGNAL_KEYBOARD_STOP:

			return 15;

		case OS_SIGNAL_CPULIMIT_EXCEEDED:

			return 16;

		case OS_SIGNAL_FILESIZE_EXCEEDED:

			return 17;

		case OS_SIGNAL_BAD_SYSCALL:

			return 18;

		default:

			return -1;
	}
}

//-------------------------------------------------------------------

int
os::toRealSignal(long signal)
{
	switch (signal)
	{
		case OS_SIGNAL_HANGUP:

			return SIGHUP;

		case OS_SIGNAL_INTERRUPT:

			return SIGINT;

		case OS_SIGNAL_QUIT:

			return SIGQUIT;

		case OS_SIGNAL_ILLEGAL_INSTRUCTION:

			return SIGILL;

		case OS_SIGNAL_ABORT:

			return SIGABRT;

		case OS_SIGNAL_BUS_FAULT:

			return SIGBUS;

		case OS_SIGNAL_FLOATINGPOINT_FAULT:

			return SIGFPE;

		case OS_SIGNAL_USER_DEFINED1:

			return SIGUSR1;

		case OS_SIGNAL_SEGMENTATION_FAULT:

			return SIGSEGV;

		case OS_SIGNAL_USER_DEFINED2:

			return SIGUSR2;

		case OS_SIGNAL_PIPE_FAULT:

			return SIGPIPE;

		case OS_SIGNAL_ALARM:

			return SIGALRM;

		case OS_SIGNAL_TERMINATION:

			return SIGTERM;

		case OS_SIGNAL_CHILD_CHANGED:

			return SIGCHLD;

		case OS_SIGNAL_CONTINUE:

			return SIGCONT;

		case OS_SIGNAL_KEYBOARD_STOP:

			return SIGTSTP;

		case OS_SIGNAL_CPULIMIT_EXCEEDED:

			return SIGXCPU;

		case OS_SIGNAL_FILESIZE_EXCEEDED:

			return SIGXFSZ;

		case OS_SIGNAL_BAD_SYSCALL:

			return SIGSYS;

		default:

			return -1;
	}
}

//-------------------------------------------------------------------

void
os::blockSignal(long signals,
				bool block)
{
	sigset_t signal_mask;
	sigemptyset(&signal_mask);

	os::sigMask(&signal_mask, signals);

	if (block)
	{
		sigprocmask(SIG_BLOCK, &signal_mask, NULL);
	}
	else
	{
		sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
	}
}

//-------------------------------------------------------------------

void
os::becomeDaemon()
{
	pid_t pid = fork();

	if (pid == 0)
	{
		if (setsid() == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		if (close(0) == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
		if (close(1) == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
		if (close(2) == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		if (chdir("/") == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
	else
	{
		if (pid == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
		else
		{
			_exit(0);
		}
	}
}

//-------------------------------------------------------------------

void
os::releaseDaemon(daemon func,
				  void   *data)
{
	pid_t pid = fork();

	if (pid == 0)
	{
		if (setsid() == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		if (close(0) == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
		if (close(1) == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
		if (close(2) == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		if (chdir("/") == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		func(data);

		exit(0);
	}
	else
	{
		if (pid == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
		else
		{
			_exit(0);
		}
	}
}

//-------------------------------------------------------------------
