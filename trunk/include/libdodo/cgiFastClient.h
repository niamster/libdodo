/***************************************************************************
 *            cgiFastClient.h
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

#ifndef _CGIFASTCLIENT_H_
#define _CGIFASTCLIENT_H_

#include <libdodo/directives.h>

#ifdef FASTCGI_EXT

#ifdef PTHREAD_EXT

#include <pthread.h>

#endif

#include <fcgiapp.h>

#include <libdodo/types.h>
#include <libdodo/cgiFastClientEx.h>
#include <libdodo/cgiFastClientExchange.h>
#include <libdodo/ioChannel.h>
#include <libdodo/systemThreadGuard.h>

namespace dodo
{
	namespace cgi
	{
		namespace fast
		{
			/**
			 * @typedef clientHandler defines type of function that will be called on new cgi request
			 */
			typedef void (*clientHandler)(clientExchange *);
		
			/**
			 * @class fast provides fast CGI functionality
			 */
			class client
			{
				private:
		
					/**
					 * copy constructor
					 * to prevent copying
					 */
					client(client &cf);
		
				public:
		
		#ifdef PTHREAD_EXT
		
					/**
					 * constructor
					 * @param threading defines if to use threads on new CGI requests
					 * @param threadsNum defines amount of threads for processing fast CGI queue
					 */
					client(bool threading = true, unsigned int threadsNum = 10);
		
		#else
		
					/**
					 * constructor
					 */
					client();
		
		#endif
		
					/**
					 * destructor
					 */
					virtual ~client();
		
					/**
					 * set function that will be called on new CGI request
					 * @param func define function handler
					 */
					virtual void setHandler(clientHandler func);
		
					/**
					 * listen for incoming requests
					 */
					virtual void listen();
		
					/**
					 * @return true if called as a fast CGI[not as a CGI]
					 */
					virtual bool isFastCgi();
		
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
		
					static clientHandler handler;    ///< function to be called on new request
			};
		};
	};
};

#endif

#endif
