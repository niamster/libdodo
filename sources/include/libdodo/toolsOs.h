/***************************************************************************
 *            toolsOs.h
 *
 *  Sat Nov 19 2005
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
#include <libdodo/pcSyncThread.h>

namespace dodo {
    namespace pc {
        namespace sync {
            class protector;
        };
    };

    namespace tools {
        /**
         * @class os
         * @brief provides os operations
         */
        class os {
          public:

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

            /**
             * @typedef daemon
             * @brief defines daemon function
             * @param data defines user data
             */
            typedef void (*daemon)(void *data);

#ifdef DL_EXT
            /**
             * @struct __signal_module__
             * @brief is returned from initSignalModule in the library
             */
            struct __signal_module__ {
                char name[64];              ///< name of the library
                char discription[256];      ///< discription of the library
                char hook[64];              ///< name of function in module that will be as a hook
                char cookie[32];            ///< cookie that would be passed to deinitModule
                long signal;                ///< signal to set handler
                int  blockSignals;          ///< signals to block during signal handling; -1 to ignore
            };

            /**
             * @typedef initSignalModule
             * @brief defines type of init function for library
             * @param data defines user data
             * @note name in the library must be initToolOsSignalModule
             */
            typedef __signal_module__ (*initSignalModule)(void *data);

            /**
             * @typedef deinitSignalModule
             * @brief defines type of deinit function for library
             * @param cookie defines cookie data returned from initModule
             * @note name in the library must be deinitToolOsSignalModule
             */
            typedef void (*deinitSignalModule)(char cookie[32]);
#endif

            /**
             * @struct __usage__
             * @brief defines process information
             */
            struct __usage__ {
                long time;                  ///< processor time of execution in milliseconds
                long mem;                   ///< memory usage in bytes
            };

            /**
             * @struct __limit__
             * @brief defines os limits
             */
            struct __limit__ {
                unsigned long current;      ///< current limit
                unsigned long max;          ///< max limit
            };

            /**
             * @enum limitEnum defines limit types
             */
            enum limitEnum {
                LIMIT_CPUTIME,
                LIMIT_MAXFILESIZE,
                LIMIT_MAXMEMUSAGE,
                LIMIT_MAXSTACK,
                LIMIT_MAXPROC,
                LIMIT_MAXOPENFILES
            };

            /**
             * @enum signalEnum defines os signals
             */
            enum signalEnum {
                SIGNAL_HANGUP = 1 << 0,
                SIGNAL_INTERRUPT = 1 << 1,
                SIGNAL_QUIT = 1 << 2,
                SIGNAL_ILLEGAL_INSTRUCTION = 1 << 3,
                SIGNAL_ABORT = 1 << 4,
                SIGNAL_BUS_FAULT = 1 << 5,
                SIGNAL_FLOATINGPOINT_FAULT = 1 << 6,
                SIGNAL_USER_DEFINED1 = 1 << 7,
                SIGNAL_SEGMENTATION_FAULT = 1 << 8,
                SIGNAL_USER_DEFINED2 = 1 << 9,
                SIGNAL_PIPE_FAULT = 1 << 10,
                SIGNAL_ALARM = 1 << 11,
                SIGNAL_TERMINATION = 1 << 12,
                SIGNAL_CHILD_CHANGED = 1 << 13,
                SIGNAL_CONTINUE = 1 << 14,
                SIGNAL_KEYBOARD_STOP = 1 << 15,
                SIGNAL_CPULIMIT_EXCEEDED = 1 << 16,
                SIGNAL_FILESIZE_EXCEEDED = 1 << 17,
                SIGNAL_BAD_SYSCALL = 1 << 18,

                SIGNAL_ENUMSIZE = 19
            };

            /**
             * @enum idEnum defines type of UID
             */
            enum idEnum {
                ID_UID,
                ID_EUID
            };

            /**
             * @struct __user__
             * @brief defines user info
             */
            struct  __user__ {
                dodoString name;            ///< user name
                dodoString pass;            ///< user password
                int        uid;             ///< user id
                int        gid;             ///< user group
                dodoString realName;        ///< user real name
                dodoString home;            ///< user home directory
                dodoString shell;           ///< user default shell
            };

            /**
             * @struct __group__
             * @brief defines group info
             */
            struct __group__ {
                dodoString      name;       ///< group name
                int             gid;        ///< group id
                dodoStringArray members;    ///< group members
            };

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
            static void daemonize(daemon func,
                                  void   *data);

            /**
             * daemonize application
             * @note the code after the call to becomeDaemon would run in background as a daemon
             */
            static void daemonize();

            /**
             * print message to stderr and terminate the program and all child processes
             * @param message defines message
             * @param status defines exit status
             * @note SIGNAL_TERMINATION is sent to the child processes
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
            static dodoString workingDir();

            /**
             * set current working directory
             * @param path defines path to the new working directory
             */
            static void setWorkingDir(const dodoString &path);

            /**
             * @return os usage info
             */
            static __usage__ usage();

            /**
             * @return os limits info
             * @param type defines type of limits, @see os::limitEnum
             */
            static __limit__ limit(short type);

            /**
             * set os limits
             * @param type defines type limits, @see os::limitEnum
             * @param lim defines os limits
             */
            static void setLimit(short           type,
                                 const __limit__ &lim);

            /**
             * @return priority of current process
             * @param type defines type of UID, @see os::idEnum
             */
            static int priority(short type);

            /**
             * set priority of current process
             * @param type defines type of UID, @see os::idEnum
             * @param prio defines priority
             */
            static void setPriority(short type,
                                    int   prio);

            /**
             * @return UID of the current process
             * @param type defines type of UID, @see os::idEnum
             */
            static int UID(short type);

            /**
             * set user id of the current process
             * @param type defines type of UID, @see os::idEnum
             * @param uid defines UID
             */
            static void setUID(short type,
                               int   uid);

            /**
             * get group id of the current process
             * @param type defines type of GID, @see os::idEnum
             */
            static int GID(short type);

            /**
             * set group id of the current process
             * @param type defines type of GID, @see os::idEnum
             * @param gid defines group id
             */
            static void setGID(short type,
                               int   gid);

            /**
             * @return user info
             * @param uid defines user id
             */
            static __user__ user(int uid);

            /**
             * @return user info
             * @param name defines user login name
             */
            static __user__ user(const dodoString &name);

            /**
             * @return users of the os
             */
            static dodoArray<__user__> users();

            /**
             * @return group info
             * @param gid defines group id
             */
            static __group__ group(int gid);

            /**
             * @return group info
             * @param name defines group name
             */
            static __group__ group(const dodoString &name);

            /**
             * @return groups of the os
             */
            static dodoArray<__group__> groups();

            /**
             * @return PID of current process
             */
            static int PID();

            /**
             * @return parent PID of current process
             */
            static int PPID();

            /**
             * @return group PID of current process
             */
            static int GPID();

            /**
             * @return group PID of given PID
             * @param pid defines PID
             */
            static int GPID(int pid);

            /**
             * set group PID of current process
             * @param gpid defines group PID where to move current process
             */
            static void setGPID(int gpid);

            /**
             * set group PID of given process
             * @param pid defines PID to move
             * @param gpid defines group PID where to move process
             */
            static void setGPID(int pid,
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
             * @param signal defines os signal, @see os::signalEnum
             * @param handler defines handle function
             * @param blockSignals defines signals to block during signal handling; -1 to ignore
             */
            static void setSignalHandler(long          signal,
                                         signalHandler handler,
                                         int           blockSignals = -1);

            /**
             * @return true if handler is set on signal
             * @param signal defines os signal, @see os::signalEnum
             */
            static bool isSignalHandled(long signal);

            /**
             * remove signal handler
             * @param signal defines os signal, @see os::signalEnum
             */
            static void removeSignalHandler(long signal);

#ifdef DL_EXT
            /**
             * @return information about module
             * @param path defines path to the library[if not in ldconfig db] or library name
             * @param toInit defines data that will be passed to the init function
             */
            static __signal_module__ module(const dodoString &path,
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
             * @param signal defines os signal, @see os::signalEnum
             */
            static void sendSignal(int  pid,
                                   long signal);

            /**
             * block or unblock signals
             * @param signal defines os signal, @see os::signalEnum
             * @param block defines block condition
             */
            static void blockSignal(long signal,
                                    bool block = true);

          protected:

            /**
             * fill __user__ with values from passwd structure
             * @return user info
             * @param info defines structure to fill
             * @param pw defines structure with info
             */
            static __user__ &fillUser(__user__ &info,
                                      void     *pw);

            /**
             * fill __group__ with values from group structure
             * @return group info
             * @param info defines structure to fill
             * @param pw defines structure with info
             */
            static __group__ &fillGroup(__group__ &info,
                                        void      *pw);

            /**
             * @return os signal number that refers to os::signalEnum
             * @param signal defines signal to convert, @see os::signalEnum
             */
            static int toRealSignal(long signal);

            /**
             * @return signal number that refers to given os::signalEnum
             * @param signal defines signal to convert, @see os::signalEnum
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
            static void *handlesSig[SIGNAL_ENUMSIZE];           ///< handles to modules
            static bool handlesOpenedSig[SIGNAL_ENUMSIZE];      ///< map of opened modules
            static char handlesCookiesSig[SIGNAL_ENUMSIZE][32]; ///< module cookies
#endif

            static pc::sync::thread keeper;                     ///< section locker
        };
    };
};
#endif
