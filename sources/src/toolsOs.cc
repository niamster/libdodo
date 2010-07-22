/***************************************************************************
 *            toolsOs.cc
 *
 *  Sat Nov 19 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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
#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifdef PTHREAD_EXT
#include <pthread.h>
#endif
#ifdef DL_EXT
#include <dlfcn.h>
#endif

#include <libdodo/pcSyncThread.h>
#include <libdodo/pcSyncStack.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsOsEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>

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
//-------------------------------------------------------------------

char os::handlesCookiesSig[][32] = {
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', },
    {'\0', }
};
#endif

//-------------------------------------------------------------------

dodo::pc::sync::thread os::keeper;

//-------------------------------------------------------------------

dodo::string
os::workingDir()
{
    char wd[PATH_MAXLEN];

    if (getcwd(wd, PATH_MAXLEN) == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_WORKINGDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    return wd;
}

//-------------------------------------------------------------------

void
os::setWorkingDir(const dodo::string &path)
{
    if (chdir(path.data()) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETWORKINGDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

os::__usage__
os::usage()
{
    rusage use;
    if (getrusage(RUSAGE_SELF, &use) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_USAGE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    __usage__ info;
    info.time = use.ru_utime.tv_sec * 100 + use.ru_utime.tv_usec;
    info.mem = use.ru_maxrss * 1024;

    return info;
}

//-------------------------------------------------------------------

void
os::changeRoot(const dodo::string &path)
{
    setWorkingDir(path);

    if (chroot(path.data()) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_CHANGEROOT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

os::__limit__
os::limit(short type)
{
    rlimit limit;

    int realRes(0);

    switch (type) {
        case LIMIT_CPUTIME:

            realRes = RLIMIT_CPU;

            break;

        case LIMIT_MAXFILESIZE:

            realRes = RLIMIT_FSIZE;

            break;

        case LIMIT_MAXMEMUSAGE:

            realRes = RLIMIT_DATA;

            break;

        case LIMIT_MAXSTACK:

            realRes = RLIMIT_STACK;

            break;

        case LIMIT_MAXPROC:

            realRes = RLIMIT_NPROC;

            break;

        case LIMIT_MAXOPENFILES:

#ifdef __FreeBSD__
            realRes = RLIMIT_NOFILE;
#else
            realRes = RLIMIT_OFILE;
#endif

            break;

        default:

            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_LIMIT, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
    }

    if (getrlimit(realRes, &limit) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_LIMIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    __limit__ lim;

    lim.current = limit.rlim_cur;
    lim.max = limit.rlim_max;

    return lim;
}

//-------------------------------------------------------------------

void
os::setLimit(short           type,
             const __limit__ &lim)
{
    rlimit limit;

    int realRes(0);

    switch (type) {
        case LIMIT_CPUTIME:

            realRes = RLIMIT_CPU;

            break;

        case LIMIT_MAXFILESIZE:

            realRes = RLIMIT_FSIZE;

            break;

        case LIMIT_MAXMEMUSAGE:

            realRes = RLIMIT_DATA;

            break;

        case LIMIT_MAXSTACK:

            realRes = RLIMIT_STACK;

            break;

        case LIMIT_MAXPROC:

            realRes = RLIMIT_NPROC;

            break;

        case LIMIT_MAXOPENFILES:

#ifdef __FreeBSD__
            realRes = RLIMIT_NOFILE;
#else
            realRes = RLIMIT_OFILE;
#endif

            break;

        default:

            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETLIMIT, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
    }

    limit.rlim_cur = lim.current;
    limit.rlim_max = lim.max;

    if (setrlimit(realRes, &limit) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETLIMIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
os::priority(short type)
{
    int prio = getpriority(PRIO_PROCESS, UID(type));
    if (prio == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_PRIORITY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    return prio;
}

//-------------------------------------------------------------------

void
os::setPriority(short type,
                int   prio)
{
    if (setpriority(PRIO_PROCESS, UID(type), prio) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETPRIORITY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
os::UID(short type)
{
    switch (type) {
        case ID_UID:

            return getuid();

        case ID_EUID:

            return geteuid();

        default:

            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_UID, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
    }
}

//-------------------------------------------------------------------

void
os::setUID(short type,
           int   uid)
{
    int res(0);

    switch (type) {
        case ID_UID:

            res = setuid(uid);

            break;

        case ID_EUID:

            res = seteuid(uid);

            break;

        default:

            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETUID, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
    }

    if (res == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETUID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
os::GID(short type)
{
    switch (type) {
        case ID_UID:

            return getgid();

        case ID_EUID:

            return getegid();

        default:

            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_GID, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
    }
}

//-------------------------------------------------------------------

void
os::setGID(short type,
           int   uid)
{
    int res(0);

    switch (type) {
        case ID_UID:

            res = setgid(uid);

            break;

        case ID_EUID:

            res = setegid(uid);

            break;

        default:

            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETGID, exception::ERRNO_LIBDODO, OSEX_WRONGPARAMETER, TOOLSOSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
    }

    if (res == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETGID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

os::__user__
os::user(int uid)
{
    passwd *in = getpwuid(uid);
    if (in == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_USER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    __user__ info;

    return fillUser(info, in);
}

//-------------------------------------------------------------------

os::__user__
os::user(const dodo::string &uid)
{
    passwd *in = getpwnam(uid.data());
    if (in == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_USER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    __user__ info;

    return fillUser(info, in);
}

//-------------------------------------------------------------------

dodoArray<os::__user__>
os::users()
{
    dodoArray<__user__> users;

    passwd *in;

    __user__ info;

    while ((in = getpwent()) != NULL)
        users.push_back(fillUser(info, in));

    switch (errno) {
        case EIO:
        case EMFILE:
        case ENFILE:
        case ENOMEM:

            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_USERS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    endpwent();

    return users;
}

//-------------------------------------------------------------------

os::__user__ &
os::fillUser(__user__ &info,
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

os::__group__ &
os::fillGroup(__group__ &info,
              void      *pw)
{
    info.gid = ((::group *)pw)->gr_gid;
    info.name = ((::group *)pw)->gr_name;

    info.members.clear();

    int i(0);

    while (((::group *)pw)->gr_mem[i] != NULL)
        info.members.push_back(((::group *)pw)->gr_mem[i++]);

    return info;
}

//-------------------------------------------------------------------

os::__group__
os::group(int uid)
{
    ::group *in = getgrgid(uid);
    if (in == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_GROUP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    __group__ info;

    return fillGroup(info, in);
}

//-------------------------------------------------------------------

os::__group__
os::group(const dodo::string &uid)
{
    ::group *in = getgrnam(uid.data());
    if (in == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_GROUP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    __group__ info;

    return fillGroup(info, in);
}

//-------------------------------------------------------------------

dodoArray<os::__group__>
os::groups()
{
    dodoArray<__group__> groups;

    ::group *in;

    __group__ info;

    while ((in = getgrent()) != NULL)
        groups.push_back(fillGroup(info, in));

    switch (errno) {
        case EIO:
        case EMFILE:
        case ENFILE:
        case EINTR:
        case ENOMEM:

            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_GROUPS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    endgrent();

    return groups;
}

//-------------------------------------------------------------------

void
os::die(const dodo::string &message,
        int              status)
{
    unsigned short tries = DIE_MAXTRIES;

    while (tries-- > 0)
        if (fwrite(message.data(), message.size(), 1, stderr) == 1)
            break;
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
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_ATEXIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
os::PID()
{
    return getpid();
}

//-------------------------------------------------------------------

int
os::PPID()
{
    return getppid();
}

//-------------------------------------------------------------------

int
os::GPID()
{
    return getpgrp();
}

//-------------------------------------------------------------------

int
os::GPID(int pid)
{
    int pgid = getpgid(pid);
    if (pgid == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_GPID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    return pgid;
}

//-------------------------------------------------------------------

void
os::setGPID(int gpid)
{
    if (setpgid(0, gpid) == 1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETGROUPPID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::setGPID(int pid,
            int gpid)
{
    if (setpgid(pid, gpid) == 1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETGROUPPID, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::sigMask(void *set,
            long     blockSignals)
{
    if (blockSignals != -1) {
        if (isSetFlag(blockSignals, SIGNAL_HANGUP))
            sigaddset((sigset_t *)set, SIGHUP);

        if (isSetFlag(blockSignals, SIGNAL_INTERRUPT))
            sigaddset((sigset_t *)set, SIGINT);

        if (isSetFlag(blockSignals, SIGNAL_QUIT))
            sigaddset((sigset_t *)set, SIGQUIT);

        if (isSetFlag(blockSignals, SIGNAL_ILLEGAL_INSTRUCTION))
            sigaddset((sigset_t *)set, SIGILL);

        if (isSetFlag(blockSignals, SIGNAL_ABORT))
            sigaddset((sigset_t *)set, SIGABRT);

        if (isSetFlag(blockSignals, SIGNAL_BUS_FAULT))
            sigaddset((sigset_t *)set, SIGBUS);

        if (isSetFlag(blockSignals, SIGNAL_FLOATINGPOINT_FAULT))
            sigaddset((sigset_t *)set, SIGFPE);

        if (isSetFlag(blockSignals, SIGNAL_USER_DEFINED1))
            sigaddset((sigset_t *)set, SIGUSR1);

        if (isSetFlag(blockSignals, SIGNAL_SEGMENTATION_FAULT))
            sigaddset((sigset_t *)set, SIGSEGV);

        if (isSetFlag(blockSignals, SIGNAL_USER_DEFINED2))
            sigaddset((sigset_t *)set, SIGUSR2);

        if (isSetFlag(blockSignals, SIGNAL_PIPE_FAULT))
            sigaddset((sigset_t *)set, SIGPIPE);

        if (isSetFlag(blockSignals, SIGNAL_ALARM))
            sigaddset((sigset_t *)set, SIGALRM);

        if (isSetFlag(blockSignals, SIGNAL_TERMINATION))
            sigaddset((sigset_t *)set, SIGTERM);

        if (isSetFlag(blockSignals, SIGNAL_CHILD_CHANGED))
            sigaddset((sigset_t *)set, SIGCHLD);

        if (isSetFlag(blockSignals, SIGNAL_CONTINUE))
            sigaddset((sigset_t *)set, SIGCONT);

        if (isSetFlag(blockSignals, SIGNAL_KEYBOARD_STOP))
            sigaddset((sigset_t *)set, SIGTSTP);

        if (isSetFlag(blockSignals, SIGNAL_CPULIMIT_EXCEEDED))
            sigaddset((sigset_t *)set, SIGXCPU);

        if (isSetFlag(blockSignals, SIGNAL_FILESIZE_EXCEEDED))
            sigaddset((sigset_t *)set, SIGXFSZ);

        if (isSetFlag(blockSignals, SIGNAL_BAD_SYSCALL))
            sigaddset((sigset_t *)set, SIGSYS);
    }
}

//-------------------------------------------------------------------

void
os::setSignalHandler(long          signal,
                     signalHandler handler,
                     int           blockSignals)
{
    pc::sync::stack tg(&keeper);

#ifdef DL_EXT
    deinitSignalModule deinit;

    int handleSignal = toSignalNumber(signal);
    if (handleSignal > 0 && handlesOpenedSig[handleSignal]) {
        deinit = (deinitSignalModule)dlsym(handlesSig[handleSignal], "deinitToolsOsSignalModule");
        if (deinit != NULL)
            deinit(handlesCookiesSig[handleSignal]);

#ifndef DL_FAST
        dlclose(handlesSig[handleSignal]);
#endif

        handlesOpenedSig[handleSignal] = false;
        handlesSig[handleSignal] = NULL;
    }
#endif

    struct sigaction act;
    act.sa_sigaction = (void(*) (int, siginfo_t *, void*))handler;
    act.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigemptyset(&act.sa_mask) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    sigMask(&act.sa_mask, blockSignals);

    if (sigaction(os::toRealSignal(signal), &act, NULL) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::setMicroTimer(unsigned long timeout,
                  signalHandler handler,
                  int           blockSignals)
{
    pc::sync::stack tg(&keeper);

#ifdef DL_EXT
    deinitSignalModule deinit;

    int handleSignal = toSignalNumber(SIGNAL_ALARM);
    if (handleSignal > 0 && handlesOpenedSig[handleSignal]) {
        deinit = (deinitSignalModule)dlsym(handlesSig[handleSignal], "deinitToolsOsSignalModule");
        if (deinit != NULL)
            deinit(handlesCookiesSig[handleSignal]);

#ifndef DL_FAST
        dlclose(handlesSig[handleSignal]);
#endif

        handlesOpenedSig[handleSignal] = false;
        handlesSig[handleSignal] = NULL;
    }
#endif

    struct sigaction act;
    act.sa_sigaction = (void(*) (int, siginfo_t *, void*))handler;
    act.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigemptyset(&act.sa_mask) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETMICROTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    sigMask(&act.sa_mask, blockSignals);

    long tSec = 0, tMicrosec = timeout;

    if (timeout > 1000000) {
        tSec = timeout / 1000000;
        tMicrosec = timeout % 1000000;
    }

    itimerval value;
    value.it_interval.tv_sec = tSec;
    value.it_interval.tv_usec = tMicrosec;
    value.it_value.tv_sec = tSec;
    value.it_value.tv_usec = tMicrosec;

    if (sigaction(SIGALRM, &act, NULL) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETMICROTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    if (setitimer(ITIMER_REAL, &value, NULL) != 0)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETMICROTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::setTimer(long          timeout,
             signalHandler handler,
             int           blockSignals)
{
    pc::sync::stack tg(&keeper);

#ifdef DL_EXT
    deinitSignalModule deinit;

    int handleSignal = toSignalNumber(SIGNAL_ALARM);
    if (handleSignal > 0 && handlesOpenedSig[handleSignal]) {
        deinit = (deinitSignalModule)dlsym(handlesSig[handleSignal], "deinitToolsOsSignalModule");
        if (deinit != NULL)
            deinit(handlesCookiesSig[handleSignal]);

#ifndef DL_FAST
        dlclose(handlesSig[handleSignal]);
#endif

        handlesOpenedSig[handleSignal] = false;
        handlesSig[handleSignal] = NULL;
    }
#endif

    struct sigaction act;
    act.sa_sigaction = (void(*) (int, siginfo_t *, void*))handler;
    act.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigemptyset(&act.sa_mask) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    sigMask(&act.sa_mask, blockSignals);

    itimerval value;
    value.it_interval.tv_sec = timeout;
    value.it_interval.tv_usec = 0;
    value.it_value.tv_sec = timeout;
    value.it_value.tv_usec = 0;

    if (sigaction(SIGALRM, &act, NULL) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    if (setitimer(ITIMER_REAL, &value, NULL) != 0)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETTIMER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
os::isSignalHandled(long signal)
{
    struct sigaction act;
    if (sigaction(os::toRealSignal(signal), NULL, &act) == 1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    if (act.sa_sigaction != NULL || act.sa_handler != NULL)
        return true;
    else
        return false;
}

//-------------------------------------------------------------------

void
os::sendSignal(int  pid,
               long signal)
{
    if (kill(pid, os::toRealSignal(signal)) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SENDSIGNAL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
os::removeSignalHandler(long signal)
{
    pc::sync::stack tg(&keeper);

#ifdef DL_EXT
    deinitSignalModule deinit;

    int handleSignal = toSignalNumber(signal);
    if (handleSignal > 0 && handlesOpenedSig[handleSignal]) {
        deinit = (deinitSignalModule)dlsym(handlesSig[handleSignal], "deinitToolsOsSignalModule");
        if (deinit != NULL)
            deinit(handlesCookiesSig[handleSignal]);

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
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_REMOVESIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

#ifdef DL_EXT
os::__signal_module__
os::module(const dodo::string &module,
           void             *toInit)
{
#ifdef DL_FAST
    void *handle = dlopen(module.data(), RTLD_LAZY | RTLD_NODELETE);
#else
    void *handle = dlopen(module.data(), RTLD_LAZY);
#endif
    if (handle == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

    initSignalModule init = (initSignalModule)dlsym(handle, "initToolsOsSignalModule");
    if (init == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

    __signal_module__ mod = init(toInit);

    deinitSignalModule deinit = (deinitSignalModule)dlsym(handle, "deinitToolsOsSignalModule");
    if (init != NULL)
        deinit(mod.cookie);


#ifndef DL_FAST
    if (dlclose(handle) != 0)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif

    return mod;
}

//-------------------------------------------------------------------

void
os::setSignalHandler(const dodo::string &path,
                     void             *toInit)
{
    pc::sync::stack tg(&keeper);

#ifdef DL_FAST
    void *handle = dlopen(path.data(), RTLD_LAZY | RTLD_NODELETE);
#else
    void *handle = dlopen(path.data(), RTLD_LAZY);
#endif
    if (handle == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    initSignalModule init = (initSignalModule)dlsym(handle, "initToolsOsSignalModule");
    if (init == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    __signal_module__ mod = init(toInit);

    deinitSignalModule deinit;

    int handleSignal = toSignalNumber(mod.signal);
    if (handleSignal > 0 && handlesOpenedSig[handleSignal]) {
        deinit = (deinitSignalModule)dlsym(handlesSig[handleSignal], "deinitToolsOsSignalModule");
        if (deinit != NULL)
            deinit(handlesCookiesSig[handleSignal]);

#ifndef DL_FAST
        dlclose(handlesSig[handleSignal]);
#endif

        handlesOpenedSig[handleSignal] = false;
        handlesSig[handleSignal] = NULL;
    }

    handlesSig[handleSignal] = handle;

    signalHandler in = (signalHandler)dlsym(handlesSig[mod.signal], mod.hook);
    if (in == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    struct sigaction act;
    act.sa_sigaction = (void(*) (int, siginfo_t *, void*))in;
    act.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigemptyset(&act.sa_mask) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    sigMask(&act.sa_mask, mod.blockSignals);

    if (sigaction(os::toRealSignal(mod.signal), &act, NULL) == 1)
        dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_SETSIGNALHANDLER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    handlesOpenedSig[handleSignal] = true;
}
#endif

//-------------------------------------------------------------------

int
os::toSignalNumber(long signal)
{
    switch (signal) {
        case SIGNAL_HANGUP:

            return 0;

        case SIGNAL_INTERRUPT:

            return 1;

        case SIGNAL_QUIT:

            return 2;

        case SIGNAL_ILLEGAL_INSTRUCTION:

            return 3;

        case SIGNAL_ABORT:

            return 4;

        case SIGNAL_BUS_FAULT:

            return 5;

        case SIGNAL_FLOATINGPOINT_FAULT:

            return 6;

        case SIGNAL_USER_DEFINED1:

            return 7;

        case SIGNAL_SEGMENTATION_FAULT:

            return 8;

        case SIGNAL_USER_DEFINED2:

            return 9;

        case SIGNAL_PIPE_FAULT:

            return 10;

        case SIGNAL_ALARM:

            return 11;

        case SIGNAL_TERMINATION:

            return 12;

        case SIGNAL_CHILD_CHANGED:

            return 13;

        case SIGNAL_CONTINUE:

            return 14;

        case SIGNAL_KEYBOARD_STOP:

            return 15;

        case SIGNAL_CPULIMIT_EXCEEDED:

            return 16;

        case SIGNAL_FILESIZE_EXCEEDED:

            return 17;

        case SIGNAL_BAD_SYSCALL:

            return 18;

        default:

            return -1;
    }
}

//-------------------------------------------------------------------

int
os::toRealSignal(long signal)
{
    switch (signal) {
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

    if (pid == 0) {
        if (setsid() == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        if (close(0) == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        if (close(1) == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        if (close(2) == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        if (chdir("/") == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    } else {
        if (pid == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        else
            _exit(0);
    }
}

//-------------------------------------------------------------------

void
os::daemonize(daemon func,
              void   *data)
{
    pid_t pid = fork();

    if (pid == 0) {
        if (setsid() == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        if (close(0) == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        if (close(1) == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        if (close(2) == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        if (chdir("/") == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        func(data);

        exit(0);
    } else {
        if (pid == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSOS, OSEX_DAEMONIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        else
            _exit(0);
    }
}

//-------------------------------------------------------------------
