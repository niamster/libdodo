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

#include <libdodo/directives.h>

#ifdef FASTCGI_EXT
#include <fcgiapp.h>

#include "cgiFastRequest.inline"

#include <libdodo/cgiFastServer.h>
#include <libdodo/types.h>
#include <libdodo/cgiFastServerEx.h>
#include <libdodo/cgiFastExchange.h>
#include <libdodo/pcSyncThreadSection.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::cgi::fast;

dodo::pc::sync::thread::section server::acceptLock;

//-------------------------------------------------------------------

dodo::pc::sync::thread::section server::requestLock;

//-------------------------------------------------------------------

void
dummyfastCGIThread(dodo::cgi::exchange &data)
{
}

//-------------------------------------------------------------------

dodo::cgi::serverHandler server::handler = &dummyfastCGIThread;

//-------------------------------------------------------------------

server::server(server &cf)
{
}

//-------------------------------------------------------------------

server::server(unsigned long a_limit,
			   bool          threading,
			   unsigned int  threadsNum) : threading(threading),
										   threadsNum(threadsNum)
{
	limit = a_limit;

	FCGX_Init();
}

//-------------------------------------------------------------------

server::~server()
{
	FCGX_Finish();
}

//-------------------------------------------------------------------

void *
server::fastCGIThread(void *data)
{
	FCGX_Request req;
	__request__ request = &req;
	FCGX_InitRequest(request.request, 0, 0);

	exchange cfSTD(request);

	unsigned long limit = *(unsigned long *)data;
	unsigned long requests = 0;

	int res = 0;

	while (true)
	{
		if (limit != 0)
		{
			pc::sync::protector rp(&requestLock);

			++requests;

			if (requests >= limit)
			{
				break;
			}
		}

		acceptLock.acquire();
		res = FCGX_Accept_r(request.request);
		acceptLock.release();

		if (res == -1)
		{
			throw exception::basic(exception::ERRMODULE_CGIFASTSERVER, SERVEREX_STACKTHREAD, exception::ERRNO_LIBDODO, SERVEREX_ACCEPTFAILED, CGIFASTSERVEREX_ACCEPTFAILED_STR, __LINE__, __FILE__);
		}

		handler(cfSTD);

		FCGX_Finish_r(request.request);
	}

	return NULL;
}

//-------------------------------------------------------------------

void
server::serve(serverHandler func)
{
	if (!isFastCgi())
	{
		throw exception::basic(exception::ERRMODULE_CGIFASTSERVER, SERVEREX_LISTEN, exception::ERRNO_LIBDODO, SERVEREX_ISCGI, CGIFASTSERVEREX_ISCGI_STR, __LINE__, __FILE__);
	}

	handler = func;

	if (threading)
	{
		pthread_t *id = new pthread_t[threadsNum];

		unsigned int i = 0;

		for (; i < threadsNum; ++i)
		{
			pthread_create(&id[i], NULL, fastCGIThread, &limit);
		}

		for (i = 0; i < threadsNum; ++i)
		{
			pthread_join(id[i], NULL);
		}

		delete [] id;
	}
	else
	{
		unsigned long requests = 0;

		FCGX_Request req;
		__request__ request = &req;
		FCGX_InitRequest(request.request, 0, 0);

		exchange cfSTD(request);

		while (true)
		{
			if (limit != 0)
			{
				++requests;

				if (requests >= limit)
				{
					break;
				}
			}

			if (FCGX_Accept_r(request.request) == -1)
			{
				throw exception::basic(exception::ERRMODULE_CGIFASTSERVER, SERVEREX_LISTEN, exception::ERRNO_LIBDODO, SERVEREX_ACCEPTFAILED, CGIFASTSERVEREX_ACCEPTFAILED_STR, __LINE__, __FILE__);
			}

			handler(cfSTD);

			FCGX_Finish_r(request.request);
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
