/***************************************************************************
 *            toolsOs.h
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

#ifndef _TOOLSOS_H_
#define _TOOLSOS_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo
{
	namespace pc
	{
		namespace sync
		{
			namespace thread
			{
				struct __lock__;
			};
		};

		namespace thread
		{
			class collection;
		};
	};

	namespace tools
	{
		/**
		 * @struct __usage__
		 * @brief defines process information
		 */
		struct __usage__
		{
			long time;              ///< processor time of execution in miliseconds
			long mem;               ///< memory usage in bytes
		};

		/**
		 * @struct __limits__
		 * @brief defines os limits
		 */
		struct __limits__
		{
			unsigned long current;  ///< current limit
			unsigned long max;      ///< max limit
		};

		/**
		 * @enum osLimitEnum defines limit types
		 */
		enum osLimitEnum
		{
			OS_CPUTIME,
			OS_MAXFILESIZE,
			OS_MAXMEMUSAGE,
			OS_MAXSTACK,
			OS_MAXPROC,
			OS_MAXOPENFILES
		};

		/**
		 * @typedef signalHandler
		 * @brief defines handler function on signal
		 * @param signal defines received signal
		 * @param info defines signal info
		 * @param context defines signal context
		 * @note info fields:
		 * int si_signo;// Signal number
		 * int si_errno;// An errno value
		 * int si_code;// Signal code
		 * pid_t si_pid;// Sending process ID
		 * uid_t si_uid;// Real user ID of sending process
		 * int  si_status;// Exit value or signal
		 * clock_t si_utime;// User time consumed
		 * clock_t si_stime;// System time consumed
		 * sigval_t si_value;// Signal value
		 * int si_int;// POSIX.1b signal
		 * void *si_ptr;// POSIX.1b signal
		 * void *si_addr;// Memory location which caused fault
		 * int si_band;// Band event
		 * int si_fd;// File descriptor
		 *
		 * context fields:
		 * int __sc___onstack;// Sigstack state to restore
		 * __sigset___t uc_sigmask;// The set of signals that are blocked when this context is active
		 * mcontext_t uc_mcontext;// Machine-specific image of saved context
		 * struct ucontext_t *uc_link;// Context resumed after this one returns
		 * stack_t uc_stack;// Stack used by context
		 */
		typedef void (*signalHandler)(int signal, void *info, void *context);


#define OS_SIGNALS 19

		/**
		 * @enum osSignalsEnum defines os signals
		 */
		enum osSignalsEnum
		{
			OS_SIGNAL_HANGUP = 0,
			OS_SIGNAL_INTERRUPT = 1 << 1,
			OS_SIGNAL_QUIT = 1 << 2,
			OS_SIGNAL_ILLEGAL_INSTRUCTION = 1 << 3,
			OS_SIGNAL_ABORT = 1 << 4,
			OS_SIGNAL_BUS_FAULT = 1 << 5,
			OS_SIGNAL_FLOATINGPOINT_FAULT = 1 << 6,
			OS_SIGNAL_USER_DEFINED1 = 1 << 7,
			OS_SIGNAL_SEGMENTATION_FAULT = 1 << 8,
			OS_SIGNAL_USER_DEFINED2 = 1 << 9,
			OS_SIGNAL_PIPE_FAULT = 1 << 10,
			OS_SIGNAL_ALARM = 1 << 11,
			OS_SIGNAL_TERMINATION = 1 << 12,
			OS_SIGNAL_CHILD_CHANGED = 1 << 13,
			OS_SIGNAL_CONTINUE = 1 << 14,
			OS_SIGNAL_KEYBOARD_STOP = 1 << 15,
			OS_SIGNAL_CPULIMIT_EXCEEDED = 1 << 16,
			OS_SIGNAL_FILESIZE_EXCEEDED = 1 << 17,
			OS_SIGNAL_BAD_SYSCALL = 1 << 18,
		};

		/**
		 * @typedef daemon
		 * @brief defines daemon function
		 * @param data defines user data
		 */
		typedef void (*daemon)(void *data);

		/**
		 * @enum osIdTypeEnum defines type of UID
		 */
		enum osIdTypeEnum
		{
			OS_UID,
			OS_EUID
		};

		/**
		 * @struct __userInfo__
		 * @brief defines user info
		 */
		struct  __userInfo__
		{
			dodoString name;            ///< user name
			dodoString pass;            ///< user password
			int        uid;             ///< user id
			int        gid;             ///< user group
			dodoString realName;        ///< user real name
			dodoString home;            ///< user home directory
			dodoString shell;           ///< user default shell
		};

		/**
		 * @struct __groupInfo__
		 * @brief defines group info
		 */
		struct __groupInfo__
		{
			dodoString      name;       ///< group name
			int             gid;        ///< group id
			dodoStringArray members;    ///< group members
		};

