/***************************************************************************
 *            cgiFastServer.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _CGIFASTSERVER_H_
#define _CGIFASTSERVER_H_

#include <libdodo/directives.h>

#ifdef FASTCGI_EXT

#ifdef PTHREAD_EXT

#include <pthread.h>

#endif

#include <fcgiapp.h>

#include <libdodo/types.h>
#include <libdodo/cgiFastServerEx.h>
#include <libdodo/cgiFastExchange.h>
#include <libdodo/ioChannel.h>
#include <libdodo/pcThreadGuard.h>

namespace dodo
{
	namespace cgi
	{
		namespace fast
		{
			/**
			 * @typedef serverHandler defines type of function that will be called on new cgi request
			 * @param ex defines fast::exchange object for I/O
			 */
			typedef void (*serverHandler)(exchange &ex);

			/**
			 * @class fast provides fast CGI functionality
			 */
			class server
			{
				private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					server(server &cf);

				public:

#ifdef PTHREAD_EXT

					/**
					 * constructor
					 * @param threading defines if to use threads on new CGI requests
					 * @param threadsNum defines amount of threads for processing fast CGI queue
					 */
					server(bool threading = true, unsigned int threadsNum = 10);

#else

					/**
					 * constructor
					 */
					server();

#endif

					/**
					 * destructor
					 */
					virtual ~server();

					/**
					 * set function that will be called on new CGI request
					 * @param func define function handler
					 */
					virtual void setHandler(serverHandler func);

					/**
					 * listen for incoming requests
					 * @param limit defines limit of incoming requests
					 * @note if limit is exhausted `listen` will return
					 * if limit is 0 `listen` never returns
					 */
					virtual void listen(unsigned long limit=0);

					/**
					 * @return true if called as a fast CGI[not as a CGI]
					 */
					virtual bool isFastCgi();

				private:

#ifdef PTHREAD_EXT

					bool threading;                                     ///< true use threading

					unsigned int threadsNum;                            ///< amount of threads

					/**
					 * thread that holds one queue of CGI requests
					 * @param data defines the data that will be passed to the thread
					 */
					static void *stackThread(void *data);

					static pthread_mutex_t acceptM;                            ///< accept request mutex
					static pthread_mutex_t requestsM;                            ///< request conter mutex

#endif

					static serverHandler handler;                        ///< function to be called on new request

					static unsigned long limit; ///< limit of requests to serve; if 0 server forever[0 by default]
					static unsigned long requests; ///< counter of requests
			};
		};
	};
};

#endif

#endif
