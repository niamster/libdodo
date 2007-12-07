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

namespace dodo
{
	/**
	 * @class cgiFastSTD provides interface to fast_cgi
	 */
	class cgiFastSTD
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			cgiFastSTD(cgiFastSTD &cf);

		public:

			/**
			 * constructor
			 * @param request describes CGI request info
			 */
			cgiFastSTD(FCGX_Request *request);

			/**
			 * destructor
			 */
			virtual ~cgiFastSTD();

			/**
			 * sends buf to specific output[fast-cgi safe]
			 * @param buf describes what to send to user
			 */
			virtual void print(const dodoString &buf);

			/**
			 * reads to buf from specific input[fast-cgi safe]
			 * @param buf describes what to receive from user
			 */
			virtual void read(char *buf, unsigned int size);

			/**
			 * @return environment variable[fast-cgi safe]
			 * @param buf describes what to receive from environment
			 */
			virtual char *getenv(const char *buf);

			/**
			 * ioes output[fast-cgi safe]
			 */
			virtual void io();

		private:

			FCGX_Request *request;    ///< CGI request
	};

	/**
	 * @typedef that describes function that will be called on new cgi request
	 */
	typedef void (*cgiProc)(cgiFastSTD *);

	/**
	 * @class cgiFast
	 * provides cpp wrapper for fast-cgi technology
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

				#ifdef PSYSTEMTHREADS_EXT

			/**
			 * constructor
			 * @param threading indicates whether to call new thread on cgi request or proceed a queue
			 * @param threadsNum indicates how many threads to use in fast-cgi server
			 */
			cgiFast(bool threading = true, unsigned int threadsNum = 10);

				#else

			/**
			 * constructor
			 * @param threading indicates whether to call new thread on cgi request or proceed a queue
			 * @param threadsNum indicates how many threads to use in fast-cgi server
			 */
			cgiFast(bool threading = true, unsigned int threadsNum = 10);

				#endif

			/**
			 * destructor
			 */
			virtual ~cgiFast();

			/**
			 * sets function that would be called on CGI request
			 * @param func indicates what function to set
			 */
			virtual void setCGIFunction(cgiProc func);

			/**
			 * listen for incoming requests
			 */
			virtual void listen();

		private:

				#ifdef PSYSTEMTHREADS_EXT

			bool threading;                 ///< threading or not

			unsigned int threadsNum;        ///< amount of threads

			/**
			 * thread that holds one queue of cgi clients
			 * @param data indicates what data will be passed to thread
			 */
			static void *stackThread(void *data);

			static pthread_mutex_t accept;        ///< accept request mutex

				#endif

			static cgiProc cgiF;    ///< function to be called on new request
	};
};

#endif

#endif
