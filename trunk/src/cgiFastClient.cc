/***************************************************************************
 *            cgiFastClient.cc
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

#include <libdodo/cgiFastClient.h>

#ifdef FASTCGI_EXT

using namespace dodo::cgi::fast;

#ifdef PTHREAD_EXT

pthread_mutex_t client::accept = PTHREAD_MUTEX_INITIALIZER;

#endif

//-------------------------------------------------------------------

void
dummyStackThread(clientExchange *data)
{
}

//-------------------------------------------------------------------

clientHandler client::handler = &dummyStackThread;

//-------------------------------------------------------------------

client::client(client &cf)
{
}

//-------------------------------------------------------------------

#ifdef PTHREAD_EXT

client::client(bool a_threading,
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

client::client()
{
	FCGX_Init();
}

#endif

//-------------------------------------------------------------------

client::~client()
{
	FCGX_Finish();
}

//-------------------------------------------------------------------

void
client::setHandler(clientHandler func)
{
	handler = func;
}

//-------------------------------------------------------------------

#ifdef PTHREAD_EXT

void *
client::stackThread(void *data)
{
	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);

	clientExchange cfSTD(&request);

	int res = 0;

	while (true)
	{
		pthread_mutex_lock(&accept);
		res = FCGX_Accept_r(&request);
		pthread_mutex_unlock(&accept);

		if (res == -1)
			throw baseEx(ERRMODULE_CGIFASTCLIENT, FASTCLIENTEX_STACKTHREAD, ERR_LIBDODO, FASTCLIENTEX_ACCEPTFAILED, FASTCLIENTEX_ACCEPTFAILED_STR, __LINE__, __FILE__);

		handler(&cfSTD);

		FCGX_Finish_r(&request);
	}

	return NULL;
}

#endif

//-------------------------------------------------------------------

void
client::listen()
{
	if (!isFastCgi())
		throw baseEx(ERRMODULE_CGIFASTCLIENT, FASTCLIENTEX_LISTEN, ERR_LIBDODO, FASTCLIENTEX_ISCGI, FASTCLIENTEX_ISCGI_STR, __LINE__, __FILE__);

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

		clientExchange cfSTD(&request);

		while (true)
		{
			if (FCGX_Accept_r(&request) == -1)
				throw baseEx(ERRMODULE_CGIFASTCLIENT, FASTCLIENTEX_LISTEN, ERR_LIBDODO, FASTCLIENTEX_ACCEPTFAILED, FASTCLIENTEX_ACCEPTFAILED_STR, __LINE__, __FILE__);

			handler(&cfSTD);

			FCGX_Finish_r(&request);
		}
	}
}

//-------------------------------------------------------------------

bool
client::isFastCgi()
{
	return !FCGX_IsCGI();
}

#endif

//-------------------------------------------------------------------
