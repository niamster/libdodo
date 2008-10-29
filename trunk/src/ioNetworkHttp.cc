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
													 "Cookie",             };

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
													  "Transfer-Encoding",
													  "WWW-Authenticate",
													  "Proxy-Authenticate",
													  "X-Powered-By",		};

//-------------------------------------------------------------------

__httpResponse::__httpResponse() : code(0),
								   redirected(false)
{
}

//-------------------------------------------------------------------

__httpPostFile::__httpPostFile(const dodoString path,
							   const dodoString mime) : path(path),
														mime(mime)
{
}

//-------------------------------------------------------------------

__httpPostFile::__httpPostFile()
{
}

//-------------------------------------------------------------------

http::__proxyAuthInfo::__proxyAuthInfo() : enabled(false),
										   authType(PROXYAUTHTYPE_NONE)
{
}

//-------------------------------------------------------------------

http::http() : httpStatusRE("^HTTP/[0-9].[0-9]\\s([0-9]+)\\s.*$"),
			   followRedirection(true),
			   cacheAuthentification(true),
			   authTries(0),
			   scheme(SCHEME_HTTP)

#ifdef OPENSSL_EXT

			   ,
			   certsSet(false)

#endif
{
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
http::setSertificates(const io::ssl::__certificates &a_certs)
{
	certs = a_certs;

	certsSet = true;
}

//-------------------------------------------------------------------

void
http::removeSertificates()
{
	certs.ca.clear();
	certs.caPath.clear();
	certs.cert.clear();
	certs.cipher.clear();
	certs.key.clear();
	certs.keyPassword.clear();
	certs.keyType = -1;

	certsSet = false;
}

#endif

//-------------------------------------------------------------------

void
http::setUrl(const dodoString &a_url)
{
	urlComponents = tools::code::parseUrl(a_url);

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

		throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_SETURL, exception::ERRNO_LIBDODO, HTTPEX_UNSUPPORTEDSURICHEME, IONETWORKHTTPEX_UNSUPPORTEDSURICHEME_STR, __LINE__, __FILE__);
	}

	unsigned long portSize = urlComponents.port.size();

	if (portSize == 0)
	{
		if (scheme == SCHEME_HTTP)
			urlComponents.port = "80";

#ifdef OPENSSL_EXT

		else
		if (scheme == SCHEME_HTTPS)
			urlComponents.port = "443";

#endif

	}

	urlBasePath.clear();
	urlQuery.clear();

	urlBasePath.append(urlComponents.protocol);
	urlBasePath.append("://" );
	urlBasePath.append(urlComponents.host);
	if (portSize > 0)
	{
		urlBasePath.append(":");
		urlBasePath.append(urlComponents.port);
	}
	urlBasePath.append("/");
	urlBasePath.append(urlComponents.path);

	if (urlComponents.request.size() > 0)
	{
		urlQuery.append("?");
		urlQuery.append(tools::code::encodeUrl(urlComponents.request));
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
		data.append(tools::code::encodeUrl(i->second));
		data.append("; ");
	}
	data.append(i->first);
	data.append("=");
	data.append(tools::code::encodeUrl(i->second));

	requestHeaders[HTTP_REQUESTHEADER_COOKIE] = data;
}

//-------------------------------------------------------------------

