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
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#ifdef PTHREAD_EXT

#include <pthread.h>

#endif

#ifdef DL_EXT

#include <dlfcn.h>

#endif

#include <libdodo/systemToolsEx.h>
#include <libdodo/types.h>
#include <libdodo/tools.h>

namespace dodo
{

	/**
	 * @struct __usage defines process information
	 */
	struct __usage
	{
		long time;  ///< processor time of execution in miliseconds
		long mem;   ///< memory usage in bytes
	};

	/**
	 * @struct __limits defines system limits
	 */
	struct __limits
	{
		unsigned long current;  ///< current limit
		unsigned long max;      ///< max limit
	};

	/**
	 * @enum systemToolsLimitEnum defines limit types
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
	 * @typedef signalHandler defines handler function on signal
	 */
	typedef void (*signalHandler)(int, siginfo_t *, void *);

	/**
	 * @enum systemToolsSignalsEnum defines system signals
	 */
	enum systemToolsSignalsEnum
	{
		SYSTEMTOOLS_SIGNAL_HANGUP = 0,
		SYSTEMTOOLS_SIGNAL_INTERRUPT = 1 << 1,
		SYSTEMTOOLS_SIGNAL_QUIT = 1 << 2,
		SYSTEMTOOLS_SIGNAL_ILLEGAL_INSTRUCTION = 1 << 3,
		SYSTEMTOOLS_SIGNAL_ABORT = 1 << 4,
		SYSTEMTOOLS_SIGNAL_BUS_FAULT = 1 << 5,
		SYSTEMTOOLS_SIGNAL_FLOATINGPOINT_FAULT = 1 << 6,
		SYSTEMTOOLS_SIGNAL_USER_DEFINED1 = 1 << 7,
		SYSTEMTOOLS_SIGNAL_SEGMENTATION_FAULT = 1 << 8,
		SYSTEMTOOLS_SIGNAL_USER_DEFINED2 = 1 << 9,
		SYSTEMTOOLS_SIGNAL_PIPE_FAULT = 1 << 10,
		SYSTEMTOOLS_SIGNAL_ALARM = 1 << 11,
		SYSTEMTOOLS_SIGNAL_TERMINATION = 1 << 12,
		SYSTEMTOOLS_SIGNAL_CHILD_CHANGED = 1 << 13,
		SYSTEMTOOLS_SIGNAL_CONTINUE = 1 << 14,
		SYSTEMTOOLS_SIGNAL_KEYBOARD_STOP = 1 << 15,
		SYSTEMTOOLS_SIGNAL_CPULIMIT_EXCEEDED = 1 << 16,
		SYSTEMTOOLS_SIGNAL_FILESIZE_EXCEEDED = 1 << 17,
		SYSTEMTOOLS_SIGNAL_BAD_SYSCALL = 1 << 18,
	};

	/**
	 * @enum systemToolsIdTypeEnum defines type of UID
	 */
	enum systemToolsIdTypeEnum
	{
		SYSTEMTOOLS_UID,
		SYSTEMTOOLS_EUID
	};

	/**
	 * @struct __userInfo defines user info
	 */
	struct  __userInfo
	{
		dodoString name;        ///< user name
		dodoString pass;        ///< user password
		int uid;                ///< user id
		int gid;                ///< user group
		dodoString realName;    ///< user real name
		dodoString home;        ///< user home directory
		dodoString shell;       ///< user default shell
	};

	/**
	 * @struct __groupInfo defines group info
	 */
	struct __groupInfo
	{
		dodoString name;            ///< group name
		int gid;                    ///< group id
		dodoStringArray members;    ///< group members
	};

#ifdef DL_EXT

	/**
	 * @struct __sigMod is returned from initSigModule in the library
	 */
	struct __sigMod
	{
		char name[64];              ///< name of the library
		char discription[256];      ///< discription of the library
		char hook[64];              ///< name of function in module that will be as a hook
		long signal;                ///< signal to set handler
		int blockSignals;           ///< signals to block during signal handling; -1 to ignore
	};

