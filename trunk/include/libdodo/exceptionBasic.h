/***************************************************************************
 *            exceptionBasic.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _EXCEPTIONBASIC_H_
#define _EXCEPTIONBASIC_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

#include <exception>

#ifdef DL_EXT

#include <dlfcn.h>

#endif

#ifdef PTHREAD_EXT

#include <pthread.h>

#endif

#include <execinfo.h>
#include <cxxabi.h>
#include <stdlib.h>

namespace dodo
{
	namespace exception
	{
		/**
		 * @enum errnoEnum defines sources, where the code of the error has been got
		 */
		enum errnoEnum
		{
			ERRNO_LIBDODO,
			ERRNO_ERRNO,
			ERRNO_MYSQL,
			ERRNO_H_ERRNO,
			ERRNO_DYNLOAD,
			ERRNO_LIBXML2,
			ERRNO_ZLIB,
			ERRNO_SQLITE,
			ERRNO_BZIP,
			ERRNO_IMAGEMAGICK,
			ERRNO_POSIXREGEX,
			ERRNO_PCRE,
			ERRNO_BFD,
			ERRNO_OPENSSL
		};

		/**
		 * @enum errorModuleEnum defines modules where exception might be thrown
		 */
		enum errorModuleEnum
		{
			ERRMODULE_DATABASEMYSQL = 0,
			ERRMODULE_DATABASEPOSTGRESQL,
			ERRMODULE_DATABASESQLCONSTRUCTOR,
			ERRMODULE_DATABASESQLITE,
			ERRMODULE_DATAFORMATXMLPROCESSOR,
			ERRMODULE_DATAFORMATJSONNODE,
			ERRMODULE_DATAFORMATJSONPROCESSOR,
			ERRMODULE_DATATPLPROCESSOR,
			ERRMODULE_DATAMEMORYSHARED,
			ERRMODULE_XEXEC,
			ERRMODULE_TOOLSFILESYSTEM,
			ERRMODULE_TOOLSOS,
			ERRMODULE_TOOLSOSSYNCTHREADSECTION,
			ERRMODULE_TOOLSTIME,
			ERRMODULE_TOOLSMISC,
			ERRMODULE_TOOLSCODE,
			ERRMODULE_TOOLSNETWORK,
			ERRMODULE_TOOLSLIBRARY,
			ERRMODULE_TOOLSREGEXP,
			ERRMODULE_IOFILEREGULAR,
			ERRMODULE_IOFILETEMP,
			ERRMODULE_IOFILEFIFO,
			ERRMODULE_IOPIPE,
			ERRMODULE_IOSTDIO,
			ERRMODULE_IOSTRING,
			ERRMODULE_IONETWORKCONNECTION,
			ERRMODULE_IONETWORKCLIENT,
			ERRMODULE_IONETWORKSERVER,
			ERRMODULE_IONETWORKSSLCLIENT,
			ERRMODULE_IONETWORKSSLSERVER,
			ERRMODULE_IONETWORKEXCHANGE,
			ERRMODULE_IONETWORKSSLEXCHANGE,
			ERRMODULE_IONETWORKHTTP,
			ERRMODULE_IOEVENT,
			ERRMODULE_PCPROCESSCOLLECTION,
			ERRMODULE_PCTHREADCOLLECTION,
			ERRMODULE_PCSYNCTHREADDATASINGLE,
			ERRMODULE_PCSYNCPROCESSDATASINGLE,
			ERRMODULE_PCSYNCTHREADDATACOLLECTION,
			ERRMODULE_PCSYNCPROCESSDATACOLLECTION,
			ERRMODULE_PCSYNCTHREADSECTION,
			ERRMODULE_PCSYNCPROCESSSECTION,
			ERRMODULE_GRAPHICSIMAGE,
			ERRMODULE_GRAPHICSTRANSFORM,
			ERRMODULE_GRAPHICSDRAW,
			ERRMODULE_CGIFASTSERVER,
			ERRMODULE_CGIFASTEXCHANGE,
			ERRMODULE_CGIDIALOGUE,
			ERRMODULE_RPCVALUE,
			ERRMODULE_RPCRESPONSE,
			ERRMODULE_RPCJSONMETHOD,
			ERRMODULE_RPCJSONRESPONSE,
		};

#define BASEEX_MODULES    51

