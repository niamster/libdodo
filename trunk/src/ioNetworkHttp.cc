/***************************************************************************
 *            ioNetworkHttp.cc
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

#include <libdodo/ioNetworkHttp.h>

using namespace dodo;

const dodoString ioNetworkHttp::requestHeaderStatements[] = { "Accept",
		"Accept-Charset",
		"Accept-Encoding",
		"Accept-Language",
		"Accept-Ranges",
		"Authorization",
		"Proxy-Authorization",
		"Connection",
		"Date",
		"If-Modified-Since",
		"User-Agent",
		"Cookie",
};

//-------------------------------------------------------------------

const dodoString ioNetworkHttp::responseHeaderStatements[] = { "Accept-Ranges",
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
		"WWW-Authenticate",
		"Proxy-Authenticate",
		"X-Powered-By",
};

//-------------------------------------------------------------------

__httpResponse::__httpResponse() : code(0),
								redirected(false)
{
}

//-------------------------------------------------------------------

ioNetworkHttp::ioNetworkHttp() : httpStatusRE("^HTTP/[0-9].[0-9]\\s([0-9]+)\\s.*$"),
								followRedirection(true),
								authTries(0)
{	
	proxyAuthInfo.enabled = false;
	proxyAuthInfo.authRequired = false;
	
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_ACCEPT] = "*/*";
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_CONNECTION] = "Close";
}

//-------------------------------------------------------------------

ioNetworkHttp::ioNetworkHttp(ioNetworkHttp &fd)
{
}

//-------------------------------------------------------------------

ioNetworkHttp::~ioNetworkHttp()
{
}

//-------------------------------------------------------------------

__httpResponse 
ioNetworkHttp::getResponse()
{
	return response;
}

//-------------------------------------------------------------------

void 
ioNetworkHttp::setUrl(const dodoString &a_url)
{
	urlComponents = tools::parseUrl(a_url);
	 
	if (urlComponents.protocol.size() == 0)
		urlComponents.protocol = "http";
	
	unsigned long portSize = urlComponents.port.size();
	
	if (portSize == 0 && stringTools::iequal(urlComponents.protocol, "http"))
		urlComponents.port = "80";
	
	url.clear();
	
	url.append(urlComponents.protocol);
	url.append("://" );
	url.append(urlComponents.host);
	if (portSize > 0)
	{
		url.append(":");
		url.append(urlComponents.port);
	}
	url.append("/");
	url.append(urlComponents.path);
	if (urlComponents.request.size() > 0)
	{
		url.append("?");
		url.append(tools::encodeUrl(urlComponents.request));
	}
}

//-------------------------------------------------------------------

void 
ioNetworkHttp::setCookies(const dodoStringMap &cookies)
{
	dodoString data;
	
	dodoStringMap::const_iterator i = cookies.begin(), j = cookies.end();
	--j;
	
	for (;i!=j;++i)
	{
		data.append(i->first);
		data.append("=");
		data.append(tools::encodeUrl(i->second));
		data.append("; ");
	}
	data.append(i->first);
	data.append("=");
	data.append(tools::encodeUrl(i->second));
	
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_COOKIE] = data;
}

//-------------------------------------------------------------------

