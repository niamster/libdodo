/***************************************************************************
 *            ioNetworkHttp.h
 *
 *  Tue Oct 8 08:19:57 2005
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

#ifndef _IONETWORKHTTP_H_
#define _IONETWORKHTTP_H_ 1

#include <libdodo/directives.h>

#include <libdodo/toolsCode.h>
#include <libdodo/ioSsl.h>
#include <libdodo/types.h>

namespace dodo
{
	namespace cgi
	{
		struct __cgiCookie__;
	};

	namespace io
	{
		namespace network
		{
			class exchange;

#define HTTP_REQUESTHEADERSTATEMENTS 12

			/**
			 * @enum httpRequestHeaderEnum defines HTTP request headers
			 */
			enum httpRequestHeaderEnum
			{
				HTTP_REQUESTHEADER_ACCEPT,              ///< content-types that are acceptable
				HTTP_REQUESTHEADER_ACCEPTCHARSET,       ///< character sets that are acceptable
				HTTP_REQUESTHEADER_ACCEPTENCODING,      ///< acceptable encodings
				HTTP_REQUESTHEADER_ACCEPTLANGUAGE,      ///< acceptable languages for response
				HTTP_REQUESTHEADER_ACCEPTRANGES,        ///< allows the server to indicate its acceptance of range requests for a resource
				HTTP_REQUESTHEADER_AUTHORIZATION,       ///< authentication credentials for HTTP authentication
				HTTP_REQUESTHEADER_PROXYAUTHORIZATION,  ///< authentication credentials for HTTP proxy authentication
				HTTP_REQUESTHEADER_CONNECTION,          ///< what type of connection the user-agent would prefer
				HTTP_REQUESTHEADER_DATE,                ///< the date and time that the message was sent
				HTTP_REQUESTHEADER_IFMODIFIEDSINCE,     ///< allows a 304 Not Modified to be returned
				HTTP_REQUESTHEADER_USERAGENT,           ///< the user agent string of the user agent
				HTTP_REQUESTHEADER_COOKIE,              ///< the user agent string of the user agent
			};

