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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/ioNetworkHttp.h>

using namespace dodo::io::network;

const dodoString http::requestHeaderStatements[] = { "Accept",
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
													 "Cookie",				};

//-------------------------------------------------------------------

const dodoString http::responseHeaderStatements[] = { "Accept-Ranges",
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
													  "X-Powered-By",        };

//-------------------------------------------------------------------

__httpResponse::__httpResponse() : code(0),
								   redirected(false)
{
}

//-------------------------------------------------------------------

http::http() : httpStatusRE("^HTTP/[0-9].[0-9]\\s([0-9]+)\\s.*$"),
			   followRedirection(true),
			   authTries(0),
			   scheme(SCHEME_HTTP)
			   
#ifdef OPENSSL_EXT
			   
			   ,
			   certsSet(false)
			   
#endif
{
	proxyAuthInfo.enabled = false;
	proxyAuthInfo.authRequired = false;

	requestHeaders[HTTP_REQUESTHEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
	requestHeaders[HTTP_REQUESTHEADER_ACCEPT] = "*/*";
	requestHeaders[HTTP_REQUESTHEADER_CONNECTION] = "Close";
}

//-------------------------------------------------------------------

http::http(http &fd)
{
}

//-------------------------------------------------------------------

http::~http()
{
}

//-------------------------------------------------------------------

#ifdef OPENSSL_EXT

void 
http::setSertificates(const ssl::__certificates &a_certs)
{
	certs = a_certs;
	
	certsSet = true;
}

//-------------------------------------------------------------------

void 
http::removeSertificates()
{
	certs = ssl::__certificates();
	
	certsSet = false;
}

#endif

//-------------------------------------------------------------------

__httpResponse
http::getResponse()
{
	return response;
}

//-------------------------------------------------------------------

void
http::setUrl(const dodoString &a_url)
{
	urlComponents = tools::misc::parseUrl(a_url);

	if (urlComponents.protocol.size() == 0 || tools::string::iequal(urlComponents.protocol, "http"))
	{
		urlComponents.protocol = "http";
		scheme = SCHEME_HTTP;
	}
	else
	{

#ifdef OPENSSL_EXT

		if (tools::string::iequal(urlComponents.protocol, "https"))
			scheme = SCHEME_HTTPS;
		else

#endif

			throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_SETURL, ERR_LIBDODO, HTTPEX_UNSUPPORTEDSURICHEME, IONETWORKHTTPEX_UNSUPPORTEDSURICHEME_STR, __LINE__, __FILE__);
	}

	unsigned long portSize = urlComponents.port.size();

	if (portSize == 0)
	{
		if (scheme = SCHEME_HTTP)
			urlComponents.port = "80";

#ifdef OPENSSL_EXT

		else
			if (scheme = SCHEME_HTTPS)
				urlComponents.port = "443";

#endif

	}
	

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
		url.append(tools::misc::encodeUrl(urlComponents.request));
	}
}

//-------------------------------------------------------------------

void
http::setCookies(const dodoStringMap &cookies)
{
	dodoString data;

	dodoStringMap::const_iterator i = cookies.begin(), j = cookies.end();
	--j;

	for (; i != j; ++i)
	{
		data.append(i->first);
		data.append("=");
		data.append(tools::misc::encodeUrl(i->second));
		data.append("; ");
	}
	data.append(i->first);
	data.append("=");
	data.append(tools::misc::encodeUrl(i->second));

	requestHeaders[HTTP_REQUESTHEADER_COOKIE] = data;
}

//-------------------------------------------------------------------

