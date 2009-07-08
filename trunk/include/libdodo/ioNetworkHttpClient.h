/***************************************************************************
 *            ioNetworkHttpClient.h
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

#ifndef _IONETWORKHTTPCLIENT_H_
#define _IONETWORKHTTPCLIENT_H_ 1

#include <libdodo/directives.h>

#include <libdodo/toolsCode.h>
#include <libdodo/ioSsl.h>
#include <libdodo/types.h>

namespace dodo
{
	namespace cgi
	{
		struct cookie;
	};

	namespace io
	{
		namespace network
		{
			class exchange;

			namespace http
			{

#define HTTP_REQUESTHEADERSTATEMENTS 12

				/**
				 * @enum requestHeaderEnum defines HTTP request headers
				 */
				enum requestHeaderEnum
				{
					REQUESTHEADER_ACCEPT,              ///< content-types that are acceptable
					REQUESTHEADER_ACCEPTCHARSET,       ///< character sets that are acceptable
					REQUESTHEADER_ACCEPTENCODING,      ///< acceptable encodings
					REQUESTHEADER_ACCEPTLANGUAGE,      ///< acceptable languages for response
					REQUESTHEADER_ACCEPTRANGES,        ///< allows the server to indicate its acceptance of range requests for a resource
					REQUESTHEADER_AUTHORIZATION,       ///< authentication credentials for HTTP authentication
					REQUESTHEADER_PROXYAUTHORIZATION,  ///< authentication credentials for HTTP proxy authentication
					REQUESTHEADER_CONNECTION,          ///< what type of connection the user-agent would prefer
					REQUESTHEADER_DATE,                ///< the date and time that the message was sent
					REQUESTHEADER_IFMODIFIEDSINCE,     ///< allows a 304 Not Modified to be returned
					REQUESTHEADER_USERAGENT,           ///< the user agent string of the user agent
					REQUESTHEADER_COOKIE,              ///< the user agent string of the user agent
				};

