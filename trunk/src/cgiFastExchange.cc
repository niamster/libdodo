/***************************************************************************
 *            cgiFastExchange.cc
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

#include <libdodo/cgiFastExchange.h>

#ifdef FASTCGI_EXT

using namespace dodo::cgi::fast;

#ifndef CGIFASTEXCHANGE_WO_XEXEC

__xexexCgiFastExchangeCollectedData::__xexexCgiFastExchangeCollectedData(int &a_operType,
																		 void *a_executor) : operType(a_operType),
																							 executor(a_executor)
{
}

#endif

//-------------------------------------------------------------------

exchange::exchange(exchange &cf)

#ifndef CGIFASTEXCHANGE_WO_XEXEC

	: collectedData(operType,
					(void *) this)

#endif

{
}

//-------------------------------------------------------------------

exchange::exchange(FCGX_Request *a_request) : request(a_request)

#ifndef CGIFASTEXCHANGE_WO_XEXEC

											  ,
											  collectedData(operType,
															(void *) this)

#endif

{
}

//-------------------------------------------------------------------

exchange::~exchange()
{
}

//-------------------------------------------------------------------

void
exchange::flush()
{
	if (FCGX_FFlush(request->out) == -1)
		throw baseEx(ERRMODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX_FLUSH, ERR_LIBDODO, FASTEXCHANGEEX_FAILEDTOFLUSH, CGIFASTEXCHANGEEX_FAILEDTOFLUSH_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

char *
exchange::getenv(const char *buf)
{
	return FCGX_GetParam(buf, request->envp);
}

//-------------------------------------------------------------------

int
exchange::getInDescriptor() const
{
	return -1;
}

//-------------------------------------------------------------------

int
exchange::getOutDescriptor() const
{
	return -1;
}

//-------------------------------------------------------------------

#ifndef CGIFASTEXCHANGE_WO_XEXEC

int
exchange::addPostExec(inExec func,
					  void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_CGIFASTEXCHANGE, data);
}

//-------------------------------------------------------------------

int
exchange::addPreExec(inExec func,
					 void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_CGIFASTEXCHANGE, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
exchange::addPostExec(const dodoString &module,
					  void             *data,
					  void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_CGIFASTEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

dodo::__xexecCounts
exchange::addExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_CGIFASTEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

int
exchange::addPreExec(const dodoString &module,
					 void             *data,
					 void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_CGIFASTEXCHANGE, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
exchange::_read(char * const a_void)
{
	memset(a_void, '\0', inSize);

	FCGX_GetStr(a_void, inSize, request->in);
}

//-------------------------------------------------------------------

void
exchange::read(char * const a_void)
{
	raceHazardGuard pg(this);

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READ;
	performXExec(preExec);

	collectedData.buffer.reserve(inSize);
#endif

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	try
	{
		_read(a_void);
	}
	catch (...)
	{
		collectedData.buffer.clear();

		throw;
	}
#else
	_read(a_void);
#endif

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	collectedData.buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, collectedData.buffer.c_str(), collectedData.buffer.size() > inSize ? inSize : collectedData.buffer.size());
	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::readString(dodoString &a_str)
{
	raceHazardGuard pg(this);

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READSTRING;
	performXExec(preExec);

	collectedData.buffer.reserve(inSize);
#endif

	char *data = new char[inSize];

	try
	{
		_read(data);
	}
	catch (...)
	{
		delete [] data;

#ifndef CGIFASTEXCHANGE_WO_XEXEC
		collectedData.buffer.clear();
#endif

		throw;
	}

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	collectedData.buffer.assign(data, inSize);
	delete [] data;

	performXExec(postExec);

	a_str = collectedData.buffer;
	collectedData.buffer.clear();
#else
	a_str.assign(data, inSize);
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
exchange::writeString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	collectedData.buffer = a_buf;

	operType = EXCHANGE_OPERATION_WRITESTRING;
	performXExec(preExec);

	try
	{
		_write(collectedData.buffer.c_str());
	}
	catch (...)
	{
		collectedData.buffer.clear();

		throw;
	}
#else
	_write(a_buf.c_str());
#endif


#ifndef CGIFASTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::write(const char *const a_buf)
{
	raceHazardGuard pg(this);

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	collectedData.buffer.assign(a_buf, outSize);

	operType = EXCHANGE_OPERATION_WRITE;
	performXExec(preExec);

	try
	{
		_write(collectedData.buffer.c_str());
	}
	catch (...)
	{
		collectedData.buffer.clear();

		throw;
	}
#else
	_write(a_buf);
#endif


#ifndef CGIFASTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::_write(const char *const buf)
{
	if (FCGX_PutStr(buf, outSize, request->out) == -1)
		throw baseEx(ERRMODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX__WRITE, ERR_LIBDODO, FASTEXCHANGEEX_FAILEDTOPRINTSTRING, CGIFASTEXCHANGEEX_FAILEDTOPRINTSTRING_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
exchange::readStream(char * const a_void)
{
	raceHazardGuard pg(this);

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	_read(a_void);

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	collectedData.buffer = a_void;

	performXExec(postExec);

	if (collectedData.buffer.size() > inSize)
		collectedData.buffer.resize(inSize);
	strcpy(a_void, collectedData.buffer.c_str());
	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::readStreamString(dodoString &a_str)
{
	raceHazardGuard pg(this);

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READSTREAMSTRING;
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

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	collectedData.buffer = data;
	delete [] data;

	performXExec(postExec);

	a_str = collectedData.buffer;
	collectedData.buffer.clear();
#else
	a_str = data;
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
exchange::writeStreamString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	collectedData.buffer = a_buf;

	operType = EXCHANGE_OPERATION_WRITESTREAMSTRING;
	performXExec(preExec);

	try
	{
		outSize = collectedData.buffer.size();

		_write(collectedData.buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		collectedData.buffer.clear();

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

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::writeStream(const char *const a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	collectedData.buffer = a_buf;

	operType = EXCHANGE_OPERATION_WRITESTREAM;
	performXExec(preExec);

	try
	{
		outSize = collectedData.buffer.size();

		_write(collectedData.buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		collectedData.buffer.clear();

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

#ifndef CGIFASTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

#endif

//-------------------------------------------------------------------
