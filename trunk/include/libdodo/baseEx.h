/***************************************************************************
 *            baseEx.h
 *
 *  Mon Feb  7 20:02:06 2005
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

#ifndef _BASEEX_H_
#define _BASEEX_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>

#ifdef DL_EXT

#include <dlfcn.h>

#endif

namespace dodo
{
	/**
	 * @enum errnoSourceEnum defines sources, where the code of the error has been got
	 */
	enum errnoSourceEnum
	{
		ERR_LIBDODO,
		ERR_ERRNO,
		ERR_MYSQL,
		ERR_H_ERRNO,
		ERR_DYNLOAD,
		ERR_LIBXML2,
		ERR_ZLIB,
		ERR_SQLITE,
		ERR_BZIP,
		ERR_IMAGEMAGICK,
		ERR_POSIXREGEX,
		ERR_PCRE,
		ERR_BFD
	};

	/**
	 * @enum errorModuleEnum defines modules where exception might be thrown
	 */
	enum errorModuleEnum
	{
		ERRMODULE_IONETWORK = 0,
		ERRMODULE_IODISK,
		ERRMODULE_IODISKTOOLS,
		ERRMODULE_DBMYSQL,
		ERRMODULE_REGEXP,
		ERRMODULE_DBSQLBASE,
		ERRMODULE_IOSTD,
		ERRMODULE_XEXEC,
		ERRMODULE_SYSTEMTOOLS,
		ERRMODULE_TIMETOOLS,
		ERRMODULE_LIBXML2,
		ERRMODULE_TOOLS,
		ERRMODULE_DBSQLITE,
		ERRMODULE_IONETWORKOPTIONS,
		ERRMODULE_IONETWORKEXCHANGE,
		ERRMODULE_IONETWORKTOOLS,
		ERRMODULE_SYSTEMPROCESSCOLLECTION,
		ERRMODULE_SYSTEMTHREADCOLLECTION,
		ERRMODULE_SYSTEMSHAREDDATA,
		ERRMODULE_SYSTEMTHREADSHAREDDATAGUARD,
		ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD,
		ERRMODULE_SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD,
		ERRMODULE_SYSTEMTHREADSHAREDDATACOLLECTIONGUARD,
		ERRMODULE_DBPOSTGRESQL,
		ERRMODULE_CGIPREPROCESSOR,
		ERRMODULE_CGIPROCESSOR,
		ERRMODULE_IONONBLOCKEDACCESS,
		ERRMODULE_ATOMICMUTEX,
		ERRMODULE_ATOMICSEMAPHORE,
		ERRMODULE_IMAGE,
		ERRMODULE_JSON,
		ERRMODULE_CGIFAST,
		ERRMODULE_IOCGIFASTEXCHANGE,
		ERRMODULE_CGI,
		ERRMODULE_SYSTEMLIBRARYLOADER,
		ERRMODULE_RPC
	};

#define AM_MODULES    36

#ifdef DL_EXT

	/**
	 * @struct __exMod is returned from initExModule in the library
	 */
	struct __exMod
	{
		char name[64];                  ///< name of the library
		char discription[256];          ///< discription of the library
		char hook[64];                  ///< name of the function in module that will be as a hook
		short module;         			///< for what module handler should be set[see errorModuleEnum]
	};

	/**
	 * @typedef initExModule defines type of init function for library
	 */
	typedef __exMod (*initExModule)(void *);

	/**
	 * @typedef deinitExModule defines type of deinit function for library
	 */
	typedef void (*deinitExModule)();

#endif

	class baseEx;

	/**
	 * @typedef errorHandler defines type of hook function
	 */
	typedef void (*errorHandler)(errorModuleEnum, baseEx *, void *);

	/**
	 * @class baseEx describes exception that has been thrown
	 */
	class baseEx
	{
		public:

			/**
			 * constructor
			 * @param errModule defines module where exception has been thrown
			 * @param functionID defines function where exception has been thrown[see *Ex.h headers for IDs]
			 * @param errnoSource defines source of the error code and of the error string[see errnoSourceEnum]
			 * @param baseErrno defines error code
			 * @param baseErrstr defines error string
			 * @param line defines line where exception has been thrown
			 * @param file defines file where exception has been thrown
			 * @param message defines custom message that might clarify the exception
			 */
			baseEx(errorModuleEnum errModule, unsigned long functionID, errnoSourceEnum errnoSource, int baseErrno, const dodoString &baseErrstr, unsigned long line, const dodoString &file, const dodoString &message = __dodostring__);

			/**
			 * destructor
			 */
			~baseEx();

			/**
			 * @return error string
			 */
			operator const dodoString &();

			errorModuleEnum errModule;      ///< module where exception has been thrown
			unsigned long funcID;           ///< function where exception has been thrown[see *Ex.h headers for IDs]
			errnoSourceEnum errnoSource;    ///< the source of the error code and of the error string

			int baseErrno;                  ///< error code
			dodoString baseErrstr;          ///< error string

			unsigned long line;             ///< line where exception has been thrown
			dodoString file;                ///< file where exception has been thrown

			dodoString message;             ///< custom message that might clarify the exception

			/**
			 * set handler for exceptions for specific module
			 * @param module defines for what module to set handler
			 * @param handler defines function that will be called when exception is thrown
			 * @param data decribes data that will be passed to the handler
			 */
			static void setErrorHandler(errorModuleEnum module, errorHandler handler, void *data);

			/**
			 * set handlers for exceptions for all modules
			 * @param handler defines function that will be called when exception is thrown
			 * @param data decribes data that will be passed to the handler
			 */
			static void setErrorHandlers(errorHandler handler, void *data);

			/**
			 * remove handler set for exceptions for specific module
			 * @param module defines for what module to remove handler
			 */
			static void unsetErrorHandler(errorModuleEnum module);

			/**
			 * remove handlers set for exceptions for all modules
			 */
			static void unsetErrorHandlers();

#ifdef DL_EXT

			/**
			 * @return information about module
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param toInit defines data that will be passed to the init function
			 */
			static __exMod getModuleInfo(const dodoString &path, void *toInit = NULL);

			/**
			 * set handler for exceptions for specific module
			 * @return false on error
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data decribes data that will be passed to the handler
			 * @param toInit defines data that will be passed to the init function
			 * @note module for what to set handler is taken from the library information[see __exMod]
			 */
			static bool setErrorHandler(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set handlers for exceptions for all modules
			 * @return false on error
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data decribes data that will be passed to the handler
			 * @param toInit defines data that will be passed to the init function
			 */
			static bool setErrorHandlers(const dodoString &path, void *data, void *toInit = NULL);

#endif

		protected:

			static errorHandler handlersEx[AM_MODULES];     ///< exception handlers

			static bool handlerSetEx[AM_MODULES];           ///< map of set handlers

			static void *handlerDataEx[AM_MODULES];         ///< data that will be passed to the handler

#ifdef DL_EXT

			static void *handlesEx[AM_MODULES];             ///< handles to the libraries

			static bool handlesOpenedEx[AM_MODULES];        //< map of the opened libraries

#endif
	};
};

#endif
