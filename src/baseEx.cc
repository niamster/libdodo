/***************************************************************************
 *            baseEx.cc
 *
 *  Sun Jul 17 17:38:02 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <baseEx.h>

#ifndef NO_EX

    using namespace dodo;

	extern "C"
	{
		static errorHandler __handlersEx[AM_MODULES];///< handlers
		static bool __handlerSetEx[AM_MODULES] = {false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false,
												false};///< indicates whether handler was set
		static void * __handlerDataEx[AM_MODULES];///< data that will be passed to handler
		
		#ifdef DL_EXT
		
			static void *__handlesEx[AM_MODULES];///< handles to modules
			static bool __handlesOpenedEx[AM_MODULES] = {false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false,
														false};///< map of opened modules
			
		#endif			
		
	};
	
	//-------------------------------------------------------------------
	
    baseEx::baseEx(errorModuleEnum a_errModule, 
    				unsigned long functionID,
                    errnoSourceEnum errnoSource,
                    long a_errno,
                    std::string a_errstr,
                    unsigned long a_line,
                    std::string a_file) : errModule(a_errModule),
                                        funcID(functionID),
                                        errnoSource(errnoSource),
                                        baseErrno(a_errno),
                                        baseErrstr(a_errstr),
                                        line(a_line),
                                        file(a_file)
    {
    	if (__handlerSetEx[errModule])
    		__handlersEx[errModule](errModule, this, __handlerDataEx[errModule]);
    }

	//-------------------------------------------------------------------
	
	baseEx::~baseEx()
	{
		#ifdef DL_EXT
		
			deinitExModule deinit;	
			
			for (register int i(0);i<AM_MODULES;++i)
			{
				if (!__handlesOpenedEx[i])
					continue;
				
				deinit = (deinitExModule)dlsym(__handlesEx[i], "deinitExModule");
				if (deinit != NULL)
					deinit();
					
				__handlesOpenedEx[i] = false;	
				
				dlclose(__handlesEx[i]);
			}
				
		#endif		
	}

	//-------------------------------------------------------------------
			
	baseEx::operator const char *()
	{
		return baseErrstr.c_str();
	}

	//-------------------------------------------------------------------
	
	void
	baseEx::setErrorHandler(errorModuleEnum module,
							errorHandler handler,
							void *data)
	{
		#ifdef DL_EXT
		
			if (__handlesOpenedEx[module])
			{
				deinitExModule deinit;	
				
				deinit = (deinitExModule)dlsym(__handlesEx[module], "deinitExModule");
				if (deinit != NULL)
					deinit();
					
				dlclose(__handlesEx[module]);
				
				__handlesOpenedEx[module] = false;
				__handlesEx[module] = NULL;
			}	
		
		#endif
		
		__handlersEx[module] = handler;
		__handlerSetEx[module] = true;
		__handlerDataEx[module] = data;
	}
	
	//-------------------------------------------------------------------
	
	void 
	baseEx::setErrorHandlers(errorHandler handler, 
							void *data)
	{
		#ifdef DL_EXT
			deinitExModule deinit;
		#endif
		
		for (register int i(0);i<AM_MODULES;++i)
		{
			#ifdef DL_EXT
				
				if (__handlesOpenedEx[i])
				{
					deinit = (deinitExModule)dlsym(__handlesEx[i], "deinitExModule");
					if (deinit != NULL)
						deinit();
						
					dlclose(__handlesEx[i]);
					
					__handlesOpenedEx[i] = false;
					__handlesEx[i] = NULL;
				}	
			
			#endif
					
			__handlersEx[i] = handler;
			__handlerSetEx[i] = true;
			__handlerDataEx[i] = data;
		}
	}
	
	//-------------------------------------------------------------------
			
	void 
	baseEx::unsetErrorHandler(errorModuleEnum module)
	{
		#ifdef DL_EXT
			
			if (__handlesOpenedEx[module])
			{
				deinitExModule deinit;	
				
				deinit = (deinitExModule)dlsym(__handlesEx[module], "deinitExModule");
				if (deinit != NULL)
					deinit();
					
				dlclose(__handlesEx[module]);
				
				__handlesOpenedEx[module] = false;
				__handlesEx[module] = NULL;
			}	
		
		#endif		
		
		__handlersEx[module] = NULL;
		__handlerSetEx[module] = false;
		__handlerDataEx[module] = NULL;			
	}
	
	//-------------------------------------------------------------------
	
	void 
	baseEx::unsetErrorHandlers()
	{
		#ifdef DL_EXT
			deinitExModule deinit;
		#endif
		
		for (register int i(0);i<AM_MODULES;++i)
		{
			#ifdef DL_EXT
				
				if (__handlesOpenedEx[i])
				{
					deinit = (deinitExModule)dlsym(__handlesEx[i], "deinitExModule");
					if (deinit != NULL)
						deinit();
						
					dlclose(__handlesEx[i]);
					
					__handlesOpenedEx[i] = false;
					__handlesEx[i] = NULL;
				}	
			
			#endif
					
			__handlersEx[i] = NULL;
			__handlerSetEx[i] = false;
			__handlerDataEx[i] = NULL;			
		}			
	}
			
	//-------------------------------------------------------------------

	#ifdef DL_EXT
	
		bool 
		baseEx::setErrorHandlers(const std::string &path,
							void *data)
		{
			initExModule init;
			errorHandler in;
			deinitExModule deinit;
			
			for (register int i(0);i<AM_MODULES;++i)
			{
				
				if (__handlesOpenedEx[i])
				{
					deinit = (deinitExModule)dlsym(__handlesEx[i], "deinitExModule");
					if (deinit != NULL)
						deinit();
						
					dlclose(__handlesEx[i]);
					
					__handlesOpenedEx[i] = false;
					__handlesEx[i] = NULL;
				}	
				
				__handlesEx[i] = dlopen(path.c_str(), RTLD_LAZY);
				if (__handlesEx[i] == NULL)
					return false;
				
				init = (initExModule)dlsym(__handlesEx[i], "initExModule");
				if (init == NULL)
					return false;
				
				in = (errorHandler)dlsym(__handlesEx[i], init().hook);
				if (in == NULL)
					return false;
			
				__handlesOpenedEx[i] = true;
	
				__handlersEx[i] = in;
				__handlerSetEx[i] = true;
				__handlerDataEx[i] = data;
			
			}
			
			return true;		
		}
		
		//-------------------------------------------------------------------
			
		bool 
		baseEx::setErrorHandler(errorModuleEnum module,
							const std::string &path,
							void *data)
		{
			deinitExModule deinit;

			if (__handlesOpenedEx[module])
			{
				deinit = (deinitExModule)dlsym(__handlesEx[module], "deinitExModule");
				if (deinit != NULL)
					deinit();
					
				dlclose(__handlesEx[module]);
				
				__handlesOpenedEx[module] = false;
				__handlesEx[module] = NULL;
			}
			
			__handlesEx[module] = dlopen(path.c_str(), RTLD_LAZY);
			if (__handlesEx[module] == NULL)
				return false;
			
			initExModule init = (initExModule)dlsym(__handlesEx[module], "initExModule");
			if (init == NULL)
				return false;
			
			errorHandler in = (errorHandler)dlsym(__handlesEx[module], init().hook);
			if (in == NULL)
				return false;
		
			__handlesOpenedEx[module] = true;

			__handlersEx[module] = in;
			__handlerSetEx[module] = true;
			__handlerDataEx[module] = data;
		
			return true;		
		}
		
		//-------------------------------------------------------------------
			
		bool 
		baseEx::setErrorHandler(const std::string &path,
							void *data)
		{
			
			void *handler = dlopen(path.c_str(), RTLD_LAZY);
			if (handler == NULL)
				return false;
			
			initExModule init = (initExModule)dlsym(handler, "initExModule");
			if (init == NULL)
				return false;			
			
			exMod mod = init();
			
			deinitExModule deinit;

			if (__handlesOpenedEx[mod.module])
			{
				deinit = (deinitExModule)dlsym(__handlesEx[mod.module], "deinitExModule");
				if (deinit != NULL)
					deinit();
					
				dlclose(__handlesEx[mod.module]);
				
				__handlesOpenedEx[mod.module] = false;
				__handlesEx[mod.module] = NULL;
			}

			__handlesEx[mod.module] = handler;
			
			errorHandler in = (errorHandler)dlsym(__handlesEx[mod.module], mod.hook);
			if (in == NULL)
				return false;
		
			__handlesOpenedEx[mod.module] = true;

			__handlersEx[mod.module] = in;
			__handlerSetEx[mod.module] = true;
			__handlerDataEx[mod.module] = data;
		
			return true;		
		}
				
		//-------------------------------------------------------------------
		
		exMod 
		baseEx::getModuleInfo(const std::string &module)
		{
			void *handle = dlopen(module.c_str(), RTLD_LAZY);
			if (handle == NULL)
				return exMod();
				
			initExModule init = (initExModule)dlsym(handle, "initExModule");
			if (init == NULL)
				return exMod();
				
			exMod mod = init();
			
			if (dlclose(handle)!=0)
				return mod;
			
			return mod;	
		}
		
		//-------------------------------------------------------------------
	
	#endif	
			
#endif