void
ioNetworkHttp::GET()
{	
	response = __httpResponse();
	
	ioNetworkExchange ex;
	ioNetworkClient net(IONETWORKOPTIONS_PROTO_FAMILY_IPV4, IONETWORKOPTIONS_TRANSFER_TYPE_STREAM);
	
	__hostInfo host = toolsNetwork::getHostInfo(urlComponents.host);
	
	if (proxyAuthInfo.enabled)
		net.connect(proxyAuthInfo.host, proxyAuthInfo.port, ex);
	else
	{
		__hostInfo host = toolsNetwork::getHostInfo(urlComponents.host);
		
		dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
		for (;o!=p;++o)
			try
			{
				net.connect(*o, stringTools::stringToI(urlComponents.port), ex);
				
				break;
			}
			catch (baseEx &ex)
			{
				if (ex.funcID == IONETWORKCLIENTEX_CONNECT)
				{
					if (*o == *p)
						throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_CANNOTCONNECT, IONETWORKHTTPEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
					else
						continue;
				}
				
				throw;
			}
	}
	
	dodoString data;
	
	data.append("GET ");
	data.append(url);
	if (proxyAuthInfo.enabled)
		data.append(" HTTP/1.1\r\n");
	else
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
	data.append(urlComponents.host);
	
	data.append("\r\n\r\n");
	
	ex.writeStreamString(data);
	
	switch (getContent(data, ex))
	{
		case GETCONTENTSTATUS_NORMAL:
			
			break;
			
		case GETCONTENTSTATUS_REDIRECT:
			
			setUrl(response.headers[IONETWORKHTTP_RESPONSEHEADER_LOCATION]);
			
			GET();
			
			break;
			
		case GETCONTENTSTATUS_PROXYBASICAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			makeBasicAuth(IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
			
			GET();
			
			break;
			
		case GETCONTENTSTATUS_WWWBASICAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}

			makeBasicAuth(IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);
			
			GET();
			
			break;
			
		case GETCONTENTSTATUS_PROXYDIGESTAUTH:

			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}

			makeDigestAuth(IONETWORKHTTP_RESPONSEHEADER_PROXYAUTHENTICATE, IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password);
			
			GET();
		
			break;
			
		case GETCONTENTSTATUS_WWWDIGESTAUTH:

			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			makeDigestAuth(IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE, IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password);
			
			GET();
			
			break;
			
		case GETCONTENTSTATUS_WWWPROXYBASICAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}

			makeBasicAuth(IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);
			makeBasicAuth(IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
			
			GET();
			
			break;
			
		case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}

			makeDigestAuth(IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE, IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password);
			makeDigestAuth(IONETWORKHTTP_RESPONSEHEADER_PROXYAUTHENTICATE, IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password);
			
			GET();
			
			break;
	}
}

//-------------------------------------------------------------------


__httpResponse
ioNetworkHttp::GET(const dodoString &a_url)
{
	setUrl(a_url);
	
	GET();
	
	return response;
}

//-------------------------------------------------------------------

__httpResponse 
ioNetworkHttp::POST(const dodoString &a_url, 
							const dodoStringMap &arguments, 
							const dodoStringMap &files)
{
	setUrl(a_url);
	
	POST(arguments, files);
	
	return response;
}

//-------------------------------------------------------------------

void
ioNetworkHttp::POST(const dodoStringMap &arguments, 
							const dodoStringMap &files)
{
	dodoString boundary = "---------------------------" + stringTools::ulToString(tools::ulRandom()) + stringTools::ulToString(tools::ulRandom());
	dodoString type = "multipart/form-data; boundary=" + boundary;
	boundary.insert(0, "--");
	
	dodoString data;
	
	dodoStringMap::const_iterator i = files.begin(), j = files.end();
	
	for (;i!=j;++i)
	{
		data.append(boundary);
		data.append("\r\nContent-Disposition: form-data; name=\"");
		data.append(i->first);
		data.append("\"; filename=\"");
		data.append(toolsFilesystem::basename(i->second));
		data.append("\"\r\n");
		
		data.append("Content-Type: application/octet-stream\r\n\r\n");
		
		data.append(toolsFilesystem::getFileContents(i->second));
		data.append("\r\n");
	}

	i = arguments.begin();
	j = arguments.end();
	for (;i!=j;++i)
	{
		data.append(boundary);
		data.append("\r\nContent-Disposition: form-data; name=\"");
		data.append(i->first);
		data.append("\"\r\n\r\n");
		
		data.append(i->second);
		data.append("\r\n");
	}
	data.append(boundary);
	data.append("--");
	
	POST(data, type);
}

//-------------------------------------------------------------------

__httpResponse 
ioNetworkHttp::POST(const dodoString &a_url, 
					const dodoStringMap &arguments)
{
	setUrl(a_url);
	
	POST(arguments);
	
	return response;
}

//-------------------------------------------------------------------

void 
ioNetworkHttp::POST(const dodoStringMap &arguments)
{
	dodoString data;
	
	dodoStringMap::const_iterator i(arguments.begin()), j(arguments.end());
	--j;
	
	for (;i!=j;++i)
	{
		data.append(tools::encodeUrl(i->first));
		data.append("=");
		data.append(tools::encodeUrl(i->second));
		data.append("&");
	}
	data.append(tools::encodeUrl(i->first));
	data.append("=");
	data.append(tools::encodeUrl(i->second));
	
	POST(data, "application/x-www-form-urlencoded");
}

//-------------------------------------------------------------------

__httpResponse
ioNetworkHttp::POST(const dodoString &a_url, 
					const dodoString &data,
					const dodoString &type)
{
	setUrl(a_url);
	
	POST(data, type);
	
	return response;
}

//-------------------------------------------------------------------

