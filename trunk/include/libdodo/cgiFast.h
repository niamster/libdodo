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

namespace dodo
{
	/**
	 * @class cgiFastIO provides interface to fast CGI I/O functionality
	 */
	class cgiFastIO
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			cgiFastIO(cgiFastIO &cf);

		public:

			/**
			 * constructor
			 * @param request defines CGI request descriptor
			 */
			cgiFastIO(FCGX_Request *request);

			/**
			 * destructor
			 */
			virtual ~cgiFastIO();

			/**
			 * print data to the output
			 * @param data defines data that would be printed
			 */
			virtual void print(const dodoString &buf);

			/**
			 * print data to the output
			 * @param data defines data that would be printed
			 * @note print until '\n' is reached
			 */
			virtual void printStream(const dodoString &buf);

			/**
			 * read from the input
			 * @param data defines buffer where data will be written
			 */
			virtual void read(char *data, unsigned int size);

			/**
			 * @return environment variable
			 * @param data defines name of environment variable
			 */
			virtual char *getenv(const char *buf);

			/**
			 * flushe output
			 */
			virtual void flush();

		private:

			FCGX_Request *request;    ///< fast CGI descriptor
	};

	/**
	 * @typedef defines type of function that will be called on new cgi request
	 */
	typedef void (*cgiProc)(cgiFastIO *);

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