void
http::GET()
{
	response = __httpResponse();

	exchange *ex;
	client *net;
	
	if (scheme = SCHEME_HTTP)
	{
		net = new client(OPTIONS_PROTO_FAMILY_IPV4, OPTIONS_TRANSFER_TYPE_STREAM);
		ex = new exchange;
	}

#ifdef OPENSSL_EXT

	else
	{
		net = new ssl::client(OPTIONS_PROTO_FAMILY_IPV4, OPTIONS_TRANSFER_TYPE_STREAM);
		ex = new ssl::exchange;
	}

#endif

	if (proxyAuthInfo.enabled)
		net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);
	else
	{
		tools::__hostInfo host = tools::network::getHostInfo(urlComponents.host);

		dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
		for (; o != p; ++o)
			try
			{
				if (scheme = SCHEME_HTTP)
					net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);

#ifdef OPENSSL_EXT

				else
				{
					((ssl::client *)net)->connect(*o, tools::string::stringToI(urlComponents.port), *(ssl::exchange *)ex);
					
					if (certsSet)
						((ssl::client *)net)->setSertificates(certs);
				}
				
#endif

				break;
			}
			catch (baseEx &exp)
			{
				if (exp.funcID == CLIENTEX_CONNECT)
				{
					if ((o + 1) == p)
					{
						delete net;
						delete ex;

						throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GET, ERR_LIBDODO, HTTPEX_CANNOTCONNECT, IONETWORKHTTPEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
					}
					else
						continue;
				}

				delete net;
				delete ex;

				throw;
			}
	}

	delete net;

	dodoString data;

	data.append("GET ");
	data.append(url);
	if (proxyAuthInfo.enabled)
		data.append(" HTTP/1.1\r\n");
	else
		data.append(" HTTP/1.0\r\n");
	dodoMap<short, dodoString>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
	for (; i != j; ++i)
	{
		data.append(requestHeaderStatements[i->first]);
		data.append(": ");
		data.append(i->second);
		data.append("\r\n");
	}
	data.append("Host: ");
	data.append(urlComponents.host);

	data.append("\r\n\r\n");

	ex->writeStreamString(data);

	try
	{
		switch (getContent(data, *ex))
		{
			case GETCONTENTSTATUS_NORMAL:

				break;

			case GETCONTENTSTATUS_REDIRECT:

				setUrl(response.headers[HTTP_RESPONSEHEADER_LOCATION]);

				GET();

				break;

			case GETCONTENTSTATUS_PROXYBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GET, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

				GET();

				break;

			case GETCONTENTSTATUS_WWWBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GET, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

				GET();

				break;

			case GETCONTENTSTATUS_PROXYDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GET, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password);

				GET();

				break;

			case GETCONTENTSTATUS_WWWDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GET, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_WWWAUTHENTICATE, HTTP_REQUESTHEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password);

				GET();

				break;

			case GETCONTENTSTATUS_WWWPROXYBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GET, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);
				makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

				GET();

				break;

			case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GET, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_WWWAUTHENTICATE, HTTP_REQUESTHEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password);
				makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password);

				GET();

				break;
		}
	}
	catch (...)
	{
		delete ex;

		throw;
	}

	delete ex;
}

//-------------------------------------------------------------------


__httpResponse
http::GET(const dodoString &a_url)
{
	setUrl(a_url);

	GET();

	return response;
}

//-------------------------------------------------------------------

__httpResponse
http::POST(const dodoString &a_url,
		   const dodoStringMap &arguments,
		   const dodoStringMap &files)
{
	setUrl(a_url);

	POST(arguments, files);

	return response;
}

//-------------------------------------------------------------------