void
ioNetworkHttp::POST(const dodoString &a_data,
					const dodoString &type)
{	
	response = __httpResponse();
	
	ioNetworkExchange ex;
	ioNetworkClient net(IONETWORKOPTIONS_PROTO_FAMILY_IPV4, IONETWORKOPTIONS_TRANSFER_TYPE_STREAM);
	
	if (proxyAuthInfo.enabled)
		net.connect(proxyAuthInfo.host, proxyAuthInfo.port, ex);
	else
	{
		__hostInfo host = toolsNetwork::getHostInfo(urlComponents.host);
		
		dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
		for (;o!=p;++o)
			try
			{		
				net.connect(*o, stringTools::stringToI(urlComponents.port), ex);
				
				break;
			}
			catch (baseEx &ex)
			{
				if (ex.funcID == IONETWORKCLIENTEX_CONNECT)
				{
					if (*o == *p)
						throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_CANNOTCONNECT, IONETWORKHTTPEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
					else
						continue;
				}
				
				throw;
			}
	}
	
	dodoString data;
	
	data.append("POST ");
	data.append(url);
	if (proxyAuthInfo.enabled)
		data.append(" HTTP/1.1\r\n");
	else
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
	data.append(urlComponents.host);
	data.append("\r\n");
	
	data.append("Content-length: ");
	data.append(stringTools::ulToString(a_data.size()));
	data.append("\r\n");
	
	data.append("Content-type: ");
	data.append(type);
	data.append("\r\n\r\n");
	
	ex.writeStreamString(data);
	
	ex.outSize = a_data.size();
	ex.writeString(a_data);
	
	switch (getContent(data, ex))
	{
		case GETCONTENTSTATUS_NORMAL:
			
			break;
			
		case GETCONTENTSTATUS_REDIRECT:
			
			setUrl(response.headers[IONETWORKHTTP_RESPONSEHEADER_LOCATION]);
			
			POST(data, type);
			
			break;
			
		case GETCONTENTSTATUS_PROXYBASICAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			makeBasicAuth(IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
			
			POST(data, type);
			
			break;
			
		case GETCONTENTSTATUS_WWWBASICAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			makeBasicAuth(IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);
			
			POST(data, type);
			
			break;
			
		case GETCONTENTSTATUS_PROXYDIGESTAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			makeDigestAuth(IONETWORKHTTP_RESPONSEHEADER_PROXYAUTHENTICATE, IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password);
			
			POST(data, type);

			break;
			
		
		case GETCONTENTSTATUS_WWWDIGESTAUTH:

			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			makeDigestAuth(IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE, IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password);
			
			POST(data, type);

			break;
			
		case GETCONTENTSTATUS_WWWPROXYBASICAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}

			makeBasicAuth(IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);
			makeBasicAuth(IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
			
			POST(data, type);
			
			break;
			
		case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}

			makeDigestAuth(IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE, IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password);
			makeDigestAuth(IONETWORKHTTP_RESPONSEHEADER_PROXYAUTHENTICATE, IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password);
			
			POST(data, type);
			
			break;
	}
}

//-------------------------------------------------------------------

void 
ioNetworkHttp::disableProxy()
{
	proxyAuthInfo.enabled = false;
}

//-------------------------------------------------------------------

void 
ioNetworkHttp::setProxyInformation(const dodoString &host, 
									unsigned int port, 
									const dodoString &user, 
									const dodoString &password)
{
	proxyAuthInfo.host = host;
	proxyAuthInfo.port = port;
	proxyAuthInfo.user = user;
	proxyAuthInfo.password = password;
	proxyAuthInfo.enabled = true;
}

//-------------------------------------------------------------------

void 
ioNetworkHttp::getHeaders(const dodoString &headers)
{
	unsigned long i(0), j(0);
	unsigned long size = headers.size();
	
	dodoStringArray arr;
	dodoString piece;
	
	short o;
	
	bool statusCode = false;
	
	while (i < size)
	{
		i = headers.find("\n", i);
		if (i == dodoString::npos)
			return;
		
		piece = stringTools::trim(headers.substr(j, i - j), '\r');

		arr = tools::explode(piece, ":", 2);
		if (arr.size() != 2)
		{
			if (!statusCode)
			{ 
				statusCode = true;

				if (httpStatusRE.match(piece, arr))
					response.code = stringTools::stringToS(stringTools::lTrim(arr[0], ' '));
			}
		}
		else
		{
			for (o = 0;o<IONETWORKHTTP_RESPONSEHEADERSTATEMENTS;++o)
				if (stringTools::equal(responseHeaderStatements[o], arr[0]))
					response.headers[o] = stringTools::lTrim(arr[1], ' ');
			
			if (stringTools::equal("Set-Cookie", arr[0]))
				response.cookies.push_back(parseCookie(stringTools::lTrim(arr[1], ' ')));
		}
		
		++i;
		j = i;
	}
}