__httpResponse
http::GET()
{
	exchange *ex = NULL;
	client *net = NULL;

	dodoString data;

	if (scheme == SCHEME_HTTP)
	{
		net = new client(CONNECTION_PROTO_FAMILY_IPV4, CONNECTION_TRANSFER_TYPE_STREAM);
		ex = new exchange;
	}

#ifdef OPENSSL_EXT

	else
	{
		net = new ssl::client(CONNECTION_PROTO_FAMILY_IPV4, CONNECTION_TRANSFER_TYPE_STREAM);
		ex = new ssl::exchange;
	}

#endif

	if (proxyAuthInfo.enabled)
	{
		if (scheme == SCHEME_HTTP)
			net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);

#ifdef OPENSSL_EXT

		else
		{
			net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *(exchange *)ex);
			data.append("CONNECT ");
			data.append(urlComponents.host);
			data.append(":");
			data.append(urlComponents.port);
			data.append(" HTTP/1.1\r\n");
			if (requestHeaders.find(HTTP_REQUESTHEADER_PROXYAUTHORIZATION) != requestHeaders.end())
			{
				data.append(requestHeaderStatements[HTTP_REQUESTHEADER_PROXYAUTHORIZATION]);
				data.append(": ");
				data.append(requestHeaders[HTTP_REQUESTHEADER_PROXYAUTHORIZATION]);
				data.append("\r\n");
			}
			data.append("\r\n");

			unsigned long outSize = ex->outSize;
			ex->outSize = data.size();
			ex->exchange::_write(data.c_str());
			ex->outSize = outSize;

			char proxyData[512];
			ex->setInBufferSize(512);
			ex->inSize = 512;

			try
			{
				switch (getProxyConnectResponse(proxyData, ex))
				{
					case GETCONTENTSTATUS_NORMAL:

						break;

					case GETCONTENTSTATUS_PROXYBASICAUTH:

						if (authTries > 2)
						{
							authTries = 0;

							throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GET, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
						}

						makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

						delete ex;
						ex = NULL;
						delete net;
						net = NULL;

						return GET();

					case GETCONTENTSTATUS_PROXYDIGESTAUTH:

						if (authTries > 2)
						{
							authTries = 0;

							throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GET, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
						}

						makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password);

						delete ex;
						ex = NULL;
						delete net;
						net = NULL;

						return GET();
				}
			}
			catch (...)
			{
				delete ex;
				delete net;

				clear();

				throw;
			}

			if (certsSet)
				((ssl::client *)net)->setSertificates(certs);
			else
				((ssl::client *)net)->initSsl();

			((ssl::client *)net)->socket = ((ssl::exchange *)ex)->socket;
			((ssl::client *)net)->connectSsl();

			((ssl::exchange *)ex)->sslHandle = ((ssl::client *)net)->sslHandle;

			((ssl::client *)net)->socket = -1;
			((ssl::client *)net)->sslHandle = NULL;
		}
