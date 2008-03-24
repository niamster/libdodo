/***************************************************************************
 *            ioHTTP.cc
 *
 *  Wed Oct 8 01:44:18 2005
 *  Copyright  2005  Ni@m
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

#include <libdodo/ioHTTP.h>

using namespace dodo;

const dodoString requestHeaderStatements[] = { "Accept",
		"Accept-Charset",
		"Accept-Encoding",
		"Accept-Language",
		"Accept-Ranges",
		"Authorization",
		"Connection",
		"Date",
		"Host",
		"If-Modified-Since",
		"User-Agent",
};

//-------------------------------------------------------------------

const dodoString responseHeaderStatements[] = { "Accept-Ranges",
		"Age",
		"Allow",
		"Cache-Control",
		"Content-Encoding",
		"Content-Language",
		"Content-Length",
		"Content-Location",
		"Content-Disposition",
		"Content-MD5",
		"Content-Range",
		"Content-Type",
		"Date",
		"Last-Modified",
		"Location",
		"Server",
};

//-------------------------------------------------------------------

ioHTTP::ioHTTP(const dodoString &url)
{
	this->url = tools::parseURL(url);

	requestHeaders[IOHTTP_REQUESTHEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
	requestHeaders[IOHTTP_REQUESTHEADER_ACCEPT] = "*/*";
	requestHeaders[IOHTTP_REQUESTHEADER_CONNECTION] = "Keep-Alive";
}

//-------------------------------------------------------------------

ioHTTP::ioHTTP(const __url &url)
{
	this->url = url;
	
	requestHeaders[IOHTTP_REQUESTHEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
	requestHeaders[IOHTTP_REQUESTHEADER_ACCEPT] = "*/*";
	requestHeaders[IOHTTP_REQUESTHEADER_CONNECTION] = "Keep-Alive";
}

//-------------------------------------------------------------------

ioHTTP::ioHTTP(ioHTTP &fd)
{
}

//-------------------------------------------------------------------

ioHTTP::~ioHTTP()
{
}

//-------------------------------------------------------------------
