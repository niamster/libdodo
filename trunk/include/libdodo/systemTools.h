/***************************************************************************
 *            systemTools.h
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

#ifndef _SYSTEMTOOLS_H_
#define _SYSTEMTOOLS_H_

#include <libdodo/directives.h>

#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#ifdef DL_EXT

	#include <dlfcn.h>

#endif

#include <libdodo/systemToolsEx.h>
#include <libdodo/types.h>
#include <libdodo/tools.h>

namespace dodo
{

	/**
	 * @struct __usage contains info about process
	 */
	struct __usage
	{
		long time;  ///<  amount of processor time of execution in miliseconds
		long mem;   ///< amount of memmory usage in bytes
	};

	/**
	 * @struct __limits describes system limits
	 */
	struct __limits
	{
		unsigned long current;  ///< can use current process
		unsigned long max;      ///< max amount
	};

	/**
	 * @enum systemToolsLimitEnum describes what type of limits you can get
	 */
	enum systemToolsLimitEnum
	{
		SYSTEMTOOLS_CPUTIME,
		SYSTEMTOOLS_MAXFILESIZE,
		SYSTEMTOOLS_MAXMEMUSAGE,
		SYSTEMTOOLS_MAXSTACK,
		SYSTEMTOOLS_MAXPROC,
		SYSTEMTOOLS_MAXOPENFILES
	};

	/**
	 * @typedef describes handler function on signal
	 */
	typedef void (*signalHandler)(int, siginfo_t *, void *);

	/**
	 * @enum systemSygnalsEnum describes system signals
	 */
	enum systemSignalsEnum
	{
		SYSTEMTOOLS_SIGNAL_HANGUP = 0,
		SYSTEMTOOLS_SIGNAL_INTERRUPT = 1<<1,
		SYSTEMTOOLS_SIGNAL_QUIT = 1<<2,
		SYSTEMTOOLS_SIGNAL_ILLEGAL_INSTRUCTION = 1<<3,
		SYSTEMTOOLS_SIGNAL_ABORT = 1<<4,
		SYSTEMTOOLS_SIGNAL_BUS_FAULT = 1<<5,
		SYSTEMTOOLS_SIGNAL_FLOATINGPOINT_FAULT = 1<<6,
		SYSTEMTOOLS_SIGNAL_USER_DEFINED1 = 1<<7,
		SYSTEMTOOLS_SIGNAL_SEGMENTATION_FAULT = 1<<8,
		SYSTEMTOOLS_SIGNAL_USER_DEFINED2 = 1<<9,
		SYSTEMTOOLS_SIGNAL_PIPE_FAULT = 1<<10,
		SYSTEMTOOLS_SIGNAL_ALARM = 1<<11,
		SYSTEMTOOLS_SIGNAL_TERMINATION = 1<<12,
		SYSTEMTOOLS_SIGNAL_CHILD_CHANGED = 1<<13,
		SYSTEMTOOLS_SIGNAL_CONTINUE = 1<<14,
		SYSTEMTOOLS_SIGNAL_KEYBOARD_STOP = 1<<15,
		SYSTEMTOOLS_SIGNAL_CPULIMIT_EXCEEDED = 1<<16,
		SYSTEMTOOLS_SIGNAL_FILESIZE_EXCEEDED = 1<<17,
		SYSTEMTOOLS_SIGNAL_BAD_SYSCALL = 1<<18,
	};

	/**
	 * @enum uidTypeEnum describes type of UID
	 * @note real - what ID started process
	 * effective - what ID's permissions process has
	 */
	enum uidTypeEnum
	{
		SYSTEMTOOLS_UID,
		SYSTEMTOOLS_EUID
	};

	/**
	 * @struct	__userInfo contains user info
	 */
	struct  __userInfo
	{
		dodoString name;        ///< user's name
		dodoString pass;        ///< user's password
		int uid;                ///< user's id
		int gid;                ///< user's group
		dodoString realName;    ///< user's real name
		dodoString home;        ///< user's home directory
		dodoString shell;       ///< user's default shell
	};

	/**
	 * @struct __groupInfo contains group info
	 */
	struct __groupInfo
	{
		dodoString name;        ///< name of the group
		int gid;                ///< group id
		dodoStringArray members;  ///< list of group members
	};

	#ifdef DL_EXT

	/**
	 * @struct __sigMod must be returned from initSigModule in the module
	 */
	struct __sigMod
	{
		char name[64];              ///< name of module
		char discription[256];      ///< discription of module
		char hook[64];              ///< name of function in module that will be a hook
		long signal;                ///< on what signal to set handler
		int blockSignals;           ///< signals to block during signal handling; can be or'ed; -1 - ignore
	};

	/**
	 * @typedef describes function in module that must return info for the hook
	 */
	typedef __sigMod (*initSigModule)(void *);

	/**
	 * @typedef describes function in module that will be called during module unloading
	 */
	typedef void (*deinitSigModule)();

	#endif


	/**
	 * @class systemTools provides misc system operations, gets diff info about system
	 */
	class systemTools
	{

			friend class systemThreads;

		public:

			/**
			 * registers functions that will be called on normal program exit
			 * @note you can register more than one
			 */
			static void atExit(void (*func)());

			/**
			 * suspend for given microseconds
			 * @param period is time in microseconds
			 */
			static void microSleep(unsigned long period);

			/**
			 * daemonize application
			 */
			static void daemonize();

			/**
			 * suspend for given seconds
			 * @param period is time in seconds
			 */
			static void sleep(long period);

			/**
			 * prints message to stderr end exits from program
			 * @param message is message to print
			 * @param status indicate with what status to exit
			 */
			static void die(const dodoString &message, int status=1);

			/**
			 * changes root(/) to new
			 * @param path indicates where to change root(/) directory
			 * @note you will appear in the root(/)
			 */
			static void changeRoot(const dodoString &path);
			/**
			 * @return current working directory
			 */
			static dodoString getWorkingDir();

			/**
			 * set current working directory (cd path)
			 * @param path is path where to go
			 */
			static void setWorkingDir(const dodoString &path);

			/**
			 * get system usage with current process
			 * @param info will be filled with system usage info
			 */
			static void getUsageInfo(__usage &info);

			/**
			 * get limits from systemToolsLimitEnum
			 * @param type is type of info to get[see systemToolsLimitEnum]
			 * @param lim will be filled with requested values
			 */
			static void getLimit(short type,  __limits &lim);

			/**
			 * set limits from systemToolsLimitEnum
			 * @param type is type of info to set[see systemToolsLimitEnum]
			 * @param lim will fill with requested values
			 */
			static void setLimit(short type, const __limits &lim);

			/**
			 * @return priority of current process for uidTypeEnum
			 * @param type is type of UID to use[see uidTypeEnum]
			 */
			static int getPriority(short type);

			/**
			 * sets priority of current process (nice)
			 * @param type is type of UID to use[see uidTypeEnum]
			 * @param prio is value of priority
			 */
			static void setPriority(short type, int prio);

			/**
			 * @return user id of the current process
			 * @param type is type of UID to use[see uidTypeEnum]
			 */
			static int getUID(short type);

			/**
			 * set user id of the current process
			 * @param type is type of UID to use[see uidTypeEnum]
			 * @param uid is user's id
			 */
			static void setUID(short type, int uid);

			/**
			 * get group id of the current process
			 * @param type is type of UID to use[see uidTypeEnum]
			 */
			static int getGID(short type);

			/**
			 * sets group id of the current process
			 * @param type is type of UID to use[see uidTypeEnum]
			 * @param gid is group id
			 */
			static void setGID(short type, int gid);

			/**
			 * gets user info
			 * @param info will be filled with user's info
			 * @param uid is user's id about what to get info
			 */
			static void getUserInfo(__userInfo &info, int uid);

			/**
			 * gets user info
			 * @param info will be filled with user's info
			 * @param name is user's login name about what to get info
			 */
			static void getUserInfo(__userInfo &info, const dodoString &uid);

			/**
			 * gets users of the system
			 * @param info will be filled with array of users' info
			 */
			static void getUsers(dodoArray<__userInfo> &info);

			/**
			 * gets group info
			 * @param info will be filled with group's info
			 * @param name is group's id name about what to get info
			 */
			static void getGroupInfo(__groupInfo &info, int gid);

			/**
			 * gets group info
			 * @param info will be filled with user's info
			 * @param name is group's name about what to get info
			 */
			static void getGroupInfo(__groupInfo &info, const dodoString &gid);

			/**
			 * gets group of the system
			 * @param info will be filled with array of group' info
			 */
			static void getGroups(dodoArray<__groupInfo> &info);
			/**
			 * @return PID of current process
			 */
			static int getPID();

			/**
			 * @return parent PID of current process
			 */
			static int getParentPID();

			/**
			 * @return group PID of current process
			 */
			static int getGroupPID();

			/**
			 * @return group PID of given PID
			 * @param pid is process id from what to get group PID
			 */
			static int getGroupPID(int pid);

			/**
			 * sets group PID of current process
			 * @param pgid specifies the group PID where to move current process
			 */
			static void setGroupPID(int gpid);

			/**
			 * sets group PID of given process
			 * @param pid specifies what pid to move
			 * @param pgid specifies the group PID where to move current process
			 */
			static void setGroupPID(int pid, int gpid);

			/**
			 * set timer and onTimer function
			 * @param timeout is timer period in microseconds
			 * @param handler is function that will be called
			 * @param blockSignals indicates what signals to block during signal handling; can be or'ed; -1 - ignore
			 */
			static void setMicroTimer(unsigned long timeout, signalHandler handler, int blockSignals=-1);

			/**
			 * set timer and onTimer function
			 * @param timeout is timer period in seconds
			 * @param handler is function that will be called
			 * @param blockSignals indicates what signals to block during signal handling; can be or'ed; -1 - ignore
			 */
			static void setTimer(long timeout, signalHandler handler, int blockSignals=-1);

			/**
			 * set signal handler
			 * @param signal is signal on what set handler[see systemSignalsEnum]
			 * @param handler is function that will be called
			 * @param blockSignals indicates what signals to block during signal handling; can be or'ed; -1 - ignore
			 */
			static void setSignalHandler(long signal, signalHandler handler, int blockSignals=-1);

			/**
			 * determines whether handler was set on signal[see systemSignalsEnum]
			 * @param is signal is on what set handler
			 */
			static bool isSignalHandled(long signal);

			/**
			 * removes signal handler
			 * @param is signal is from what unset handler[see systemSignalsEnum]
			 */
			static void unsetSignalHandler(long signal);

			#ifdef DL_EXT

			/**
			 * @return info about module
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param toInit indicates data that will path to initialize function
			 */
			static __sigMod getModuleInfo(const dodoString &module, void *toInit=NULL);

			/**
			 * set handler on signal from specific module
			 * @param signal indicates for what signal to set handler[see systemSignalsEnum]
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param toInit indicates data that will path to initialize function
			 * @param blockSignals indicates what signals to block during signal handling; can be or'ed; -1 - ignore; if != -1 => overrides given from module
			 */
			static void setSignalHandler(long signal, const dodoString &module, void *toInit=NULL, int blockSignals=-1);

			/**
			 * set handler on signal from specific module
			 * @param signal indicates for what signal to set handler
			 * @param @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param toInit indicates data that will path to initialize function
			 * @param blockSignals indicates what signals to block during signal handling; can be or'ed; -1 - ignore
			 */
			static void setSignalHandler(const dodoString &module, void *toInit=NULL, int blockSignals=-1);

			#endif

			/**
			 * send signal to process
			 * @param pid indicates where to send signal
			 * @param is signal is what signal to send[see systemSignalsEnum]
			 */
			static void sendSignal(int pid, long signal);

			/**
			 * block or unblock signals
			 * @param signal indicates what signals to block/unblock; can be or'ed;
			 * @param block indicates whether to block or unblock
			 */
			static void blockSignal(long signals, bool block=true);

		protected:

			/**
			 * fills __userInfo with values from passwd structure
			 */
			static __userInfo &fillUserInfo(__userInfo &info, passwd *pw);

			/**
			 * fills __groupInfo with values from group structure
			 */
			static __groupInfo &fillGroupInfo(__groupInfo &info, group *pw);

			/**
			 * @return system signal number that refers to given systemSignalsEnum
			 * @param signal describes signal to convert
			 */
			static int toRealSignal(long signal);

			/**
			 * @return signal number that refers to given systemSignalsEnum
			 * @param signal describes signal to convert
			 */
			static int toSignalNumber(long signal);

			/**
			 * fills 'set' structure with given signal mask
			 */
			static void sigMask(sigset_t *set, long signal);

			#ifdef DL_EXT
			
			static void *handlesSig[19];        ///< handles to modules
			static bool handlesOpenedSig[19];   ///< map of opened modules
			
			#endif

	};

};

#endif