#endif
	}
	else
	{
		tools::__hostInfo host = tools::network::getHostInfo(urlComponents.host);

		dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
		for (; o != p; ++o)
			try
			{
				if (scheme == SCHEME_HTTP)
				{
					net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
					ex->setInBufferSize(512);
					ex->inSize = 512;
				}

#ifdef OPENSSL_EXT

				else
				{
					if (certsSet)
						((ssl::client *)net)->setSertificates(certs);

					((ssl::client *)net)->connect(*o, tools::string::stringToI(urlComponents.port), *(ssl::exchange *)ex);
					ex->setInBufferSize(512);
					ex->inSize = 512;
				}

#endif

				break;
			}
			catch (exception::basic &exp)
			{
#ifdef OPENSSL_EXT

				if (exp.funcID == CLIENTEX_CONNECT || exp.funcID == ssl::CLIENTEX_CONNECT)

#else

				if (exp.funcID == CLIENTEX_CONNECT)

#endif
				{
					if ((o + 1) == p)
					{
						delete net;
						delete ex;

						throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GET, exception::ERRNO_LIBDODO, HTTPEX_CANNOTCONNECT, IONETWORKHTTPEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
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

	data.clear();

	data.append("GET ");
	data.append(urlBasePath);
	data.append(urlQuery);
	data.append(" HTTP/1.1\r\n");

	if (cacheAuthentification)
	{
		dodoStringMap::iterator header = httpAuth.find(urlBasePath);
		if (header != httpAuth.end())
			requestHeaders[HTTP_REQUESTHEADER_AUTHORIZATION] = header->second;
	}

	dodoMap<short, dodoString>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
	for (; i != j; ++i)
	{
#ifdef OPENSSL_EXT

		if (proxyAuthInfo.enabled && scheme == SCHEME_HTTPS && i->first == HTTP_REQUESTHEADER_PROXYAUTHORIZATION)
			continue;

#endif

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
		switch (getContent(data, ex))
		{
			case GETCONTENTSTATUS_NORMAL:

				break;

			case GETCONTENTSTATUS_REDIRECT:

				setUrl(response.headers[HTTP_RESPONSEHEADER_LOCATION]);

				delete ex;
				ex = NULL;

				return GET();

			case GETCONTENTSTATUS_PROXYBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GET, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

				delete ex;
				ex = NULL;

				return GET();

			case GETCONTENTSTATUS_WWWBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GET, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

				delete ex;
				ex = NULL;

				return GET();

			case GETCONTENTSTATUS_PROXYDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GET, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password);

				delete ex;
				ex = NULL;

				return GET();

			case GETCONTENTSTATUS_WWWDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GET, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_WWWAUTHENTICATE, HTTP_REQUESTHEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password);

				delete ex;
				ex = NULL;

				return GET();

			case GETCONTENTSTATUS_WWWPROXYBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GET, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

				if (proxyAuthInfo.authType == PROXYAUTHTYPE_BASIC)
					makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
				else
					makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password);

				delete ex;
				ex = NULL;

				return GET();

			case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GET, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_WWWAUTHENTICATE, HTTP_REQUESTHEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password);

				if (proxyAuthInfo.authType == PROXYAUTHTYPE_BASIC)
					makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
				else
					makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password);

				delete ex;
				ex = NULL;

				return GET();
		}
	}
	catch (...)
	{
		delete ex;

		clear();

		throw;
	}

	delete ex;

	__httpResponse response = this->response;

	clear();

	return response;
}

//-------------------------------------------------------------------

__httpResponse
http::GET(const dodoString &a_url)
{
	setUrl(a_url);

	return GET();
}

//-------------------------------------------------------------------

__httpResponse
http::POST(const dodoString &a_url,
		   const dodoStringMap &arguments,
		   const dodoMap<dodoString, __httpPostFile> &files)
{
	setUrl(a_url);

	return POST(arguments, files);
}

//-------------------------------------------------------------------

__httpResponse
http::POST(const dodoStringMap &arguments,
		   const dodoMap<dodoString, __httpPostFile> &files)
{
	dodoString boundary = "---------------------------" + tools::string::ulToString(tools::misc::ulRandom()) + tools::string::ulToString(tools::misc::ulRandom());
	dodoString type = "multipart/form-data; boundary=" + boundary;
	boundary.insert(0, "--");

	dodoString data;

	dodoMap<dodoString, __httpPostFile>::const_iterator i = files.begin(), j = files.end();
	for (; i != j; ++i)
	{
		data.append(boundary);
		data.append("\r\nContent-Disposition: form-data; name=\"");
		data.append(i->first);
		data.append("\"; filename=\"");
		data.append(tools::filesystem::basename(i->second.path));
		data.append("\"\r\n");

		data.append("Content-Type: ");
		data.append(i->second.mime);
		data.append("\r\n\r\n");

		data.append(tools::filesystem::getFileContents(i->second.path));
		data.append("\r\n");
	}

	dodoStringMap::const_iterator o = arguments.begin(), p = arguments.end();
	for (; o != p; ++o)
	{
		data.append(boundary);
		data.append("\r\nContent-Disposition: form-data; name=\"");
		data.append(o->first);
		data.append("\"\r\n\r\n");

		data.append(o->second);
		data.append("\r\n");
	}
	data.append(boundary);
	data.append("--");

	return POST(data, type);
}

//-------------------------------------------------------------------

__httpResponse
http::POST(const dodoString &a_url,
		   const dodoStringMap &arguments)
{
	setUrl(a_url);

	return POST(arguments);
}

