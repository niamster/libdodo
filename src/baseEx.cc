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
		static errorHandler __handlers[AM_MODULES];///< handlers
		static bool __handlerSet[AM_MODULES];///< indicates whether handler was set
		static void * __handlerData[AM_MODULES];///< data that will be passed to handler
		
		#ifdef DL_EXT
		
			static void *__handles[AM_MODULES];///< handles to modules
			static bool __handlesOpened[AM_MODULES];///< map of opened modules
			
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
    	if (__handlerSet[errModule])
    		__handlers[errModule](errModule, this, __handlerData[errModule]);
    }

	//-------------------------------------------------------------------
	
	baseEx::~baseEx()
	{
		#ifdef DL_EXT
		
			deinitExModule deinit;	
			
			for (register int i(0);i<AM_MODULES;++i)
			{
				if (!__handlesOpened[i])
					continue;
				
				deinit = (deinitExModule)dlsym(__handles[i], "deinitExModule");
				if (deinit != NULL)
					deinit();
					
				__handlesOpened[i] = false;	
				
				dlclose(__handles[i]);
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
			
			if (__handlesOpened[module])
			{
				deinitExModule deinit;	
				
				deinit = (deinitExModule)dlsym(__handles[module], "deinitExModule");
				if (deinit != NULL)
					deinit();
					
				dlclose(__handles[module]);
				
				__handlesOpened[module] = false;
			}	
		
		#endif
		
		__handlers[module] = handler;
		__handlerSet[module] = true;
		__handlerData[module] = data;
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
				
				if (__handlesOpened[i])
				{
					deinit = (deinitExModule)dlsym(__handles[i], "deinitExModule");
					if (deinit != NULL)
						deinit();
						
					dlclose(__handles[i]);
					
					__handlesOpened[i] = false;
				}	
			
			#endif
					
			__handlers[i] = handler;
			__handlerSet[i] = true;
			__handlerData[i] = data;
		}
	}
	
	//-------------------------------------------------------------------
			
	void 
	baseEx::unsetErrorHandler(errorModuleEnum module)
	{
		#ifdef DL_EXT
			
			if (__handlesOpened[module])
			{
				deinitExModule deinit;	
				
				deinit = (deinitExModule)dlsym(__handles[module], "deinitExModule");
				if (deinit != NULL)
					deinit();
					
				dlclose(__handles[module]);
				
				__handlesOpened[module] = false;
			}	
		
		#endif		
		
		__handlers[module] = NULL;
		__handlerSet[module] = false;
		__handlerData[module] = NULL;			
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
				
				if (__handlesOpened[i])
				{
					deinit = (deinitExModule)dlsym(__handles[i], "deinitExModule");
					if (deinit != NULL)
						deinit();
						
					dlclose(__handles[i]);
					
					__handlesOpened[i] = false;
				}	
			
			#endif
					
			__handlers[i] = NULL;
			__handlerSet[i] = false;
			__handlerData[i] = NULL;			
		}			
	}
			
	//-------------------------------------------------------------------

	#ifdef DL_EXT
	
		bool 
		baseEx::setErrorHandlers(std::string path,
							void *data)
		{
			initExModule init;
			errorHandler in;
			deinitExModule deinit;
			
			for (register int i(0);i<AM_MODULES;++i)
			{
				
				if (__handlesOpened[i])
				{
					deinit = (deinitExModule)dlsym(__handles[i], "deinitExModule");
					if (deinit != NULL)
						deinit();
						
					dlclose(__handles[i]);
					
					__handlesOpened[i] = false;
				}	
				
				__handles[i] = dlopen(path.c_str(), RTLD_LAZY);
				if (__handles[i] == NULL)
					return false;
				
				init = (initExModule)dlsym(__handles[i], "initExModule");
				if (init == NULL)
					return false;
				
				in = (errorHandler)dlsym(__handles[i], init().hook);
				if (in == NULL)
					return false;
			
				__handlesOpened[i] = true;
	
				__handlers[i] = in;
				__handlerSet[i] = true;
				__handlerData[i] = data;
			
			}
			
			return true;		
		}
		
		//-------------------------------------------------------------------
			
		bool 
		baseEx::setErrorHandler(errorModuleEnum module,
							std::string path,
							void *data)
		{
			deinitExModule deinit;

			if (__handlesOpened[module])
			{
				deinit = (deinitExModule)dlsym(__handles[module], "deinitExModule");
				if (deinit != NULL)
					deinit();
					
				dlclose(__handles[module]);
				
				__handlesOpened[module] = false;
			}
			
			__handles[module] = dlopen(path.c_str(), RTLD_LAZY);
			if (__handles[module] == NULL)
				return false;
			
			initExModule init = (initExModule)dlsym(__handles[module], "initExModule");
			if (init == NULL)
				return false;
			
			errorHandler in = (errorHandler)dlsym(__handles[module], init().hook);
			if (in == NULL)
				return false;
		
			__handlesOpened[module] = true;

			__handlers[module] = in;
			__handlerSet[module] = true;
			__handlerData[module] = data;
		
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
				return exMod();
			
			return mod;	
		}
		
		//-------------------------------------------------------------------
	
	#endif	
			
#endif