	/**
	 * @typedef initSigModule defines type of init function for library
	 */
	typedef __sigMod (*initSigModule)(void *);

	/**
	 * @typedef deinitSigModule defines type of deinit function for library
	 */
	typedef void (*deinitSigModule)();

#endif

	/**
	 * @class systemTools provides system operations
	 */
	class systemTools
	{
			friend class systemThreadCollection;

		public:

			/**
			 * register function that will be called on normal program exit
			 * @note can be registered more than one
			 */
			static void atExit(void (*func)());

			/**
			 * suspend
			 * @param period defines time in microseconds
			 */
			static void microSleep(unsigned long period);

			/**
			 * suspend
			 * @param period defines time in seconds
			 */
			static void sleep(long period);

			/**
			 * daemonize application
			 */
			static void daemonize();

			/**
			 * print message to stderr end exit the program
			 * @param message defines message
			 * @param status defines exit status
			 */
			static void die(const dodoString &message, int status = 1);

			/**
			 * changes root(/) of the application
			 * @param path defines new root(/) directory
			 * @note automaticaly changes current directory to the new root(/)
			 */
			static void changeRoot(const dodoString &path);
			
			/**
			 * @return current working directory
			 */
			static dodoString getWorkingDir();

			/**
			 * set current working directory
			 * @param path defines path to the new working directory
			 */
			static void setWorkingDir(const dodoString &path);

			/**
			 * @return system usage info
			 */
			static __usage getUsageInfo();

			/**
			 * @return system limits info
			 * @param type defines type of limits[see systemToolsLimitEnum]
			 */
			static __limits getLimit(short type);

			/**
			 * set system limits
			 * @param type defines type limits[see systemToolsLimitEnum]
			 * @param lim defines system limits
			 */
			static void setLimit(short type, const __limits &lim);

			/**
			 * @return priority of current process
			 * @param type defines type of UID[see systemToolsIdTypeEnum]
			 */
			static int getPriority(short type);

			/**
			 * set priority of current process
			 * @param type defines type of UID[see systemToolsIdTypeEnum]
			 * @param prio defines priority
			 */
			static void setPriority(short type, int prio);

			/**
			 * @return UID of the current process
			 * @param type defines type of UID[see systemToolsIdTypeEnum]
			 */
			static int getUID(short type);

			/**
			 * set user id of the current process
			 * @param type defines type of UID[see systemToolsIdTypeEnum]
			 * @param uid defines UID
			 */
			static void setUID(short type, int uid);

			/**
			 * get group id of the current process
			 * @param type defines type of GID[see systemToolsIdTypeEnum]
			 */
			static int getGID(short type);

			/**
			 * set group id of the current process
			 * @param type defines type of GID[see systemToolsIdTypeEnum]
			 * @param gid defines group id
			 */
			static void setGID(short type, int gid);

			/**
			 * @return user info
			 * @param uid defines user id
			 */
			static __userInfo getUserInfo(int uid);

			/**
			 * @return user info
			 * @param name defines user login name
			 */
			static __userInfo getUserInfo(const dodoString &name);

			/**
			 * @return users of the system
			 */
			static dodoArray<__userInfo> getUsers();

			/**
			 * @return group info
			 * @param gid defines group id
			 */
			static __groupInfo getGroupInfo(int gid);

			/**
			 * @return group info
			 * @param name defines group name
			 */
			static __groupInfo getGroupInfo(const dodoString &name);

			/**
			 * @return groups of the system
			 */
			static dodoArray<__groupInfo> getGroups();
			
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
			 * @param pid defines PID
			 */
			static int getGroupPID(int pid);

			/**
			 * set group PID of current process
			 * @param gpid defines group PID where to move current process
			 */
			static void setGroupPID(int gpid);

			/**
			 * set group PID of given process
			 * @param pid defines PID to move
			 * @param gpid defines group PID where to move process
			 */
			static void setGroupPID(int pid, int gpid);

			/**
			 * set timer and onTimer function
			 * @param timeout defines timer period in microseconds
			 * @param handler defines timer function
			 * @param blockSignals defines signals to block during signal handling; -1 to ignore
			 */
			static void setMicroTimer(unsigned long timeout, signalHandler handler, int blockSignals = -1);