//-------------------------------------------------------------------

__httpResponse
http::POST(const dodoStringMap &arguments)
{
	dodoString data;

	dodoStringMap::const_iterator i(arguments.begin()), j(arguments.end());
	--j;

	for (; i != j; ++i)
	{
		data.append(tools::code::encodeUrl(i->first));
		data.append("=");
		data.append(tools::code::encodeUrl(i->second));
		data.append("&");
	}
	data.append(tools::code::encodeUrl(i->first));
	data.append("=");
	data.append(tools::code::encodeUrl(i->second));

	return POST(data, "application/x-www-form-urlencoded");
}

//-------------------------------------------------------------------

__httpResponse
http::POST(const dodoString &a_url,
		   const dodoString &data,
		   const dodoString &type)
{
	setUrl(a_url);

	return POST(data, type);
}

//-------------------------------------------------------------------

__httpResponse
http::POST(const dodoString &a_data,
		   const dodoString &type)
{
	exchange *ex = NULL;
	client *net = NULL;

	dodoString data;

	if (scheme == SCHEME_HTTP)
	{
		net = new client(CONNECTION_PROTO_FAMILY_IPV4, CONNECTION_TRANSFER_TYPE_STREAM);
		ex = new exchange;
	}

#ifdef OPENSSL_EXT

	else
	{
		net = new ssl::client(CONNECTION_PROTO_FAMILY_IPV4, CONNECTION_TRANSFER_TYPE_STREAM);
		ex = new ssl::exchange;
	}

#endif

	if (proxyAuthInfo.enabled)
	{
		if (scheme == SCHEME_HTTP)
			net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);

#ifdef OPENSSL_EXT

		else
		{
			net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *(exchange *)ex);
			data.append("CONNECT ");
			data.append(urlComponents.host);
			data.append(":");
			data.append(urlComponents.port);
			data.append(" HTTP/1.1\r\n");
			if (requestHeaders.find(HTTP_REQUESTHEADER_PROXYAUTHORIZATION) != requestHeaders.end())
			{
				data.append(requestHeaderStatements[HTTP_REQUESTHEADER_PROXYAUTHORIZATION]);
				data.append(": ");
				data.append(requestHeaders[HTTP_REQUESTHEADER_PROXYAUTHORIZATION]);
				data.append("\r\n");
			}
			data.append("\r\n");

			unsigned long outSize = ex->outSize;
			ex->outSize = data.size();
			ex->exchange::_write(data.c_str());
			ex->outSize = outSize;

			char proxyData[512];
			ex->setInBufferSize(512);
			ex->inSize = 512;

			try
			{
				switch (getProxyConnectResponse(proxyData, ex))
				{
					case GETCONTENTSTATUS_NORMAL:

						break;

					case GETCONTENTSTATUS_PROXYBASICAUTH:

						if (authTries > 2)
						{
							authTries = 0;

							throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_POST, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
						}

						makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

						delete ex;
						ex = NULL;
						delete net;
						net = NULL;

						return POST(data, type);

					case GETCONTENTSTATUS_PROXYDIGESTAUTH:

						if (authTries > 2)
						{
							authTries = 0;

							throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_POST, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
						}

						makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password);

						delete ex;
						ex = NULL;
						delete net;
						net = NULL;

						return POST(data, type);
				}
			}
			catch (...)
			{
				delete ex;
				delete net;

				clear();

				throw;
			}

			if (certsSet)
				((ssl::client *)net)->setSertificates(certs);
			else
				((ssl::client *)net)->initSsl();

			((ssl::client *)net)->socket = ((ssl::exchange *)ex)->socket;
			((ssl::client *)net)->connectSsl();

			((ssl::exchange *)ex)->sslHandle = ((ssl::client *)net)->sslHandle;

			((ssl::client *)net)->socket = -1;
			((ssl::client *)net)->sslHandle = NULL;
		}

