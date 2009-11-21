/***************************************************************************
 *            pcExecutionProcess.cc
 *
 *  Wed Oct 08 2009
 *  Copyright  2009  Ni@m
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

#ifdef DL_EXT
#include <dlfcn.h>
#endif

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

namespace dodo {
    namespace pc {
        namespace execution {
            /**
             * @struct __process__
             * @brief defines process information
             */
            struct __process__ {
                /**
                 * constuctor
                 */
                __process__() : executed(false),
                                joined(false),
                                status(0)
#ifdef DL_EXT
                              ,
                                handle(NULL)
#endif
                {
#ifdef DL_EXT
                    memset(cookie, 0x0, 32);
#endif
                }

                pid_t           pid;                ///< process pid
                void            *data;              ///< process data
                bool            executed;           ///< true if the process was executed
                bool            joined;             ///< true if the process was joined
                int             status;             ///< process exit status
                void            *func;              ///< function to execute
                short           action;             ///< action on object destruction[@see onDestructionEnum]
#ifdef DL_EXT
                void            *handle;            ///< handle to library
                char            cookie[32];         ///< cookie that would be passed to deinitModule
#endif
            };
        };
    };
};

#include <libdodo/pcExecutionProcess.h>
#include <libdodo/types.h>
#include <libdodo/pcExecutionJob.h>
#include <libdodo/pcExecutionProcessEx.h>

using namespace dodo::pc::execution;

process::process(const process &p) : job(p),
                                     handle(new __process__)
{
    *handle = *p.handle;
}

//-------------------------------------------------------------------

process::process(routine func,
                 void    *data,
                 short   action) : job(TYPE_PROCESS),
                                   handle(new __process__)
{
    handle->data = data;
    handle->func = (void *)func;
    handle->action = action;

#ifdef DL_EXT
    handle->handle = NULL;
#endif
}

//-------------------------------------------------------------------

#ifdef DL_EXT
process::process(const dodoString &module,
                 void             *data,
                 void             *toInit)
try : job(TYPE_PROCESS),
      handle(NULL)
    {
        handle = new __process__;

        handle->data = data;

#ifdef DL_FAST
        handle->handle = dlopen(module.data(), RTLD_LAZY | RTLD_NODELETE);
#else
        handle->handle = dlopen(module.data(), RTLD_LAZY);
#endif
        if (handle->handle == NULL)
            throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_CONSTRUCTOR, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

        initModule init = (initModule)dlsym(handle->handle, "initPcExecutionProcessModule");
        if (init == NULL)
            throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_CONSTRUCTOR, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

        __module__ m = init(toInit);

        void *f = dlsym(handle->handle, m.hook);
        if (f == NULL)
            throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_CONSTRUCTOR, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

        handle->action = m.action;
        handle->func = f;
        memcpy(handle->cookie, m.cookie, 32);
    } catch (...) {
        if (handle) {
            if (handle->handle)
                dlclose(handle->handle);

            delete handle;
        }
    }
#endif

//-------------------------------------------------------------------

process::~process()
{
    if (!cloned) {
#ifdef DL_EXT
        deinitModule deinit;
#endif

        if (isRunning()) {
            switch (handle->action) {
                case ON_DESTRUCTION_KEEP_ALIVE:

                    waitpid(handle->pid, NULL, WNOHANG);

                    break;

                case ON_DESTRUCTION_STOP:

                    kill(handle->pid, 2);

                    break;

                case ON_DESTRUCTION_WAIT:
                default:

                    waitpid(handle->pid, NULL, 0);
            }
        }

#ifdef DL_EXT
        if (handle->handle != NULL) {
            deinit = (deinitModule)dlsym(handle->handle, "deinitPcExecutionProcessModule");
            if (deinit != NULL)
                deinit(handle->cookie);

#ifndef DL_FAST
            dlclose(handle->handle);
#endif
        }
#endif
    }

    delete handle;
}

//-------------------------------------------------------------------

bool
process::isRunning() const
{
    if (!handle->executed)
        return false;

    int res = kill(handle->pid, 0);
    if (res != 0) {
        if (errno == ESRCH)
            return false;

        throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_ISRUNNING, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    return true;
}

//-------------------------------------------------------------------

void
process::run()
{
    if (isRunning())
        throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_RUN, exception::ERRNO_LIBDODO, PROCESSEX_ISALREADYRUNNING, PCEXECUTIONPROCESSEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);

    pid_t pid = fork();

    if (pid == 0) {
        _exit(((routine)handle->func)(handle->data));
    } else {
        if (pid == -1)
            throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_RUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        else
            handle->pid = pid;
    }

    handle->executed = true;
    handle->joined = false;
}

//-------------------------------------------------------------------

void
process::stop()
{
    if (kill(handle->pid, 9) == -1)
        throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    handle->executed = false;
}

//-------------------------------------------------------------------

int
process::wait()
{
    if (handle->joined)
        return handle->status;

    if (!handle->executed)
        throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_WAIT, exception::ERRNO_LIBDODO, PROCESSEX_ISNOTLAUNCHED, PCEXECUTIONPROCESSEX_ISNOTLAUNCHED_STR, __LINE__, __FILE__);

    int status;

    if (waitpid(handle->pid, &status, 0) == -1)
        throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    if (WIFEXITED(status))
        handle->status = WEXITSTATUS(status);

    handle->executed = false;
    handle->joined = true;

    return handle->status;
}

//-------------------------------------------------------------------

#ifdef DL_EXT
process::__module__
process::module(const dodoString &module,
                void             *toInit)
{
#ifdef DL_FAST
    void *handle = dlopen(module.data(), RTLD_LAZY | RTLD_NODELETE);
#else
    void *handle = dlopen(module.data(), RTLD_LAZY);
#endif
    if (handle == NULL)
        throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

    initModule init = (initModule)dlsym(handle, "initPcExecutionProcessModule");
    if (init == NULL)
        throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

    __module__ mod = init(toInit);

    deinitModule deinit = (deinitModule)dlsym(handle, "deinitPcExecutionProcessModule");
    if (deinit != NULL)
        deinit(mod.cookie);

#ifndef DL_FAST
    if (dlclose(handle) != 0)
        throw exception::basic(exception::MODULE_PCEXECUTIONPROCESS, PROCESSEX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif

    return mod;
}
#endif

//-------------------------------------------------------------------

