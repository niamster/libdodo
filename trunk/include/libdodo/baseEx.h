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

#ifdef PTHREAD_EXT

#include <pthread.h>

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
		ERRMODULE_DBMYSQL = 0,
		ERRMODULE_DBPOSTGRESQL,
		ERRMODULE_DBSQLCONSTRUCTOR,
		ERRMODULE_DBSQLITE,
		ERRMODULE_REGEXP,
		ERRMODULE_XEXEC,
		ERRMODULE_TOOLSFILESYSTEM,
		ERRMODULE_TOOLSOS,
		ERRMODULE_TOOLSOSSTATICATOMICMUTEX,
		ERRMODULE_TOOLSTIME,
		ERRMODULE_TOOLSMISC,
		ERRMODULE_TOOLSNETWORK,
		ERRMODULE_XMLPROCESSOR,
		ERRMODULE_IONETWORKCLIENT,
		ERRMODULE_IONETWORKSERVER,
		ERRMODULE_IOFILE,
		ERRMODULE_IONETWORKOPTIONS,
		ERRMODULE_IOSTDIO,
		ERRMODULE_IONETWORKEXCHANGE,
		ERRMODULE_IONETWORKHTTP,
		ERRMODULE_IONONBLOCKEDACCESS,
		ERRMODULE_IPCPROCESSCOLLECTION,
		ERRMODULE_IPCTHREADCOLLECTION,
		ERRMODULE_IPCSHAREDDATA,
		ERRMODULE_IPCTHREADSHAREDDATAGUARD,
		ERRMODULE_IPCPROCESSSHAREDDATAGUARD,
		ERRMODULE_IPCPROCESSSHAREDDATACOLLECTIONGUARD,
		ERRMODULE_IPCTHREADSHAREDDATACOLLECTIONGUARD,
		ERRMODULE_IPCATOMICMUTEX,
		ERRMODULE_IPCATOMICSEMAPHORE,
		ERRMODULE_LIBRARYLOADER,
		ERRMODULE_IMAGE,
		ERRMODULE_JSONPROCESSOR,
		ERRMODULE_JSONNODE,
		ERRMODULE_CGIPREPROCESSOR,
		ERRMODULE_CGIPROCESSOR,
		ERRMODULE_CGIFASTSERVER,
		ERRMODULE_CGIFASTEXCHANGE,
		ERRMODULE_CGISERVER,
		ERRMODULE_RPCVALUE,
		ERRMODULE_RPCRESPONSE,
	};

#define BASEEX_MODULES    41

#ifdef DL_EXT

	/**
	 * @struct __baseExMod is returned from initBaseExModule in the library
	 */
	struct __baseExMod
	{
		char name[64];                  ///< name of the library
		char discription[256];          ///< discription of the library
		char hook[64];                  ///< name of the function in module that will be as a hook
		short module;                   ///< for what module handler should be set[see errorModuleEnum]
	};

	/**
	 * @typedef initBaseExModule defines type of init function for library
	 */
	typedef __baseExMod (*initBaseExModule)(void *);

	/**
	 * @typedef deinitBaseExModule defines type of deinit function for library
	 */
	typedef void (*deinitBaseExModule)();

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
			static __baseExMod getModuleInfo(const dodoString &path, void *toInit = NULL);

			/**
			 * set handler for exceptions for specific module
			 * @return false on error
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data decribes data that will be passed to the handler
			 * @param toInit defines data that will be passed to the init function
			 * @note module for what to set handler is taken from the library information[see __baseExMod]
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

			static errorHandler handlersEx[BASEEX_MODULES];     ///< exception handlers

			static bool handlerSetEx[BASEEX_MODULES];           ///< map of set handlers

			static void *handlerDataEx[BASEEX_MODULES];         ///< data that will be passed to the handler

#ifdef DL_EXT

			static void *handlesEx[BASEEX_MODULES];             ///< handles to the libraries

			static bool handlesOpenedEx[BASEEX_MODULES];        //< map of the opened libraries

#endif

			/**
			 * @class staticAtomicMutex performs atomic locks using mutexes
			 */
			class staticAtomicMutex
			{
				public:

					/**
					 * consructor
					 */
					staticAtomicMutex();

					/**
					 * destructor
					 */
					virtual ~staticAtomicMutex();

					/**
					 * lock critical section
					 */
					virtual void acquire();

					/**
					 * unlock critical section
					 */
					virtual void release();

				protected:

#ifdef PTHREAD_EXT

					static pthread_mutex_t keeper; ///< mutex

#endif
			};

			static staticAtomicMutex keeper; ///< lock

			/**
			 * @class raceHazardGuard provides thread safe behaviour
			 * @note it locks in constructor and unlocks in destructor
			 */
			class raceHazardGuard
			{
				public:

					/**
					 * contructor
					 */
					raceHazardGuard();

					/**
					 * destructor
					 */
					virtual ~raceHazardGuard();
			};
	};
};

#endif
