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
dummyStackThread(ioCGIFast *data)
{
}

//-------------------------------------------------------------------

cgiProc cgiFast::cgiF = &dummyStackThread;

//-------------------------------------------------------------------

ioCGIFast::ioCGIFast(ioCGIFast &cf)
{
}

//-------------------------------------------------------------------

ioCGIFast::ioCGIFast(FCGX_Request *a_request) : request(a_request)
{
}

//-------------------------------------------------------------------

ioCGIFast::~ioCGIFast()
{
}

//-------------------------------------------------------------------

void
ioCGIFast::flush()
{
	if (FCGX_FFlush(request->out) == -1)
		throw baseEx(ERRMODULE_CGIFAST, IOCGIFASTEX_FLUSH, ERR_LIBDODO, CGIFASTIOEX_FAILEDTOFLUSH, CGIFASTIOEX_FAILEDTOFLUSH_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

char *
ioCGIFast::getenv(const char *buf)
{
	return FCGX_GetParam(buf, request->envp);
}

//-------------------------------------------------------------------

int
ioCGIFast::getInDescriptor() const
{
	return -1;
}

//-------------------------------------------------------------------

int
ioCGIFast::getOutDescriptor() const
{
	return -1;
}

//-------------------------------------------------------------------

#ifndef IOCGIFAST_WO_XEXEC

int
ioCGIFast::addPostExec(inExec func,
				   void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_IOCGIFAST, data);
}

//-------------------------------------------------------------------

int
ioCGIFast::addPreExec(inExec func,
				  void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_IOCGIFAST, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
ioCGIFast::addPostExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_IOCGIFAST, data, toInit);
}

//-------------------------------------------------------------------

__xexecCounts
ioCGIFast::addExec(const dodoString &module,
			   void             *data,
			   void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_IOCGIFAST, data, toInit);
}

//-------------------------------------------------------------------

int
ioCGIFast::addPreExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_IOCGIFAST, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
ioCGIFast::_read(char * const a_void)
{
	memset(a_void, '\0', inSize);

	FCGX_GetStr(a_void, inSize, request->in);
}

//-------------------------------------------------------------------

void
ioCGIFast::read(char * const a_void)
{
	guard pg(this);

#ifndef IOCGIFAST_WO_XEXEC
	operType = IOCGIFAST_OPERATION_READ;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

#ifndef IOCGIFAST_WO_XEXEC
	try
	{
		_read(a_void);
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_read(a_void);
#endif

#ifndef IOCGIFAST_WO_XEXEC
	buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, buffer.c_str(), buffer.size() > inSize ? inSize : buffer.size());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioCGIFast::readString(dodoString &a_str)
{
	guard pg(this);

#ifndef IOCGIFAST_WO_XEXEC
	operType = IOCGIFAST_OPERATION_READSTRING;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

	char *data = new char[inSize];

	try
	{
		_read(data);
	}
	catch (...)
	{
		delete [] data;

#ifndef IOCGIFAST_WO_XEXEC
		buffer.clear();
#endif

		throw;
	}

#ifndef IOCGIFAST_WO_XEXEC
	buffer.assign(data, inSize);
	delete [] data;

	performXExec(postExec);

	a_str = buffer;
	buffer.clear();
#else
	a_str.assign(data, inSize);
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
ioCGIFast::writeString(const dodoString &a_buf)
{
	guard pg(this);

#ifndef IOCGIFAST_WO_XEXEC
	buffer = a_buf;

	operType = IOCGIFAST_OPERATION_WRITESTRING;
	performXExec(preExec);

	try
	{
		_write(buffer.c_str());
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_write(a_buf.c_str());
#endif


#ifndef IOCGIFAST_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioCGIFast::write(const char *const a_buf)
{
	guard pg(this);

#ifndef IOCGIFAST_WO_XEXEC
	buffer.assign(a_buf, outSize);

	operType = IOCGIFAST_OPERATION_WRITE;
	performXExec(preExec);

	try
	{
		_write(buffer.c_str());
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_write(a_buf);
#endif


#ifndef IOCGIFAST_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioCGIFast::_write(const char *const buf)
{
	if (FCGX_PutStr(buf, outSize, request->out) == -1)
		throw baseEx(ERRMODULE_CGIFAST, IOCGIFASTEX__WRITE, ERR_LIBDODO, CGIFASTIOEX_FAILEDTOPRINTSTRING, CGIFASTIOEX_FAILEDTOPRINTSTRING_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
ioCGIFast::readStream(char * const a_void)
{
	guard pg(this);

#ifndef IOCGIFAST_WO_XEXEC
	operType = IOCGIFAST_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	_read(a_void);

#ifndef IOCGIFAST_WO_XEXEC
	buffer = a_void;

	performXExec(postExec);

	if (buffer.size() > inSize)
		buffer.resize(inSize);
	strcpy(a_void, buffer.c_str());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioCGIFast::readStreamString(dodoString &a_str)
{
	guard pg(this);

#ifndef IOCGIFAST_WO_XEXEC
	operType = IOCGIFAST_OPERATION_READSTREAMSTRING;
	performXExec(preExec);
#endif

	char *data = new char[inSize];

	try
	{
		_read(data);
	}
	catch (...)
	{
		delete [] data;

		throw;
	}

#ifndef IOCGIFAST_WO_XEXEC
	buffer = data;
	delete [] data;

	performXExec(postExec);

	a_str = buffer;
	buffer.clear();
#else
	a_str = data;
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
ioCGIFast::writeStreamString(const dodoString &a_buf)
{
	guard pg(this);

	unsigned long _outSize = outSize;

#ifndef IOCGIFAST_WO_XEXEC
	buffer = a_buf;

	operType = IOCGIFAST_OPERATION_WRITESTREAMSTRING;
	performXExec(preExec);

	try
	{
		outSize = buffer.size();

		_write(buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		buffer.clear();

		throw;
	}
#else
	try
	{
		outSize = a_buf.size();

		_write(a_buf.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
#endif

#ifndef IOCGIFAST_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioCGIFast::writeStream(const char *const a_buf)
{
	guard pg(this);

	unsigned long _outSize = outSize;

#ifndef IOCGIFAST_WO_XEXEC
	buffer = a_buf;

	operType = IOCGIFAST_OPERATION_WRITESTREAM;
	performXExec(preExec);

	try
	{
		outSize = buffer.size();

		_write(buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		buffer.clear();

		throw;
	}
#else
	try
	{
		outSize = strlen(a_buf);

		_write(a_buf);

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
#endif

#ifndef IOCGIFAST_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
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

	ioCGIFast cfSTD(&request);

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

		ioCGIFast cfSTD(&request);

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
