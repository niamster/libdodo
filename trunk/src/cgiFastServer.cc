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

#include <libdodo/cgiFastServer.h>

#ifdef FASTCGI_EXT

using namespace dodo::cgi::fast;

#ifdef PTHREAD_EXT

pthread_mutex_t server::accept = PTHREAD_MUTEX_INITIALIZER;

#endif

//-------------------------------------------------------------------

void
dummyStackThread(exchange *data)
{
}

//-------------------------------------------------------------------

serverHandler server::handler = &dummyStackThread;

//-------------------------------------------------------------------

server::server(server &cf)
{
}

//-------------------------------------------------------------------

#ifdef PTHREAD_EXT

server::server(bool a_threading,
			   unsigned int a_threadsNum) : threading(a_threading),
											threadsNum(a_threadsNum)
{
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&accept, &attr);

	pthread_mutexattr_destroy(&attr);

	FCGX_Init();
}

//-------------------------------------------------------------------

#else

server::server()
{
	FCGX_Init();
}

#endif

//-------------------------------------------------------------------

server::~server()
{
	FCGX_Finish();
}

//-------------------------------------------------------------------

void
server::setHandler(serverHandler func)
{
	handler = func;
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
		pthread_mutex_lock(&accept);
		res = FCGX_Accept_r(&request);
		pthread_mutex_unlock(&accept);

		if (res == -1)
			throw baseEx(ERRMODULE_CGIFASTSERVER, SERVEREX_STACKTHREAD, ERR_LIBDODO, SERVEREX_ACCEPTFAILED, CGIFASTSERVEREX_ACCEPTFAILED_STR, __LINE__, __FILE__);

		handler(&cfSTD);

		FCGX_Finish_r(&request);
	}

	return NULL;
}

#endif

//-------------------------------------------------------------------

void
server::listen()
{
	if (!isFastCgi())
		throw baseEx(ERRMODULE_CGIFASTSERVER, SERVEREX_LISTEN, ERR_LIBDODO, SERVEREX_ISCGI, CGIFASTSERVEREX_ISCGI_STR, __LINE__, __FILE__);

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
			if (FCGX_Accept_r(&request) == -1)
				throw baseEx(ERRMODULE_CGIFASTSERVER, SERVEREX_LISTEN, ERR_LIBDODO, SERVEREX_ACCEPTFAILED, CGIFASTSERVEREX_ACCEPTFAILED_STR, __LINE__, __FILE__);

			handler(&cfSTD);

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
