/***************************************************************************
 *            cgiFastServer.cc
 *
 *  Sat Aug  5 03:37:19 2006
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

#include <libdodo/cgiFastServer.h>

#ifdef FASTCGI_EXT

using namespace dodo::cgi::fast;

#ifdef PTHREAD_EXT

pthread_mutex_t server::acceptM = PTHREAD_MUTEX_INITIALIZER;

//-------------------------------------------------------------------

pthread_mutex_t server::requestsM = PTHREAD_MUTEX_INITIALIZER;

#endif

//-------------------------------------------------------------------

unsigned long server::limit = 0;

//-------------------------------------------------------------------

unsigned long server::requests = -1;

//-------------------------------------------------------------------

void
dummyStackThread(dodo::cgi::exchange &data)
{
}

//-------------------------------------------------------------------

dodo::cgi::serverHandler server::handler = &dummyStackThread;

//-------------------------------------------------------------------

server::server(server &cf)
{
}

//-------------------------------------------------------------------

#ifdef PTHREAD_EXT

server::server(unsigned long a_limit,
               bool threading,
			   unsigned int threadsNum) : threading(threading),
									  threadsNum(threadsNum)
{
	limit = a_limit;

	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&acceptM, &attr);
	pthread_mutex_init(&requestsM, &attr);

	pthread_mutexattr_destroy(&attr);

	FCGX_Init();
}

//-------------------------------------------------------------------

#else

server::server(unsigned long a_limit)
{
	limit = a_limit;

	FCGX_Init();
}

#endif

//-------------------------------------------------------------------

server::~server()
{
	FCGX_Finish();
}

//-------------------------------------------------------------------

#ifdef PTHREAD_EXT

void *
server::stackThread(void *data)
{
	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);

	exchange cfSTD(&request);

	int res = 0;

	while (true)
	{

		if (limit != 0)
		{
			pthread_mutex_lock(&requestsM);

			++requests;

			if (requests >= limit)
			{
				pthread_mutex_unlock(&requestsM);

				break;
			}

			pthread_mutex_lock(&requestsM);
		}

		pthread_mutex_lock(&acceptM);
		res = FCGX_Accept_r(&request);
		pthread_mutex_unlock(&acceptM);

		if (res == -1)
			throw exception::basic(exception::ERRMODULE_CGIFASTSERVER, SERVEREX_STACKTHREAD, exception::ERRNO_LIBDODO, SERVEREX_ACCEPTFAILED, CGIFASTSERVEREX_ACCEPTFAILED_STR, __LINE__, __FILE__);

		handler(cfSTD);

		FCGX_Finish_r(&request);
	}

	return NULL;
}

#endif

//-------------------------------------------------------------------

void
server::listen(serverHandler func)
{
	if (!isFastCgi())
		throw exception::basic(exception::ERRMODULE_CGIFASTSERVER, SERVEREX_LISTEN, exception::ERRNO_LIBDODO, SERVEREX_ISCGI, CGIFASTSERVEREX_ISCGI_STR, __LINE__, __FILE__);

	handler = func;

	requests = 0;

#ifdef PTHREAD_EXT
	if (threading)
	{
		pthread_t *id = new pthread_t[threadsNum];

		unsigned int i = 0;

		for (; i < threadsNum; ++i)
			pthread_create(&id[i], NULL, stackThread, NULL);

		for (i = 0; i < threadsNum; ++i)
			pthread_join(id[i], NULL);

		delete [] id;
	}
	else
#endif
	{
		FCGX_Request request;
		FCGX_InitRequest(&request, 0, 0);

		exchange cfSTD(&request);

		while (true)
		{
			if (limit != 0)
			{
				++requests;

				if (requests >= limit)
					break;
			}

			if (FCGX_Accept_r(&request) == -1)
				throw exception::basic(exception::ERRMODULE_CGIFASTSERVER, SERVEREX_LISTEN, exception::ERRNO_LIBDODO, SERVEREX_ACCEPTFAILED, CGIFASTSERVEREX_ACCEPTFAILED_STR, __LINE__, __FILE__);

			handler(cfSTD);

			FCGX_Finish_r(&request);
		}
	}
}

//-------------------------------------------------------------------

bool
server::isFastCgi()
{
	return !FCGX_IsCGI();
}

#endif

//-------------------------------------------------------------------
