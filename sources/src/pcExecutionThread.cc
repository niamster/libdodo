/***************************************************************************
 *            pcJobThreadThread.cc
 *
 *  Wed Oct 08 2009
 *  Copyright  2009  Dmytro Milinevskyy
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

#ifdef DL_EXT
#include <dlfcn.h>
#endif
#ifdef PTHREAD_EXT
#include <pthread.h>
#endif
#include <signal.h>
#include <errno.h>
#include <string.h>

namespace dodo {
    namespace exception {
        class basic;
    };

    namespace pc {
        namespace execution {
            /**
             * @struct __thread__
             * @brief defines process information
             */
            struct __thread__ {
                /**
                 * contructor
                 */
                __thread__() :
#ifdef PTHREAD_EXT
                    thread(0),
#endif
                    executed(false),
                    joined(false),
                    status(0),
                    ex(NULL)
#ifdef DL_EXT
                    ,
                    handle(NULL)
#endif
                {
#ifdef DL_EXT
                    memset(cookie, 0x0, 32);
#endif
                }

#ifdef PTHREAD_EXT
                pthread_t      thread;              ///< thread descriptor
                pthread_attr_t attr;                ///< thread attribute

                /**
                 * @return thread exit status
                 * @param data defines user data
                 */
                static void *routine(void *data);
#endif

                void           *func;               ///< function to execute
                void           *data;               ///< thread data

                bool           executed;            ///< true if thread is running
                bool           joined;              ///< true if the thread was joined
                int            status;              ///< thread exit status
                bool           detached;            ///< true if thread is detached
                short          action;              ///< action on object destruction[@see onDestructionEnum]

                exception::basic *ex; ///< uncought exception dodo_rethrown by thread routine

#ifdef DL_EXT
                void           *handle;             ///< handle to library
                char            cookie[32];         ///< cookie that would be passed to deinitModule
#endif
            };
        };
    };
};

#include <libdodo/pcExecutionThread.h>
#include <libdodo/pcExecutionJob.h>
#include <libdodo/toolsOs.h>
#include <libdodo/pcExecutionThreadEx.h>
#include <libdodo/types.h>
#include <libdodo/exceptionBasic.h>

using namespace dodo::pc::execution;

#ifdef PTHREAD_EXT
void *
__thread__::routine(void *data)
{
    __thread__ *ti = (__thread__ *)data;

    dodo_try {
        delete ti->ex;
        ti->ex = NULL;

        ti->status = ((execution::routine)ti->func)(ti->data);
    } dodo_catch (exception::basic *e UNUSED) {
        /* FIXME */
        ti->ex = new exception::basic(*e);
        ti->status = 0;
    }

    return NULL;
}
#endif

//-------------------------------------------------------------------

thread::thread(const thread &t) : job(t),
                                  handle(new __thread__)
{
    *handle = *t.handle;
}

//-------------------------------------------------------------------

thread::thread(routine       func,
               void          *data,
               short         action,
               bool          detached,
               unsigned long stackSize)
    /* FIXME */
/* dodo_try */ : job(TYPE_THREAD),
      handle(NULL)
    {
        handle = new __thread__;

#ifdef PTHREAD_EXT
        pthread_attr_init(&handle->attr);
#endif

        handle->detached = detached;
        handle->data = data;
        handle->func = (void *)func;
        handle->action = action;

#ifdef DL_EXT
        handle->handle = NULL;
#endif

#ifdef PTHREAD_EXT
        if (detached)
            pthread_attr_setdetachstate(&handle->attr, PTHREAD_CREATE_DETACHED);
        else
            pthread_attr_setdetachstate(&handle->attr, PTHREAD_CREATE_JOINABLE);

        errno = pthread_attr_setstacksize(&handle->attr, stackSize);
        if (errno != 0)
            dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#endif

#ifdef PTHREAD_EXT
        pthread_attr_destroy(&handle->attr);
#endif
    }/*  dodo_catch (exception::basic *e UNUSED) { */
/*         if (handle) { */
/* #ifdef PTHREAD_EXT */
/*             pthread_attr_destroy(&handle->attr); */
/* #endif */
/*             delete handle; */
/*         } */
/*     } */

#ifdef DL_EXT
thread::thread(const dodo::string &module,
               void             *data,
               void             *toInit)
    /* FIXME */
/* dodo_try */ : job(TYPE_THREAD),
      handle(NULL)
    {
        handle = new __thread__;

        handle->data = data;

#ifdef DL_FAST
        handle->handle = dlopen(module.data(), RTLD_LAZY | RTLD_NODELETE);
#else
        handle->handle = dlopen(module.data(), RTLD_LAZY);
#endif
        if (handle->handle == NULL)
            dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

        initModule init = (initModule)dlsym(handle->handle, "initPcExecutionThreadModule");
        if (init == NULL)
            dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

        __module__ m = init(toInit);

        void *f = dlsym(handle->handle, m.hook);
        if (f == NULL)
            dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

        if (m.detached)
            pthread_attr_setdetachstate(&handle->attr, PTHREAD_CREATE_DETACHED);
        else
            pthread_attr_setdetachstate(&handle->attr, PTHREAD_CREATE_JOINABLE);

        errno = pthread_attr_setstacksize(&handle->attr, m.stackSize);
        if (errno != 0)
            dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_CONSTRUCTOR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        handle->detached = m.detached;
        handle->action = m.action;
        handle->func = f;
        memcpy(handle->cookie, m.cookie, 32);
    } /* dodo_catch (exception::basic *e UNUSED) { */
