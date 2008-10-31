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

#include <libdodo/exceptionBasic.h>

using namespace dodo::exception;

unsigned long basic::instances = 0;

//-------------------------------------------------------------------

bool basic::handlerSetEx[] = {
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

errorHandler basic::handlersEx[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
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

void *basic::handlerDataEx[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
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

bool basic::handlesOpenedEx[] = {
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

void *basic::handlesEx[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
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

#undef DL_EXT

basic::basic(int a_errModule,
			 int functionID,
			 int errnoSource,
			 int a_errno,
			 const dodoString &a_errstr,
			 unsigned long a_line,
			 const dodoString &a_file,
			 const dodoString &a_message) throw () : errModule(a_errModule),
													 funcID(functionID),
													 errnoSource(errnoSource),
													 baseErrno(a_errno),
													 baseErrstr(a_errstr),
													 line(a_line),
													 file(a_file),
													 message(a_message)
{
	syncThreadStack tg;

	void *trace[MAXCALLSTACKLEN];

#ifdef DL_EXT

	using namespace abi;

	Dl_info dlinfo;

	int status;
	const char *symname;
	char *demangled;

#endif

	__call call;

	int trace_size = backtrace(trace, MAXCALLSTACKLEN);

#ifndef DL_EXT

	char **symbols = backtrace_symbols(trace, trace_size);

#endif

	for (int i=0; i<trace_size; ++i)
	{
#ifdef DL_EXT

		if(dladdr(trace[i], &dlinfo) == 0)
			continue;

		symname = dlinfo.dli_sname;

		demangled = __cxa_demangle(symname, NULL, 0, &status);
		if(status == 0 && demangled != NULL)
			symname = demangled;

		call.address = trace[i];
		call.symbol = (symname != NULL)?symname:"undefined";
		call.object = (dlinfo.dli_fname != NULL)?dlinfo.dli_fname:"undefined";

		callStack.push_back(call);

		if (demangled)
			free(demangled);

#else

		call.symbol = symbols[i];

		callStack.push_back(call);

#endif
	}

#ifndef DL_EXT

	free(symbols);

#endif

	getInstance();

	++instances;

	if (handlerSetEx[errModule])
		handlersEx[errModule](errModule, this, handlerDataEx[errModule]);
}

//-------------------------------------------------------------------

basic::~basic() throw ()
{
	syncThreadStack tg;

	--instances;

	if (instances == 0)
	{
#ifdef DL_EXT

		deinitBaseExModule deinit;

		for (int i(0); i < BASEEX_MODULES; ++i)
		{
			if (!handlesOpenedEx[i])
				continue;

			deinit = (deinitBaseExModule)dlsym(handlesEx[i], "deinitBaseExModule");
			if (deinit != NULL)
				deinit();

			handlesOpenedEx[i] = false;

#ifndef DL_FAST
			dlclose(handlesEx[i]);
#endif
		}

#endif
	}
}

//-------------------------------------------------------------------

dodoString
basic::getCallStack()
{
	dodoString stack;

#ifdef DL_EXT

	char str[32];

#endif

	dodoArray<__call>::iterator i = callStack.begin(), j = callStack.end();
	for (;i!=j;++i)
	{
#ifdef DL_EXT

		snprintf(str, 32, " [0x%x]", (long)i->address);
		stack.append(i->object + ": " + i->symbol + dodoString(str));

#else

		stack.append(i->symbol);

#endif

		stack.append("\n");
	}

	return stack;
}

//-------------------------------------------------------------------

basic::operator const dodoString & ()
{
	syncThreadStack tg;

	return baseErrstr;
}

//-------------------------------------------------------------------

const char *
basic::what() const throw ()
{
	syncThreadStack tg;

	return baseErrstr.c_str();
}

//-------------------------------------------------------------------

void
basic::setErrorHandler(errorModuleEnum module,
					   errorHandler handler,
					   void *data)
{
	syncThreadStack tg;

	getInstance();

#ifdef DL_EXT

	if (handlesOpenedEx[module])
	{
		deinitBaseExModule deinit;

		deinit = (deinitBaseExModule)dlsym(handlesEx[module], "deinitBaseExModule");
		if (deinit != NULL)
			deinit();

#ifndef DL_FAST
		dlclose(handlesEx[module]);
#endif

		handlesOpenedEx[module] = false;
		handlesEx[module] = NULL;
	}

#endif

	handlersEx[module] = handler;
	handlerSetEx[module] = true;
	handlerDataEx[module] = data;
}

//-------------------------------------------------------------------

void
basic::setErrorHandlers(errorHandler handler,
						void *data)
{
	syncThreadStack tg;

	getInstance();

#ifdef DL_EXT
	deinitBaseExModule deinit;
#endif

	for (int i(0); i < BASEEX_MODULES; ++i)
	{
#ifdef DL_EXT

		if (handlesOpenedEx[i])
		{
			deinit = (deinitBaseExModule)dlsym(handlesEx[i], "deinitBaseExModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			dlclose(handlesEx[i]);
#endif

			handlesOpenedEx[i] = false;
			handlesEx[i] = NULL;
		}

#endif

		handlersEx[i] = handler;
		handlerSetEx[i] = true;
		handlerDataEx[i] = data;
	}
}

//-------------------------------------------------------------------

void
basic::unsetErrorHandler(errorModuleEnum module)
{
	syncThreadStack tg;

#ifdef DL_EXT

	if (handlesOpenedEx[module])
	{
		deinitBaseExModule deinit;

		deinit = (deinitBaseExModule)dlsym(handlesEx[module], "deinitBaseExModule");
		if (deinit != NULL)
			deinit();

#ifndef DL_FAST
		dlclose(handlesEx[module]);
#endif

		handlesOpenedEx[module] = false;
		handlesEx[module] = NULL;
	}

#endif

	handlersEx[module] = NULL;
	handlerSetEx[module] = false;
	handlerDataEx[module] = NULL;
}

//-------------------------------------------------------------------

void
basic::unsetErrorHandlers()
{
	syncThreadStack tg;

#ifdef DL_EXT
	deinitBaseExModule deinit;
#endif

	for (int i(0); i < BASEEX_MODULES; ++i)
	{
#ifdef DL_EXT

		if (handlesOpenedEx[i])
		{
			deinit = (deinitBaseExModule)dlsym(handlesEx[i], "deinitBaseExModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			dlclose(handlesEx[i]);
#endif

			handlesOpenedEx[i] = false;
			handlesEx[i] = NULL;
		}

#endif

		handlersEx[i] = NULL;
		handlerSetEx[i] = false;
		handlerDataEx[i] = NULL;
	}
}

//-------------------------------------------------------------------

#ifdef DL_EXT

bool
basic::setErrorHandlers(const dodoString &path,
						void *data,
						void *toInit)
{
	syncThreadStack tg;

	getInstance();

	initBaseExModule init;
	errorHandler in;
	deinitBaseExModule deinit;

	for (int i(0); i < BASEEX_MODULES; ++i)
	{

		if (handlesOpenedEx[i])
		{
			deinit = (deinitBaseExModule)dlsym(handlesEx[i], "deinitBaseExModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			dlclose(handlesEx[i]);
#endif

			handlesOpenedEx[i] = false;
			handlesEx[i] = NULL;
		}

#ifdef DL_FAST
		handlesEx[i] = dlopen(path.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
		handlesEx[i] = dlopen(path.c_str(), RTLD_LAZY);
#endif
		if (handlesEx[i] == NULL)
			return false;

		init = (initBaseExModule)dlsym(handlesEx[i], "initBaseExModule");
		if (init == NULL)
			return false;

		in = (errorHandler)dlsym(handlesEx[i], init(toInit).hook);
		if (in == NULL)
			return false;

		handlesOpenedEx[i] = true;

		handlersEx[i] = in;
		handlerSetEx[i] = true;
		handlerDataEx[i] = data;

	}

	return true;
}

//-------------------------------------------------------------------

bool
basic::setErrorHandler(const dodoString &path,
					   void *data,
					   void *toInit)
{
	syncThreadStack tg;

	getInstance();

#ifdef DL_FAST
	void *handler = dlopen(path.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handler = dlopen(path.c_str(), RTLD_LAZY);
#endif
	if (handler == NULL)
		return false;

	initBaseExModule init = (initBaseExModule)dlsym(handler, "initBaseExModule");
	if (init == NULL)
		return false;

	__basicMod mod = init(toInit);

	deinitBaseExModule deinit;

	if (handlesOpenedEx[mod.module])
	{
		deinit = (deinitBaseExModule)dlsym(handlesEx[mod.module], "deinitBaseExModule");
		if (deinit != NULL)
			deinit();

#ifndef DL_FAST
		dlclose(handlesEx[mod.module]);
#endif

		handlesOpenedEx[mod.module] = false;
		handlesEx[mod.module] = NULL;
	}

	handlesEx[mod.module] = handler;

	errorHandler in = (errorHandler)dlsym(handlesEx[mod.module], mod.hook);
	if (in == NULL)
		return false;

	handlesOpenedEx[mod.module] = true;

	handlersEx[mod.module] = in;
	handlerSetEx[mod.module] = true;
	handlerDataEx[mod.module] = data;

	return true;
}

//-------------------------------------------------------------------

__basicMod
basic::getModuleInfo(const dodoString &module,
					 void *toInit)
{
	syncThreadStack tg;

#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		return __basicMod();

	initBaseExModule init = (initBaseExModule)dlsym(handle, "initBaseExModule");
	if (init == NULL)
		return __basicMod();

	__basicMod mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		return mod;
#endif

	return mod;
}

#endif

//-------------------------------------------------------------------

