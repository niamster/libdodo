/***************************************************************************
 *            cgiFastExchange.cc
 *
 *  Sat Aug  5 2006
 *  Copyright  2006  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

exchange::exchange(exchange &cf) : io::stream::channel(cf.protection),
                                   dodo::cgi::exchange(cf.protection)
{
}

//-------------------------------------------------------------------

exchange::exchange(const __request__ &req,
                   short             protection) : io::stream::channel(protection),
                                                   dodo::cgi::exchange(protection),
                                                   request(new __request__)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_CGIFASTEXCHANGE);
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
        dodo_throw exception::basic(exception::MODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX_FLUSH, exception::ERRNO_LIBDODO, FASTEXCHANGEEX_FAILEDTOFLUSH, CGIFASTEXCHANGEEX_FAILEDTOFLUSH_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

char *
exchange::getenv(const char *buf)
{
    return FCGX_GetParam(buf, request->request->envp);
}

//-------------------------------------------------------------------

int
exchange::inDescriptor() const
{
    dodo_throw exception::basic(exception::MODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX_INDESCRIPTOR, exception::ERRNO_LIBDODO, FASTEXCHANGEEX_CANTBEUSEDWITHIOEVENT, CGIFASTEXCHANGEEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
exchange::outDescriptor() const
{
    dodo_throw exception::basic(exception::MODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX_OUTDESCRIPTOR, exception::ERRNO_LIBDODO, FASTEXCHANGEEX_CANTBEUSEDWITHIOEVENT, CGIFASTEXCHANGEEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
exchange::_read(char * const data) const
{
    return FCGX_GetStr(data, bs, request->request->in);
}

//-------------------------------------------------------------------

unsigned long
exchange::_write(const char *const data) const
{
    long n;

    if ((n = FCGX_PutStr(data, bs, request->request->out)) == -1)
        dodo_throw exception::basic(exception::MODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX__WRITE, exception::ERRNO_LIBDODO, FASTEXCHANGEEX_FAILEDTOPRINTSTRING, CGIFASTEXCHANGEEX_FAILEDTOPRINTSTRING_STR, __LINE__, __FILE__);

    return n;
}

//-------------------------------------------------------------------

unsigned long
exchange::_writeString(const char * const data) const
{
    long n;

    if ((n = FCGX_PutStr(data, strnlen(data, bs), request->request->out)) == -1)
        dodo_throw exception::basic(exception::MODULE_CGIFASTEXCHANGE, FASTEXCHANGEEX__WRITESTRING, exception::ERRNO_LIBDODO, FASTEXCHANGEEX_FAILEDTOPRINTSTRING, CGIFASTEXCHANGEEX_FAILEDTOPRINTSTRING_STR, __LINE__, __FILE__);

    return n;
}

//-------------------------------------------------------------------

unsigned long
exchange::_readString(char * const data) const
{
    return FCGX_GetStr(data, bs+1, request->request->in);
}
#endif

//-------------------------------------------------------------------
