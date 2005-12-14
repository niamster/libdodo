/***************************************************************************
 *            baseEx.h
 *
 *  Mon Feb  7 20:02:06 2005
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _BASEEX_H_
#define _BASEEX_H_

#include <directives.h>

#include <string>

#ifndef NO_EX

	namespace dodo
	{	
		/**
		 * @enum errnoSourceEnum describes names of sources, where code of error got
		 */
		enum errnoSourceEnum
		{
			ERR_LIBDODO,
			ERR_ERRNO,
			ERR_MYSQL,
			ERR_H_ERRNO,
			ERR_DYNLOAD,
			ERR_LIBXML2,
			ERR_ZLIB
		};

		/**
		 * @enum errorModuleEnum describes names of modules where exception was thrown
		 */
		enum errorModuleEnum
		{
			ERRMODULE_FLUSHSOCKET,
			ERRMODULE_CGITOOLS,
			ERRMODULE_FLUSHDISK,
			ERRMODULE_DBMYSQL,
			ERRMODULE_REGEXPTOOLS,
			ERRMODULE_DBSQLBASE,
			ERRMODULE_FLUSHSTD,
			ERRMODULE_XEXEC,
			ERRMODULE_SYSTEMTOOLS,
			ERRMODULE_TIMETOOLS,
			ERRMODULE_LIBXML2,
			ERRMODULE_TOOLS,
		};	

#define AM_MODULES 12
		
		class baseEx;///< to make typedef before class declaration
		
		/**
		 * @typedef describes function that can handle error
		 */
		typedef void (*errorHandler)(errorModuleEnum, baseEx *, void *);
			
		/**
		 * @class baseEx
		 * describes exception that was thrown
		 */	
		class baseEx
		{
			public:		
				
				/**
				 * constructor
				 * @param errModule in what module was thrown
				 * @param functionID in what function was thrown[see *Ex.h headers for IDs]
				 * @param errnoSource where error code and string was taken
				 * @param baseErrno error code
				 * @param baseErrstr describes error code
				 * @param line line, where was thrown
				 * @param file file, where was thrown
				 */			
				baseEx(errorModuleEnum errModule, unsigned long functionID, errnoSourceEnum errnoSource, long baseErrno, std::string baseErrstr, unsigned long line, std::string file);
				
				/**
				 * @return string that describes error
				 * 	catch(baseEx ex)
				 *	{
				 *		cout << ex << endl;
				 *	}
				 */
				operator const char *();
			
				errorModuleEnum errModule;///< in what module was thrown
				unsigned long funcID;///< in what function was thrown[see *Ex.h headers for IDs]
				errnoSourceEnum errnoSource;///< 	where error code and string was taken	
			
				unsigned long baseErrno;///< error code
				std::string baseErrstr;///< describes error code
				
				unsigned long line;///< line, where was thrown
				std::string file;///< file, where was thrown
				
				/**
				 * set handler on error for specific module
				 * @param module indicates for what module to set handler
				 * @param handler is function that will be called when error occured[in catch]
				 * @param data is data that will be passed to handler
				 */
				static void setErrorHandler(errorModuleEnum module, errorHandler handler, void *data);

				/**
				 * set handlers on error for all modules
				 * @param handler is function that will be called when error occured[in catch]
				 * @param data is data that will be passed to handler
				 */
				static void setErrorHandlers(errorHandler handler, void *data);
				
				/**
				 * remove handler on error for specific module
				 * @param module indicates for what module to set handler
				 */
				static void unsetErrorHandler(errorModuleEnum module);

				/**
				 * unset handlers on error for all modules
				 */
				static void unsetErrorHandlers();
		};
	};

#endif	
	
#endif
