/***************************************************************************
 *            baseEx.cc
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

#include <libdodo/baseEx.h>

using namespace dodo;

bool baseEx::handlerSetEx[] = {
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
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

errorHandler baseEx::handlersEx[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
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

void *baseEx::handlerDataEx[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
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

bool baseEx::handlesOpenedEx[] = {
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
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

void *baseEx::handlesEx[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
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

pthread_mutex_t baseEx::staticAtomicMutex::keeper;

#endif

//-------------------------------------------------------------------

baseEx::staticAtomicMutex baseEx::keeper;

//-------------------------------------------------------------------

baseEx::staticAtomicMutex::staticAtomicMutex()
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

baseEx::staticAtomicMutex::~staticAtomicMutex()
{
#ifdef PTHREAD_EXT

	pthread_mutex_destroy(&keeper);

#endif
}

//-------------------------------------------------------------------

void
baseEx::staticAtomicMutex::acquire()
{
#ifdef PTHREAD_EXT

	pthread_mutex_lock(&keeper);

#endif
}

//-------------------------------------------------------------------

void
baseEx::staticAtomicMutex::release()
{
#ifdef PTHREAD_EXT

	pthread_mutex_unlock(&keeper);

#endif
}

//-------------------------------------------------------------------

baseEx::raceHazardGuard::raceHazardGuard()
{
	keeper.acquire();
}

//-------------------------------------------------------------------

baseEx::raceHazardGuard::~raceHazardGuard()
{
	keeper.release();
}

//-------------------------------------------------------------------

baseEx::baseEx(errorModuleEnum a_errModule,
			   unsigned long functionID,
			   errnoSourceEnum errnoSource,
			   int a_errno,
			   const dodoString &a_errstr,
			   unsigned long a_line,
			   const dodoString &a_file,
			   const dodoString &a_message) : errModule(a_errModule),
											  funcID(functionID),
											  errnoSource(errnoSource),
											  baseErrno(a_errno),
											  baseErrstr(a_errstr),
											  line(a_line),
											  file(a_file),
											  message(a_message)
{
	raceHazardGuard tg;

	if (handlerSetEx[errModule])
		handlersEx[errModule](errModule, this, handlerDataEx[errModule]);
}

//-------------------------------------------------------------------

baseEx::~baseEx()
{
	raceHazardGuard tg;

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

//-------------------------------------------------------------------

baseEx::operator const dodoString & ()
{
	raceHazardGuard tg;

	return baseErrstr;
}

//-------------------------------------------------------------------

void
baseEx::setErrorHandler(errorModuleEnum module,
						errorHandler handler,
						void *data)
{
	raceHazardGuard tg;

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
baseEx::setErrorHandlers(errorHandler handler,
						 void *data)
{
	raceHazardGuard tg;

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
baseEx::unsetErrorHandler(errorModuleEnum module)
{
	raceHazardGuard tg;

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
baseEx::unsetErrorHandlers()
{
	raceHazardGuard tg;

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
baseEx::setErrorHandlers(const dodoString &path,
						 void *data,
						 void *toInit)
{
	raceHazardGuard tg;

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
baseEx::setErrorHandler(const dodoString &path,
						void *data,
						void *toInit)
{
	raceHazardGuard tg;

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

	__baseExMod mod = init(toInit);

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

__baseExMod
baseEx::getModuleInfo(const dodoString &module,
					  void *toInit)
{
	raceHazardGuard tg;

#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		return __baseExMod();

	initBaseExModule init = (initBaseExModule)dlsym(handle, "initBaseExModule");
	if (init == NULL)
		return __baseExMod();

	__baseExMod mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		return mod;
#endif

	return mod;
}

#endif

//-------------------------------------------------------------------

