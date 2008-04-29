/***************************************************************************
 *            cgiFastClientExchange.cc
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

#include <libdodo/cgiFastClientExchange.h>

#ifdef FASTCGI_EXT

using namespace dodo::cgi::fast;

clientExchange::clientExchange(clientExchange &cf)
{
}

//-------------------------------------------------------------------

clientExchange::clientExchange(FCGX_Request *a_request) : request(a_request)
{
}

//-------------------------------------------------------------------

clientExchange::~clientExchange()
{
}

//-------------------------------------------------------------------

void
clientExchange::flush()
{
	if (FCGX_FFlush(request->out) == -1)
		throw baseEx(ERRMODULE_CGIFASTCLIENTEXCHANGE, FASTCLIENTEXCHANGEEX_FLUSH, ERR_LIBDODO, FASTCLIENTEXCHANGEEX_FAILEDTOFLUSH, FASTCLIENTEXCHANGEEX_FAILEDTOFLUSH_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

char *
clientExchange::getenv(const char *buf)
{
	return FCGX_GetParam(buf, request->envp);
}

//-------------------------------------------------------------------

int
clientExchange::getInDescriptor() const
{
	return -1;
}

//-------------------------------------------------------------------

int
clientExchange::getOutDescriptor() const
{
	return -1;
}

//-------------------------------------------------------------------

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC

int
clientExchange::addPostExec(inExec func,
				   void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_CGIFASTCLIENTEXCHANGE, data);
}

//-------------------------------------------------------------------

int
clientExchange::addPreExec(inExec func,
				  void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_CGIFASTCLIENTEXCHANGE, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
clientExchange::addPostExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_CGIFASTCLIENTEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

dodo::__xexecCounts
clientExchange::addExec(const dodoString &module,
			   void             *data,
			   void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_CGIFASTCLIENTEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

int
clientExchange::addPreExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_CGIFASTCLIENTEXCHANGE, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
clientExchange::_read(char * const a_void)
{
	memset(a_void, '\0', inSize);

	FCGX_GetStr(a_void, inSize, request->in);
}

//-------------------------------------------------------------------

void
clientExchange::read(char * const a_void)
{
	systemRaceHazardGuard pg(this);

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	operType = FASTCLIENTEXCHANGE_OPERATION_READ;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
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

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, buffer.c_str(), buffer.size() > inSize ? inSize : buffer.size());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
clientExchange::readString(dodoString &a_str)
{
	systemRaceHazardGuard pg(this);

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	operType = FASTCLIENTEXCHANGE_OPERATION_READSTRING;
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

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
		buffer.clear();
#endif

		throw;
	}

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
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
clientExchange::writeString(const dodoString &a_buf)
{
	systemRaceHazardGuard pg(this);

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	buffer = a_buf;

	operType = FASTCLIENTEXCHANGE_OPERATION_WRITESTRING;
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


#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
clientExchange::write(const char *const a_buf)
{
	systemRaceHazardGuard pg(this);

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	buffer.assign(a_buf, outSize);

	operType = FASTCLIENTEXCHANGE_OPERATION_WRITE;
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


#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
clientExchange::_write(const char *const buf)
{
	if (FCGX_PutStr(buf, outSize, request->out) == -1)
		throw baseEx(ERRMODULE_CGIFASTCLIENTEXCHANGE, FASTCLIENTEXCHANGEEX__WRITE, ERR_LIBDODO, FASTCLIENTEXCHANGEEX_FAILEDTOPRINTSTRING, FASTCLIENTEXCHANGEEX_FAILEDTOPRINTSTRING_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
clientExchange::readStream(char * const a_void)
{
	systemRaceHazardGuard pg(this);

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	operType = FASTCLIENTEXCHANGE_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	_read(a_void);

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
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
clientExchange::readStreamString(dodoString &a_str)
{
	systemRaceHazardGuard pg(this);

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	operType = FASTCLIENTEXCHANGE_OPERATION_READSTREAMSTRING;
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

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
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
clientExchange::writeStreamString(const dodoString &a_buf)
{
	systemRaceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	buffer = a_buf;

	operType = FASTCLIENTEXCHANGE_OPERATION_WRITESTREAMSTRING;
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

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
clientExchange::writeStream(const char *const a_buf)
{
	systemRaceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	buffer = a_buf;

	operType = FASTCLIENTEXCHANGE_OPERATION_WRITESTREAM;
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

#ifndef CGIFASTCLIENTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

#endif

//-------------------------------------------------------------------