#ifdef DL_EXT

		/**
		 * @struct __basicMod
		 * @brief is returned from initBaseExModule in the library
		 */
		struct __basicMod
		{
			char name[64];                                      ///< name of the library
			char discription[256];                              ///< discription of the library
			char hook[64];                                      ///< name of the function in module that will be as a hook
			short module;                                       ///< for what module handler should be set[see errorModuleEnum]
		};

		/**
		 * @typedef initBaseExModule
		 * @brief defines type of init function for library
		 * @param data defines user data
		 */
		typedef __basicMod (*initBaseExModule)(void *data);

		/**
		 * @typedef deinitBaseExModule
		 * @brief defines type of deinit function for library
		 */
		typedef void (*deinitBaseExModule)();

#endif

		class basic;

		/**
		 * @typedef errorHandler
		 * @brief defines type of hook function
		 * @param module defines module where exception occured[see errorModuleEnum]
		 * @param ex defines pointer to basic object with exception information
		 * @param data defines user data
		 */
		typedef void (*errorHandler)(int module, basic *ex, void *data);

		/**
		 * @struct __call
		 * @brief describes function call in call stack
		 */
		struct __call
		{
#ifdef DL_EXT

			dodoString object; ///< name of the object where call was found
			dodoString symbol; ///< name of the call
			void *address; ///< address of the call

#else

			dodoString symbol; ///< the symbolic representation of each address consists of the function name, a hexadecimal offset into the function, and the actual return address

#endif
		};

		/**
		 * @class basic
		 * @brief describes exception that has been thrown
		 */
		class basic : public std::exception,
					  public singleton<basic>
		{
			friend class singleton<basic>;

			private:

				/**
				 * constructor
				 */
				basic() throw ();

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
				basic(int errModule, int functionID, int errnoSource, int baseErrno, const dodoString &baseErrstr, unsigned long line, const dodoString &file, const dodoString &message = __dodostring__) throw ();

				/**
				 * destructor
				 */
				~basic() throw ();

				/**
				 * @return error string
				 */
				operator const dodoString &();

				/**
				 * return error string
				 */
				virtual const char *what() const throw ();

				/**
				 * @return call stack to the exception point
				 */
				virtual dodoString getCallStack();

				int errModule;                                              ///< module where exception has been thrown
				int funcID;                                                 ///< function where exception has been thrown[see *Ex.h headers for IDs]
				int errnoSource;                                            ///< the source of the error code and of the error string

				int baseErrno;                                              ///< error code
				dodoString baseErrstr;                                      ///< error string

				unsigned long line;                                         ///< line where exception has been thrown
				dodoString file;                                            ///< file where exception has been thrown

				dodoString message;                                         ///< custom message that might clarify the exception

				dodoArray<__call> callStack; 								///< call stack of the raised exception

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
				static __basicMod getModuleInfo(const dodoString &path, void *toInit = NULL);

				/**
				 * set handler for exceptions for specific module
				 * @return false on error
				 * @param path defines path to the library[if not in ldconfig db] or library name
				 * @param data decribes data that will be passed to the handler
				 * @param toInit defines data that will be passed to the init function
				 * @note module for what to set handler is taken from the library information[see __basicMod]
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

				static errorHandler handlersEx[BASEEX_MODULES];                                     ///< exception handlers

				static bool handlerSetEx[BASEEX_MODULES];                                           ///< map of set handlers

				static void *handlerDataEx[BASEEX_MODULES];                                         ///< data that will be passed to the handler

#ifdef DL_EXT

				static void *handlesEx[BASEEX_MODULES];                                             ///< handles to the libraries

				static bool handlesOpenedEx[BASEEX_MODULES];                                        //< map of the opened libraries

#endif

				/**
				 * @class syncThreadSection
				 * @brief performs atomic locks using mutexes
				 */
				class syncThreadSection
				{
					public:

						/**
						 * consructor
						 */
						syncThreadSection();

						/**
						 * destructor
						 */
						virtual ~syncThreadSection();

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

						static pthread_mutex_t keeper;                                             ///< mutex

#endif
				};

				static syncThreadSection keeper;                             ///< lock

				/**
				 * @class syncThreadStack
				 * @brief provides thread safe behaviour
				 * @note it locks in constructor and unlocks in destructor
				 */
				class syncThreadStack
				{
					public:

						/**
						 * contructor
						 */
						syncThreadStack();

						/**
						 * destructor
						 */
						virtual ~syncThreadStack();
				};

				static unsigned long instances;
		};
	};
};

#endif
