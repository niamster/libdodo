/***************************************************************************
 *            systemTools.cc
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

#include <systemTools.h>

using namespace dodo;

#ifdef DL_EXT

void *systemTools::handlesSig[] = { NULL,
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
									NULL };

//-------------------------------------------------------------------

bool systemTools::handlesOpenedSig[] = { false,
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
										 false };

#endif

//-------------------------------------------------------------------

dodoString
systemTools::getWorkingDir()
{
	char wd[MAXPATHLEN];

	if (getcwd(wd, MAXPATHLEN) == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETWORKINGDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return wd;
}

//-------------------------------------------------------------------

void
systemTools::setWorkingDir(const dodoString &path)
{
	if (chdir(path.c_str()) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETWORKINGDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------


void
systemTools::getUsageInfo(__usage &info)
{
	rusage use;
	if (getrusage(RUSAGE_SELF, &use) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETUSAGEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	info.time = use.ru_utime.tv_sec * 100 + use.ru_utime.tv_usec;
	info.mem = use.ru_maxrss * 1024;
}

//-------------------------------------------------------------------

void
systemTools::changeRoot(const dodoString &path)
{
	setWorkingDir(path);

	if (chroot(path.c_str()) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_CHANGEROOT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
systemTools::getLimit(short type,
					  __limits &lim)
{
	rlimit limit;

	int realRes(0);

	switch (type)
	{
		case SYSTEM_CPUTIME:

			realRes = RLIMIT_CPU;

			break;

		case SYSTEM_MAXFILESIZE:

			realRes = RLIMIT_FSIZE;

			break;

		case SYSTEM_MAXMEMUSAGE:

			realRes = RLIMIT_DATA;

			break;

		case SYSTEM_MAXSTACK:

			realRes = RLIMIT_STACK;

			break;

		case SYSTEM_MAXPROC:

			realRes = RLIMIT_NPROC;

			break;

		case SYSTEM_MAXOPENFILES:

            #ifdef __FreeBSD__
			realRes = RLIMIT_NOFILE;
            #else
			realRes = RLIMIT_OFILE;
            #endif

			break;

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETLIMIT, ERR_LIBDODO, SYSTEMTOOLS_WRONG_PARAMETHER, SYSTEMTOOLS_WRONG_PARAMETHER_STR, __LINE__, __FILE__);
	}

	if (getrlimit(realRes, &limit) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETLIMIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	lim.current = limit.rlim_cur;
	lim.max = limit.rlim_max;
}

//-------------------------------------------------------------------

void
systemTools::setLimit(short type,
					  const __limits &lim)
{
	rlimit limit;

	int realRes(0);

	switch (type)
	{
		case SYSTEM_CPUTIME:

			realRes = RLIMIT_CPU;

			break;

		case SYSTEM_MAXFILESIZE:

			realRes = RLIMIT_FSIZE;

			break;

		case SYSTEM_MAXMEMUSAGE:

			realRes = RLIMIT_DATA;

			break;

		case SYSTEM_MAXSTACK:

			realRes = RLIMIT_STACK;

			break;

		case SYSTEM_MAXPROC:

			realRes = RLIMIT_NPROC;

			break;

		case SYSTEM_MAXOPENFILES:

            #ifdef __FreeBSD__
			realRes = RLIMIT_NOFILE;
            #else
			realRes = RLIMIT_OFILE;
            #endif

			break;

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETLIMIT, ERR_LIBDODO, SYSTEMTOOLS_WRONG_PARAMETHER, SYSTEMTOOLS_WRONG_PARAMETHER_STR, __LINE__, __FILE__);
	}

	limit.rlim_cur = lim.current;
	limit.rlim_max = lim.max;

	if (setrlimit(realRes, &limit) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETLIMIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
systemTools::getPriority(short type)
{
	int prio = getpriority(PRIO_PROCESS, getUID(type));
	if (prio == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETPRIORITY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return prio;
}

//-------------------------------------------------------------------

void
systemTools::setPriority(short type,
						 int prio)
{
	if (setpriority(PRIO_PROCESS, getUID(type), prio) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETPRIORITY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
systemTools::getUID(short type)
{
	switch (type)
	{
		case SYSTEM_UID:

			return getuid();

		case SYSTEM_EUID:

			return geteuid();

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETUID, ERR_LIBDODO, SYSTEMTOOLS_WRONG_PARAMETHER, SYSTEMTOOLS_WRONG_PARAMETHER_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
systemTools::setUID(short type,
					int uid)
{
	int res(0);

	switch (type)
	{
		case SYSTEM_UID:

			res = setuid(uid);

			break;

		case SYSTEM_EUID:

			res = seteuid(uid);

			break;

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETUID, ERR_LIBDODO, SYSTEMTOOLS_WRONG_PARAMETHER, SYSTEMTOOLS_WRONG_PARAMETHER_STR, __LINE__, __FILE__);
	}

	if (res == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETUID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
systemTools::getGID(short type)
{
	switch (type)
	{
		case SYSTEM_UID:

			return getgid();

		case SYSTEM_EUID:

			return getegid();

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETGID, ERR_LIBDODO, SYSTEMTOOLS_WRONG_PARAMETHER, SYSTEMTOOLS_WRONG_PARAMETHER_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
systemTools::setGID(short type,
					int uid)
{
	int res(0);

	switch (type)
	{
		case SYSTEM_UID:

			res = setgid(uid);

			break;

		case SYSTEM_EUID:

			res = setegid(uid);

			break;

		default:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETGID, ERR_LIBDODO, SYSTEMTOOLS_WRONG_PARAMETHER, SYSTEMTOOLS_WRONG_PARAMETHER_STR, __LINE__, __FILE__);
	}

	if (res == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETGID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemTools::getUserInfo(__userInfo &info,
						 int uid)
{
	passwd *in = getpwuid(uid);
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETUSERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	fillUserInfo(info, in);

}

//-------------------------------------------------------------------

void
systemTools::getUserInfo(__userInfo &info,
						 const dodoString &uid)
{
	passwd *in = getpwnam(uid.c_str());
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETUSERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	fillUserInfo(info, in);
}

//-------------------------------------------------------------------


void
systemTools::getUsers(dodoArray<__userInfo> &users)
{
	users.clear();

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

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETUSERS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	endpwent();
}

//-------------------------------------------------------------------

__userInfo &
systemTools::fillUserInfo(__userInfo &info,
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
systemTools::fillGroupInfo(__groupInfo &info,
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

void
systemTools::getGroupInfo(__groupInfo &info,
						  int uid)
{
	group *in = getgrgid(uid);
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETGROUPINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	fillGroupInfo(info, in);
}

//-------------------------------------------------------------------

void
systemTools::getGroupInfo(__groupInfo &info,
						  const dodoString &uid)
{
	group *in = getgrnam(uid.c_str());
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETGROUPINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	fillGroupInfo(info, in);
}

//-------------------------------------------------------------------


void
systemTools::getGroups(dodoArray<__groupInfo> &users)
{
	users.clear();

	group *in;

	__groupInfo info;

	while ((in = getgrent()) != NULL)
		users.push_back(fillGroupInfo(info, in));

	switch (errno)
	{
		case EIO:
		case EMFILE:
		case ENFILE:
		case EINTR:
		case ENOMEM:

			throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETGROUPS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	endgrent();
}

//-------------------------------------------------------------------

void
systemTools::die(const dodoString &message,
				 int status)
{
	fwrite(message.c_str(), message.size(), 1, stderr);
	fflush(stderr);

	_exit(status);
}

//-------------------------------------------------------------------

void
systemTools::microSleep(unsigned long period)
{
	if (period < 1000000)
		::usleep(period);
	else
		::sleep(period / 1000000);
}

//-------------------------------------------------------------------

void
systemTools::sleep(long period)
{
	::sleep(period);
}

//-------------------------------------------------------------------

void
systemTools::atExit(void (*func)())
{
	if (atexit(func) != 0)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_ATEXIT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
systemTools::getPID()
{
	return getpid();
}

//-------------------------------------------------------------------

int
systemTools::getParentPID()
{
	return getppid();
}

//-------------------------------------------------------------------

int
systemTools::getGroupPID()
{
	return getpgrp();
}

//-------------------------------------------------------------------

int
systemTools::getGroupPID(int pid)
{
	int pgid = getpgid(pid);
	if (pgid == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETGROUPPID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return pgid;
}

//-------------------------------------------------------------------

void
systemTools::setGroupPID(int gpid)
{
	if (setpgid(0, gpid) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETGROUPPID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}


//-------------------------------------------------------------------

void
systemTools::setGroupPID(int pid,
						 int gpid)
{
	if (setpgid(pid, gpid) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETGROUPPID, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemTools::sigMask(sigset_t *set,
					 long blockSignals)
{
	if (blockSignals != -1)
	{
		if (isSetFlag(blockSignals, SIGNAL_HANGUP))
			sigaddset(set, SIGHUP);

		if (isSetFlag(blockSignals, SIGNAL_INTERRUPT))
			sigaddset(set, SIGINT);

		if (isSetFlag(blockSignals, SIGNAL_QUIT))
			sigaddset(set, SIGQUIT);

		if (isSetFlag(blockSignals, SIGNAL_ILLEGAL_INSTRUCTION))
			sigaddset(set, SIGILL);

		if (isSetFlag(blockSignals, SIGNAL_ABORT))
			sigaddset(set, SIGABRT);

		if (isSetFlag(blockSignals, SIGNAL_BUS_FAULT))
			sigaddset(set, SIGBUS);

		if (isSetFlag(blockSignals, SIGNAL_FLOATINGPOINT_FAULT))
			sigaddset(set, SIGFPE);

		if (isSetFlag(blockSignals, SIGNAL_USER_DEFINED1))
			sigaddset(set, SIGUSR1);

		if (isSetFlag(blockSignals, SIGNAL_SEGMENTATION_FAULT))
			sigaddset(set, SIGSEGV);

		if (isSetFlag(blockSignals, SIGNAL_USER_DEFINED2))
			sigaddset(set, SIGUSR2);

		if (isSetFlag(blockSignals, SIGNAL_PIPE_FAULT))
			sigaddset(set, SIGPIPE);

		if (isSetFlag(blockSignals, SIGNAL_ALARM))
			sigaddset(set, SIGALRM);

		if (isSetFlag(blockSignals, SIGNAL_TERMINATION))
			sigaddset(set, SIGTERM);

        #ifndef __FreeBSD__

		if (isSetFlag(blockSignals, SIGNAL_STACK_FAULT))
			sigaddset(set, SIGSTKFLT);

        #endif

		if (isSetFlag(blockSignals, SIGNAL_CHILD_CHANGED))
			sigaddset(set, SIGCHLD);

		if (isSetFlag(blockSignals, SIGNAL_CONTINUE))
			sigaddset(set, SIGCONT);

		if (isSetFlag(blockSignals, SIGNAL_KEYBOARD_STOP))
			sigaddset(set, SIGTSTP);

		if (isSetFlag(blockSignals, SIGNAL_CPULIMIT_EXCEEDED))
			sigaddset(set, SIGXCPU);

		if (isSetFlag(blockSignals, SIGNAL_FILESIZE_EXCEEDED))
			sigaddset(set, SIGXFSZ);

		if (isSetFlag(blockSignals, SIGNAL_BAD_SYSCALL))
			sigaddset(set, SIGSYS);
	}
}

//-------------------------------------------------------------------

void
systemTools::setSignalHandler(long signal,
							  signalHandler handler,
							  int blockSignals)
{
    #ifdef DL_EXT

	deinitSigModule deinit;

	if (handlesOpenedSig[signal])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[signal], "deinitSigModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesSig[signal]);

		handlesOpenedSig[signal] = false;
		handlesSig[signal] = NULL;
	}

    #endif

	struct sigaction act;
	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMask(&act.sa_mask, blockSignals);

	if (sigaction(systemTools::toRealSignal(signal), &act, NULL) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemTools::setMicroTimer(unsigned long timeout,
						   signalHandler handler,
						   int blockSignals)
{
    #ifdef DL_EXT

	deinitSigModule deinit;

	if (handlesOpenedSig[SIGNAL_ALARM])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[SIGNAL_ALARM], "deinitSigModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesSig[SIGNAL_ALARM]);

		handlesOpenedSig[SIGNAL_ALARM] = false;
		handlesSig[SIGNAL_ALARM] = NULL;
	}

    #endif

	struct sigaction act;
	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETMICROTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETMICROTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (setitimer(ITIMER_REAL, &value, NULL) != 0)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETMICROTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemTools::setTimer(long timeout,
					  signalHandler handler,
					  int blockSignals)
{
    #ifdef DL_EXT

	deinitSigModule deinit;

	if (handlesOpenedSig[SIGNAL_ALARM])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[SIGNAL_ALARM], "deinitSigModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesSig[SIGNAL_ALARM]);

		handlesOpenedSig[SIGNAL_ALARM] = false;
		handlesSig[SIGNAL_ALARM] = NULL;
	}

    #endif

	struct sigaction act;
	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMask(&act.sa_mask, blockSignals);

	itimerval value;
	value.it_interval.tv_sec = timeout;
	value.it_interval.tv_usec = 0;
	value.it_value.tv_sec = timeout;
	value.it_value.tv_usec = 0;

	if (sigaction(SIGALRM, &act, NULL) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (setitimer(ITIMER_REAL, &value, NULL) != 0)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETTIMER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
systemTools::isSignalHandled(long signal)
{
	struct sigaction act;
	if (sigaction(systemTools::toRealSignal(signal), NULL, &act) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (act.sa_sigaction != NULL || act.sa_handler != NULL)
		return true;
	else
		return false;
}

//-------------------------------------------------------------------

void
systemTools::sendSignal(int pid,
						long signal)
{
	if (kill(pid, systemTools::toRealSignal(signal)) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SENDSIGNAL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemTools::unsetSignalHandler(long signal)
{
    #ifdef DL_EXT

	deinitSigModule deinit;

	if (handlesOpenedSig[signal])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[signal], "deinitSigModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesSig[signal]);

		handlesOpenedSig[signal] = false;
		handlesSig[signal] = NULL;
	}

    #endif

	struct sigaction act;
	act.sa_sigaction = NULL;

	if (sigaction(systemTools::toRealSignal(signal), &act, NULL) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_UNSETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

sigMod
systemTools::getModuleInfo(const dodoString &module,
						   void             *toInit)
{
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
	if (handle == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initSigModule init = (initSigModule)dlsym(handle, "initSigModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	sigMod mod = init(toInit);

	if (dlclose(handle) != 0)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	return mod;
}

//-------------------------------------------------------------------

void
systemTools::setSignalHandler(long signal,
							  const dodoString &path,
							  void             *toInit,
							  int blockSignals)
{
	deinitSigModule deinit;

	if (handlesOpenedSig[signal])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[signal], "deinitSigModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesSig[signal]);

		handlesOpenedSig[signal] = false;
		handlesSig[signal] = NULL;
	}

	handlesSig[signal] = dlopen(path.c_str(), RTLD_LAZY);
	if (handlesSig[signal] == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	initSigModule init = (initSigModule)dlsym(handlesSig[signal], "initSigModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMod mod = init(toInit);

	signalHandler in = (signalHandler)dlsym(handlesSig[signal], mod.hook);
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	struct sigaction act;
	act.sa_sigaction = in;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (blockSignals != -1)
		sigMask(&act.sa_mask, blockSignals);
	else
		sigMask(&act.sa_mask, mod.blockSignals);

	if (sigaction(systemTools::toRealSignal(signal), &act, NULL) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	handlesOpenedSig[signal] = true;
}

//-------------------------------------------------------------------

void
systemTools::setSignalHandler(const dodoString &path,
							  void             *toInit,
							  int blockSignals)
{

	void *handle = dlopen(path.c_str(), RTLD_LAZY);
	if (handle == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	initSigModule init = (initSigModule)dlsym(handle, "initSigModule");
	if (init == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sigMod mod = init(toInit);

	deinitSigModule deinit;

	if (handlesOpenedSig[mod.signal])
	{
		deinit = (deinitSigModule)dlsym(handlesSig[mod.signal], "deinitSigModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesSig[mod.signal]);

		handlesOpenedSig[mod.signal] = false;
		handlesSig[mod.signal] = NULL;
	}

	handlesSig[mod.signal] = handle;

	signalHandler in = (signalHandler)dlsym(handlesSig[mod.signal], mod.hook);
	if (in == NULL)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	struct sigaction act;
	act.sa_sigaction = in;
	act.sa_flags = SA_SIGINFO | SA_RESTART;

	if (sigemptyset(&act.sa_mask) == -1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (blockSignals != -1)
		sigMask(&act.sa_mask, blockSignals);
	else
		sigMask(&act.sa_mask, mod.blockSignals);

	if (sigaction(systemTools::toRealSignal(mod.signal), &act, NULL) == 1)
		throw baseEx(ERRMODULE_SYSTEMTOOLS, SYSTEMTOOLS_SETSIGNALHANDLER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	handlesOpenedSig[mod.signal] = true;
}

#endif

//-------------------------------------------------------------------

int
systemTools::toRealSignal(long signal)
{
	switch (signal)
	{
		case SIGNAL_HANGUP:

			return SIGHUP;

		case SIGNAL_INTERRUPT:

			return SIGINT;

		case SIGNAL_QUIT:

			return SIGQUIT;

		case SIGNAL_ILLEGAL_INSTRUCTION:

			return SIGILL;

		case SIGNAL_ABORT:

			return SIGABRT;

		case SIGNAL_BUS_FAULT:

			return SIGBUS;

		case SIGNAL_FLOATINGPOINT_FAULT:

			return SIGFPE;

		case SIGNAL_USER_DEFINED1:

			return SIGUSR1;

		case SIGNAL_SEGMENTATION_FAULT:

			return SIGSEGV;

		case SIGNAL_USER_DEFINED2:

			return SIGUSR2;

		case SIGNAL_PIPE_FAULT:

			return SIGPIPE;

		case SIGNAL_ALARM:

			return SIGALRM;

		case SIGNAL_TERMINATION:

			return SIGTERM;

        #ifndef __FreeBSD__

		case SIGNAL_STACK_FAULT:
			return SIGSTKFLT;

        #endif

		case SIGNAL_CHILD_CHANGED:

			return SIGCHLD;

		case SIGNAL_CONTINUE:

			return SIGCONT;

		case SIGNAL_KEYBOARD_STOP:

			return SIGTSTP;

		case SIGNAL_CPULIMIT_EXCEEDED:

			return SIGXCPU;

		case SIGNAL_FILESIZE_EXCEEDED:

			return SIGXFSZ;

		case SIGNAL_BAD_SYSCALL:

			return SIGSYS;

		default:

			return 0;
	}
}

//-------------------------------------------------------------------

void
systemTools::blockSignal(long signals,
						 bool block)
{
	sigset_t signal_mask;
	sigemptyset(&signal_mask);

	systemTools::sigMask(&signal_mask, signals);

	if (block)
		sigprocmask(SIG_BLOCK, &signal_mask, NULL);
	else
		sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
}

//-------------------------------------------------------------------
