/***************************************************************************
 *            exceptionBasic.cc
 *
 *  Sun Jul 17 17:38:02 2005
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
#endif
#ifdef CALLSTACK_EX
#include <dlfcn.h>
#include <execinfo.h>
#include <cxxabi.h>
#endif
#ifdef PTHREAD_EXT
#include <pthread.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include <libdodo/exceptionBasic.h>

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
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
#endif

//-------------------------------------------------------------------

#ifdef PTHREAD_EXT
pthread_mutex_t basic::syncThreadSection::keeper;
#endif

//-------------------------------------------------------------------

basic::syncThreadSection basic::keeper;

//-------------------------------------------------------------------

basic::syncThreadSection::syncThreadSection()
{
#ifdef PTHREAD_EXT
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&keeper, &attr);

	pthread_mutexattr_destroy(&attr);
#endif
}

//-------------------------------------------------------------------

basic::syncThreadSection::~syncThreadSection()
{
#ifdef PTHREAD_EXT
	pthread_mutex_destroy(&keeper);
#endif
}

//-------------------------------------------------------------------

void
basic::syncThreadSection::acquire()
{
#ifdef PTHREAD_EXT
	pthread_mutex_lock(&keeper);
#endif
}

//-------------------------------------------------------------------

void
basic::syncThreadSection::release()
{
#ifdef PTHREAD_EXT
	pthread_mutex_unlock(&keeper);
#endif
}

//-------------------------------------------------------------------

basic::syncThreadStack::syncThreadStack()
{
	keeper.acquire();
}

//-------------------------------------------------------------------

basic::syncThreadStack::~syncThreadStack()
{
	keeper.release();
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
			 const dodoString &a_errstr,
			 unsigned long    a_line,
			 const dodoString &a_file,
			 const dodoString &a_message) throw () : source(a_module),
													 function(functionID),
													 errnoSource(errnoSource),
													 errNo(a_errno),
													 errStr(a_errstr),
													 line(a_line),
													 file(a_file),
													 message(a_message)
{
	syncThreadStack tg;

#ifdef CALLSTACK_EX
	void *trace[MAXCALLSTACKLEN];

	using namespace abi;

	Dl_info dlinfo;

	int status;
	const char *symname;
	char *demangled;

	__call__ call;

	int trace_size = ::backtrace(trace, MAXCALLSTACKLEN);
	char **symbols = backtrace_symbols(trace, trace_size);

	for (int i = 0; i < trace_size; ++i) {
		if (dladdr(trace[i], &dlinfo) == 0)
			continue;

		symname = dlinfo.dli_sname;

		demangled = __cxa_demangle(symname, NULL, 0, &status);
		if (status == 0 && demangled != NULL)
			symname = demangled;

		call.address = trace[i];
		call.symbol = (symname != NULL) ? symname : "undefined";
		call.object = (dlinfo.dli_fname != NULL) ? dlinfo.dli_fname : "undefined";

		callStack.push_back(call);

		if (demangled)
			free(demangled);
	}

	free(symbols);
#endif

	instance();

	++instances;

	if (handlerMap[source])
		handlers[source](source, this, handlerData[source]);
}

//-------------------------------------------------------------------

basic::~basic() throw ()
{
	syncThreadStack tg;

	--instances;

	if (instances == 0) {
#ifdef DL_EXT
		deinitModule deinit;

		for (int i(0); i < MODULE_ENUMSIZE; ++i) {
			if (!handlesOpened[i])
				continue;

			deinit = (deinitModule)dlsym(handles[i], "deinitExceptionBasicModule");
			if (deinit != NULL)
				deinit();

			handlesOpened[i] = false;

#ifndef DL_FAST
			dlclose(handles[i]);
#endif
		}
#endif
	}
}

//-------------------------------------------------------------------

#ifdef CALLSTACK_EX
dodoString
basic::backtrace()
{
	dodoString stack;

	char str[32];

	dodoArray<__call__>::iterator i = callStack.begin(), j = callStack.end();
	for (; i != j; ++i) {
		snprintf(str, 32, " [0x%lx]", (long)i->address);
		stack.append(i->object + ": " + i->symbol + dodoString(str) + "\n");
	}

	return stack;
}
#endif

//-------------------------------------------------------------------

basic::operator const dodoString
& ()
{
	syncThreadStack tg;

	return errStr;
}

//-------------------------------------------------------------------

const char *
basic::what() const throw ()
{
	syncThreadStack tg;

	return errStr.data();
}

//-------------------------------------------------------------------

void
basic::setHandler(moduleEnum module,
					   handler    handler,
					   void            *data)
{
	syncThreadStack tg;

	instance();

#ifdef DL_EXT
	if (handlesOpened[module]) {
		deinitModule deinit;

		deinit = (deinitModule)dlsym(handles[module], "deinitExceptionBasicModule");
		if (deinit != NULL)
			deinit();

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
						void         *data)
{
	syncThreadStack tg;

	instance();

#ifdef DL_EXT
	deinitModule deinit;
#endif

	for (int i(0); i < MODULE_ENUMSIZE; ++i) {
#ifdef DL_EXT
		if (handlesOpened[i]) {
			deinit = (deinitModule)dlsym(handles[i], "deinitExceptionBasicModule");
			if (deinit != NULL)
				deinit();

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
	syncThreadStack tg;

#ifdef DL_EXT
	if (handlesOpened[module]) {
		deinitModule deinit;

		deinit = (deinitModule)dlsym(handles[module], "deinitExceptionBasicModule");
		if (deinit != NULL)
			deinit();

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
	syncThreadStack tg;

#ifdef DL_EXT
	deinitModule deinit;
#endif

	for (int i(0); i < MODULE_ENUMSIZE; ++i) {
#ifdef DL_EXT
		if (handlesOpened[i]) {
			deinit = (deinitModule)dlsym(handles[i], "deinitExceptionBasicModule");
			if (deinit != NULL)
				deinit();

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
basic::setHandler(const dodoString &path,
					   void             *data,
					   void             *toInit)
{
	syncThreadStack tg;

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

	for (int i=0;i<MODULE_ENUMSIZE;++i) {
		if (!mod.modules[i])
			continue;

		if (handlesOpened[i]) {
			deinit = (deinitModule)dlsym(handles[i], "deinitExceptionBasicModule");
			if (deinit != NULL)
				deinit();

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

		handlers[i] = in;
		handlerMap[i] = true;
		handlerData[i] = data;
	}
}

//-------------------------------------------------------------------

basic::__module__
basic::module(const dodoString &module,
					 void             *toInit)
{
	syncThreadStack tg;

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

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		return mod;

#endif

	return mod;
}
#endif

//-------------------------------------------------------------------

