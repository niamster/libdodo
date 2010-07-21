/***************************************************************************
 *			  exceptionBasic.h
 *
 *	Mon Feb	 7 2005
 *	Copyright  2005	 Dmytro Milinevskyy
 *	milinevskyy@gmail.com
 ****************************************************************************/

/*
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *	the Free Software Foundation;
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Library General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _EXCEPTIONBASIC_H_
#define _EXCEPTIONBASIC_H_ 1

#include <libdodo/directives.h>

#include <exception>

#include <libdodo/types.h>

namespace dodo {
    namespace exception {
        /**
         * @enum errnoEnum defines sources, where the code of the error has been got
         */
        enum errnoEnum {
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
            ERRNO_OPENSSL,
        };

        /**
         * @enum moduleEnum defines modules where exception might be thrown
         */
        enum moduleEnum {
            MODULE_DATABASEMYSQL = 0,
            MODULE_DATABASEPOSTGRESQL,
            MODULE_DATABASESQLITE,
            MODULE_DATAFORMATXMLPROCESSOR,
            MODULE_DATAFORMATJSONNODE,
            MODULE_DATAFORMATJSONPROCESSOR,
            MODULE_DATATPLPROCESSOR,
            MODULE_DATAMEMORYSHARED,
            MODULE_XEXEC,
            MODULE_TOOLSFILESYSTEM,
            MODULE_TOOLSOS,
            MODULE_TOOLSOSSYNCTHREADSECTION,
            MODULE_TOOLSTIME,
            MODULE_TOOLSMISC,
            MODULE_TOOLSCODE,
            MODULE_TOOLSNETWORK,
            MODULE_TOOLSLIBRARY,
            MODULE_TOOLSREGEXP,
            MODULE_IOFILEREGULAR,
            MODULE_IOFILETEMP,
            MODULE_IOFILEFIFO,
            MODULE_IOPIPE,
            MODULE_IOSTDIO,
            MODULE_IOMEMORY,
            MODULE_IONETWORKCONNECTION,
            MODULE_IONETWORKCLIENT,
            MODULE_IONETWORKSERVER,
            MODULE_IONETWORKSSLCLIENT,
            MODULE_IONETWORKSSLSERVER,
            MODULE_IONETWORKEXCHANGE,
            MODULE_IONETWORKSSLEXCHANGE,
            MODULE_IONETWORKHTTP,
            MODULE_IOEVENTMANAGER,
            MODULE_PCEXECUTIONMANAGER,
            MODULE_PCEXECUTIONSCHEDULER,
            MODULE_PCEXECUTIONPROCESS,
            MODULE_PCEXECUTIONTHREAD,
            MODULE_PCSYNCTHREAD,
            MODULE_PCSYNCPROCESS,
            MODULE_GRAPHICSIMAGE,
            MODULE_GRAPHICSTRANSFORM,
            MODULE_GRAPHICSDRAW,
            MODULE_CGIFASTSERVER,
            MODULE_CGIFASTEXCHANGE,
            MODULE_CGIDIALOGUE,
            MODULE_RPCVALUE,
            MODULE_RPCRESPONSE,
            MODULE_RPCJSONMETHOD,
            MODULE_RPCJSONRESPONSE,

            MODULE_ENUMSIZE
        };

        /**
         * @struct __call__
         * @brief describes function call in call stack
         */
        struct __call__ {
            dodo::string object;      ///< name of the object where call was found
            dodo::string symbol;      ///< name of the call
            void       *address;    ///< address of the call
        };

        /**
         * @class basic
         * @brief describes exception that has been thrown
         */
        class basic : public std::exception,
                      public singleton<basic>{
            friend class singleton<basic>;

          private:

            /**
             * constructor
             */
            basic() throw ();

          public:

            /**
             * constructor
             * @param module defines module where exception has been thrown
             * @param function defines function where exception has been thrown, @see *Ex.h headers for IDs
             * @param errnoSource defines source of the error code and of the error string, @see exception::errnoEnum
             * @param errNo defines error code
             * @param errStr defines error string
             * @param line defines line where exception has been thrown
             * @param file defines file where exception has been thrown
             * @param message defines custom message that might clarify the exception
             */
            basic(int              module,
                  int              function,
                  int              errnoSource,
                  int              errNo,
                  const dodo::string &errStr,
                  unsigned long    line,
                  const dodo::string &file,
                  const dodo::string &message = __dodostring__) throw ();

            /**
             * destructor
             */
            ~basic() throw ();

#ifdef DL_EXT
            /**
             * @struct __module__
             * @brief is returned from initModule in the library
             */
            struct __module__ {
                char name[64];                              ///< name of the library
                char discription[256];                      ///< discription of the library
                char hook[64];                              ///< name of the function in module that will be as a hook
                char cookie[32];                            ///< cookie that would be passed to deinitModule
                bool modules[MODULE_ENUMSIZE];              ///< for what modules handler should be set, @see exception::moduleEnum
            };

            /**
             * @typedef initModule
             * @brief defines type of init function for library
             * @param data defines user data
             * @note name in the library must be initExceptionBasicModule
             */
            typedef __module__ (*initModule)(void *data);

            /**
             * @typedef deinitModule
             * @brief defines type of deinit function for library
             * @param cookie defines cookie data returned from initModule
             * @note name in the library must be deinitExceptionBasicModule
             */
            typedef void (*deinitModule)(char cookie[32]);
#endif

            /**
             * @typedef handler
             * @brief defines type of hook function
             * @param module defines module where exception occured, @see exception::moduleEnum
             * @param ex defines pointer to basic object with exception information
             * @param data defines user data
             */
            typedef void (*handler)(int module, basic *ex, void *data);

            /**
             * @return error string
             */
            operator const dodo::string & ();

            /**
             * return error string
             */
            virtual const char *what() const throw ();

            /**
             * @return call stack to the exception point
             */
            virtual dodo::string backtrace();

            int source;                             ///< module where exception has been thrown
            int function;                           ///< function where exception has been thrown, @see *Ex.h headers for IDs
            int errnoSource;                        ///< the source of the error code and of the error string
            int errNo;                              ///< error code
            dodo::string errStr;                      ///< error string
            unsigned long line;                     ///< line where exception has been thrown
            dodo::string file;                        ///< file where exception has been thrown
            dodo::string message;                     ///< custom message that might clarify the exception

            dodoArray<__call__> callStack;          ///< call stack of the raised exception

            /**
             * set handler for exceptions for specific module
             * @param module defines for what module to set handler
             * @param handler defines function that will be called when exception is thrown
             * @param data decribes data that will be passed to the handler
             */
            static void setHandler(moduleEnum module,
                                   handler    handler,
                                   void       *data);

            /**
             * set handlers for exceptions for all modules
             * @param handler defines function that will be called when exception is thrown
             * @param data decribes data that will be passed to the handler
             */
            static void setHandler(handler handler,
                                   void    *data);

            /**
             * remove handler set for exceptions for specific module
             * @param module defines for what module to remove handler
             */
            static void removeHandler(moduleEnum module);

            /**
             * remove handlers set for exceptions for all modules
             */
            static void removeHandlers();

#ifdef DL_EXT
            /**
             * @return information about module
             * @param path defines path to the library[if not in ldconfig db] or library name
             * @param toInit defines data that will be passed to the init function
             */
            static __module__ module(const dodo::string &path,
                                     void             *toInit = NULL);

            /**
             * set handler for exceptions for specific module
             * @return false on error
             * @param path defines path to the library[if not in ldconfig db] or library name
             * @param data decribes data that will be passed to the handler
             * @param toInit defines data that will be passed to the init function
             * @note module for what to set handler is taken from the library information, @see exception::basic::__module__
             */
            static void setHandler(const dodo::string &path,
                                   void             *data,
                                   void             *toInit = NULL);
#endif

          protected:

            static handler handlers[MODULE_ENUMSIZE];       ///< exception handlers

            static bool handlerMap[MODULE_ENUMSIZE];        ///< map of set handlers

            static void *handlerData[MODULE_ENUMSIZE];      ///< data that will be passed to the handler

#ifdef DL_EXT
            static void *handles[MODULE_ENUMSIZE];          ///< handles to the libraries
            static char cookies[MODULE_ENUMSIZE][32];          ///< handles to the libraries

            static bool handlesOpened[MODULE_ENUMSIZE];     ///< map of the opened libraries
#endif

            class sync {
              public:

                /**
                 * @class stack
                 * @brief provides thread safe behaviour
                 * @note locked in constructor and unlocked in destructor
                 * @note no exceptions thrown
                 */
                class stack {
                  public:

                    /**
                     * contructor
                     */
                    stack();

                    /**
                     * destructor
                     */
                    virtual ~stack();

                    void *keeper; ///< section locker
                };
            };

            static unsigned long instances;
        };
    };
};
#endif