			/**
			 * set timer and onTimer function
			 * @param timeout defines timer period in seconds
			 * @param handler defines timer function
			 * @param blockSignals defines signals to block during signal handling; -1 to ignore
			 */
			static void setTimer(long timeout, signalHandler handler, int blockSignals = -1);

			/**
			 * set signal handler
			 * @param signal defines system signal[see systemToolsSignalsEnum]
			 * @param handler defines handle function
			 * @param blockSignals defines signals to block during signal handling; -1 to ignore
			 */
			static void setSignalHandler(long signal, signalHandler handler, int blockSignals = -1);

			/**
			 * @return true if handler is set on signal
			 * @param signal defines system signal[see systemToolsSignalsEnum]
			 */
			static bool isSignalHandled(long signal);

			/**
			 * remove signal handler
			 * @param signal defines system signal[see systemToolsSignalsEnum]
			 */
			static void unsetSignalHandler(long signal);

#ifdef DL_EXT

			/**
			 * @return information about module
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param toInit defines data that will be passed to the init function
			 */
			static __sigMod getModuleInfo(const dodoString &path, void *toInit = NULL);

			/**
			 * set handler on signal from specific module
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param toInit defines data that will be passed to the init function
			 */
			static void setSignalHandler(const dodoString &path, void *toInit = NULL);

#endif

			/**
			 * send signal to process
			 * @param pid defines PID of the process
			 * @param signal defines system signal[see systemToolsSignalsEnum]
			 */
			static void sendSignal(int pid, long signal);

			/**
			 * block or unblock signals
			 * @param signal defines system signal[see systemToolsSignalsEnum]
			 * @param block defines block condition
			 */
			static void blockSignal(long signal, bool block = true);

		protected:

			/**
			 * fill __userInfo with values from passwd structure
			 * @return user info
			 * @param info defines structure to fill
			 * @param pw defines structure with info
			 */
			static __userInfo &fillUserInfo(__userInfo &info, passwd *pw);

			/**
			 * fill __groupInfo with values from group structure
			 * @return group info
			 * @param info defines structure to fill
			 * @param pw defines structure with info
			 */
			static __groupInfo &fillGroupInfo(__groupInfo &info, group *pw);

			/**
			 * @return system signal number that refers to systemToolsSignalsEnum
			 * @param signal defines signal to convert[see systemToolsSignalsEnum]
			 */
			static int toRealSignal(long signal);

			/**
			 * @return signal number that refers to given systemToolsSignalsEnum
			 * @param signal defines signal to convert[see systemToolsSignalsEnum]
			 * @note returns position in the internal structures
			 */
			static int toSignalNumber(long signal);

			/**
			 * fill 'set' structure with given signal mask
			 * @param set defines set of signals
			 * @param signal defines system signal
			 */
			static void sigMask(sigset_t *set, long signal);

#ifdef DL_EXT

			static void *handlesSig[19];        ///< handles to modules
			static bool handlesOpenedSig[19];   ///< map of opened modules

#endif
			/**
			 * @class staticAtomicMutex performs atomic locks using mutexes
			 */
			class staticAtomicMutex
			{
				public:

					/**
					 * consructor
					 */
					staticAtomicMutex();

					/**
					 * destructor
					 */
					virtual ~staticAtomicMutex();

					/**
					 * lock critical section
					 */
					virtual void lock();

					/**
					 * unlock critical section
					 */
					virtual void unlock();

				protected:

#ifdef PTHREAD_EXT
					static pthread_mutex_t mutex; ///< mutex
#endif
			};

			static staticAtomicMutex mutex;///< lock
			
			/**
			 * @class systemRaceHazardGuard provides thread safe behaviour
			 * @note it locks in constructor and unlocks in destructor
			 */
			class systemRaceHazardGuard
			{
				public:

					/**
					 * contructor
					 */
					systemRaceHazardGuard();

					/**
					 * destructor
					 */
					virtual ~systemRaceHazardGuard();
			};	
	};
};

#endif