/*         if (handle) { */
/*             if (handle->handle) */
/*                 dlclose(handle->handle); */

/* #ifdef PTHREAD_EXT */
/*             pthread_attr_destroy(&handle->attr); */
/* #endif */

/*             delete handle; */
/*         } */
/*     } */
#endif

//-------------------------------------------------------------------

thread::~thread()
{
    if (!cloned) {
#ifdef DL_EXT
        deinitModule deinit;
#endif

#ifdef PTHREAD_EXT
        pthread_attr_destroy(&handle->attr);
#endif

        if (!isRunning() || handle->detached)
            return;

        switch (handle->action) {
            case ON_DESTRUCTION_KEEP_ALIVE:

#ifdef PTHREAD_EXT
                pthread_detach(handle->thread);
#endif

                break;

            case ON_DESTRUCTION_STOP:

#ifdef PTHREAD_EXT
                pthread_cancel(handle->thread);
#endif

                break;

            case ON_DESTRUCTION_WAIT:
            default:

#ifdef PTHREAD_EXT
                pthread_join(handle->thread, NULL);
#endif

                break;
        }

#ifdef DL_EXT
        if (handle->handle != NULL) {
            deinit = (deinitModule)dlsym(handle->handle, "deinitPcExecutionThreadModule");
            if (deinit != NULL)
                deinit(handle->cookie);

#ifndef DL_FAST
            dlclose(handle->handle);
#endif
        }
#endif
    }

    delete handle->ex;

    delete handle;
}

//-------------------------------------------------------------------

void
thread::run()
{
    if (isRunning())
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_RUN, exception::ERRNO_LIBDODO, THREADEX_ISALREADYRUNNING, PCEXECUTIONTHREADEX_ISALREADYRUNNING_STR, __LINE__, __FILE__);

#ifdef PTHREAD_EXT
    errno = pthread_create(&handle->thread, &handle->attr, __thread__::routine, handle);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_RUN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#endif

    handle->executed = true;
}

//-------------------------------------------------------------------

int
thread::wait()
{
    if (handle->detached) {
        if (isRunning())
            dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_WAIT, exception::ERRNO_LIBDODO, THREADEX_ISDETACHED, PCEXECUTIONTHREADEX_ISDETACHED_STR, __LINE__, __FILE__);
        else
            return handle->status;
    }

    if (handle->joined)
        return handle->status;

    if (!handle->executed)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_WAIT, exception::ERRNO_LIBDODO, THREADEX_ISNOTLAUNCHED, PCEXECUTIONTHREADEX_ISNOTLAUNCHED_STR, __LINE__, __FILE__);

    int status = 0;

#ifdef PTHREAD_EXT
    errno = pthread_join(handle->thread, NULL);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_WAIT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    status = handle->status;
#endif

    handle->joined = true;
    handle->executed = false;

    return status;
}

//-------------------------------------------------------------------

void
thread::stop()
{
#ifdef PTHREAD_EXT
    errno = pthread_cancel(handle->thread);
    if (errno != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_STOP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#endif

    handle->executed = false;
}

//-------------------------------------------------------------------

bool
thread::isRunning() const
{
    if (!handle->executed)
        return false;

#ifdef PTHREAD_EXT
    errno = pthread_kill(handle->thread, 0);
    if (errno != 0) {
        if (errno == ESRCH || errno == EAGAIN)
            return false;

        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_ISRUNNING, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }
#endif

    return true;
}

//-------------------------------------------------------------------

#ifdef DL_EXT
thread::__module__
thread::module(const dodo::string &module,
               void             *toInit)
{
#ifdef DL_FAST
    void *handle = dlopen(module.data(), RTLD_LAZY | RTLD_NODELETE);
#else
    void *handle = dlopen(module.data(), RTLD_LAZY);
#endif
    if (handle == NULL)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

    initModule init = (initModule)dlsym(handle, "initPcExecutionThreadModule");
    if (init == NULL)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

    __module__ mod = init(toInit);

    deinitModule deinit = (deinitModule)dlsym(handle, "deinitPcExecutionThreadModule");
    if (deinit != NULL)
        deinit(mod.cookie);

#ifndef DL_FAST
    if (dlclose(handle) != 0)
        dodo_throw exception::basic(exception::MODULE_PCEXECUTIONTHREAD, THREADEX_MODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

    return mod;
}
#endif

//-------------------------------------------------------------------

dodo::exception::basic *
thread::exception()
{
    return handle->ex;
}

//-------------------------------------------------------------------

bool
thread::self() const
{
    return (pthread_equal(pthread_self(), handle->thread) > 0);
}

//-------------------------------------------------------------------