#define HTTP_RESPONSEHEADERSTATEMENTS 20

			/**
			 * @enum httpResponseHeaderEnum defines HTTP response headers
			 */
			enum httpResponseHeaderEnum
			{
				HTTP_RESPONSEHEADER_ACCEPTRANGES,       ///< what partial content range types this server supports
				HTTP_RESPONSEHEADER_AGE,                ///< the age the object has been in a proxy cache in seconds
				HTTP_RESPONSEHEADER_ALLOW,              ///< valid actions for a specified resource. To be used for a 405 Method not allowed
				HTTP_RESPONSEHEADER_CACHECONTROL,       ///< tells all caching mechanisms from server to client whether they may cache this object
				HTTP_RESPONSEHEADER_CONTENTENCODING,    ///< the type of encoding used on the data
				HTTP_RESPONSEHEADER_CONTENTLANGUAGE,    ///< the language the content is in
				HTTP_RESPONSEHEADER_CONTENTLENGTH,      ///< the length of the content in bytes
				HTTP_RESPONSEHEADER_CONTENTLOCATION,    ///< an alternate location for the returned data
				HTTP_RESPONSEHEADER_CONTENTDISPOSITION, ///< an opportunity to raise a "File Download" dialog box for a known MIME type
				HTTP_RESPONSEHEADER_CONTENTMD5,         ///< an MD5 sum of the content of the response
				HTTP_RESPONSEHEADER_CONTENTRANGE,       ///< where in a full body message this partial message belongs
				HTTP_RESPONSEHEADER_CONTENTTYPE,        ///< the mime type of this content
				HTTP_RESPONSEHEADER_DATE,               ///< the date and time that the message was sent
				HTTP_RESPONSEHEADER_LASTMODIFIED,       ///< the last modified date for the requested object
				HTTP_RESPONSEHEADER_LOCATION,           ///< used in redirection
				HTTP_RESPONSEHEADER_SERVER,             ///< a name for the server
				HTTP_RESPONSEHEADER_TRANSFERENCODING,   ///< encoding of the response
				HTTP_RESPONSEHEADER_WWWAUTHENTICATE,    ///< authentication request
				HTTP_RESPONSEHEADER_PROXYAUTHENTICATE,  ///< proxy authentication request
				HTTP_RESPONSEHEADER_XPOWEREDBY,         ///< cgi provider
			};

			/**
			 * @enum httpRequestTypeEnum defines type of HTTP request
			 */
			enum httpRequestTypeEnum
			{
				HTTP_REQUESTTYPE_GET,
				HTTP_REQUESTTYPE_POST
			};

			/**
			 * @struct __httpResponse__
			 * @brief defines HTTP response
			 */
			struct __httpResponse__
			{
				/**
				 * constructor
				 */
				__httpResponse__();

				dodoMap<short, dodoString>  headers;    ///< response headers[see httpResponseHeaderEnum]
				dodoArray<cgi::__cgiCookie__> cookies;    ///< cookies from the server
				dodoString                  data;       ///< response data
				short                       code;       ///< response code
				bool                        redirected; ///< true if redirection was performeed
			};

			/**
			 * @struct __httpPostFile__
			 * @brief defines file for POST request
			 */
			struct __httpPostFile__
			{
				/**
				 * constructor
				 * @param path defines path to file
				 * @param mime defines mimetype of the file
				 */
				__httpPostFile__(const dodoString path,
							   const dodoString mime);

				/**
				 * constructor
				 */
				__httpPostFile__();

				dodoString path;                        ///< path to file
				dodoString mime;                        ///< mimetype of the file
			};

			/**
			 * @class http
			 * @brief provides HTTP I/O manipulation
			 */
			class http
			{
			  private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				http(http &fd);

			  public:

				/**
				 * constructor
				 */
				http();

				/**
				 * destructor
				 */
				virtual ~http();

#ifdef OPENSSL_EXT
				/**
				 * set certificates information
				 * @param certs defines certificates information
				 */
				virtual void setSertificates(const io::ssl::__certificates__ &certs);
#endif

				dodoMap<short, dodoString> requestHeaders; ///< headers that will be sent with request[see httpRequestHeaderEnum]

				/**
				 * @param url defines Url
				 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
				 */
				virtual void setUrl(const dodoString &url);

				/**
				 * @param host defines proxy ip
				 * @param port defines proxy port
				 * @param user defines proxy user for authentication
				 * @param password defines proxy password for authentication
				 */
				virtual void setProxy(const dodoString &host,
									  unsigned int     port = 3128,
									  const dodoString &user = __dodostring__,
									  const dodoString &password = __dodostring__);

				/**
				 * set cookies for the request
				 * @param cookies defines cookies
				 */
				virtual void setCookies(const dodoStringMap &cookies);

				/**
				 * perform GET request
				 * @return server response
				 * @param url defines Url
				 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
				 */
				virtual __httpResponse__ GET(const dodoString &url);

				/**
				 * perform GET request
				 */
				virtual __httpResponse__ GET();

				/**
				 * perform POST request
				 * @return server response
				 * @param url defines Url
				 * @param data defines POST data
				 * @param type defines content type of the POST request
				 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
				 */
				virtual __httpResponse__ POST(const dodoString &url,
											const dodoString &data,
											const dodoString &type);

				/**
				 * perform POST request
				 * @param data defines POST data
				 * @param type defines content type of the POST request
				 */
				virtual __httpResponse__ POST(const dodoString &data,
											const dodoString &type);

				/**
				 * perform POST request
				 * @return server response
				 * @param url defines Url
				 * @param arguments defines request arguments
				 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
				 */
				virtual __httpResponse__ POST(const dodoString    &url,
											const dodoStringMap &arguments);

				/**
				 * perform POST request
				 * @param arguments defines request arguments
				 */
				virtual __httpResponse__ POST(const dodoStringMap &arguments);

				/**
				 * perform POST request
				 * @return server response
				 * @param url defines Url
				 * @param arguments defines request arguments
				 * @param files defines files for POST request
				 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
				 */
				virtual __httpResponse__ POST(const dodoString &url,
											const dodoStringMap &arguments,
											const dodoMap<dodoString, __httpPostFile__> &files);

				/**
				 * perform POST request
				 * @param arguments defines request arguments
				 * @param files defines files for POST request
				 */
				virtual __httpResponse__ POST(const dodoStringMap &arguments,
											const dodoMap<dodoString, __httpPostFile__> &files);

				/**
				 * set HTTP authentification information
				 * @param user defines user name for HTTP authentification
				 * @param password defines password for HTTP authentification
				 */
				virtual void setAuthInfo(const dodoString &user,
										 const dodoString &password);

				bool followRedirection;     ///< if true follow the `Location` header; true by default

				bool cacheAuthentification; ///< if true cache authentification HTTP and proxy info; true by default

			  private:

				/**
				 * @return cookie parse from the header
				 * @param header defines `Set-Cookie` value
				 */
				virtual cgi::__cgiCookie__ parseCookie(const dodoString &header);

				/**
				 * @enum getContentStatusEnum defines status of getContent routine
				 */
				enum getContentStatusEnum
				{
					GETCONTENTSTATUS_NORMAL,
					GETCONTENTSTATUS_REDIRECT,
					GETCONTENTSTATUS_WWWBASICAUTH,
					GETCONTENTSTATUS_WWWDIGESTAUTH,
					GETCONTENTSTATUS_WWWPROXYBASICAUTH,
					GETCONTENTSTATUS_WWWPROXYDIGESTAUTH,
					GETCONTENTSTATUS_PROXYBASICAUTH,
					GETCONTENTSTATUS_PROXYDIGESTAUTH,
				};

				/**
				 * get response data and fetch headers
				 * @return flag of redirection/authentification status
				 * @param ex defines network connection
				 * @param response defines response data to be filled
				 */
				virtual short getContent(exchange   *ex,
										 __httpResponse__ &response);

				/**
				 * get proxy CONNECT response data
				 * @return flag of authentification status
				 * @param data defines buffer to store response
				 * @param ex defines network connection
				 * @param response defines response data to be filled
				 */
				virtual short getProxyConnectResponse(exchange *ex,
													  __httpResponse__ &response);

				unsigned short authTries;                                                           ///< autherization request counter

				static const dodoString requestHeaderStatements[HTTP_REQUESTHEADERSTATEMENTS];      ///< HTTP request headers[see httpRequestHeaderEnum]
				static const dodoString responseHeaderStatements[HTTP_RESPONSEHEADERSTATEMENTS];    ///< HTTP response headers[see httpResponseHeaderEnum]

				tools::__url__ urlComponents;                                                         ///< HTTP URL components
				dodoString urlQuery;                                                                ///< HTTP URL query
				dodoString urlBasePath;                                                             ///< HTTP URL base path

				/**
				 * @return status code from header
				 */
				virtual short getStatusCode(const dodoString &header);

				/**
				 * @return offset of HTTP body in data or zero if headers are not complete
				 * @param data defines response data
				 * @param headers defines data with headers
				 */
				virtual unsigned int extractHeaders(const dodoString &data,
													dodoString       &headers);

				/**
				 * fetch headers
				 * @param headers defines data with headers
				 * @param response defines response data to be filled
				 */
				virtual void getHeaders(const dodoString &headers,
										__httpResponse__ &response);

				/**
				 * @param responseHeader defines what header create[HTTP_REQUESTHEADER_AUTHORIZATION or HTTP_REQUESTHEADER_PROXYAUTHORIZATION]
				 * @param user defines user name
				 * @param password defines user password
				 */
				virtual void makeBasicAuth(short            responseHeader,
										   const dodoString &user,
										   const dodoString &password);

				/**
				 * @param requestHeader defines what header parse[HTTP_RESPONSEHEADER_WWWAUTHENTICATE or HTTP_RESPONSEHEADER_PROXYAUTHENTICATE]
				 * @param responseHeader defines what header create[HTTP_REQUESTHEADER_AUTHORIZATION or HTTP_REQUESTHEADER_PROXYAUTHORIZATION]
				 * @param method defines request method
				 * @param user defines user name
				 * @param password defines user password
				 * @param response defines response data to be filled
				 */
				virtual void makeDigestAuth(short            requestHeader,
											short            responseHeader,
											const dodoString &method,
											const dodoString &user,
											const dodoString &password,
											__httpResponse__ &response);

				/**
				 * clear authentification/cookies information
				 */
				virtual void clear();

				/**
				 * @enum schemeEnum defines URI scheme of connection
				 */
				enum schemeEnum
				{
					SCHEME_HTTP,

#ifdef OPENSSL_EXT
					SCHEME_HTTPS,
#endif
				};

				short scheme; ///< URI scheme of connection

				/**
				 * @enum proxyAuthTypeEnum defines proxy authentification type
				 */
				enum proxyAuthTypeEnum
				{
					PROXYAUTHTYPE_NONE,
					PROXYAUTHTYPE_BASIC,
					PROXYAUTHTYPE_DIGEST
				};

				/**
				 * @struct __proxyAuthInfo__ defines proxy authentication information
				 */
				struct __proxyAuthInfo__
				{
					dodoString   user;          ///< user name
					dodoString   password;      ///< user password
					dodoString   host;          ///< proxy ip address
					unsigned int port;          ///< proxy port
					short        authType;      ///< type of proxy authentication[see proxyAuthTypeEnum]
				};

				__proxyAuthInfo__ proxyAuthInfo;  ///< proxy authentication information

				dodoStringMap httpAuth;         ///< cached HTTP auth info

#ifdef OPENSSL_EXT
				io::ssl::__certificates__	certs;  ///< SSL certificates
				bool						certsSet;
#endif
			};
		};
	};
};

#endif