#ifdef DL_EXT
		/**
		 * @struct __signalMod__
		 * @brief is returned from initOsSignalModule in the library
		 */
		struct __signalMod__
		{
			char name[64];              ///< name of the library
			char discription[256];      ///< discription of the library
			char hook[64];              ///< name of function in module that will be as a hook
			long signal;                ///< signal to set handler
			int  blockSignals;          ///< signals to block during signal handling; -1 to ignore
		};

		/**
		 * @typedef initOsSignalModule
		 * @brief defines type of init function for library
		 * @param data defines user data
		 */
		typedef __signalMod__ (*initOsSignalModule)(void *data);

		/**
		 * @typedef deinitOsSignalModule
		 * @brief defines type of deinit function for library
		 */
		typedef void (*deinitOsSignalModule)();
#endif

		/**
		 * @class os
		 * @brief provides os operations
		 */
		class os
		{
			friend class pc::thread::collection;

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
			 * daemonize function call
			 * @param func defines function that would be executed as a daemon
			 * @param data defines daemon data
			 * @note the code after the call to releaseDaemon would never be executed
			 */
			static void releaseDaemon(daemon func,
									  void   *data);

			/**
			 * daemonize application
			 * @note the code after the call to becomeDaemon would run in background as a daemon
			 */
			static void becomeDaemon();

			/**
			 * print message to stderr and terminate the program and all child processes
			 * @param message defines message
			 * @param status defines exit status
			 * @note OS_SIGNAL_TERMINATION is sent to the child processes
			 */
			static void die(const dodoString &message,
							int              status = 1);

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
			 * @return os usage info
			 */
			static __usage__ getUsageInfo();

			/**
			 * @return os limits info
			 * @param type defines type of limits[see osLimitEnum]
			 */
			static __limits__ getLimit(short type);

			/**
			 * set os limits
			 * @param type defines type limits[see osLimitEnum]
			 * @param lim defines os limits
			 */
			static void setLimit(short          type,
								 const __limits__ &lim);

			/**
			 * @return priority of current process
			 * @param type defines type of UID[see osIdTypeEnum]
			 */
			static int getPriority(short type);

			/**
			 * set priority of current process
			 * @param type defines type of UID[see osIdTypeEnum]
			 * @param prio defines priority
			 */
			static void setPriority(short type,
									int   prio);

			/**
			 * @return UID of the current process
			 * @param type defines type of UID[see osIdTypeEnum]
			 */
			static int getUID(short type);

			/**
			 * set user id of the current process
			 * @param type defines type of UID[see osIdTypeEnum]
			 * @param uid defines UID
			 */
			static void setUID(short type,
							   int   uid);

			/**
			 * get group id of the current process
			 * @param type defines type of GID[see osIdTypeEnum]
			 */
			static int getGID(short type);

			/**
			 * set group id of the current process
			 * @param type defines type of GID[see osIdTypeEnum]
			 * @param gid defines group id
			 */
			static void setGID(short type,
							   int   gid);

			/**
			 * @return user info
			 * @param uid defines user id
			 */
			static __userInfo__ getUserInfo(int uid);

			/**
			 * @return user info
			 * @param name defines user login name
			 */
			static __userInfo__ getUserInfo(const dodoString &name);

			/**
			 * @return users of the os
			 */
			static dodoArray<__userInfo__> getUsers();

			/**
			 * @return group info
			 * @param gid defines group id
			 */
			static __groupInfo__ getGroupInfo(int gid);

			/**
			 * @return group info
			 * @param name defines group name
			 */
			static __groupInfo__ getGroupInfo(const dodoString &name);

			/**
			 * @return groups of the os
			 */
			static dodoArray<__groupInfo__> getGroups();

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
			static void setGroupPID(int pid,
									int gpid);

			/**
			 * set timer and onTimer function
			 * @param timeout defines timer period in microseconds
			 * @param handler defines timer function
			 * @param blockSignals defines signals to block during signal handling; -1 to ignore
			 */
			static void setMicroTimer(unsigned long timeout,
									  signalHandler handler,
									  int           blockSignals = -1);

			/**
			 * set timer and onTimer function
			 * @param timeout defines timer period in seconds
			 * @param handler defines timer function
			 * @param blockSignals defines signals to block during signal handling; -1 to ignore
			 */
			static void setTimer(long          timeout,
								 signalHandler handler,
								 int           blockSignals = -1);

			/**
			 * set signal handler
			 * @param signal defines os signal[see osSignalsEnum]
			 * @param handler defines handle function
			 * @param blockSignals defines signals to block during signal handling; -1 to ignore
			 */
			static void setSignalHandler(long          signal,
										 signalHandler handler,
										 int           blockSignals = -1);

			/**
			 * @return true if handler is set on signal
			 * @param signal defines os signal[see osSignalsEnum]
			 */
			static bool isSignalHandled(long signal);

			/**
			 * remove signal handler
			 * @param signal defines os signal[see osSignalsEnum]
			 */
			static void unsetSignalHandler(long signal);

#ifdef DL_EXT
			/**
			 * @return information about module
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param toInit defines data that will be passed to the init function
			 */
			static __signalMod__ getModuleInfo(const dodoString &path,
											 void             *toInit = NULL);

			/**
			 * set handler on signal from specific module
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param toInit defines data that will be passed to the init function
			 */
			static void setSignalHandler(const dodoString &path,
										 void             *toInit = NULL);
#endif

			/**
			 * send signal to process
			 * @param pid defines PID of the process
			 * @param signal defines os signal[see osSignalsEnum]
			 */
			static void sendSignal(int  pid,
								   long signal);

			/**
			 * block or unblock signals
			 * @param signal defines os signal[see osSignalsEnum]
			 * @param block defines block condition
			 */
			static void blockSignal(long signal,
									bool block = true);

		  protected:

			/**
			 * fill __userInfo__ with values from passwd structure
			 * @return user info
			 * @param info defines structure to fill
			 * @param pw defines structure with info
			 */
			static __userInfo__ &fillUserInfo(__userInfo__ &info,
											void     *pw);

			/**
			 * fill __groupInfo__ with values from group structure
			 * @return group info
			 * @param info defines structure to fill
			 * @param pw defines structure with info
			 */
			static __groupInfo__ &fillGroupInfo(__groupInfo__ &info,
											  void       *pw);

			/**
			 * @return os signal number that refers to osSignalsEnum
			 * @param signal defines signal to convert[see osSignalsEnum]
			 */
			static int toRealSignal(long signal);

			/**
			 * @return signal number that refers to given osSignalsEnum
			 * @param signal defines signal to convert[see osSignalsEnum]
			 * @note returns position in the internal structures
			 */
			static int toSignalNumber(long signal);

			/**
			 * fill 'set' structure with given signal mask
			 * @param set defines set of signals
			 * @param signal defines os signal
			 */
			static void sigMask(sigset_t *set,
								long     signal);

#ifdef DL_EXT
			static void *handlesSig[OS_SIGNALS];        ///< handles to modules
			static bool handlesOpenedSig[OS_SIGNALS];   ///< map of opened modules
#endif
			/**
			 * @class syncThreadSection
			 * @brief performs atomic locks using mutexes
			 */
			class syncThreadSection
			{
			  public:

				/**
				 * consructor
				 */
				syncThreadSection();

				/**
				 * destructor
				 */
				virtual ~syncThreadSection();

				/**
				 * lock critical section
				 */
				virtual void acquire();

				/**
				 * unlock critical section
				 */
				virtual void release();

			  protected:

				static pc::sync::thread::__lock__ keeper;  ///< mutex
			};

			static syncThreadSection keeper;    ///< lock

			/**
			 * @class syncThreadStack
			 * @brief provides thread safe behaviour
			 * @note it locks in constructor and unlocks in destructor
			 */
			class syncThreadStack
			{
			  public:

				/**
				 * contructor
				 */
				syncThreadStack();

				/**
				 * destructor
				 */
				virtual ~syncThreadStack();
			};
		};
	};
};

#endif
