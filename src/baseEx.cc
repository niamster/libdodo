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
		for (register int i(0);i<AM_MODULES;++i)
		{
			#ifdef DL_EXT
				
				if (__handlesOpened[i])
				{
					deinitExModule deinit;	
					
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
		for (register int i(0);i<AM_MODULES;++i)
		{
			#ifdef DL_EXT
				
				if (__handlesOpened[i])
				{
					deinitExModule deinit;	
					
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
		baseEx::setErrorHandler(errorModuleEnum module,
							std::string path,
							void *data)
		{
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
		
/*		xexecCounts 
		xexec::_addExec(const std::string &module, 
						void *obj, 
	 					xexecObjTypeEnum type, 
						void *data) const
		{
			if (handlesOpened == XEXEC_MAXMODULES)
				return xexecCounts();
			
			__execItem temp;
			temp.data = data;
			temp.obj = obj;
			temp.present = true;
			temp.enabled = true;
			temp.type = type;
			
			handles[handlesOpened] = dlopen(module.c_str(), RTLD_LAZY);
			if (handles[handlesOpened] == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_XEXEC,XEXEC_ADDXEXECMODULE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return xexecCounts();
				#endif
			
			initXexecModule init = (initXexecModule)dlsym(handles[handlesOpened], "initXexecModule");
			if (init == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_XEXEC,XEXEC_ADDXEXECMODULE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return xexecCounts();
				#endif	
			
			xexecMod info = init();
			
			inExec in = (inExec)dlsym(handles[handlesOpened], info.hook);
			if (in == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_XEXEC,XEXEC_ADDXEXECMODULE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return xexecCounts();
				#endif
		
			
			temp.func = in;
			
			xexecCounts count;
			
			switch (info.execType)
			{
				case XEXECMODULE_PRE:
					preExec.exec.push_back(temp);
					count.pre = preExec.exec.size();
					break;
					
				case XEXECMODULE_POST:
					postExec.exec.push_back(temp);
					count.post = postExec.exec.size();
					break;
					
				case XEXECMODULE_BOTH:
					preExec.exec.push_back(temp);
					postExec.exec.push_back(temp);
					count.post = postExec.exec.size();
					count.pre = preExec.exec.size();
					break;
			}
				
			++handlesOpened;
			
			return count;
		}*/
		
		//-------------------------------------------------------------------
	
	#endif	
			
#endif
