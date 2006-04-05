/***************************************************************************
 *            systemTools.cc
 *
 *  Sat Nov 19 15:19:57 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <systemTools.h>

using namespace dodo;

#ifdef DL_EXT

	extern "C"
	{
			static void *__handlesSig[20];///< handles to modules
			static bool __handlesOpenedSig[20] = {false,
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
													false};///< map of opened modules
						
		
	};
	
#endif
	
std::string 
systemTools::getWorkingDir()
{
	char wd[MAXPATHLEN];
	
	if (getcwd(wd,MAXPATHLEN) == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETWORKINGDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __string__;
		#endif
	
	return wd;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif 
systemTools::setWorkingDir(const std::string &path)
{
	if (chdir(path.c_str()) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETWORKINGDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

	
#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::getUsageInfo(__usage &info)
{
	rusage use;
	if (getrusage(RUSAGE_SELF,&use) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETUSAGEINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	info.time = use.ru_utime.tv_sec*100 + use.ru_utime.tv_usec;
	info.mem = use.ru_maxrss*1024;

	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif 
systemTools::changeRoot(const std::string &path)
{
	#ifndef NO_EX
		setWorkingDir(path);
	#else
		if(!setWorkingDir(path))
			return false;
	#endif 	
	
	if (chroot(path.c_str()) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_CHANGEROOT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::getLimit(systemToolsLimitEnum type, 
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
			#ifdef FREE_BSD
				realRes = RLIMIT_NOFILE;
			#else
				realRes = RLIMIT_OFILE;
			#endif
			break;
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETLIMIT,ERR_LIBDODO,SYSTEMTOOLS_WRONG_PARAMETHER,SYSTEMTOOLS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif				
	}
	
	if (getrlimit(realRes,&limit) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETLIMIT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	lim.current = limit.rlim_cur;
	lim.max = limit.rlim_max;			

	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::setLimit(systemToolsLimitEnum type, 
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
			#ifdef FREE_BSD
				realRes = RLIMIT_NOFILE;
			#else
				realRes = RLIMIT_OFILE;
			#endif
			break;
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETLIMIT,ERR_LIBDODO,SYSTEMTOOLS_WRONG_PARAMETHER,SYSTEMTOOLS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif				
	}
	
	limit.rlim_cur = lim.current;
	limit.rlim_max = lim.max;		
	
	if (setrlimit(realRes,&limit) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETLIMIT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif			

	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

int 
systemTools::getPriority(uidTypeEnum type)
{
	register int prio = getpriority(PRIO_PROCESS,getUID(type));
	if (prio == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETPRIORITY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;
		#endif
		
	return prio;	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif	
systemTools::setPriority(uidTypeEnum type,
						int prio)
{
	if (setpriority(PRIO_PROCESS,getUID(type),prio) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETPRIORITY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

int 
systemTools::getUID(uidTypeEnum type)
{
	switch (type)
	{
		case SYSTEM_UID:
			return getuid();
		case SYSTEM_EUID:
			return geteuid();
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETUID,ERR_LIBDODO,SYSTEMTOOLS_WRONG_PARAMETHER,SYSTEMTOOLS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return -1;
			#endif	
	}
}

//-------------------------------------------------------------------
			
#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::setUID(uidTypeEnum type, 
					int uid)
{
	register int res(0);
	
	switch (type)
	{
		case SYSTEM_UID:
			res = setuid(uid);
		case SYSTEM_EUID:
			res = seteuid(uid);
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETUID,ERR_LIBDODO,SYSTEMTOOLS_WRONG_PARAMETHER,SYSTEMTOOLS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif		
	}
	
	if (res == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETUID,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif							
}

//-------------------------------------------------------------------

int 
systemTools::getGID(uidTypeEnum type)
{
	switch (type)
	{
		case SYSTEM_UID:
			return getgid();
		case SYSTEM_EUID:
			return getegid();
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETGID,ERR_LIBDODO,SYSTEMTOOLS_WRONG_PARAMETHER,SYSTEMTOOLS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return -1;
			#endif	
	}
}

//-------------------------------------------------------------------
			
#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::setGID(uidTypeEnum type, 
					int uid)
{
	register int res(0);
	
	switch (type)
	{
		case SYSTEM_UID:
			res = setgid(uid);
		case SYSTEM_EUID:
			res = setegid(uid);
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETGID,ERR_LIBDODO,SYSTEMTOOLS_WRONG_PARAMETHER,SYSTEMTOOLS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif		
	}
	
	if (res == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETGID,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif							
}
	
//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::getUserInfo(__userInfo &info, 
			int uid)
{
	passwd *in = getpwuid(uid);
	if (in == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETUSERINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	fillUserInfo(info,in);

	#ifdef NO_EX
		return true;
	#endif
	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::getUserInfo(__userInfo &info, 
			const std::string &uid)
{
	passwd *in = getpwnam(uid.c_str());
	if (in == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETUSERINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	fillUserInfo(info,in);

	#ifdef NO_EX
		return true;
	#endif
	
}	

//-------------------------------------------------------------------

		
#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::getUsers(std::vector<__userInfo> &users)
{
	users.clear();
	
	passwd *in;
	
	__userInfo info;
	
	while ( (in=getpwent())!=NULL )
		users.push_back(fillUserInfo(info,in));
		
	switch (errno)
	{
		case EIO:
		case EMFILE:
		case ENFILE:
		case ENOMEM:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETUSERS,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
	}
	
	endpwent();
	
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

__userInfo & 
systemTools::fillUserInfo(__userInfo &info, 
						passwd *in)
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
						group *pw)
{
	info.gid = pw->gr_gid;
	info.name = pw->gr_name;
	
	info.members.clear();
	
	register int i(0);
	
	while (pw->gr_mem[i] != NULL)
		info.members.push_back(pw->gr_mem[i++]);
	
	return info;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::getGroupInfo(__groupInfo &info, 
			int uid)
{
	group *in = getgrgid(uid);
	if (in == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETGROUPINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	fillGroupInfo(info,in);

	#ifdef NO_EX
		return true;
	#endif
	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::getGroupInfo(__groupInfo &info, 
			const std::string &uid)
{
	group *in = getgrnam(uid.c_str());
	if (in == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETGROUPINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	fillGroupInfo(info,in);

	#ifdef NO_EX
		return true;
	#endif
	
}	

//-------------------------------------------------------------------

		
#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::getGroups(std::vector<__groupInfo> &users)
{
	users.clear();
	
	group *in;
	
	__groupInfo info;
	
	while ( (in=getgrent())!=NULL )
		users.push_back(fillGroupInfo(info,in));
		
	switch (errno)
	{
		case EIO:
		case EMFILE:
		case ENFILE:
		case EINTR:
		case ENOMEM:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETGROUPS,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
	}
	
	endgrent();
	
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

void 
systemTools::die(const std::string &message, 
				int status)
{
	std::cerr << message << std::endl;
	_exit(status);
}

//-------------------------------------------------------------------

void 
systemTools::microSleep(unsigned long period)
{
	if (period<1000000)
		::usleep(period);
	else
		::sleep(period/1000000);
}

//-------------------------------------------------------------------

void 
systemTools::sleep(unsigned long period)
{
	::sleep(period);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::atExit(void (*func)())
{
	if (atexit(func)!=0)
	#ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_ATEXIT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
	#else
		return false;
	#endif
	
	#ifdef NO_EX
		return true;
	#endif
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
	register int pgid = getpgid(pid);
	if (pgid==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETGROUPPID,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#endif		
	
	return -1;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::setGroupPID(int gpid)
{
	if (setpgid(0,gpid)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETGROUPPID,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif	
}


//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif			
systemTools::setGroupPID(int pid,
						int gpid)
{
	if (setpgid(pid,gpid)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETGROUPPID,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif 
systemTools::sigMask(sigset_t *set,
					int blockSignals)
{
	if (blockSignals != -1)
	{
		if ((SIGNAL_HANGUP&blockSignals) == SIGNAL_HANGUP)
			if (sigaddset(set, SIGHUP) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_INTERRUPT&blockSignals) == SIGNAL_INTERRUPT)
			if (sigaddset(set, SIGINT) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_QUIT&blockSignals) == SIGNAL_QUIT)
			if (sigaddset(set, SIGQUIT) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_ILLEGAL_INSTRUCTION&blockSignals) == SIGNAL_ILLEGAL_INSTRUCTION)
			if (sigaddset(set, SIGILL) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_ABORT&blockSignals) == SIGNAL_ABORT)
			if (sigaddset(set, SIGABRT) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_BUS_FAULT&blockSignals) == SIGNAL_BUS_FAULT)
			if (sigaddset(set, SIGBUS) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_FLOATINGPOINT_FAULT&blockSignals) == SIGNAL_FLOATINGPOINT_FAULT)
			if (sigaddset(set, SIGFPE) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_USER_DEFINED1&blockSignals) == SIGNAL_USER_DEFINED1)
			if (sigaddset(set, SIGUSR1) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
	
		if ((SIGNAL_SEGMENTATION_FAULT&blockSignals) == SIGNAL_SEGMENTATION_FAULT)
			if (sigaddset(set, SIGSEGV) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_USER_DEFINED2&blockSignals) == SIGNAL_USER_DEFINED2)
			if (sigaddset(set, SIGUSR2) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_PIPE_FAULT&blockSignals) == SIGNAL_PIPE_FAULT)
			if (sigaddset(set, SIGPIPE) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_ALARM&blockSignals) == SIGNAL_ALARM)
			if (sigaddset(set, SIGALRM) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_TERMINATION&blockSignals) == SIGNAL_TERMINATION)
			if (sigaddset(set, SIGTERM) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_STACK_FAULT&blockSignals) == SIGNAL_STACK_FAULT)
			if (sigaddset(set, SIGSTKFLT) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_CHILD_CHANGED&blockSignals) == SIGNAL_CHILD_CHANGED)
			if (sigaddset(set, SIGCHLD) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_CONTINUE&blockSignals) == SIGNAL_CONTINUE)
			if (sigaddset(set, SIGCONT) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
							
		if ((SIGNAL_KEYBOARD_STOP&blockSignals) == SIGNAL_KEYBOARD_STOP)
			if (sigaddset(set, SIGTSTP) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_CPULIMIT_EXCEEDED&blockSignals) == SIGNAL_CPULIMIT_EXCEEDED)
			if (sigaddset(set, SIGXCPU) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_FILESIZE_EXCEEDED&blockSignals) == SIGNAL_FILESIZE_EXCEEDED)
			if (sigaddset(set, SIGXFSZ) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
			
		if ((SIGNAL_BAD_SYSCALL&blockSignals) == SIGNAL_BAD_SYSCALL)
			if (sigaddset(set, SIGSYS) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SIGMASK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif		
	}
		
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif
systemTools::setSignalHandler(systemSignalsEnum signal, 
							signalHandler handler,
							int blockSignals)
{
	#ifdef DL_EXT
	
		deinitSigModule deinit;
		
		if (__handlesOpenedSig[signal])
		{
			deinit = (deinitSigModule)dlsym(__handlesSig[signal], "deinitSigModule");
			if (deinit != NULL)
				deinit();
				
			dlclose(__handlesSig[signal]);
			
			__handlesOpenedSig[signal] = false;
			__handlesSig[signal] = NULL;
		}	
	
	#endif	
	
	struct sigaction act;
	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO|SA_RESTART;
	
	if (sigemptyset(&act.sa_mask) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif		
		
	sigMask(&act.sa_mask, blockSignals);
	
	if (sigaction(systemTools::toRealSignal(signal),&act,NULL) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

bool 
systemTools::isSignalHandled(systemSignalsEnum signal)
{
	struct sigaction act;
	if (sigaction(systemTools::toRealSignal(signal),NULL,&act)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif	
		
	if (act.sa_sigaction!=NULL || act.sa_handler!=NULL)	
		return true;
	else
		return false;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif 
systemTools::sendSignal(int pid, 
						systemSignalsEnum signal)
{
	if (kill(pid, systemTools::toRealSignal(signal)) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SENDSIGNAL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif 
systemTools::unsetSignalHandler(systemSignalsEnum signal)
{
	#ifdef DL_EXT
	
		deinitSigModule deinit;
		
		if (__handlesOpenedSig[signal])
		{
			deinit = (deinitSigModule)dlsym(__handlesSig[signal], "deinitSigModule");
			if (deinit != NULL)
				deinit();
				
			dlclose(__handlesSig[signal]);
			
			__handlesOpenedSig[signal] = false;
			__handlesSig[signal] = NULL;
		}	
	
	#endif
	
	struct sigaction act;
	act.sa_sigaction = NULL;
		
	if (sigaction(systemTools::toRealSignal(signal),&act,NULL)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_UNSETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif	
	
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

#ifdef DL_EXT

	sigMod 
	systemTools::getModuleInfo(const std::string &module, 
								void *toInit)
	{
			void *handle = dlopen(module.c_str(), RTLD_LAZY);
			if (handle == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return sigMod();
				#endif
				
			initSigModule init = (initSigModule)dlsym(handle, "initSigModule");
			if (init == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return sigMod();
				#endif
				
			sigMod mod = init(toInit);
			
			if (dlclose(handle)!=0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return mod;
				#endif
			
			return mod;		
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool 
	#endif 
	systemTools::setSignalHandler(systemSignalsEnum signal, 
								const std::string &path, 
								void *toInit,
								int blockSignals)
	{
		deinitSigModule deinit;
		
		if (__handlesOpenedSig[signal])
		{
			deinit = (deinitSigModule)dlsym(__handlesSig[signal], "deinitSigModule");
			if (deinit != NULL)
				deinit();
				
			dlclose(__handlesSig[signal]);
			
			__handlesOpenedSig[signal] = false;
			__handlesSig[signal] = NULL;
		}
		
		__handlesSig[signal] = dlopen(path.c_str(), RTLD_LAZY);
		if (__handlesSig[signal] == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		initSigModule init = (initSigModule)dlsym(__handlesSig[signal], "initSigModule");
		if (init == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		sigMod mod = init(toInit);
		
		signalHandler in = (signalHandler)dlsym(__handlesSig[signal], mod.hook);
		if (in == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	
		struct sigaction act;
		act.sa_sigaction = in;
		act.sa_flags = SA_SIGINFO|SA_RESTART;

		if (sigemptyset(&act.sa_mask) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif		
		
		if (blockSignals != -1)	
			sigMask(&act.sa_mask, blockSignals);
		else
			sigMask(&act.sa_mask, mod.blockSignals);
		
		if (sigaction(systemTools::toRealSignal(signal),&act,NULL)==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		__handlesOpenedSig[signal] = true;
		
		#ifdef NO_EX
			return true;
		#endif					
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool 
	#endif 
	systemTools::setSignalHandler(const std::string &path, 
								void *toInit,
								int blockSignals)
	{
		
		void *handle = dlopen(path.c_str(), RTLD_LAZY);
		if (handle == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		initSigModule init = (initSigModule)dlsym(handle, "initSigModule");
		if (init == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		sigMod mod = init(toInit);	
		
		deinitSigModule deinit;
		
		if (__handlesOpenedSig[mod.signal])
		{
			deinit = (deinitSigModule)dlsym(__handlesSig[mod.signal], "deinitSigModule");
			if (deinit != NULL)
				deinit();
				
			dlclose(__handlesSig[mod.signal]);
			
			__handlesOpenedSig[mod.signal] = false;
			__handlesSig[mod.signal] = NULL;
		}
		
		__handlesSig[mod.signal] = handle;
		
		signalHandler in = (signalHandler)dlsym(__handlesSig[mod.signal], mod.hook);
		if (in == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	
		struct sigaction act;
		act.sa_sigaction = in;
		act.sa_flags = SA_SIGINFO|SA_RESTART;

		if (sigemptyset(&act.sa_mask) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif		
					
		if (blockSignals != -1)	
			sigMask(&act.sa_mask, blockSignals);
		else
			sigMask(&act.sa_mask, mod.blockSignals);
		
		if (sigaction(systemTools::toRealSignal(mod.signal),&act,NULL)==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETSIGNALHANDLER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		__handlesOpenedSig[mod.signal] = true;
		
		#ifdef NO_EX
			return true;
		#endif					
	}
	
#endif

//-------------------------------------------------------------------

int 
systemTools::toRealSignal(systemSignalsEnum signal)
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
			
		case SIGNAL_STACK_FAULT:
			return SIGSTKFLT;
			
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
systemTools::blockSignal(int signals, 
							bool block)
{	
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	
	systemTools::sigMask(&signal_mask, signals);
	
	if (block)
		sigprocmask(SIG_BLOCK,&signal_mask,NULL);	
	else
		sigprocmask(SIG_UNBLOCK,&signal_mask,NULL);
}

//-------------------------------------------------------------------