#define HTTP_RESPONSEHEADERSTATEMENTS 20

				/**
				 * @enum responseHeaderEnum defines HTTP response headers
				 */
				enum responseHeaderEnum
				{
					RESPONSEHEADER_ACCEPTRANGES,       ///< what partial content range types this server supports
					RESPONSEHEADER_AGE,                ///< the age the object has been in a proxy cache in seconds
					RESPONSEHEADER_ALLOW,              ///< valid actions for a specified resource. To be used for a 405 Method not allowed
					RESPONSEHEADER_CACHECONTROL,       ///< tells all caching mechanisms from server to client whether they may cache this object
					RESPONSEHEADER_CONTENTENCODING,    ///< the type of encoding used on the data
					RESPONSEHEADER_CONTENTLANGUAGE,    ///< the language the content is in
					RESPONSEHEADER_CONTENTLENGTH,      ///< the length of the content in bytes
					RESPONSEHEADER_CONTENTLOCATION,    ///< an alternate location for the returned data
					RESPONSEHEADER_CONTENTDISPOSITION, ///< an opportunity to raise a "File Download" dialog box for a known MIME type
					RESPONSEHEADER_CONTENTMD5,         ///< an MD5 sum of the content of the response
					RESPONSEHEADER_CONTENTRANGE,       ///< where in a full body message this partial message belongs
					RESPONSEHEADER_CONTENTTYPE,        ///< the mime type of this content
					RESPONSEHEADER_DATE,               ///< the date and time that the message was sent
					RESPONSEHEADER_LASTMODIFIED,       ///< the last modified date for the requested object
					RESPONSEHEADER_LOCATION,           ///< used in redirection
					RESPONSEHEADER_SERVER,             ///< a name for the server
					RESPONSEHEADER_TRANSFERENCODING,   ///< encoding of the response
					RESPONSEHEADER_WWWAUTHENTICATE,    ///< authentication request
					RESPONSEHEADER_PROXYAUTHENTICATE,  ///< proxy authentication request
					RESPONSEHEADER_XPOWEREDBY,         ///< cgi provider
				};

				/**
				 * @class response
				 * @brief defines HTTP response
				 */
				class response
				{
				  public:
					/**
					 * constructor
					 */
					response();

					dodoMap<short, dodoString>  headers;    ///< response headers[see responseHeaderEnum]
					dodoArray<cgi::cookie> cookies;    ///< cookies from the server
					dodoString                  data;       ///< response data
					short                       code;       ///< response code
					bool                        redirected; ///< true if redirection was performeed
				};

				/**
				 * @class file
				 * @brief defines file for POST request
				 */
				class file
				{
				  public:
					/**
					 * constructor
					 * @param path defines path to file
					 * @param mime defines mimetype of the file
					 */
					file(const dodoString path,
						 const dodoString mime);

					/**
					 * constructor
					 */
					file();

					dodoString path;                        ///< path to file
					dodoString mime;                        ///< mimetype of the file
				};

				/**
				 * @class client
				 * @brief provides HTTP I/O manipulation
				 */
				class client
				{
				  private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					client(client &fd);

				  public:

					/**
					 * constructor
					 */
					client();

					/**
					 * constructor
					 * @param url defines Url
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					client(const dodoString &url);

					/**
					 * destructor
					 */
					virtual ~client();

#ifdef OPENSSL_EXT
					/**
					 * set certificates information
					 * @param certs defines certificates information
					 */
					virtual void setSertificates(const io::ssl::__certificates__ &certs) const;
#endif

					mutable dodoMap<short, dodoString> requestHeaders; ///< headers that will be sent with request[see requestHeaderEnum]

					/**
					 * @param url defines Url
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					virtual void setUrl(const dodoString &url) const;

					/**
					 * @param host defines proxy ip
					 * @param port defines proxy port
					 * @param user defines proxy user for authentication
					 * @param password defines proxy password for authentication
					 */
					virtual void setProxy(const dodoString &host,
										  unsigned int     port = 3128,
										  const dodoString &user = __dodostring__,
										  const dodoString &password = __dodostring__) const;

					/**
					 * set cookies for the request
					 * @param cookies defines cookies
					 */
					virtual void setCookies(const dodoStringMap &cookies) const;

					/**
					 * perform GET request
					 * @return server response
					 * @param url defines Url
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					virtual response GET(const dodoString &url) const;

					/**
					 * perform GET request
					 */
					virtual response GET() const;

					/**
					 * perform POST request
					 * @return server response
					 * @param url defines Url
					 * @param data defines POST data
					 * @param type defines content type of the POST request
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					virtual response POST(const dodoString &url,
												const dodoString &data,
												const dodoString &type) const;

					/**
					 * perform POST request
					 * @param data defines POST data
					 * @param type defines content type of the POST request
					 */
					virtual response POST(const dodoString &data,
												const dodoString &type) const;

					/**
					 * perform POST request
					 * @return server response
					 * @param url defines Url
					 * @param arguments defines request arguments
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					virtual response POST(const dodoString    &url,
												const dodoStringMap &arguments) const;

					/**
					 * perform POST request
					 * @param arguments defines request arguments
					 */
					virtual response POST(const dodoStringMap &arguments) const;

					/**
					 * perform POST request
					 * @return server response
					 * @param url defines Url
					 * @param arguments defines request arguments
					 * @param files defines files for POST request
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					virtual response POST(const dodoString &url,
												const dodoStringMap &arguments,
												const dodoMap<dodoString, file> &files) const;

					/**
					 * perform POST request
					 * @param arguments defines request arguments
					 * @param files defines files for POST request
					 */
					virtual response POST(const dodoStringMap &arguments,
												const dodoMap<dodoString, file> &files) const;

					/**
					 * set HTTP authentification information
					 * @param user defines user name for HTTP authentification
					 * @param password defines password for HTTP authentification
					 */
					virtual void setAuthInfo(const dodoString &user,
											 const dodoString &password) const;

					bool followRedirection;     ///< if true follow the `Location` header; true by default

					bool cacheAuthentification; ///< if true cache authentification HTTP and proxy info; true by default

				  private:

					/**
					 * @return cookie parse from the header
					 * @param header defines `Set-Cookie` value
					 */
					virtual cgi::cookie parseCookie(const dodoString &header) const;

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
											 response &response) const;

					/**
					 * get proxy CONNECT response data
					 * @return flag of authentification status
					 * @param data defines buffer to store response
					 * @param ex defines network connection
					 * @param response defines response data to be filled
					 */
					virtual short getProxyConnectResponse(exchange *ex,
														  response &response) const;

					mutable unsigned short authTries;                                                           ///< autherization request counter

					static const dodoString requestHeaderStatements[HTTP_REQUESTHEADERSTATEMENTS];      ///< HTTP request headers[see requestHeaderEnum]
					static const dodoString responseHeaderStatements[HTTP_RESPONSEHEADERSTATEMENTS];    ///< HTTP response headers[see responseHeaderEnum]

					mutable tools::__url__ urlComponents;                                                         ///< HTTP URL components
					mutable dodoString urlQuery;                                                                ///< HTTP URL query
					mutable dodoString urlBasePath;                                                             ///< HTTP URL base path

					/**
					 * @return status code from header
					 */
					virtual short getStatusCode(const dodoString &header) const;

					/**
					 * @return offset of HTTP body in data or zero if headers are not complete
					 * @param data defines response data
					 * @param headers defines data with headers
					 */
					virtual unsigned int extractHeaders(const dodoString &data,
														dodoString       &headers) const;

					/**
					 * fetch headers
					 * @param headers defines data with headers
					 * @param response defines response data to be filled
					 */
					virtual void getHeaders(const dodoString &headers,
											response &response) const;

					/**
					 * @param responseHeader defines what header create[REQUESTHEADER_AUTHORIZATION or REQUESTHEADER_PROXYAUTHORIZATION]
					 * @param user defines user name
					 * @param password defines user password
					 */
					virtual void makeBasicAuth(short            responseHeader,
											   const dodoString &user,
											   const dodoString &password) const;

					/**
					 * @param requestHeader defines what header parse[RESPONSEHEADER_WWWAUTHENTICATE or RESPONSEHEADER_PROXYAUTHENTICATE]
					 * @param responseHeader defines what header create[REQUESTHEADER_AUTHORIZATION or REQUESTHEADER_PROXYAUTHORIZATION]
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
												response &response) const;

					/**
					 * clear authentification/cookies information
					 */
					virtual void clear() const;

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

					mutable short scheme; ///< URI scheme of connection

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

					mutable __proxyAuthInfo__ proxyAuthInfo;  ///< proxy authentication information

					mutable dodoStringMap httpAuth;         ///< cached HTTP auth info

#ifdef OPENSSL_EXT
					mutable io::ssl::__certificates__	certs;  ///< SSL certificates
					mutable bool						certsSet;
#endif
				};
			};
		};
	};
};

#endif