void
http::POST(const dodoStringMap &arguments,
		   const dodoStringMap &files)
{
	dodoString boundary = "---------------------------" + tools::string::ulToString(tools::misc::ulRandom()) + tools::string::ulToString(tools::misc::ulRandom());
	dodoString type = "multipart/form-data; boundary=" + boundary;
	boundary.insert(0, "--");

	dodoString data;

	dodoStringMap::const_iterator i = files.begin(), j = files.end();

	for (; i != j; ++i)
	{
		data.append(boundary);
		data.append("\r\nContent-Disposition: form-data; name=\"");
		data.append(i->first);
		data.append("\"; filename=\"");
		data.append(tools::filesystem::basename(i->second));
		data.append("\"\r\n");

		data.append("Content-Type: application/octet-stream\r\n\r\n");

		data.append(tools::filesystem::getFileContents(i->second));
		data.append("\r\n");
	}

	i = arguments.begin();
	j = arguments.end();
	for (; i != j; ++i)
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
http::POST(const dodoString &a_url,
		   const dodoStringMap &arguments)
{
	setUrl(a_url);

	POST(arguments);

	return response;
}

//-------------------------------------------------------------------

void
http::POST(const dodoStringMap &arguments)
{
	dodoString data;

	dodoStringMap::const_iterator i(arguments.begin()), j(arguments.end());
	--j;

	for (; i != j; ++i)
	{
		data.append(tools::misc::encodeUrl(i->first));
		data.append("=");
		data.append(tools::misc::encodeUrl(i->second));
		data.append("&");
	}
	data.append(tools::misc::encodeUrl(i->first));
	data.append("=");
	data.append(tools::misc::encodeUrl(i->second));

	POST(data, "application/x-www-form-urlencoded");
}

//-------------------------------------------------------------------

__httpResponse
http::POST(const dodoString &a_url,
		   const dodoString &data,
		   const dodoString &type)
{
	setUrl(a_url);

	POST(data, type);

	return response;
}

//-------------------------------------------------------------------

void
http::POST(const dodoString &a_data,
		   const dodoString &type)
{
	response = __httpResponse();

	exchange *ex;
	client *net;
	
	if (scheme = SCHEME_HTTP)
	{
		net = new client(OPTIONS_PROTO_FAMILY_IPV4, OPTIONS_TRANSFER_TYPE_STREAM);
		ex = new exchange;
	}

#ifdef OPENSSL_EXT

	else
	{
		net = new ssl::client(OPTIONS_PROTO_FAMILY_IPV4, OPTIONS_TRANSFER_TYPE_STREAM);
		ex = new ssl::exchange;
	}

#endif

	if (proxyAuthInfo.enabled)
		net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);
	else
	{
		tools::__hostInfo host = tools::network::getHostInfo(urlComponents.host);

		dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
		for (; o != p; ++o)
			try
			{
				if (scheme = SCHEME_HTTP)
					net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);

#ifdef OPENSSL_EXT

				else
				{
					((ssl::client *)net)->connect(*o, tools::string::stringToI(urlComponents.port), *(ssl::exchange *)ex);
					
					if (certsSet)
						((ssl::client *)net)->setSertificates(certs);
				}

#endif

				break;
			}
			catch (baseEx &exp)
			{
				if (exp.funcID == CLIENTEX_CONNECT)
				{
					if ((o + 1) == p)
					{
						delete net;
						delete ex;

						throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_POST, ERR_LIBDODO, HTTPEX_CANNOTCONNECT, IONETWORKHTTPEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
					}
					else
						continue;
				}

				delete net;
				delete ex;
			
				throw;
			}
	}

	delete net;

	dodoString data;


	data.append("POST ");
	data.append(url);
	if (proxyAuthInfo.enabled)
		data.append(" HTTP/1.1\r\n");
	else
		data.append(" HTTP/1.0\r\n");
	dodoMap<short, dodoString>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
	for (; i != j; ++i)
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
	data.append(tools::string::ulToString(a_data.size()));
	data.append("\r\n");

	data.append("Content-type: ");
	data.append(type);
	data.append("\r\n\r\n");

	ex->writeStreamString(data);

	ex->outSize = a_data.size();
	ex->writeString(a_data);

	try
	{
		switch (getContent(data, *ex))
		{
			case GETCONTENTSTATUS_NORMAL:

				break;

			case GETCONTENTSTATUS_REDIRECT:

				setUrl(response.headers[HTTP_RESPONSEHEADER_LOCATION]);

				POST(data, type);

				break;

			case GETCONTENTSTATUS_PROXYBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_POST, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

				POST(data, type);

				break;

			case GETCONTENTSTATUS_WWWBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_POST, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

				POST(data, type);

				break;

			case GETCONTENTSTATUS_PROXYDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_POST, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password);

				POST(data, type);

				break;


			case GETCONTENTSTATUS_WWWDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_POST, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_WWWAUTHENTICATE, HTTP_REQUESTHEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password);

				POST(data, type);

				break;

			case GETCONTENTSTATUS_WWWPROXYBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GET, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);
				makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

				POST(data, type);

				break;

			case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GET, ERR_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_WWWAUTHENTICATE, HTTP_REQUESTHEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password);
				makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password);

				POST(data, type);

				break;
		}
	}
	catch (...)
	{
		delete ex;

		throw;
	}

	delete ex;
}

