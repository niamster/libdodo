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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#ifdef FASTCGI_EXT
#include <fcgiapp.h>
#include <string.h>

#include "cgiFastRequest.inline"

#include <libdodo/cgiFastExchange.h>
#include <libdodo/cgiFastExchangeEx.h>
#include <libdodo/types.h>
#include <libdodo/cgiExchange.h>
#include <libdodo/ioChannel.h>

using namespace dodo::cgi::fast;

exchange::exchange(exchange &cf) : dodo::cgi::exchange(cf.protection),
								   io::stream::channel(cf.protection),
								   request(new __request__)
{
}

//-------------------------------------------------------------------

exchange::exchange(const __request__ &req,
				   short		   protection) : dodo::cgi::exchange(protection),
												 io::stream::channel(protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_CGIFASTEXCHANGE);
#endif

	request->request = req.request;
}

//-------------------------------------------------------------------

exchange::~exchange()
{
	delete request;
}

//-------------------------------------------------------------------

void
exchange::flush() const
{
	if (FCGX_FFlush(request->request->out) == -1)
	{
		throw exception::basic(exception::ERRMODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX_FLUSH, exception::ERRNO_LIBDODO, FASTEXCHANGEEX_FAILEDTOFLUSH, CGIFASTEXCHANGEEX_FAILEDTOFLUSH_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

char *
exchange::getenv(const char *buf)
{
	return FCGX_GetParam(buf, request->request->envp);
}

//-------------------------------------------------------------------

int
exchange::getInDescriptor() const
{
	throw exception::basic(exception::ERRMODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX_GETINDESCRIPTOR, exception::ERRNO_LIBDODO, FASTEXCHANGEEX_CANTBEUSEDWITHIOEVENT, CGIFASTEXCHANGEEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
exchange::getOutDescriptor() const
{
	throw exception::basic(exception::ERRMODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX_GETOUTDESCRIPTOR, exception::ERRNO_LIBDODO, FASTEXCHANGEEX_CANTBEUSEDWITHIOEVENT, CGIFASTEXCHANGEEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
exchange::_read(char * const a_data) const
{
	memset(a_data, '\0', inSize);

	FCGX_GetStr(a_data, inSize, request->request->in);
}

//-------------------------------------------------------------------

void
exchange::_write(const char *const buf) const
{
	if (FCGX_PutStr(buf, outSize, request->request->out) == -1)
	{
		throw exception::basic(exception::ERRMODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX__WRITE, exception::ERRNO_LIBDODO, FASTEXCHANGEEX_FAILEDTOPRINTSTRING, CGIFASTEXCHANGEEX_FAILEDTOPRINTSTRING_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
exchange::_writeStream(const char * const data) const
{
	unsigned long _outSize = outSize;

	try
	{
		unsigned int bufSize = strlen(data);

		if (bufSize < outSize)
		{
			outSize = bufSize;
		}

		_write(data);

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
}

//-------------------------------------------------------------------

unsigned long
exchange::_readStream(char * const data) const
{
	unsigned long _inSize = inSize++;

	_read(data);

	inSize = _inSize;

	return strlen(data);
}
#endif

//-------------------------------------------------------------------
