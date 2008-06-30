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

#include <libdodo/cgiFastExchange.h>

#ifdef FASTCGI_EXT

using namespace dodo::cgi::fast;

exchange::exchange(exchange &cf)
{
}

//-------------------------------------------------------------------

exchange::exchange(FCGX_Request *a_request) : request(a_request)
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_CGIFASTEXCHANGE;
	execObjectData = (void *)&collectedData;

#endif
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

void
exchange::_read(char * const a_void)
{
	memset(a_void, '\0', inSize);

	FCGX_GetStr(a_void, inSize, request->in);
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
exchange::_writeStream(const char * const data)
{
	_write(data);
}

//-------------------------------------------------------------------

unsigned long
exchange::_readStream(char * const data)
{
	_read(data);

	return strlen(data);
}

#endif

//-------------------------------------------------------------------