#endif
	}
	else
	{
		tools::__hostInfo host = tools::network::getHostInfo(urlComponents.host);

		dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
		for (; o != p; ++o)
			try
			{
				if (scheme == SCHEME_HTTP)
				{
					net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
					ex->setInBufferSize(512);
					ex->inSize = 512;
				}

#ifdef OPENSSL_EXT

				else
				{
					if (certsSet)
						((ssl::client *)net)->setSertificates(certs);

					((ssl::client *)net)->connect(*o, tools::string::stringToI(urlComponents.port), *(ssl::exchange *)ex);
					ex->setInBufferSize(512);
					ex->inSize = 512;
				}

#endif

				break;
			}
			catch (exception::basic &exp)
			{
#ifdef OPENSSL_EXT

				if (exp.funcID == CLIENTEX_CONNECT || exp.funcID == ssl::CLIENTEX_CONNECT)

#else

				if (exp.funcID == CLIENTEX_CONNECT)

#endif
				{
					if ((o + 1) == p)
					{
						delete net;
						delete ex;

						throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_POST, exception::ERRNO_LIBDODO, HTTPEX_CANNOTCONNECT, IONETWORKHTTPEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
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

	data.append("POST ");
	data.append(urlBasePath);
	data.append(urlQuery);
	data.append(" HTTP/1.1\r\n");

	if (cacheAuthentification)
	{
		dodoStringMap::iterator header = httpAuth.find(urlBasePath);
		if (header != httpAuth.end())
			requestHeaders[HTTP_REQUESTHEADER_AUTHORIZATION] = header->second;
	}

	dodoMap<short, dodoString>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
	for (; i != j; ++i)
	{
#ifdef OPENSSL_EXT

		if (proxyAuthInfo.enabled && scheme == SCHEME_HTTPS && i->first == HTTP_REQUESTHEADER_PROXYAUTHORIZATION)
			continue;

#endif

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

	unsigned long outSize = ex->outSize;
	ex->outSize = a_data.size();
	ex->writeString(a_data);
	ex->outSize = outSize;

	try
	{
		switch (getContent(data, ex))
		{
			case GETCONTENTSTATUS_NORMAL:

				break;

			case GETCONTENTSTATUS_REDIRECT:

				setUrl(response.headers[HTTP_RESPONSEHEADER_LOCATION]);

				return POST(data, type);

			case GETCONTENTSTATUS_PROXYBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_POST, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

				return POST(data, type);

			case GETCONTENTSTATUS_WWWBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_POST, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

				return POST(data, type);

			case GETCONTENTSTATUS_PROXYDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_POST, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password);

				return POST(data, type);

			case GETCONTENTSTATUS_WWWDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_POST, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_WWWAUTHENTICATE, HTTP_REQUESTHEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password);

				return POST(data, type);

			case GETCONTENTSTATUS_WWWPROXYBASICAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_POST, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeBasicAuth(HTTP_REQUESTHEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

				if (proxyAuthInfo.authType == PROXYAUTHTYPE_BASIC)
					makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
				else
					makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password);

				return POST(data, type);

			case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:

				if (authTries > 2)
				{
					authTries = 0;

					throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_POST, exception::ERRNO_LIBDODO, HTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
				}

				makeDigestAuth(HTTP_RESPONSEHEADER_WWWAUTHENTICATE, HTTP_REQUESTHEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password);

				if (proxyAuthInfo.authType == PROXYAUTHTYPE_BASIC)
					makeBasicAuth(HTTP_REQUESTHEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
				else
					makeDigestAuth(HTTP_RESPONSEHEADER_PROXYAUTHENTICATE, HTTP_REQUESTHEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password);

				return POST(data, type);
		}
	}
	catch (...)
	{
		delete ex;

		clear();

		throw;
	}

	delete ex;

	__httpResponse response = this->response;

	clear();

	return response;
}

//-------------------------------------------------------------------

void
http::removeProxy()
{
	proxyAuthInfo = __proxyAuthInfo();

	requestHeaders.erase(HTTP_REQUESTHEADER_PROXYAUTHORIZATION);
}

//-------------------------------------------------------------------

void
http::setProxy(const dodoString &host,
			   unsigned int port,
			   const dodoString &user,
			   const dodoString &password)
{
	proxyAuthInfo.host = host;
	proxyAuthInfo.port = port;
	proxyAuthInfo.user = user;
	proxyAuthInfo.password = password;
	proxyAuthInfo.enabled = true;
	proxyAuthInfo.authType = PROXYAUTHTYPE_NONE;

	requestHeaders.erase(HTTP_REQUESTHEADER_PROXYAUTHORIZATION);
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

		arr = tools::misc::split(piece, ":", 2);
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
http::getProxyConnectResponse(char *data,
							  exchange *ex)
{
	unsigned long size = 0, i;
	bool endOfHeaders = false;

	dodoString headers;

	while (true)
	{
		try
		{
			size = ex->exchange::_readStream(data);

			if (size == 0)
				break;

			headers.append(data);

			i = headers.find("\r\n\r\n");
			if (i == dodoString::npos)
			{
				i = headers.find("\n\n");
				if (i != dodoString::npos)
				{
					headers.resize(i + 1);

					endOfHeaders = true;
				}
			}
			else
			{
				headers.resize(i + 2);

				endOfHeaders = true;
			}

			if (endOfHeaders)
			{
				getHeaders(headers);

				if (response.code == 407)
				{
					++authTries;

					if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_PROXYAUTHENTICATE], "Basic"))
					{
						proxyAuthInfo.authType = PROXYAUTHTYPE_BASIC;

						return GETCONTENTSTATUS_PROXYBASICAUTH;
					}
					else
					{
						if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_PROXYAUTHENTICATE], "Digest"))
						{
							proxyAuthInfo.authType = PROXYAUTHTYPE_DIGEST;

							return GETCONTENTSTATUS_PROXYDIGESTAUTH;
						}
						else
							throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GETPROXYCONNECTRESPONSE, exception::ERRNO_LIBDODO, HTTPEX_UNKNOWNPROXYAUTHTYPE, IONETWORKHTTPEX_UNKNOWNPROXYAUTHTYPE_STR, __LINE__, __FILE__);
					}
				}

				break;
			}
		}
		catch (exception::basic &ex)
		{
			if (ex.funcID == EXCHANGEEX__READSTREAM)
				break;
			else
				throw;
		}
	}

	authTries = 0;

	return GETCONTENTSTATUS_NORMAL;
}

