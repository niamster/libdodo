/***************************************************************************
 *            cgiBasicExchange.cc
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

#include <stdlib.h>

#include <libdodo/cgiBasicExchange.h>
#include <libdodo/cgiExchange.h>
#include <libdodo/ioStdio.h>

using namespace dodo::cgi::basic;

exchange::exchange(exchange &e) : io::stream::channel(e.protection),
                                  dodo::cgi::exchange(e.protection),
                                  io::pipe(e.protection),
                                  io::stdio(e.protection)
{
}

//-------------------------------------------------------------------

exchange::exchange(short protection) :  io::stream::channel(protection),
                                        dodo::cgi::exchange(protection)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_CGIBASICEXCHANGE);
#endif
}

//-------------------------------------------------------------------

exchange::~exchange()
{
}

//-------------------------------------------------------------------

char *
exchange::getenv(const char *buf)
{
    return ::getenv(buf);
}

//-------------------------------------------------------------------