//-------------------------------------------------------------------

void
http::disableProxy()
{
	proxyAuthInfo.enabled = false;
}

//-------------------------------------------------------------------

void
http::setProxyInformation(const dodoString &host,
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
http::getHeaders(const dodoString &headers)
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

		piece = tools::string::trim(headers.substr(j, i - j), '\r');

		arr = tools::misc::explode(piece, ":", 2);
		if (arr.size() != 2)
		{
			if (!statusCode)
			{
				statusCode = true;

				if (httpStatusRE.match(piece, arr))
					response.code = tools::string::stringToS(tools::string::lTrim(arr[0], ' '));
			}
		}
		else
		{
			for (o = 0; o < HTTP_RESPONSEHEADERSTATEMENTS; ++o)
				if (tools::string::equal(responseHeaderStatements[o], arr[0]))
					response.headers[o] = tools::string::lTrim(arr[1], ' ');

			if (tools::string::equal("Set-Cookie", arr[0]))
				response.cookies.push_back(parseCookie(tools::string::lTrim(arr[1], ' ')));
		}

		++i;
		j = i;
	}
}

//-------------------------------------------------------------------

bool
http::extractHeaders(const dodoString &data,
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
http::getContent(dodoString &data,
				 exchange &ex)
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

					contentSize = tools::string::stringToUL(response.headers[HTTP_RESPONSEHEADER_CONTENTLENGTH]);

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
							if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Basic"))
								return GETCONTENTSTATUS_WWWPROXYBASICAUTH;
							else
							{
								if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Digest"))
									return GETCONTENTSTATUS_WWWPROXYDIGESTAUTH;
								else
									throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GETCONTENT, ERR_LIBDODO, HTTPEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE_STR, __LINE__, __FILE__);
							}
						}
						else
						{
							if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Basic"))
								return GETCONTENTSTATUS_WWWBASICAUTH;
							else
							{
								if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Digest"))
									return GETCONTENTSTATUS_WWWDIGESTAUTH;
								else
									throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GETCONTENT, ERR_LIBDODO, HTTPEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE_STR, __LINE__, __FILE__);
							}
						}
					}

					if (response.code == 407)
					{
						++authTries;

						proxyAuthInfo.authRequired = true;

						if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_PROXYAUTHENTICATE], "Basic"))
							return GETCONTENTSTATUS_PROXYBASICAUTH;
						else
						{
							if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_PROXYAUTHENTICATE], "Digest"))
								return GETCONTENTSTATUS_PROXYDIGESTAUTH;
							else
								throw baseEx(ERRMODULE_IONETWORKHTTP, HTTPEX_GETCONTENT, ERR_LIBDODO, HTTPEX_UNKNOWNPROXYAUTHTYPE, IONETWORKHTTPEX_UNKNOWNPROXYAUTHTYPE_STR, __LINE__, __FILE__);
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
			if (ex.funcID == EXCHANGEEX__READSTREAM)
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
http::trim(const dodoString &data)
{
	return tools::string::trim(data, ' ');
}

//-------------------------------------------------------------------