//-------------------------------------------------------------------

short
http::getContent(dodoString &data,
				 exchange *ex)
{
	unsigned long contentSize = 0;

	long chunkSize = 0;
	dodoString chunkSizeHex;

	unsigned long eoc;

	bool endOfHeaders = false;
	bool chunked = false;

	dodoString headers;

	response.data.clear();
	response.headers.clear();
	response.code = 0;

	while (true)
	{
		try
		{
			if (chunked)
			{
				if (chunkSize > 0)
				{
					ex->inSize = chunkSize;
					ex->readString(data);
					response.data.append(data, 0, ex->inSize - 2);///< remove 2 bytes of CRLF after chunk
				}

				ex->inSize = 512;
				ex->readStreamString(data);

				if (data.size() == 0)
					break;

				eoc = data.find("\r\n");
				if (eoc == dodoString::npos)
				{
					eoc = data.find('\n');
					if (eoc != dodoString::npos)
						++eoc;
				}
				else
					eoc += 2;

				if (eoc != dodoString::npos)
				{
					chunkSizeHex.clear();

					for (unsigned long i=0;i<eoc;++i)
					{
						if (data[i] == '\r' || data[i] == ';' || data[i] == '\n')
							break;

						chunkSizeHex.append(1, data[i]);
					}

					if (chunkSizeHex.size() == 0)
					{
						chunkSize = 0;

						continue;
					}

					chunkSize = tools::code::hexToLong(chunkSizeHex);

					if (chunkSize == 0)
						break;

					unsigned long dataSize = data.size();
					if (dataSize > eoc)
						response.data.append(data.data() + eoc);

					chunkSize -= dataSize - 2 - eoc;///< 2 bytes for CRLF after chunk
				}
			}
			else
			{
				ex->readStreamString(data);

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

						if (followRedirection && (response.code / 100) == 3 && response.code != 304)
						{
							response.redirected = true;

							return GETCONTENTSTATUS_REDIRECT;
						}

						if (response.code == 401)
						{
							++authTries;

							if (proxyAuthInfo.authType != PROXYAUTHTYPE_NONE)
							{
								if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Basic"))
									return GETCONTENTSTATUS_WWWPROXYBASICAUTH;
								else
								{
									if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Digest"))
										return GETCONTENTSTATUS_WWWPROXYDIGESTAUTH;
									else
										throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GETCONTENT, exception::ERRNO_LIBDODO, HTTPEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE_STR, __LINE__, __FILE__);
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
										throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GETCONTENT, exception::ERRNO_LIBDODO, HTTPEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE_STR, __LINE__, __FILE__);
								}
							}
						}

						if (response.code == 407)
						{
							++authTries;

							if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_PROXYAUTHENTICATE], "Basic"))
							{
								proxyAuthInfo.authType = PROXYAUTHTYPE_BASIC;

								return GETCONTENTSTATUS_PROXYBASICAUTH;
							}
							else
							{
								if (tools::string::contains(response.headers[HTTP_RESPONSEHEADER_PROXYAUTHENTICATE], "Digest"))
								{
									proxyAuthInfo.authType = PROXYAUTHTYPE_DIGEST;

									return GETCONTENTSTATUS_PROXYDIGESTAUTH;
								}
								else
									throw exception::basic(exception::ERRMODULE_IONETWORKHTTP, HTTPEX_GETCONTENT, exception::ERRNO_LIBDODO, HTTPEX_UNKNOWNPROXYAUTHTYPE, IONETWORKHTTPEX_UNKNOWNPROXYAUTHTYPE_STR, __LINE__, __FILE__);
							}
						}

						chunked = tools::string::equal(response.headers[HTTP_RESPONSEHEADER_TRANSFERENCODING], "chunked");

						if (chunked)
						{
							eoc = response.data.find("\r\n");
							if (eoc == dodoString::npos)
							{
								eoc = response.data.find('\n');
								if (eoc != dodoString::npos)
									++eoc;
							}
							else
								eoc += 2;

							if (eoc != dodoString::npos)
							{
								for (unsigned long i=0;i<eoc;++i)
								{
									if (response.data[i] == '\r' || response.data[i] == ';' || response.data[i] == '\n')
										break;

									chunkSizeHex.append(1, response.data[i]);
								}

								response.data.erase(0, eoc);

								chunkSize = tools::code::hexToLong(chunkSizeHex) - response.data.size() + 2;///< 2 bytes for CRLF after chunk
							}
						}
						else
						{
							contentSize = tools::string::stringToUL(response.headers[HTTP_RESPONSEHEADER_CONTENTLENGTH]);

							ex->inSize = 16384;
						}

						ex->setInBufferSize(16384);
					}
				}

				if (contentSize > 0 && response.data.size() == contentSize)
					break;
			}
		}
		catch (exception::basic &ex)
		{
#ifdef OPENSSL_EXT

			if (ex.funcID == EXCHANGEEX__READSTREAM || ex.funcID == ssl::EXCHANGEEX__READSTREAM)

#else

			if (ex.funcID == EXCHANGEEX__READSTREAM)

#endif
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

	dodoStringArray parts = tools::misc::split(response.headers[requestHeader].substr(7), &trim, ",");

	dodoString HA1;

	unsigned char HA[16];
	tools::code::__MD5Context context;

	dodoStringArray tuple;

	dodoStringArray::iterator i = parts.begin(), j = parts.end();
	for (; i != j; ++i)
	{
		tuple = tools::misc::split(*i, "=");
		if (tuple.size() != 2)
			continue;

		if (tools::string::iequal(tuple[0], "realm"))
		{
			realm = tools::string::trim(tuple[1], '"');

			tools::code::MD5Init(&context);
			tools::code::MD5Update(&context, (unsigned char *)user.c_str(), user.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)realm.c_str(), realm.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)password.c_str(), password.size());
			tools::code::MD5Final(HA, &context);

			HA1 = tools::code::binToHex(dodoString((char *)&HA, 16));
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

	dodoString cnonce = tools::code::MD5Hex(tools::misc::stringRandom(5));

	dodoString methodForAuth = method + ":";

	dodoString url = urlBasePath;
	url.append(urlQuery);

	tools::code::MD5Init(&context);
	tools::code::MD5Update(&context, (unsigned char *)methodForAuth.c_str(), methodForAuth.size());
	tools::code::MD5Update(&context, (unsigned char *)url.c_str(), url.size());
	tools::code::MD5Final(HA, &context);

	dodoString HA2 = tools::code::binToHex(dodoString((char *)&HA, 16));

	tools::code::MD5Init(&context);
	tools::code::MD5Update(&context, (unsigned char *)HA1.c_str(), HA1.size());
	tools::code::MD5Update(&context, (unsigned char *)":", 1);
	tools::code::MD5Update(&context, (unsigned char *)nonce.c_str(), nonce.size());
	tools::code::MD5Update(&context, (unsigned char *)":00000001:", 10);
	tools::code::MD5Update(&context, (unsigned char *)cnonce.c_str(), cnonce.size());
	tools::code::MD5Update(&context, (unsigned char *)":auth:", 6);
	tools::code::MD5Update(&context, (unsigned char *)HA2.c_str(), HA2.size());
	tools::code::MD5Final(HA, &context);

	dodoString response = tools::code::binToHex(dodoString((char *)&HA, 16));

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
	requestHeaders[responseHeader] = "Basic " + tools::code::encodeBase64(user + ":" + password);
}

