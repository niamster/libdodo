/***************************************************************************
 *            cgiFast.cc
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

#include <libdodo/cgiFast.h>

#ifdef FCGI_EXT

using namespace dodo;

	#ifdef PTHREAD_EXT

pthread_mutex_t cgiFast::accept = PTHREAD_MUTEX_INITIALIZER;

	#endif

//-------------------------------------------------------------------

void
dummyStackThread(cgiFastIO *data)
{
}

//-------------------------------------------------------------------

cgiProc cgiFast::cgiF = &dummyStackThread;

//-------------------------------------------------------------------

cgiFastIO::cgiFastIO(cgiFastIO &cf)
{
}

//-------------------------------------------------------------------

cgiFastIO::cgiFastIO(FCGX_Request *a_request) : request(a_request)
{
}

//-------------------------------------------------------------------

cgiFastIO::~cgiFastIO()
{
}

//-------------------------------------------------------------------

void
cgiFastIO::flush()
{
	if (FCGX_FFlush(request->out) == -1)
		throw baseEx(ERRMODULE_CGIFAST, CGIFASTIOEX_FLUSH, ERR_LIBDODO, CGIFASTIOEX_FAILEDTOFLUSH, CGIFASTIOEX_FAILEDTOFLUSH_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
cgiFastIO::print(const dodoString &buf)
{
	if (FCGX_PutStr(buf.c_str(), buf.size(), request->out) == -1)
		throw baseEx(ERRMODULE_CGIFAST, CGIFASTIOEX_PRINT, ERR_LIBDODO, CGIFASTIOEX_FAILEDTOPRINTSTRING, CGIFASTIOEX_FAILEDTOPRINTSTRING_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
cgiFastIO::read(char         *buf,
				unsigned int size)
{
	FCGX_GetStr(buf, size, request->in);
}

//-------------------------------------------------------------------

char *
cgiFastIO::getenv(const char *buf)
{
	return FCGX_GetParam(buf, request->envp);
}

//-------------------------------------------------------------------

cgiFast::cgiFast(cgiFast &cf)
{
}

//-------------------------------------------------------------------

	#ifdef PTHREAD_EXT

cgiFast::cgiFast(bool a_threading,
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

cgiFast::cgiFast()
{
	FCGX_Init();
}

	#endif

//-------------------------------------------------------------------

cgiFast::~cgiFast()
{
	FCGX_Finish();
}

//-------------------------------------------------------------------

void
cgiFast::setCGIFunction(cgiProc func)
{
	cgiF = func;
}

//-------------------------------------------------------------------

	#ifdef PTHREAD_EXT

void *
cgiFast::stackThread(void *data)
{
	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);

	cgiFastIO cfSTD(&request);

	int res = 0;

	while (true)
	{
		pthread_mutex_lock(&accept);
		res = FCGX_Accept_r(&request);
		pthread_mutex_unlock(&accept);

		if (res == -1)
			throw baseEx(ERRMODULE_CGIFAST, CGIFASTEX_STACKTHREAD, ERR_LIBDODO, CGIFASTEX_ACCEPTFAILED, CGIFASTEX_ACCEPTFAILED_STR, __LINE__, __FILE__);

		cgiF(&cfSTD);

		FCGX_Finish_r(&request);
	}

	return NULL;
}

	#endif

//-------------------------------------------------------------------

void
cgiFast::listen()
{
	if (!isFastCGI())
		throw baseEx(ERRMODULE_CGIFAST, CGIFASTEX_LISTEN, ERR_LIBDODO, CGIFASTEX_ISCGI, CGIFASTEX_ISCGI_STR, __LINE__, __FILE__);

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

		cgiFastIO cfSTD(&request);

		while (true)
		{
			if (FCGX_Accept_r(&request) == -1)
				throw baseEx(ERRMODULE_CGIFAST, CGIFASTEX_LISTEN, ERR_LIBDODO, CGIFASTEX_ACCEPTFAILED, CGIFASTEX_ACCEPTFAILED_STR, __LINE__, __FILE__);

			cgiF(&cfSTD);

			FCGX_Finish_r(&request);
		}
	}
}

//-------------------------------------------------------------------

bool
cgiFast::isFastCGI()
{
	return !FCGX_IsCGI();
}

#endif

//-------------------------------------------------------------------
