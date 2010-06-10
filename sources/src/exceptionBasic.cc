/***************************************************************************
 *            exceptionBasic.cc
 *
 *  Sun Jul 17 2005
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

#include <libdodo/directives.h>

#ifdef DL_EXT
#include <dlfcn.h>
#include <string.h>
#endif
#ifdef __GNUC__
#include <cxxabi.h>
#endif
#ifdef PTHREAD_EXT
#include <pthread.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include <libdodo/exceptionBasic.h>

#define CALLSTACK_MAXLEN 64

using namespace dodo::exception;

unsigned long basic::instances = 0;

//-------------------------------------------------------------------

bool basic::handlerMap[] = {
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

basic::handler basic::handlers[] = {
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

void *basic::handlerData[] = {
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

#ifdef DL_EXT
bool basic::handlesOpened[] = {
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

void *basic::handles[] = {
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

char basic::cookies[][32] = {
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

basic::sync::stack::stack()
{
#ifdef PTHREAD_EXT
    static pthread_mutex_t mutex;
    static unsigned char mutex_init = 0;
    if (!mutex_init) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

        pthread_mutex_init(&mutex, &attr);

        pthread_mutexattr_destroy(&attr);

        mutex_init = 1;
    }

    keeper = &mutex;

    pthread_mutex_lock((pthread_mutex_t *)keeper);
#endif
}

//-------------------------------------------------------------------

basic::sync::stack::~stack()
{
#ifdef PTHREAD_EXT
    pthread_mutex_unlock((pthread_mutex_t *)keeper);
#endif
}

//-------------------------------------------------------------------

basic::basic() throw ()
{
    instances = 1;
}

//-------------------------------------------------------------------

basic::basic(int              a_module,
             int              functionID,
             int              errnoSource,
             int              a_errno,
             const dodo::string &a_errstr,
             unsigned long    a_line,
             const dodo::string &a_file,
             const dodo::string &a_message) throw () : source(a_module),
                                                     function(functionID),
                                                     errnoSource(errnoSource),
                                                     errNo(a_errno),
                                                     errStr(a_errstr),
                                                     line(a_line),
                                                     file(a_file),
                                                     message(a_message)
{
    sync::stack tg;

#ifdef __GNUC__
    using namespace abi;

    __call__ call;

#ifdef DL_EXT
    Dl_info dlinfo;
    const char *symname;
    char *demangled;
    int status;
#endif

    for (int i = 0; i < CALLSTACK_MAXLEN; ++i) {
        switch (i) {
            case 0:
                if (!__builtin_frame_address(0))
                    goto handle;
                call.address = __builtin_return_address(0);
                break;

            case 1:
                if (!__builtin_frame_address(1))
                    goto handle;
                call.address = __builtin_return_address(1);
                break;

            case 2:
                if (!__builtin_frame_address(2))
                    goto handle;
                call.address = __builtin_return_address(2);
                break;

            case 3:
                if (!__builtin_frame_address(3))
                    goto handle;
                call.address = __builtin_return_address(3);
                break;

            case 4:
                if (!__builtin_frame_address(4))
                    goto handle;
                call.address = __builtin_return_address(4);
                break;

            case 5:
                if (!__builtin_frame_address(5))
                    goto handle;
                call.address = __builtin_return_address(5);
                break;

            case 6:
                if (!__builtin_frame_address(6))
                    goto handle;
                call.address = __builtin_return_address(6);
                break;

            case 7:
                if (!__builtin_frame_address(7))
                    goto handle;
                call.address = __builtin_return_address(7);
                break;

            case 8:
                if (!__builtin_frame_address(8))
                    goto handle;
                call.address = __builtin_return_address(8);
                break;

            case 9:
                if (!__builtin_frame_address(9))
                    goto handle;
                call.address = __builtin_return_address(9);
                break;

            case 10:
                if (!__builtin_frame_address(10))
                    goto handle;
                call.address = __builtin_return_address(10);
                break;

            case 11:
                if (!__builtin_frame_address(11))
                    goto handle;
                call.address = __builtin_return_address(11);
                break;

            case 12:
                if (!__builtin_frame_address(12))
                    goto handle;
                call.address = __builtin_return_address(12);
                break;

            case 13:
                if (!__builtin_frame_address(13))
                    goto handle;
                call.address = __builtin_return_address(13);
                break;

            case 14:
                if (!__builtin_frame_address(14))
                    goto handle;
                call.address = __builtin_return_address(14);
                break;

            case 15:
                if (!__builtin_frame_address(15))
                    goto handle;
                call.address = __builtin_return_address(15);
                break;

            case 16:
                if (!__builtin_frame_address(16))
                    goto handle;
                call.address = __builtin_return_address(16);
                break;

            case 17:
                if (!__builtin_frame_address(17))
                    goto handle;
                call.address = __builtin_return_address(17);
                break;

            case 18:
                if (!__builtin_frame_address(18))
                    goto handle;
                call.address = __builtin_return_address(18);
                break;

            case 19:
                if (!__builtin_frame_address(19))
                    goto handle;
                call.address = __builtin_return_address(19);
                break;

            case 20:
                if (!__builtin_frame_address(20))
                    goto handle;
                call.address = __builtin_return_address(20);
                break;

            case 21:
                if (!__builtin_frame_address(21))
                    goto handle;
                call.address = __builtin_return_address(21);
                break;

            case 22:
                if (!__builtin_frame_address(22))
                    goto handle;
                call.address = __builtin_return_address(22);
                break;

            case 23:
                if (!__builtin_frame_address(23))
                    goto handle;
                call.address = __builtin_return_address(23);
                break;

            case 24:
                if (!__builtin_frame_address(24))
                    goto handle;
                call.address = __builtin_return_address(24);
                break;

            case 25:
                if (!__builtin_frame_address(25))
                    goto handle;
                call.address = __builtin_return_address(25);
                break;

            case 26:
                if (!__builtin_frame_address(26))
                    goto handle;
                call.address = __builtin_return_address(26);
                break;

            case 27:
                if (!__builtin_frame_address(27))
                    goto handle;
                call.address = __builtin_return_address(27);
                break;

            case 28:
                if (!__builtin_frame_address(28))
                    goto handle;
                call.address = __builtin_return_address(28);
                break;

            case 29:
                if (!__builtin_frame_address(29))
                    goto handle;
                call.address = __builtin_return_address(29);
                break;

            case 30:
                if (!__builtin_frame_address(30))
                    goto handle;
                call.address = __builtin_return_address(30);
                break;

            case 31:
                if (!__builtin_frame_address(31))
                    goto handle;
                call.address = __builtin_return_address(31);
                break;

            case 32:
                if (!__builtin_frame_address(32))
                    goto handle;
                call.address = __builtin_return_address(32);
                break;

            case 33:
                if (!__builtin_frame_address(33))
                    goto handle;
                call.address = __builtin_return_address(33);
                break;

            case 34:
                if (!__builtin_frame_address(34))
                    goto handle;
                call.address = __builtin_return_address(34);
                break;

            case 35:
                if (!__builtin_frame_address(35))
                    goto handle;
                call.address = __builtin_return_address(35);
                break;

            case 36:
                if (!__builtin_frame_address(36))
                    goto handle;
                call.address = __builtin_return_address(36);
                break;

            case 37:
                if (!__builtin_frame_address(37))
                    goto handle;
                call.address = __builtin_return_address(37);
                break;

            case 38:
                if (!__builtin_frame_address(38))
                    goto handle;
                call.address = __builtin_return_address(38);
                break;

            case 39:
                if (!__builtin_frame_address(39))
                    goto handle;
                call.address = __builtin_return_address(39);
                break;

            case 40:
                if (!__builtin_frame_address(40))
                    goto handle;
                call.address = __builtin_return_address(40);
                break;

            case 41:
                if (!__builtin_frame_address(41))
                    goto handle;
                call.address = __builtin_return_address(41);
                break;

            case 42:
                if (!__builtin_frame_address(42))
                    goto handle;
                call.address = __builtin_return_address(42);
                break;

            case 43:
                if (!__builtin_frame_address(43))
                    goto handle;
                call.address = __builtin_return_address(43);
                break;

            case 44:
                if (!__builtin_frame_address(44))
                    goto handle;
                call.address = __builtin_return_address(44);
                break;

            case 45:
                if (!__builtin_frame_address(45))
                    goto handle;
                call.address = __builtin_return_address(45);
                break;

            case 46:
                if (!__builtin_frame_address(46))
                    goto handle;
                call.address = __builtin_return_address(46);
                break;

            case 47:
                if (!__builtin_frame_address(47))
                    goto handle;
                call.address = __builtin_return_address(47);
                break;

            case 48:
                if (!__builtin_frame_address(48))
                    goto handle;
                call.address = __builtin_return_address(48);
                break;

            case 49:
                if (!__builtin_frame_address(49))
                    goto handle;
                call.address = __builtin_return_address(49);
                break;

            case 50:
                if (!__builtin_frame_address(50))
                    goto handle;
                call.address = __builtin_return_address(50);
                break;

            case 51:
                if (!__builtin_frame_address(51))
                    goto handle;
                call.address = __builtin_return_address(51);
                break;

            case 52:
                if (!__builtin_frame_address(52))
                    goto handle;
                call.address = __builtin_return_address(52);
                break;

            case 53:
                if (!__builtin_frame_address(53))
                    goto handle;
                call.address = __builtin_return_address(53);
                break;

            case 54:
                if (!__builtin_frame_address(54))
                    goto handle;
                call.address = __builtin_return_address(54);
                break;

            case 55:
                if (!__builtin_frame_address(55))
                    goto handle;
                call.address = __builtin_return_address(55);
                break;

            case 56:
                if (!__builtin_frame_address(56))
                    goto handle;
                call.address = __builtin_return_address(56);
                break;

            case 57:
                if (!__builtin_frame_address(57))
                    goto handle;
                call.address = __builtin_return_address(57);
                break;

            case 58:
                if (!__builtin_frame_address(58))
                    goto handle;
                call.address = __builtin_return_address(58);
                break;

            case 59:
                if (!__builtin_frame_address(59))
                    goto handle;
                call.address = __builtin_return_address(59);
                break;

            case 60:
                if (!__builtin_frame_address(60))
                    goto handle;
                call.address = __builtin_return_address(60);
                break;

            case 61:
                if (!__builtin_frame_address(61))
                    goto handle;
                call.address = __builtin_return_address(61);
                break;

            case 62:
                if (!__builtin_frame_address(62))
                    goto handle;
                call.address = __builtin_return_address(62);
                break;

            case 63:
                if (!__builtin_frame_address(63))
                    goto handle;
                call.address = __builtin_return_address(63);
                break;
        }

#ifdef DL_EXT
        if (dladdr(call.address, &dlinfo) != 0) {
            symname = dlinfo.dli_sname;

            demangled = __cxa_demangle(symname, NULL, 0, &status);
            if (status == 0 && demangled != NULL)
                symname = demangled;

            call.symbol = (symname != NULL) ? symname : "undefined";
            call.object = (dlinfo.dli_fname != NULL) ? dlinfo.dli_fname : "undefined";


            if (demangled)
                free(demangled);
        }
#endif

        callStack.push_back(call);
    }

handle:
#endif
    instance();

    ++instances;

    if (handlerMap[source])
        handlers[source](source, this, handlerData[source]);
}

//-------------------------------------------------------------------

basic::~basic() throw ()
{
    sync::stack tg;

    --instances;

    if (instances == 0) {
#ifdef DL_EXT
        deinitModule deinit;

        for (int i(0); i < MODULE_ENUMSIZE; ++i) {
            if (!handlesOpened[i])
                continue;

            deinit = (deinitModule)dlsym(handles[i], "deinitExceptionBasicModule");
            if (deinit != NULL)
                deinit(cookies[i]);

            handlesOpened[i] = false;

#ifndef DL_FAST
            dlclose(handles[i]);
#endif
        }
#endif
    }
}

//-------------------------------------------------------------------

dodo::string
basic::backtrace()
{
    dodo::string stack;

    char str[32];

    dodoArray<__call__>::iterator i = callStack.begin(), j = callStack.end();
    for (; i != j; ++i) {
        snprintf(str, 32, " [0x%lx]", (long)i->address);
        stack += dodo::string(i->object + ": " + i->symbol + dodo::string(str) + "\n");
    }

    return stack;
}

//-------------------------------------------------------------------

basic::operator const dodo::string
& ()
{
    sync::stack tg;

    return errStr;
}

//-------------------------------------------------------------------

const char *
basic::what() const throw ()
{
    sync::stack tg;

    return errStr.data();
}

//-------------------------------------------------------------------

void
basic::setHandler(moduleEnum module,
                  handler    handler,
                  void       *data)
{
    sync::stack tg;

    instance();

#ifdef DL_EXT
    if (handlesOpened[module]) {
        deinitModule deinit;

        deinit = (deinitModule)dlsym(handles[module], "deinitExceptionBasicModule");
        if (deinit != NULL)
            deinit(cookies[module]);

#ifndef DL_FAST
        dlclose(handles[module]);
#endif

        handlesOpened[module] = false;
        handles[module] = NULL;
    }
#endif

    handlers[module] = handler;
    handlerMap[module] = true;
    handlerData[module] = data;
}

//-------------------------------------------------------------------

void
basic::setHandler(handler handler,
                  void    *data)
{
    sync::stack tg;

    instance();

#ifdef DL_EXT
    deinitModule deinit;
#endif

    for (int i(0); i < MODULE_ENUMSIZE; ++i) {
#ifdef DL_EXT
        if (handlesOpened[i]) {
            deinit = (deinitModule)dlsym(handles[i], "deinitExceptionBasicModule");
            if (deinit != NULL)
                deinit(cookies[i]);

#ifndef DL_FAST
            dlclose(handles[i]);
#endif

            handlesOpened[i] = false;
            handles[i] = NULL;
        }
#endif

        handlers[i] = handler;
        handlerMap[i] = true;
        handlerData[i] = data;
    }
}

//-------------------------------------------------------------------

void
basic::removeHandler(moduleEnum module)
{
    sync::stack tg;

#ifdef DL_EXT
    if (handlesOpened[module]) {
        deinitModule deinit;

        deinit = (deinitModule)dlsym(handles[module], "deinitExceptionBasicModule");
        if (deinit != NULL)
            deinit(cookies[module]);

#ifndef DL_FAST
        dlclose(handles[module]);
#endif

        handlesOpened[module] = false;
        handles[module] = NULL;
    }
#endif

    handlers[module] = NULL;
    handlerMap[module] = false;
    handlerData[module] = NULL;
}

//-------------------------------------------------------------------

void
basic::removeHandlers()
{
    sync::stack tg;

#ifdef DL_EXT
    deinitModule deinit;
#endif

    for (int i(0); i < MODULE_ENUMSIZE; ++i) {
#ifdef DL_EXT
        if (handlesOpened[i]) {
            deinit = (deinitModule)dlsym(handles[i], "deinitExceptionBasicModule");
            if (deinit != NULL)
                deinit(cookies[i]);

#ifndef DL_FAST
            dlclose(handles[i]);
#endif

            handlesOpened[i] = false;
            handles[i] = NULL;
        }
#endif

        handlers[i] = NULL;
        handlerMap[i] = false;
        handlerData[i] = NULL;
    }
}

//-------------------------------------------------------------------

#ifdef DL_EXT
void
basic::setHandler(const dodo::string &path,
                  void             *data,
                  void             *toInit)
{
    sync::stack tg;

    instance();

#ifdef DL_FAST
    void *h = dlopen(path.data(), RTLD_LAZY | RTLD_NODELETE);
#else
    void *h = dlopen(path.data(), RTLD_LAZY);
#endif
    if (h == NULL)
        return;

    initModule init = (initModule)dlsym(h, "initExceptionBasicModule");
    if (init == NULL)
        return;

    __module__ mod = init(toInit);

    deinitModule deinit;

    for (int i = 0; i < MODULE_ENUMSIZE; ++i) {
        if (!mod.modules[i])
            continue;

        if (handlesOpened[i]) {
            deinit = (deinitModule)dlsym(handles[i], "deinitExceptionBasicModule");
            if (deinit != NULL)
                deinit(cookies[i]);

#ifndef DL_FAST
            dlclose(handles[i]);
#endif

            handlesOpened[i] = false;
            handles[i] = NULL;
        }


        handles[i] = h;

        handler in = (handler)dlsym(handles[i], mod.hook);
        if (in == NULL)
            continue;

        handlesOpened[i] = true;
        memcpy(cookies[i], mod.cookie, 32);

        handlers[i] = in;
        handlerMap[i] = true;
        handlerData[i] = data;
    }
}

//-------------------------------------------------------------------

basic::__module__
basic::module(const dodo::string &module,
              void             *toInit)
{
    sync::stack tg;

#ifdef DL_FAST
    void *handle = dlopen(module.data(), RTLD_LAZY | RTLD_NODELETE);
#else
    void *handle = dlopen(module.data(), RTLD_LAZY);
#endif
    if (handle == NULL)
        return __module__();

    initModule init = (initModule)dlsym(handle, "initExceptionBasicModule");
    if (init == NULL)
        return __module__();

    __module__ mod = init(toInit);

	deinitModule deinit = (deinitModule)dlsym(handle, "deinitExceptionBasicModule");
	if (deinit != NULL)
		deinit(mod.cookie);

#ifndef DL_FAST
    if (dlclose(handle) != 0)
        return mod;

#endif

    return mod;
}
#endif

//-------------------------------------------------------------------

