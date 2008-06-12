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

#include <libdodo/toolsOs.h>

using namespace dodo::tools;

#ifdef DL_EXT

void *os::handlesSig[] = {
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

bool os::handlesOpenedSig[] = {
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

#ifdef PTHREAD_EXT

pthread_mutex_t os::staticAtomicMutex::keeper;

#endif

//-------------------------------------------------------------------

os::staticAtomicMutex os::keeper;

//-------------------------------------------------------------------

os::staticAtomicMutex::staticAtomicMutex()
{
#ifdef PTHREAD_EXT

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&keeper, &attr);

	pthread_mutexattr_destroy(&attr);

#endif
}

//-------------------------------------------------------------------

os::staticAtomicMutex::~staticAtomicMutex()
{
#ifdef PTHREAD_EXT

	pthread_mutex_destroy(&keeper);

#endif
}

//-------------------------------------------------------------------

void
os::staticAtomicMutex::acquire()
{
#ifdef PTHREAD_EXT

	errno = pthread_mutex_lock(&keeper);
	if (errno != 0 && errno != EDEADLK)
		throw baseEx(ERRMODULE_TOOLSOSSTATICATOMICMUTEX, SYSTEMSTATICATOMICMUTEXEX_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

void
os::staticAtomicMutex::release()
{
#ifdef PTHREAD_EXT

	errno = pthread_mutex_unlock(&keeper);
	if (errno != 0)
		throw baseEx(ERRMODULE_TOOLSOSSTATICATOMICMUTEX, SYSTEMSTATICATOMICMUTEXEX_UNLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

os::raceHazardGuard::raceHazardGuard()
{
	keeper.acquire();
}

//-------------------------------------------------------------------

os::raceHazardGuard::~raceHazardGuard()
{
	keeper.release();
}

//-------------------------------------------------------------------

dodoString
os::getWorkingDir()
{
	char wd[MAXPATHLEN];

	if (getcwd(wd, MAXPATHLEN) == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETWORKINGDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return wd;
}

//-------------------------------------------------------------------

void
os::setWorkingDir(const dodoString &path)
{
	if (chdir(path.c_str()) == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETWORKINGDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------


__usage
os::getUsageInfo()
{
	rusage use;
	if (getrusage(RUSAGE_SELF, &use) == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETUSAGEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__usage info;
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
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_CHANGEROOT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

__limits
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

#ifdef __FreeBSD__
			realRes = RLIMIT_NOFILE;
#else
			realRes = RLIMIT_OFILE;
#endif

			break;

		default:

			throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETLIMIT, ERR_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (getrlimit(realRes, &limit) == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETLIMIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__limits lim;

	lim.current = limit.rlim_cur;
	lim.max = limit.rlim_max;

	return lim;
}

//-------------------------------------------------------------------

void
os::setLimit(short type,
	     const __limits &lim)
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

#ifdef __FreeBSD__
			realRes = RLIMIT_NOFILE;
#else
			realRes = RLIMIT_OFILE;
#endif

			break;

		default:

			throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETLIMIT, ERR_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	limit.rlim_cur = lim.current;
	limit.rlim_max = lim.max;

	if (setrlimit(realRes, &limit) == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETLIMIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
os::getPriority(short type)
{
	int prio = getpriority(PRIO_PROCESS, getUID(type));
	if (prio == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETPRIORITY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return prio;
}

//-------------------------------------------------------------------

void
os::setPriority(short type,
		int prio)
{
	if (setpriority(PRIO_PROCESS, getUID(type), prio) == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETPRIORITY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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

			throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETUID, ERR_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
os::setUID(short type,
	   int uid)
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

			throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETUID, ERR_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (res == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETUID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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

			throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETGID, ERR_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
os::setGID(short type,
	   int uid)
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

			throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETGID, ERR_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (res == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETGID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

__userInfo
os::getUserInfo(int uid)
{
	passwd *in = getpwuid(uid);
	if (in == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETUSERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__userInfo info;

	return fillUserInfo(info, in);
}

//-------------------------------------------------------------------

__userInfo
os::getUserInfo(const dodoString &uid)
{
	passwd *in = getpwnam(uid.c_str());
	if (in == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETUSERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__userInfo info;

	return fillUserInfo(info, in);
}

//-------------------------------------------------------------------

dodoArray<__userInfo>
os::getUsers()
{
	dodoArray<__userInfo> users;

	passwd *in;

	__userInfo info;

	while ((in = getpwent()) != NULL)
		users.push_back(fillUserInfo(info, in));

	switch (errno)
	{
		case EIO:
		case EMFILE:
		case ENFILE:
		case ENOMEM:

			throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETUSERS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	endpwent();

	return users;
}

//-------------------------------------------------------------------

__userInfo &
os::fillUserInfo(__userInfo &info,
		 passwd     *in)
{
	info.gid = in->pw_gid;
	info.home = in->pw_dir;
	info.name = in->pw_name;
	info.pass = in->pw_passwd;
	info.realName = in->pw_gecos;
	info.shell = in->pw_shell;
	info.uid = in->pw_uid;

	return info;
}

//-------------------------------------------------------------------

__groupInfo &
os::fillGroupInfo(__groupInfo &info,
		  group       *pw)
{
	info.gid = pw->gr_gid;
	info.name = pw->gr_name;

	info.members.clear();

	int i(0);

	while (pw->gr_mem[i] != NULL)
		info.members.push_back(pw->gr_mem[i++]);

	return info;
}

//-------------------------------------------------------------------

__groupInfo
os::getGroupInfo(int uid)
{
	group *in = getgrgid(uid);
	if (in == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETGROUPINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__groupInfo info;

	return fillGroupInfo(info, in);
}

//-------------------------------------------------------------------

__groupInfo
os::getGroupInfo(const dodoString &uid)
{
	group *in = getgrnam(uid.c_str());
	if (in == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETGROUPINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__groupInfo info;

	return fillGroupInfo(info, in);
}

//-------------------------------------------------------------------

dodoArray<__groupInfo>
os::getGroups()
{
	dodoArray<__groupInfo> groups;

	group *in;

	__groupInfo info;

	while ((in = getgrent()) != NULL)
		groups.push_back(fillGroupInfo(info, in));

	switch (errno)
	{
		case EIO:
		case EMFILE:
		case ENFILE:
		case EINTR:
		case ENOMEM:

			throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETGROUPS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	endgrent();

	return groups;
}

//-------------------------------------------------------------------

void
os::die(const dodoString &message,
	int status)
{
	fwrite(message.c_str(), message.size(), 1, stderr);
	fflush(stderr);

	_exit(status);
}

//-------------------------------------------------------------------

void
os::microSleep(unsigned long period)
{
	if (period < 1000000)
		::usleep(period);
	else
		::sleep(period / 1000000);
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
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_ATEXIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETGROUPPID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return pgid;
}

//-------------------------------------------------------------------

void
os::setGroupPID(int gpid)
{
	if (setpgid(0, gpid) == 1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETGROUPPID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::setGroupPID(int pid,
		int gpid)
{
	if (setpgid(pid, gpid) == 1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETGROUPPID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::sigMask(sigset_t *set,
	    long blockSignals)
{
	if (blockSignals != -1)
	{
		if (isSetFlag(blockSignals, OS_SIGNAL_HANGUP))
			sigaddset(set, SIGHUP);

		if (isSetFlag(blockSignals, OS_SIGNAL_INTERRUPT))
			sigaddset(set, SIGINT);

		if (isSetFlag(blockSignals, OS_SIGNAL_QUIT))
			sigaddset(set, SIGQUIT);

		if (isSetFlag(blockSignals, OS_SIGNAL_ILLEGAL_INSTRUCTION))
			sigaddset(set, SIGILL);

		if (isSetFlag(blockSignals, OS_SIGNAL_ABORT))
			sigaddset(set, SIGABRT);

		if (isSetFlag(blockSignals, OS_SIGNAL_BUS_FAULT))
			sigaddset(set, SIGBUS);

		if (isSetFlag(blockSignals, OS_SIGNAL_FLOATINGPOINT_FAULT))
			sigaddset(set, SIGFPE);

		if (isSetFlag(blockSignals, OS_SIGNAL_USER_DEFINED1))
			sigaddset(set, SIGUSR1);

		if (isSetFlag(blockSignals, OS_SIGNAL_SEGMENTATION_FAULT))
			sigaddset(set, SIGSEGV);

		if (isSetFlag(blockSignals, OS_SIGNAL_USER_DEFINED2))
			sigaddset(set, SIGUSR2);

		if (isSetFlag(blockSignals, OS_SIGNAL_PIPE_FAULT))
			sigaddset(set, SIGPIPE);

		if (isSetFlag(blockSignals, OS_SIGNAL_ALARM))
			sigaddset(set, SIGALRM);

		if (isSetFlag(blockSignals, OS_SIGNAL_TERMINATION))
			sigaddset(set, SIGTERM);

		if (isSetFlag(blockSignals, OS_SIGNAL_CHILD_CHANGED))
			sigaddset(set, SIGCHLD);

		if (isSetFlag(blockSignals, OS_SIGNAL_CONTINUE))
			sigaddset(set, SIGCONT);

		if (isSetFlag(blockSignals, OS_SIGNAL_KEYBOARD_STOP))
			sigaddset(set, SIGTSTP);

		if (isSetFlag(blockSignals, OS_SIGNAL_CPULIMIT_EXCEEDED))
			sigaddset(set, SIGXCPU);

		if (isSetFlag(blockSignals, OS_SIGNAL_FILESIZE_EXCEEDED))
			sigaddset(set, SIGXFSZ);

		if (isSetFlag(blockSignals, OS_SIGNAL_BAD_SYSCALL))
			sigaddset(set, SIGSYS);
	}
}

//-------------------------------------------------------------------

void
os::setSignalHandler(long signal,
		     signalHandler handler,
		     int blockSignals)
{
	raceHazardGuard tg;

#ifdef DL_EXT

	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(signal);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
			deinit();

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
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMask(&act.sa_mask, blockSignals);

	if (sigaction(os::toRealSignal(signal), &act, NULL) == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::setMicroTimer(unsigned long timeout,
		  signalHandler handler,
		  int blockSignals)
{
	raceHazardGuard tg;

#ifdef DL_EXT

	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(OS_SIGNAL_ALARM);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
			deinit();

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
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETMICROTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETMICROTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (setitimer(ITIMER_REAL, &value, NULL) != 0)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETMICROTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::setTimer(long timeout,
	     signalHandler handler,
	     int blockSignals)
{
	raceHazardGuard tg;

#ifdef DL_EXT

	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(OS_SIGNAL_ALARM);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
			deinit();

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
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMask(&act.sa_mask, blockSignals);

	itimerval value;
	value.it_interval.tv_sec = timeout;
	value.it_interval.tv_usec = 0;
	value.it_value.tv_sec = timeout;
	value.it_value.tv_usec = 0;

	if (sigaction(SIGALRM, &act, NULL) == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (setitimer(ITIMER_REAL, &value, NULL) != 0)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
os::isSignalHandled(long signal)
{
	struct sigaction act;
	if (sigaction(os::toRealSignal(signal), NULL, &act) == 1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (act.sa_sigaction != NULL || act.sa_handler != NULL)
		return true;
	else
		return false;
}

//-------------------------------------------------------------------

void
os::sendSignal(int pid,
	       long signal)
{
	if (kill(pid, os::toRealSignal(signal)) == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SENDSIGNAL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::unsetSignalHandler(long signal)
{
	raceHazardGuard tg;

#ifdef DL_EXT

	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(signal);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
			deinit();

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
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_UNSETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

__signalMod
os::getModuleInfo(const dodoString &module,
		  void             *toInit)
{
#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initOsSignalModule init = (initOsSignalModule)dlsym(handle, "initOsSignalModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__signalMod mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif

	return mod;
}

//-------------------------------------------------------------------

void
os::setSignalHandler(const dodoString &path,
		     void             *toInit)
{
	raceHazardGuard tg;

#ifdef DL_FAST
	void *handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(path.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	initOsSignalModule init = (initOsSignalModule)dlsym(handle, "initOsSignalModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__signalMod mod = init(toInit);

	deinitOsSignalModule deinit;

	int handleSignal = toSignalNumber(mod.signal);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitOsSignalModule)dlsym(handlesSig[handleSignal], "deinitOsSignalModule");
		if (deinit != NULL)
			deinit();

#ifndef DL_FAST
		dlclose(handlesSig[handleSignal]);
#endif

		handlesOpenedSig[handleSignal] = false;
		handlesSig[handleSignal] = NULL;
	}

	handlesSig[handleSignal] = handle;

	signalHandler in = (signalHandler)dlsym(handlesSig[mod.signal], mod.hook);
	if (in == NULL)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	struct sigaction act;
	act.sa_sigaction = in;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMask(&act.sa_mask, mod.blockSignals);

	if (sigaction(os::toRealSignal(mod.signal), &act, NULL) == 1)
		throw baseEx(ERRMODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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
		sigprocmask(SIG_BLOCK, &signal_mask, NULL);
	else
		sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
}

//-------------------------------------------------------------------

void
os::daemonize()
{
	pid_t pid = fork();

	if (pid == 0)
	{
		int tty = open("/dev/tty", O_RDWR);
		if (tty == -1)
			throw baseEx(ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (ioctl(tty, TIOCNOTTY, (char *)0) == -1)
			throw baseEx(ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (close(tty) == -1)
			throw baseEx(ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (close(0) == -1)
			throw baseEx(ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		if (close(1) == -1)
			throw baseEx(ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		if (close(2) == -1)
			throw baseEx(ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		if (pid == -1)
			throw baseEx(ERRMODULE_TOOLSOS, OSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		else
			_exit(0);
	}
}

//-------------------------------------------------------------------