//-------------------------------------------------------------------

bool
ioNetworkHttp::extractHeaders(const dodoString &data,
						dodoString &headers)
{	
	headers.append(data);
	
	unsigned long i = headers.find("\r\n\r\n");
	if (i == dodoString::npos)
	{
		i = headers.find("\n\n");
		if (i == dodoString::npos)
			return false;
		else
		{
			response.data.append(headers.substr(i + 2));

			headers.resize(i + 1);
			
			return true;
		}	
	}
	else
	{
		response.data.append(headers.substr(i + 4));
		
		headers.resize(i + 2);
		
		return true;
	}
	
	return false;
}

//-------------------------------------------------------------------

short 
ioNetworkHttp::getContent(dodoString &data, 
						ioNetworkExchange &ex)
{
	ex.setInBufferSize(512);
	ex.inSize = 512;
	
	unsigned long contentSize = 0;
	bool endOfHeaders = false;
	
	dodoString headers;
	
	while (true)
	{
		try
		{
			ex.readStreamString(data);
			
			if (data.size() == 0 && contentSize <= 0)
			{
				if (!endOfHeaders && headers.size() > 0)
					response.data.assign(headers);
				
				break;
			}
			
			if (endOfHeaders)
				response.data.append(data);
			else
			{
				endOfHeaders = extractHeaders(data, headers);
				
				if (endOfHeaders)
				{
					getHeaders(headers);
					headers.clear();
					
					contentSize = stringTools::stringToUL(response.headers[IONETWORKHTTP_RESPONSEHEADER_CONTENTLENGTH]);

					if (followRedirection && (response.code / 100) == 3 && response.code != 304)
					{
						response.redirected = true;
					
						return GETCONTENTSTATUS_REDIRECT;
					}
					
					if (response.code == 401)
					{
						++authTries;
						
						if (proxyAuthInfo.authRequired)
						{
							if (stringTools::contains(response.headers[IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Basic"))
								return GETCONTENTSTATUS_WWWPROXYBASICAUTH;
							else 
							{
								if (stringTools::contains(response.headers[IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Digest"))
									return GETCONTENTSTATUS_WWWPROXYDIGESTAUTH;
								else
									throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GETCONTENT, ERR_LIBDODO, IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE_STR, __LINE__, __FILE__);
							}
						}
						else
						{
							if (stringTools::contains(response.headers[IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Basic"))
								return GETCONTENTSTATUS_WWWBASICAUTH;
							else 
							{
								if (stringTools::contains(response.headers[IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Digest"))
									return GETCONTENTSTATUS_WWWDIGESTAUTH;
								else
									throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GETCONTENT, ERR_LIBDODO, IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE_STR, __LINE__, __FILE__);
							}
						}
					}
					
					if (response.code == 407)
					{
						++authTries;
						
						proxyAuthInfo.authRequired = true;
						
						if (stringTools::contains(response.headers[IONETWORKHTTP_RESPONSEHEADER_PROXYAUTHENTICATE], "Basic"))
							return GETCONTENTSTATUS_PROXYBASICAUTH;
						else 
						{
							if (stringTools::contains(response.headers[IONETWORKHTTP_RESPONSEHEADER_PROXYAUTHENTICATE], "Digest"))
								return GETCONTENTSTATUS_PROXYDIGESTAUTH;
							else
								throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GETCONTENT, ERR_LIBDODO, IONETWORKHTTPEX_UNKNOWNPROXYAUTHTYPE, IONETWORKHTTPEX_UNKNOWNPROXYAUTHTYPE_STR, __LINE__, __FILE__);
						}
					}
						
					
					ex.setInBufferSize(16384);
					ex.inSize = 16384;
				}
			}
			
			if (contentSize > 0 && response.data.size() == contentSize)
				break;
		}
		catch (baseEx &ex)
		{
			if (ex.funcID == IONETWORKEXCHANGEEX__READSTREAM)
			{
				if (!endOfHeaders && headers.size() > 0)
					response.data.assign(headers);
				
				break;
			}
			else
				throw;
		}
	}
	
	authTries = 0;
	
	return GETCONTENTSTATUS_NORMAL;
}

//-------------------------------------------------------------------

dodoString 
ioNetworkHttp::trim(const dodoString &data)
{
	return stringTools::trim(data, ' ');
}

//-------------------------------------------------------------------

void 
ioNetworkHttp::makeDigestAuth(short requestHeader, 
							short responseHeader, 
							const dodoString &method, 
							const dodoString &user, 
							const dodoString &password)
{
	dodoString nonce, opaque, realm;
	
	dodoStringArray parts = tools::explode(response.headers[requestHeader].substr(7), &trim, ",");

	dodoString HA1;

	unsigned char HA[16];			
	tools::MD5_CTX context;
	
	dodoStringArray tuple;
	
	dodoStringArray::iterator i = parts.begin(), j = parts.end();
	for (;i!=j;++i)
	{
		tuple = tools::explode(*i, "=");
		if (tuple.size() != 2)
			continue;
		
		if (stringTools::iequal(tuple[0], "realm"))
		{
			realm = stringTools::trim(tuple[1], '"');
			
			tools::MD5Init(&context);
			tools::MD5Update(&context, (unsigned char *)user.c_str(), user.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)realm.c_str(), realm.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)password.c_str(), password.size());
			tools::MD5Final(HA, &context);

			HA1 = tools::binToHex(dodoString((char *)&HA, 16));
		}
		else
		{
			if (stringTools::iequal(tuple[0], "nonce"))
				nonce = stringTools::trim(tuple[1], '"');
			else
			{
				if (stringTools::iequal(tuple[0], "opaque"))
					opaque = stringTools::trim(tuple[1], '"');
			}
		}
	}
	
	dodoString cnonce = tools::MD5Hex(tools::stringRandom(5));
	
	dodoString methodForAuth = method + ":"; 
	
	tools::MD5Init(&context);
	tools::MD5Update(&context, (unsigned char *)methodForAuth.c_str(), methodForAuth.size());
	tools::MD5Update(&context, (unsigned char *)url.c_str(), url.size());
	tools::MD5Final(HA, &context);

	dodoString HA2 = tools::binToHex(dodoString((char *)&HA, 16));
	
	tools::MD5Init(&context);
	tools::MD5Update(&context, (unsigned char *)HA1.c_str(), HA1.size());
	tools::MD5Update(&context, (unsigned char *)":", 1);
	tools::MD5Update(&context, (unsigned char *)nonce.c_str(), nonce.size());
	tools::MD5Update(&context, (unsigned char *)":00000001:", 10);
	tools::MD5Update(&context, (unsigned char *)cnonce.c_str(), cnonce.size());
	tools::MD5Update(&context, (unsigned char *)":auth:", 6);
	tools::MD5Update(&context, (unsigned char *)HA2.c_str(), HA2.size());
	tools::MD5Final(HA, &context);
	
	dodoString response = tools::binToHex(dodoString((char *)&HA, 16));

	requestHeaders[responseHeader] = "Digest username=\"" + user + 
										"\", realm=\"" + realm + 
										 + "\", nonce=\"" + nonce + 
										 + "\", uri=\"" + url + 
										 + "\", qop=\"auth\", nc=00000001" + 
										 + ", cnonce=\"" + cnonce + 
										 + "\", response=\"" + response + 
										 + "\", opaque=\"" + opaque + "\"";
}

//-------------------------------------------------------------------

void 
ioNetworkHttp::makeBasicAuth(short responseHeader,
							const dodoString &user, 
							const dodoString &password)
{
	requestHeaders[responseHeader] = "Basic " + tools::encodeBase64(user + ":" + password);
}

//-------------------------------------------------------------------

__cookie 
ioNetworkHttp::parseCookie(const dodoString &header)
{
	dodoStringArray parts = tools::explode(header, &trim, ";");
	dodoStringArray tuple;
	
	dodoStringArray::iterator i = parts.begin(), j = parts.end();
	
	tuple = tools::explode(*i, "=", 2);
	if (tuple.size() != 2)
		return __cookie();
		
	__cookie cookie;
	cookie.name = tuple[0];
	cookie.value = tools::decodeUrl(tuple[1]);
	
	++i;
	
	for (;i!=j;++i)
	{
		tuple = tools::explode(*i, "=");
		
		if (stringTools::iequal(tuple[0], "path"))
			cookie.path = tuple[1];
		else
		{
			if (stringTools::iequal(tuple[0], "expires"))
				cookie.expires = tuple[1];
			else
			{
				if (stringTools::iequal(tuple[0], "domain"))
					cookie.domain = tuple[1];
				else
				{
					if (stringTools::iequal(tuple[0], "secure"))
						cookie.secure = true;
				}
			}
		}
	}
	
	return cookie;
}

//-------------------------------------------------------------------