void
http::makeDigestAuth(short requestHeader,
					 short responseHeader,
					 const dodoString &method,
					 const dodoString &user,
					 const dodoString &password)
{
	dodoString nonce, opaque, realm;

	dodoStringArray parts = tools::misc::explode(response.headers[requestHeader].substr(7), &trim, ",");

	dodoString HA1;

	unsigned char HA[16];
	tools::misc::MD5_CTX context;

	dodoStringArray tuple;

	dodoStringArray::iterator i = parts.begin(), j = parts.end();
	for (; i != j; ++i)
	{
		tuple = tools::misc::explode(*i, "=");
		if (tuple.size() != 2)
			continue;

		if (tools::string::iequal(tuple[0], "realm"))
		{
			realm = tools::string::trim(tuple[1], '"');

			tools::misc::MD5Init(&context);
			tools::misc::MD5Update(&context, (unsigned char *)user.c_str(), user.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)realm.c_str(), realm.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)password.c_str(), password.size());
			tools::misc::MD5Final(HA, &context);

			HA1 = tools::misc::binToHex(dodoString((char *)&HA, 16));
		}
		else
		{
			if (tools::string::iequal(tuple[0], "nonce"))
				nonce = tools::string::trim(tuple[1], '"');
			else
			{
				if (tools::string::iequal(tuple[0], "opaque"))
					opaque = tools::string::trim(tuple[1], '"');
			}
		}
	}

	dodoString cnonce = tools::misc::MD5Hex(tools::misc::stringRandom(5));

	dodoString methodForAuth = method + ":";

	tools::misc::MD5Init(&context);
	tools::misc::MD5Update(&context, (unsigned char *)methodForAuth.c_str(), methodForAuth.size());
	tools::misc::MD5Update(&context, (unsigned char *)url.c_str(), url.size());
	tools::misc::MD5Final(HA, &context);

	dodoString HA2 = tools::misc::binToHex(dodoString((char *)&HA, 16));

	tools::misc::MD5Init(&context);
	tools::misc::MD5Update(&context, (unsigned char *)HA1.c_str(), HA1.size());
	tools::misc::MD5Update(&context, (unsigned char *)":", 1);
	tools::misc::MD5Update(&context, (unsigned char *)nonce.c_str(), nonce.size());
	tools::misc::MD5Update(&context, (unsigned char *)":00000001:", 10);
	tools::misc::MD5Update(&context, (unsigned char *)cnonce.c_str(), cnonce.size());
	tools::misc::MD5Update(&context, (unsigned char *)":auth:", 6);
	tools::misc::MD5Update(&context, (unsigned char *)HA2.c_str(), HA2.size());
	tools::misc::MD5Final(HA, &context);

	dodoString response = tools::misc::binToHex(dodoString((char *)&HA, 16));

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
http::makeBasicAuth(short responseHeader,
					const dodoString &user,
					const dodoString &password)
{
	requestHeaders[responseHeader] = "Basic " + tools::misc::encodeBase64(user + ":" + password);
}

//-------------------------------------------------------------------

dodo::cgi::__serverCookie
http::parseCookie(const dodoString &header)
{
	dodoStringArray parts = tools::misc::explode(header, &trim, ";");
	dodoStringArray tuple;

	dodoStringArray::iterator i = parts.begin(), j = parts.end();

	tuple = tools::misc::explode(*i, "=", 2);
	if (tuple.size() != 2)
		return cgi::__serverCookie();

	cgi::__serverCookie cookie;
	cookie.name = tuple[0];
	cookie.value = tools::misc::decodeUrl(tuple[1]);

	++i;

	for (; i != j; ++i)
	{
		tuple = tools::misc::explode(*i, "=");

		if (tools::string::iequal(tuple[0], "path"))
			cookie.path = tuple[1];
		else
		{
			if (tools::string::iequal(tuple[0], "expires"))
				cookie.expires = tuple[1];
			else
			{
				if (tools::string::iequal(tuple[0], "domain"))
					cookie.domain = tuple[1];
				else
				{
					if (tools::string::iequal(tuple[0], "secure"))
						cookie.secure = true;
				}
			}
		}
	}

	return cookie;
}

//-------------------------------------------------------------------
