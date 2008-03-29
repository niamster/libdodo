/***************************************************************************
 *            ioNetworkHTTP.cc
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

#include <libdodo/ioNetworkHTTP.h>

using namespace dodo;

const dodoString ioNetworkHTTP::requestHeaderStatements[] = { "Accept",
		"Accept-Charset",
		"Accept-Encoding",
		"Accept-Language",
		"Accept-Ranges",
		"Authorization",
		"Connection",
		"Date",
		"If-Modified-Since",
		"User-Agent",
};

//-------------------------------------------------------------------

const dodoString ioNetworkHTTP::responseHeaderStatements[] = { "Accept-Ranges",
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

const char ioNetworkHTTP::trimSymbols[] = {' ',
		'\r'
};

//-------------------------------------------------------------------

ioNetworkHTTP::ioNetworkHTTP() : httpStatusRE("^HTTP/[0-9].[0-9]\\s([0-9]+)\\s.*$")
{	
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_ACCEPT] = "*/*";
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_CONNECTION] = "Keep-Alive";
}

//-------------------------------------------------------------------

ioNetworkHTTP::ioNetworkHTTP(ioNetworkHTTP &fd)
{
}

//-------------------------------------------------------------------

ioNetworkHTTP::~ioNetworkHTTP()
{
}

//-------------------------------------------------------------------

bool
ioNetworkHTTP::extractHeaders(const dodoString &data,
						__httpResponse &response)
{
	unsigned long i(0), j(0);
	unsigned long size = data.size();
	dodoStringArray arr;
	dodoString piece;
	
	short o;
	
	bool statusCode = false;
	
	while (i < size)
	{
		i = data.find("\n", i);
		if (i == dodoString::npos)
		{
			response.data.append(data.substr(j));
			
			return true;
		}
		
		piece = stringTools::trim(data.substr(j, i - j), '\r');
		if (piece.size() == 0)
		{
			response.data.append(data.substr(i + 1));
			
			return true;
		}

		arr = tools::explode(piece, ":", 2);
		if (arr.size() != 2)
		{
			if (!statusCode)
			{ 
				statusCode = true;

				if (httpStatusRE.match(piece, arr))
					response.code = stringTools::stringToS(stringTools::trim(arr[0], trimSymbols, 2));
			}
			else
			{
				response.data.append(data.substr(j));
				
				return true;
			}
		}
		else
		{
			for (o = 0;o<IONETWORKHTTP_RESPONSEHEADERSTATEMENTS_SIZE;++o)
				if (stringTools::equal(responseHeaderStatements[o], arr[0]))
					response.headers[o] = stringTools::trim(arr[1], trimSymbols, 2);
		}
		
		i += 1;
		j = i;
	}
	
	response.data.append(data.substr(j));
	
	return false;
}

//-------------------------------------------------------------------

__httpResponse 
ioNetworkHTTP::getResponse()
{
	return response;
}

//-------------------------------------------------------------------

void 
ioNetworkHTTP::setUrl(const __url &url)
{
	this->url = url;
}

//-------------------------------------------------------------------

void 
ioNetworkHTTP::setUrl(const dodoString &a_url)
{
	this->url = tools::parseURL(a_url);
}

//-------------------------------------------------------------------

void
ioNetworkHTTP::GET()
{	
	response = __httpResponse();
	
	ioNetworkExchange ex;
	ioNetwork net(false, IONETWORKOPTIONS_PROTO_FAMILY_IPV4, IONETWORKOPTIONS_TRANSFER_TYPE_STREAM);
	
	__hostInfo host = ioNetworkTools::getHostInfo(url.host);
	
	dodoString protocol = url.protocol; 
	if (protocol.size() == 0)
		protocol = "http";
	
	int port = stringTools::stringToI(url.port);
	if (port == 0)
	{
		if (stringTools::iequal(protocol, "http"))
			port = 80;
	}
	
	net.connect(host.addresses[0], port, ex);
	
	dodoString data;
	
	data.append("GET ");
	data.append(url.request.size()>0?url.request:"/");
	data.append(" HTTP/1.0\r\n");
	dodoMap<short, dodoString>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
	for (;i!=j;++i)
	{
		data.append(requestHeaderStatements[i->first]);
		data.append(": ");
		data.append(i->second);
		data.append("\r\n");
	}
	data.append("Host: ");
	data.append(url.host);
	data.append("\r\n\r\n");
	
	ex.writeStreamString(data);

	ex.setInBufferSize(8096);
	ex.inSize = 8096;
	
	unsigned long contentSize = 0;
	
	bool endOfHeaders = false;
	try
	{
		while (true)
		{
			ex.readStreamString(data);
			
			if (data.size() == 0)
				break;
			
			if (endOfHeaders)
				response.data.append(data);
			else
			{
				endOfHeaders = extractHeaders(data, response);
				
				if (endOfHeaders)
					contentSize = stringTools::stringToUL(response.headers[IONETWORKHTTP_RESPONSEHEADER_CONTENTLENGTH]);
			}
			
			if (response.data.size() == contentSize)
				break;
		}
	}
	catch (baseEx &ex)
	{
		if (ex.funcID != IONETWORKEXCHANGEEX__READSTREAM)
			throw;
	}
}

//-------------------------------------------------------------------

__httpResponse
ioNetworkHTTP::GET(const __url &url)
{	
	setUrl(url);
	
	GET();
	
	return response;
}

//-------------------------------------------------------------------

__httpResponse
ioNetworkHTTP::GET(const dodoString &a_url)
{
	setUrl(tools::parseURL(a_url));
	
	GET();
	
	return response;
}

//-------------------------------------------------------------------

__httpResponse
ioNetworkHTTP::POST(const __url &url, 
					const dodoString &data)
{	
	setUrl(url);
	
	POST(data);
	
	return response;
}

//-------------------------------------------------------------------

__httpResponse
ioNetworkHTTP::POST(const dodoString &a_url, 
					const dodoString &data)
{
	setUrl(tools::parseURL(a_url));
	
	POST(data);
	
	return response;
}

//-------------------------------------------------------------------

void
ioNetworkHTTP::POST(const dodoString &data)
{	
	response = __httpResponse();
}

//-------------------------------------------------------------------
