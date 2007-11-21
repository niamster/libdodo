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

#include <libdodo/baseEx.h>

using namespace dodo;

bool baseEx::handlerSetEx[] = { false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false,
								false };

//-------------------------------------------------------------------

errorHandler baseEx::handlersEx[] = { NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL,
									  NULL };

//-------------------------------------------------------------------

void *baseEx::handlerDataEx[] = { NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  NULL };

//-------------------------------------------------------------------

    #ifdef DL_EXT

bool baseEx::handlesOpenedEx[] = { false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false,
								   false };

//-------------------------------------------------------------------

void *baseEx::handlesEx[] = { NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL };

    #endif

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
	if (handlerSetEx[errModule])
		handlersEx[errModule](errModule, this, handlerDataEx[errModule]);
}

//-------------------------------------------------------------------

baseEx::~baseEx()
{
        #ifdef DL_EXT

	deinitExModule deinit;

	for (int i(0); i < AM_MODULES; ++i)
	{
		if (!handlesOpenedEx[i])
			continue;

		deinit = (deinitExModule)dlsym(handlesEx[i], "deinitExModule");
		if (deinit != NULL)
			deinit();

		handlesOpenedEx[i] = false;

		dlclose(handlesEx[i]);
	}

        #endif
}

//-------------------------------------------------------------------

baseEx::operator const dodoString&()
{
	return baseErrstr;
}

//-------------------------------------------------------------------

void
baseEx::setErrorHandler(errorModuleEnum module,
						errorHandler handler,
						void *data)
{
        #ifdef DL_EXT

	if (handlesOpenedEx[module])
	{
		deinitExModule deinit;

		deinit = (deinitExModule)dlsym(handlesEx[module], "deinitExModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesEx[module]);

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
        #ifdef DL_EXT
	deinitExModule deinit;
        #endif

	for (int i(0); i < AM_MODULES; ++i)
	{
            #ifdef DL_EXT

		if (handlesOpenedEx[i])
		{
			deinit = (deinitExModule)dlsym(handlesEx[i], "deinitExModule");
			if (deinit != NULL)
				deinit();

			dlclose(handlesEx[i]);

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
        #ifdef DL_EXT

	if (handlesOpenedEx[module])
	{
		deinitExModule deinit;

		deinit = (deinitExModule)dlsym(handlesEx[module], "deinitExModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesEx[module]);

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
        #ifdef DL_EXT
	deinitExModule deinit;
        #endif

	for (int i(0); i < AM_MODULES; ++i)
	{
            #ifdef DL_EXT

		if (handlesOpenedEx[i])
		{
			deinit = (deinitExModule)dlsym(handlesEx[i], "deinitExModule");
			if (deinit != NULL)
				deinit();

			dlclose(handlesEx[i]);

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
	initExModule init;
	errorHandler in;
	deinitExModule deinit;

	for (int i(0); i < AM_MODULES; ++i)
	{

		if (handlesOpenedEx[i])
		{
			deinit = (deinitExModule)dlsym(handlesEx[i], "deinitExModule");
			if (deinit != NULL)
				deinit();

			dlclose(handlesEx[i]);

			handlesOpenedEx[i] = false;
			handlesEx[i] = NULL;
		}

		handlesEx[i] = dlopen(path.c_str(), RTLD_LAZY);
		if (handlesEx[i] == NULL)
			return false;

		init = (initExModule)dlsym(handlesEx[i], "initExModule");
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
baseEx::setErrorHandler(errorModuleEnum module,
						const dodoString &path,
						void *data,
						void *toInit)
{
	deinitExModule deinit;

	if (handlesOpenedEx[module])
	{
		deinit = (deinitExModule)dlsym(handlesEx[module], "deinitExModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesEx[module]);

		handlesOpenedEx[module] = false;
		handlesEx[module] = NULL;
	}

	handlesEx[module] = dlopen(path.c_str(), RTLD_LAZY);
	if (handlesEx[module] == NULL)
		return false;

	initExModule init = (initExModule)dlsym(handlesEx[module], "initExModule");
	if (init == NULL)
		return false;

	errorHandler in = (errorHandler)dlsym(handlesEx[module], init(toInit).hook);
	if (in == NULL)
		return false;

	handlesOpenedEx[module] = true;

	handlersEx[module] = in;
	handlerSetEx[module] = true;
	handlerDataEx[module] = data;

	return true;
}

//-------------------------------------------------------------------

bool
baseEx::setErrorHandler(const dodoString &path,
						void *data,
						void *toInit)
{

	void *handler = dlopen(path.c_str(), RTLD_LAZY);
	if (handler == NULL)
		return false;

	initExModule init = (initExModule)dlsym(handler, "initExModule");
	if (init == NULL)
		return false;

	exMod mod = init(toInit);

	deinitExModule deinit;

	if (handlesOpenedEx[mod.module])
	{
		deinit = (deinitExModule)dlsym(handlesEx[mod.module], "deinitExModule");
		if (deinit != NULL)
			deinit();

		dlclose(handlesEx[mod.module]);

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

exMod
baseEx::getModuleInfo(const dodoString &module,
					  void *toInit)
{
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
	if (handle == NULL)
		return exMod();

	initExModule init = (initExModule)dlsym(handle, "initExModule");
	if (init == NULL)
		return exMod();

	exMod mod = init(toInit);

	if (dlclose(handle) != 0)
		return mod;

	return mod;
}

#endif