//-------------------------------------------------------------------

void
http::clear()
{
	requestHeaders.erase(HTTP_REQUESTHEADER_COOKIE);

	if (!cacheAuthentification)
	{
		requestHeaders.erase(HTTP_REQUESTHEADER_PROXYAUTHORIZATION);
		proxyAuthInfo.authType = PROXYAUTHTYPE_NONE;
	}
	else
	{
		dodoMap<short, dodoString>::iterator header = requestHeaders.find(HTTP_REQUESTHEADER_AUTHORIZATION);
		if (header != requestHeaders.end())
			httpAuth[urlBasePath] = header->second;
	}

	requestHeaders.erase(HTTP_REQUESTHEADER_AUTHORIZATION);

	response.headers.clear();
	response.cookies.clear();
	response.data.clear();
	response.code = 0;
	response.redirected = false;
}

//-------------------------------------------------------------------

void
http::setAuthInfo(const dodoString &user,
				  const dodoString &password)
{
	urlComponents.login = user;
	urlComponents.password = password;
}

//-------------------------------------------------------------------

dodo::cgi::__cgiCookie
http::parseCookie(const dodoString &header)
{
	dodoStringArray parts = tools::misc::split(header, &trim, ";");
	dodoStringArray tuple;

	dodoStringArray::iterator i = parts.begin(), j = parts.end();

	tuple = tools::misc::split(*i, "=", 2);
	if (tuple.size() != 2)
		return cgi::__cgiCookie();

	cgi::__cgiCookie cookie;
	cookie.name = tuple[0];
	cookie.value = tools::code::decodeUrl(tuple[1]);

	++i;

	for (; i != j; ++i)
	{
		tuple = tools::misc::split(*i, "=");

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
