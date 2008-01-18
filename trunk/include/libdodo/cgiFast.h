/***************************************************************************
 *            cgiFast.h
 *
 *  Sat Aug  5 03:31:19 2006
 *  Copyright  2006  Ni@m
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

#ifndef CGIFAST_H_
#define CGIFAST_H_

#include <libdodo/directives.h>

#ifdef FCGI_EXT

#include <fcgiapp.h>

#include <libdodo/types.h>
#include <libdodo/cgiFastEx.h>
#include <libdodo/io.h>
#include <libdodo/threadGuard.h>

namespace dodo
{
	/**
	 * @enum cgiFastIOOperationTypeEnum describes type of operation for hook
	 */
	enum cgiFastIOOperationTypeEnum
	{
		IOCGIFAST_OPERATION_READ,
		IOCGIFAST_OPERATION_READSTRING,
		IOCGIFAST_OPERATION_READSTREAM,
		IOCGIFAST_OPERATION_READSTREAMSTRING,
		IOCGIFAST_OPERATION_WRITE,
		IOCGIFAST_OPERATION_WRITESTRING,
		IOCGIFAST_OPERATION_WRITESTREAM,
		IOCGIFAST_OPERATION_WRITESTREAMSTRING,
	};

	/**
	 * @class cgiFastIO provides interface to fast CGI I/O functionality
	 */
	class ioCGIFast : public io,
					virtual public threadGuardHolder
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			ioCGIFast(ioCGIFast &cf);

		public:

			/**
			 * constructor
			 * @param request defines CGI request descriptor
			 */
			ioCGIFast(FCGX_Request *request);

			/**
			 * destructor
			 */
			virtual ~ioCGIFast();
			
#ifndef IOCGIFAST_WO_XEXEC

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * add hook before the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPreExec(inExec func, void *data);

#ifdef DL_EXT

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPostExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPreExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * set hook from the library that will be executed before/after the operation
			 * @return number in list where function is set
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 * @note type of hook[pre/post] is defined in the library
			 */
			virtual __xexecCounts addExec(const dodoString &module, void *data, void *toInit = NULL);

#endif

#endif
			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readString(dodoString &data);

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void read(char * const data);

			/**
			 * @param data defines data that will be written
			 */
			virtual void writeString(const dodoString &data);

			/**
			 * @param data defines data that will be written
			 */
			virtual void write(const char * const data);

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readStreamString(dodoString &data);

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readStream(char * const data);

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void writeStreamString(const dodoString &data);

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void writeStream(const char * const data);

			/**
			 * flush output
			 */
			virtual void flush();

			/**
			 * @return environment variable
			 * @param data defines name of environment variable
			 */
			virtual char *getenv(const char *buf);
			
		protected:

			/**
			 * @return descriptor of input stream
			 */
			virtual int getInDescriptor() const;

			/**
			 * @return descriptor of output stream
			 */
			virtual int getOutDescriptor() const;

			/**
			 * read
			 * @param data is filled with read data
			 * if inSize bigger than buffer size - reads with few iterations
			 */
			virtual void _read(char * const data);

			/**
			 * write
			 * @param data is data that will be written
			 * if outSize bigger than buffer size - writes with few iterations
			 */
			virtual void _write(const char * const data);

		private:

			FCGX_Request *request;    ///< fast CGI descriptor
	};

	/**
	 * @typedef defines type of function that will be called on new cgi request
	 */
	typedef void (*cgiProc)(ioCGIFast *);

	/**
	 * @class cgiFast provides fast CGI functionality
	 */
	class cgiFast
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			cgiFast(cgiFast &cf);

		public:

#ifdef PTHREAD_EXT

			/**
			 * constructor
			 * @param threading defines if to use threads on new CGI requests
			 * @param threadsNum defines amount of threads for processing fast CGI queue
			 */
			cgiFast(bool threading = true, unsigned int threadsNum = 10);

#else

			/**
			 * constructor
			 */
			cgiFast();

#endif

			/**
			 * destructor
			 */
			virtual ~cgiFast();

			/**
			 * set function that will be called on new CGI request
			 * @param func define function handler
			 */
			virtual void setCGIFunction(cgiProc func);

			/**
			 * listen for incoming requests
			 */
			virtual void listen();

			/**
			 * @return true if called as a fast CGI[not as a CGI]
			 */
			virtual bool isFastCGI();

		private:

#ifdef PTHREAD_EXT

			bool threading;                 ///< true use threading

			unsigned int threadsNum;        ///< amount of threads

			/**
			 * thread that holds one queue of CGI requests
			 * @param data defines the data that will be passed to the thread
			 */
			static void *stackThread(void *data);

			static pthread_mutex_t accept;        ///< accept request mutex

#endif

			static cgiProc cgiF;    ///< function to be called on new request
	};
};

#endif

#endif
