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
		static errorHandler handlers[AM_MODULES];///< handlers
		static bool handlerSet[AM_MODULES];///< indicates whether handler was set
		static void * handlerData[AM_MODULES];///< data that will be passed to handler
	};
	
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
    	if (handlerSet[errModule])
    		handlers[errModule](errModule, this, handlerData[errModule]);
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
		handlers[module] = handler;
		handlerSet[module] = true;
		handlerData[module] = data;
	}
	
	//-------------------------------------------------------------------
	
	void 
	baseEx::setErrorHandlers(errorHandler handler, 
							void *data)
	{
		for (register int i(0);i<AM_MODULES;++i)
		{
			handlers[i] = handler;
			handlerSet[i] = true;
			handlerData[i] = data;
		}
	}
	
	//-------------------------------------------------------------------
			
	void 
	baseEx::unsetErrorHandler(errorModuleEnum module)
	{
		handlers[module] = NULL;
		handlerSet[module] = false;
		handlerData[module] = NULL;			
	}
	
	//-------------------------------------------------------------------
	
	void 
	baseEx::unsetErrorHandlers()
	{
		for (register int i(0);i<AM_MODULES;++i)
		{
			handlers[i] = NULL;
			handlerSet[i] = false;
			handlerData[i] = NULL;
		}			
	}
			
	//-------------------------------------------------------------------
		
#endif
