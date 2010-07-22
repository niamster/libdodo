/***************************************************************************
 *            exceptionBasic.cc
 *
 *  Sun Jul 17 2005
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

__thread __context__ dodo::exception::global_exeption_context = {
    NULL,
    NULL};

__thread char dodo::exception::exeption_storage[sizeof(basic)];

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

basic::basic()
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
             const dodo::string &a_message) : source(a_module),
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
                call.address = (void *)((unsigned long)__builtin_return_address(0)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 1:
                call.address = (void *)((unsigned long)__builtin_return_address(1)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 2:
                call.address = (void *)((unsigned long)__builtin_return_address(2)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 3:
                call.address = (void *)((unsigned long)__builtin_return_address(3)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 4:
                call.address = (void *)((unsigned long)__builtin_return_address(4)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 5:
                call.address = (void *)((unsigned long)__builtin_return_address(5)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 6:
                call.address = (void *)((unsigned long)__builtin_return_address(6)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 7:
                call.address = (void *)((unsigned long)__builtin_return_address(7)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 8:
                call.address = (void *)((unsigned long)__builtin_return_address(8)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 9:
                call.address = (void *)((unsigned long)__builtin_return_address(9)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 10:
                call.address = (void *)((unsigned long)__builtin_return_address(10)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 11:
                call.address = (void *)((unsigned long)__builtin_return_address(11)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 12:
                call.address = (void *)((unsigned long)__builtin_return_address(12)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 13:
                call.address = (void *)((unsigned long)__builtin_return_address(13)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 14:
                call.address = (void *)((unsigned long)__builtin_return_address(14)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 15:
                call.address = (void *)((unsigned long)__builtin_return_address(15)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 16:
                call.address = (void *)((unsigned long)__builtin_return_address(16)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 17:
                call.address = (void *)((unsigned long)__builtin_return_address(17)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 18:
                call.address = (void *)((unsigned long)__builtin_return_address(18)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 19:
                call.address = (void *)((unsigned long)__builtin_return_address(19)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 20:
                call.address = (void *)((unsigned long)__builtin_return_address(20)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 21:
                call.address = (void *)((unsigned long)__builtin_return_address(21)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 22:
                call.address = (void *)((unsigned long)__builtin_return_address(22)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 23:
                call.address = (void *)((unsigned long)__builtin_return_address(23)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 24:
                call.address = (void *)((unsigned long)__builtin_return_address(24)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 25:
                call.address = (void *)((unsigned long)__builtin_return_address(25)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 26:
                call.address = (void *)((unsigned long)__builtin_return_address(26)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 27:
                call.address = (void *)((unsigned long)__builtin_return_address(27)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 28:
                call.address = (void *)((unsigned long)__builtin_return_address(28)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 29:
                call.address = (void *)((unsigned long)__builtin_return_address(29)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 30:
                call.address = (void *)((unsigned long)__builtin_return_address(30)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 31:
                call.address = (void *)((unsigned long)__builtin_return_address(31)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 32:
                call.address = (void *)((unsigned long)__builtin_return_address(32)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 33:
                call.address = (void *)((unsigned long)__builtin_return_address(33)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 34:
                call.address = (void *)((unsigned long)__builtin_return_address(34)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 35:
                call.address = (void *)((unsigned long)__builtin_return_address(35)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 36:
                call.address = (void *)((unsigned long)__builtin_return_address(36)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 37:
                call.address = (void *)((unsigned long)__builtin_return_address(37)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 38:
                call.address = (void *)((unsigned long)__builtin_return_address(38)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 39:
                call.address = (void *)((unsigned long)__builtin_return_address(39)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 40:
                call.address = (void *)((unsigned long)__builtin_return_address(40)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 41:
                call.address = (void *)((unsigned long)__builtin_return_address(41)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 42:
                call.address = (void *)((unsigned long)__builtin_return_address(42)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 43:
                call.address = (void *)((unsigned long)__builtin_return_address(43)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 44:
                call.address = (void *)((unsigned long)__builtin_return_address(44)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 45:
                call.address = (void *)((unsigned long)__builtin_return_address(45)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 46:
                call.address = (void *)((unsigned long)__builtin_return_address(46)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 47:
                call.address = (void *)((unsigned long)__builtin_return_address(47)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 48:
                call.address = (void *)((unsigned long)__builtin_return_address(48)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 49:
                call.address = (void *)((unsigned long)__builtin_return_address(49)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 50:
                call.address = (void *)((unsigned long)__builtin_return_address(50)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 51:
                call.address = (void *)((unsigned long)__builtin_return_address(51)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 52:
                call.address = (void *)((unsigned long)__builtin_return_address(52)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 53:
                call.address = (void *)((unsigned long)__builtin_return_address(53)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 54:
                call.address = (void *)((unsigned long)__builtin_return_address(54)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 55:
                call.address = (void *)((unsigned long)__builtin_return_address(55)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 56:
                call.address = (void *)((unsigned long)__builtin_return_address(56)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 57:
                call.address = (void *)((unsigned long)__builtin_return_address(57)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 58:
                call.address = (void *)((unsigned long)__builtin_return_address(58)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 59:
                call.address = (void *)((unsigned long)__builtin_return_address(59)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 60:
                call.address = (void *)((unsigned long)__builtin_return_address(60)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 61:
                call.address = (void *)((unsigned long)__builtin_return_address(61)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 62:
                call.address = (void *)((unsigned long)__builtin_return_address(62)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
                break;
            case 63:
                call.address = (void *)((unsigned long)__builtin_return_address(63)&0xFFFFFF00);
                if (!call.address)
                    goto handle;
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

    if (!exception::global_exeption_context.context)
        abort();
}

//-------------------------------------------------------------------

basic::~basic()
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
        stack += i->object + ": " + i->symbol + str + "\n";
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
basic::what() const
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

void *
basic::operator new(size_t size UNUSED,
        void *mem)
{
    return mem;
}

//-------------------------------------------------------------------

void *
basic::operator new(size_t size)
{
    return new char [size];
}

//-------------------------------------------------------------------

void
basic::operator delete(void *p)
{
    if (p != (void *)&exeption_storage)
        delete [] (char *)p;
}

//-------------------------------------------------------------------

