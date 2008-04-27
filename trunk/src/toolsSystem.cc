/***************************************************************************
 *            toolsSystem.cc
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

#include <libdodo/toolsSystem.h>

using namespace dodo;

#ifdef DL_EXT

void *toolsSystem::handlesSig[] = { NULL,
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
									NULL };

//-------------------------------------------------------------------

bool toolsSystem::handlesOpenedSig[] = { false,
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
										 false };

#endif

//-------------------------------------------------------------------

#ifdef PTHREAD_EXT

pthread_mutex_t toolsSystem::staticAtomicMutex::mutex;

#endif

//-------------------------------------------------------------------

toolsSystem::staticAtomicMutex toolsSystem::mutex;

//-------------------------------------------------------------------

toolsSystem::staticAtomicMutex::staticAtomicMutex()
{
#ifdef PTHREAD_EXT
	
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&mutex, &attr);

	pthread_mutexattr_destroy(&attr);
	
#endif
}

//-------------------------------------------------------------------

toolsSystem::staticAtomicMutex::~staticAtomicMutex()
{
#ifdef PTHREAD_EXT
	
	pthread_mutex_destroy(&mutex);
	
#endif
}

//-------------------------------------------------------------------

void
toolsSystem::staticAtomicMutex::lock()
{
#ifdef PTHREAD_EXT
	
	errno = pthread_mutex_lock(&mutex);
	if (errno != 0 && errno != EDEADLK)
		throw baseEx(ERRMODULE_SYSTEMTOOLSSYSTEMATOMICMUTEX, SYSTEMTOOLSSYSTEMATOMICMUTEXEX_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	
#endif
}

//-------------------------------------------------------------------

void
toolsSystem::staticAtomicMutex::unlock()
{
#ifdef PTHREAD_EXT
	
	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		throw baseEx(ERRMODULE_SYSTEMTOOLSSYSTEMATOMICMUTEX, SYSTEMTOOLSSYSTEMATOMICMUTEXEX_UNLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	
#endif
}

//-------------------------------------------------------------------

toolsSystem::systemRaceHazardGuard::systemRaceHazardGuard()
{
	mutex.lock();
}

//-------------------------------------------------------------------

toolsSystem::systemRaceHazardGuard::~systemRaceHazardGuard()
{
	mutex.unlock();
}

//-------------------------------------------------------------------

dodoString
toolsSystem::getWorkingDir()
{
	char wd[MAXPATHLEN];

	if (getcwd(wd, MAXPATHLEN) == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETWORKINGDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return wd;
}

//-------------------------------------------------------------------

void
toolsSystem::setWorkingDir(const dodoString &path)
{
	if (chdir(path.c_str()) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETWORKINGDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------


__usage
toolsSystem::getUsageInfo()
{
	rusage use;
	if (getrusage(RUSAGE_SELF, &use) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETUSAGEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__usage info;
	info.time = use.ru_utime.tv_sec * 100 + use.ru_utime.tv_usec;
	info.mem = use.ru_maxrss * 1024;

	return info;
}

//-------------------------------------------------------------------

void
toolsSystem::changeRoot(const dodoString &path)
{
	setWorkingDir(path);

	if (chroot(path.c_str()) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_CHANGEROOT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

__limits
toolsSystem::getLimit(short type)
{
	rlimit limit;

	int realRes(0);

	switch (type)
	{
		case SYSTEMTOOLS_CPUTIME:

			realRes = RLIMIT_CPU;

			break;

		case SYSTEMTOOLS_MAXFILESIZE:

			realRes = RLIMIT_FSIZE;

			break;

		case SYSTEMTOOLS_MAXMEMUSAGE:

			realRes = RLIMIT_DATA;

			break;

		case SYSTEMTOOLS_MAXSTACK:

			realRes = RLIMIT_STACK;

			break;

		case SYSTEMTOOLS_MAXPROC:

			realRes = RLIMIT_NPROC;

			break;

		case SYSTEMTOOLS_MAXOPENFILES:

#ifdef __FreeBSD__
			realRes = RLIMIT_NOFILE;
#else
			realRes = RLIMIT_OFILE;
#endif

			break;

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETLIMIT, ERR_LIBDODO, SYSTEMTOOLSEX_WRONGPARAMETER, SYSTEMTOOLSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (getrlimit(realRes, &limit) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETLIMIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__limits lim;

	lim.current = limit.rlim_cur;
	lim.max = limit.rlim_max;

	return lim;
}

//-------------------------------------------------------------------

void
toolsSystem::setLimit(short type,
					  const __limits &lim)
{
	rlimit limit;

	int realRes(0);

	switch (type)
	{
		case SYSTEMTOOLS_CPUTIME:

			realRes = RLIMIT_CPU;

			break;

		case SYSTEMTOOLS_MAXFILESIZE:

			realRes = RLIMIT_FSIZE;

			break;

		case SYSTEMTOOLS_MAXMEMUSAGE:

			realRes = RLIMIT_DATA;

			break;

		case SYSTEMTOOLS_MAXSTACK:

			realRes = RLIMIT_STACK;

			break;

		case SYSTEMTOOLS_MAXPROC:

			realRes = RLIMIT_NPROC;

			break;

		case SYSTEMTOOLS_MAXOPENFILES:

#ifdef __FreeBSD__
			realRes = RLIMIT_NOFILE;
#else
			realRes = RLIMIT_OFILE;
#endif

			break;

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETLIMIT, ERR_LIBDODO, SYSTEMTOOLSEX_WRONGPARAMETER, SYSTEMTOOLSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	limit.rlim_cur = lim.current;
	limit.rlim_max = lim.max;

	if (setrlimit(realRes, &limit) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETLIMIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
toolsSystem::getPriority(short type)
{
	int prio = getpriority(PRIO_PROCESS, getUID(type));
	if (prio == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETPRIORITY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return prio;
}

//-------------------------------------------------------------------

void
toolsSystem::setPriority(short type,
						 int prio)
{
	if (setpriority(PRIO_PROCESS, getUID(type), prio) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETPRIORITY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
toolsSystem::getUID(short type)
{
	switch (type)
	{
		case SYSTEMTOOLS_UID:

			return getuid();

		case SYSTEMTOOLS_EUID:

			return geteuid();

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETUID, ERR_LIBDODO, SYSTEMTOOLSEX_WRONGPARAMETER, SYSTEMTOOLSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
toolsSystem::setUID(short type,
					int uid)
{
	int res(0);

	switch (type)
	{
		case SYSTEMTOOLS_UID:

			res = setuid(uid);

			break;

		case SYSTEMTOOLS_EUID:

			res = seteuid(uid);

			break;

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETUID, ERR_LIBDODO, SYSTEMTOOLSEX_WRONGPARAMETER, SYSTEMTOOLSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (res == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETUID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
toolsSystem::getGID(short type)
{
	switch (type)
	{
		case SYSTEMTOOLS_UID:

			return getgid();

		case SYSTEMTOOLS_EUID:

			return getegid();

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETGID, ERR_LIBDODO, SYSTEMTOOLSEX_WRONGPARAMETER, SYSTEMTOOLSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
toolsSystem::setGID(short type,
					int uid)
{
	int res(0);

	switch (type)
	{
		case SYSTEMTOOLS_UID:

			res = setgid(uid);

			break;

		case SYSTEMTOOLS_EUID:

			res = setegid(uid);

			break;

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETGID, ERR_LIBDODO, SYSTEMTOOLSEX_WRONGPARAMETER, SYSTEMTOOLSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (res == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETGID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

__userInfo
toolsSystem::getUserInfo(int uid)
{
	passwd *in = getpwuid(uid);
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETUSERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__userInfo info;

	return fillUserInfo(info, in);
}

//-------------------------------------------------------------------

__userInfo
toolsSystem::getUserInfo(const dodoString &uid)
{
	passwd *in = getpwnam(uid.c_str());
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETUSERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__userInfo info;

	return fillUserInfo(info, in);
}

//-------------------------------------------------------------------

dodoArray<__userInfo>
toolsSystem::getUsers()
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

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETUSERS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	endpwent();

	return users;
}

//-------------------------------------------------------------------

__userInfo &
toolsSystem::fillUserInfo(__userInfo &info,
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
toolsSystem::fillGroupInfo(__groupInfo &info,
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
toolsSystem::getGroupInfo(int uid)
{
	group *in = getgrgid(uid);
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETGROUPINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__groupInfo info;

	return fillGroupInfo(info, in);
}

//-------------------------------------------------------------------

__groupInfo
toolsSystem::getGroupInfo(const dodoString &uid)
{
	group *in = getgrnam(uid.c_str());
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETGROUPINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__groupInfo info;

	return fillGroupInfo(info, in);
}

//-------------------------------------------------------------------

dodoArray<__groupInfo>
toolsSystem::getGroups()
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

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETGROUPS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	endgrent();

	return groups;
}

//-------------------------------------------------------------------

void
toolsSystem::die(const dodoString &message,
				 int status)
{
	fwrite(message.c_str(), message.size(), 1, stderr);
	fflush(stderr);

	_exit(status);
}

//-------------------------------------------------------------------

void
toolsSystem::microSleep(unsigned long period)
{
	if (period < 1000000)
		::usleep(period);
	else
		::sleep(period / 1000000);
}

//-------------------------------------------------------------------

void
toolsSystem::sleep(long period)
{
	::sleep(period);
}

//-------------------------------------------------------------------

void
toolsSystem::atExit(void (*func)())
{
	if (atexit(func) != 0)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_ATEXIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
toolsSystem::getPID()
{
	return getpid();
}

//-------------------------------------------------------------------

int
toolsSystem::getParentPID()
{
	return getppid();
}

//-------------------------------------------------------------------

int
toolsSystem::getGroupPID()
{
	return getpgrp();
}

//-------------------------------------------------------------------

int
toolsSystem::getGroupPID(int pid)
{
	int pgid = getpgid(pid);
	if (pgid == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETGROUPPID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return pgid;
}

//-------------------------------------------------------------------

void
toolsSystem::setGroupPID(int gpid)
{
	if (setpgid(0, gpid) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETGROUPPID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
toolsSystem::setGroupPID(int pid,
						 int gpid)
{
	if (setpgid(pid, gpid) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETGROUPPID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
toolsSystem::sigMask(sigset_t *set,
					 long blockSignals)
{
	if (blockSignals != -1)
	{
		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_HANGUP))
			sigaddset(set, SIGHUP);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_INTERRUPT))
			sigaddset(set, SIGINT);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_QUIT))
			sigaddset(set, SIGQUIT);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_ILLEGAL_INSTRUCTION))
			sigaddset(set, SIGILL);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_ABORT))
			sigaddset(set, SIGABRT);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_BUS_FAULT))
			sigaddset(set, SIGBUS);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_FLOATINGPOINT_FAULT))
			sigaddset(set, SIGFPE);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_USER_DEFINED1))
			sigaddset(set, SIGUSR1);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_SEGMENTATION_FAULT))
			sigaddset(set, SIGSEGV);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_USER_DEFINED2))
			sigaddset(set, SIGUSR2);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_PIPE_FAULT))
			sigaddset(set, SIGPIPE);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_ALARM))
			sigaddset(set, SIGALRM);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_TERMINATION))
			sigaddset(set, SIGTERM);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_CHILD_CHANGED))
			sigaddset(set, SIGCHLD);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_CONTINUE))
			sigaddset(set, SIGCONT);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_KEYBOARD_STOP))
			sigaddset(set, SIGTSTP);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_CPULIMIT_EXCEEDED))
			sigaddset(set, SIGXCPU);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_FILESIZE_EXCEEDED))
			sigaddset(set, SIGXFSZ);

		if (isSetFlag(blockSignals, SYSTEMTOOLS_SIGNAL_BAD_SYSCALL))
			sigaddset(set, SIGSYS);
	}
}

//-------------------------------------------------------------------

void
toolsSystem::setSignalHandler(long signal,
							  signalHandler handler,
							  int blockSignals)
{
	systemRaceHazardGuard tg;
	
#ifdef DL_EXT

	deinitSigModule deinit;

	int handleSignal = toSignalNumber(signal);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[handleSignal], "deinitSigModule");
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
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMask(&act.sa_mask, blockSignals);

	if (sigaction(toolsSystem::toRealSignal(signal), &act, NULL) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
toolsSystem::setMicroTimer(unsigned long timeout,
						   signalHandler handler,
						   int blockSignals)
{
	systemRaceHazardGuard tg;
	
#ifdef DL_EXT

	deinitSigModule deinit;

	int handleSignal = toSignalNumber(SYSTEMTOOLS_SIGNAL_ALARM);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[handleSignal], "deinitSigModule");
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
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETMICROTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETMICROTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (setitimer(ITIMER_REAL, &value, NULL) != 0)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETMICROTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
toolsSystem::setTimer(long timeout,
					  signalHandler handler,
					  int blockSignals)
{
	systemRaceHazardGuard tg;
	
#ifdef DL_EXT

	deinitSigModule deinit;

	int handleSignal = toSignalNumber(SYSTEMTOOLS_SIGNAL_ALARM);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[handleSignal], "deinitSigModule");
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
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMask(&act.sa_mask, blockSignals);

	itimerval value;
	value.it_interval.tv_sec = timeout;
	value.it_interval.tv_usec = 0;
	value.it_value.tv_sec = timeout;
	value.it_value.tv_usec = 0;

	if (sigaction(SIGALRM, &act, NULL) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (setitimer(ITIMER_REAL, &value, NULL) != 0)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
toolsSystem::isSignalHandled(long signal)
{
	struct sigaction act;
	if (sigaction(toolsSystem::toRealSignal(signal), NULL, &act) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (act.sa_sigaction != NULL || act.sa_handler != NULL)
		return true;
	else
		return false;
}

//-------------------------------------------------------------------

void
toolsSystem::sendSignal(int pid,
						long signal)
{
	if (kill(pid, toolsSystem::toRealSignal(signal)) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SENDSIGNAL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
toolsSystem::unsetSignalHandler(long signal)
{
	systemRaceHazardGuard tg;
	
#ifdef DL_EXT

	deinitSigModule deinit;

	int handleSignal = toSignalNumber(signal);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[handleSignal], "deinitSigModule");
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

	if (sigaction(toolsSystem::toRealSignal(signal), &act, NULL) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_UNSETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

__sigMod
toolsSystem::getModuleInfo(const dodoString &module,
						   void             *toInit)
{
#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY|RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initSigModule init = (initSigModule)dlsym(handle, "initSigModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__sigMod mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif

	return mod;
}

//-------------------------------------------------------------------

void
toolsSystem::setSignalHandler(const dodoString &path,
							  void             *toInit)
{
	systemRaceHazardGuard tg;
	
#ifdef DL_FAST
	void *handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_NODELETE);
#else
	void *handle = dlopen(path.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	initSigModule init = (initSigModule)dlsym(handle, "initSigModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	__sigMod mod = init(toInit);

	deinitSigModule deinit;

	int handleSignal = toSignalNumber(mod.signal);
	if (handleSignal > 0 && handlesOpenedSig[handleSignal])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[handleSignal], "deinitSigModule");
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
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	struct sigaction act;
	act.sa_sigaction = in;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMask(&act.sa_mask, mod.blockSignals);

	if (sigaction(toolsSystem::toRealSignal(mod.signal), &act, NULL) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	handlesOpenedSig[handleSignal] = true;
}

#endif

//-------------------------------------------------------------------

int
toolsSystem::toSignalNumber(long signal)
{
	switch (signal)
	{
		case SYSTEMTOOLS_SIGNAL_HANGUP:

			return 0;

		case SYSTEMTOOLS_SIGNAL_INTERRUPT:

			return 1;

		case SYSTEMTOOLS_SIGNAL_QUIT:

			return 2;

		case SYSTEMTOOLS_SIGNAL_ILLEGAL_INSTRUCTION:

			return 3;

		case SYSTEMTOOLS_SIGNAL_ABORT:

			return 4;

		case SYSTEMTOOLS_SIGNAL_BUS_FAULT:

			return 5;

		case SYSTEMTOOLS_SIGNAL_FLOATINGPOINT_FAULT:

			return 6;

		case SYSTEMTOOLS_SIGNAL_USER_DEFINED1:

			return 7;

		case SYSTEMTOOLS_SIGNAL_SEGMENTATION_FAULT:

			return 8;

		case SYSTEMTOOLS_SIGNAL_USER_DEFINED2:

			return 9;

		case SYSTEMTOOLS_SIGNAL_PIPE_FAULT:

			return 10;

		case SYSTEMTOOLS_SIGNAL_ALARM:

			return 11;

		case SYSTEMTOOLS_SIGNAL_TERMINATION:

			return 12;

		case SYSTEMTOOLS_SIGNAL_CHILD_CHANGED:

			return 13;

		case SYSTEMTOOLS_SIGNAL_CONTINUE:

			return 14;

		case SYSTEMTOOLS_SIGNAL_KEYBOARD_STOP:

			return 15;

		case SYSTEMTOOLS_SIGNAL_CPULIMIT_EXCEEDED:

			return 16;

		case SYSTEMTOOLS_SIGNAL_FILESIZE_EXCEEDED:

			return 17;

		case SYSTEMTOOLS_SIGNAL_BAD_SYSCALL:

			return 18;

		default:

			return -1;
	}
}

//-------------------------------------------------------------------

int
toolsSystem::toRealSignal(long signal)
{
	switch (signal)
	{
		case SYSTEMTOOLS_SIGNAL_HANGUP:

			return SIGHUP;

		case SYSTEMTOOLS_SIGNAL_INTERRUPT:

			return SIGINT;

		case SYSTEMTOOLS_SIGNAL_QUIT:

			return SIGQUIT;

		case SYSTEMTOOLS_SIGNAL_ILLEGAL_INSTRUCTION:

			return SIGILL;

		case SYSTEMTOOLS_SIGNAL_ABORT:

			return SIGABRT;

		case SYSTEMTOOLS_SIGNAL_BUS_FAULT:

			return SIGBUS;

		case SYSTEMTOOLS_SIGNAL_FLOATINGPOINT_FAULT:

			return SIGFPE;

		case SYSTEMTOOLS_SIGNAL_USER_DEFINED1:

			return SIGUSR1;

		case SYSTEMTOOLS_SIGNAL_SEGMENTATION_FAULT:

			return SIGSEGV;

		case SYSTEMTOOLS_SIGNAL_USER_DEFINED2:

			return SIGUSR2;

		case SYSTEMTOOLS_SIGNAL_PIPE_FAULT:

			return SIGPIPE;

		case SYSTEMTOOLS_SIGNAL_ALARM:

			return SIGALRM;

		case SYSTEMTOOLS_SIGNAL_TERMINATION:

			return SIGTERM;

		case SYSTEMTOOLS_SIGNAL_CHILD_CHANGED:

			return SIGCHLD;

		case SYSTEMTOOLS_SIGNAL_CONTINUE:

			return SIGCONT;

		case SYSTEMTOOLS_SIGNAL_KEYBOARD_STOP:

			return SIGTSTP;

		case SYSTEMTOOLS_SIGNAL_CPULIMIT_EXCEEDED:

			return SIGXCPU;

		case SYSTEMTOOLS_SIGNAL_FILESIZE_EXCEEDED:

			return SIGXFSZ;

		case SYSTEMTOOLS_SIGNAL_BAD_SYSCALL:

			return SIGSYS;

		default:

			return -1;
	}
}

//-------------------------------------------------------------------

void
toolsSystem::blockSignal(long signals,
						 bool block)
{
	sigset_t signal_mask;
	sigemptyset(&signal_mask);

	toolsSystem::sigMask(&signal_mask, signals);

	if (block)
		sigprocmask(SIG_BLOCK, &signal_mask, NULL);
	else
		sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
}

//-------------------------------------------------------------------

void
toolsSystem::daemonize()
{
	pid_t pid = fork();

	if (pid == 0)
	{
		int tty = open("/dev/tty", O_RDWR);
		if (tty == -1)
			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		
		if (ioctl(tty, TIOCNOTTY, (char *) 0) == -1)
			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		
		if (close(tty) == -1)
			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		
		if (close(0) == -1)
			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		if (close(1) == -1)
			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		if (close(2) == -1)
			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		if (pid == -1)
			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLSEX_DAEMONIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		else
			_exit(0);
	}
}

//-------------------------------------------------------------------

