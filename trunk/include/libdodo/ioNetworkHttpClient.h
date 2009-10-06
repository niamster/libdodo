/***************************************************************************
 *            ioNetworkHttpClient.h
 *
 *  Tue Oct 8 2005
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

#include <libdodo/ioSsl.h>
#include <libdodo/types.h>

namespace dodo {
	namespace cgi {
		struct cookie;
	};

	namespace io {
		namespace network {
			class exchange;

			namespace http {
				/**
				 * @enum http::requestHeaderEnum defines HTTP request headers
				 */
				enum requestHeaderEnum {
					REQUEST_HEADER_ACCEPT,                   ///< content-types that are acceptable
					REQUEST_HEADER_ACCEPTCHARSET,            ///< character sets that are acceptable
					REQUEST_HEADER_ACCEPTENCODING,           ///< acceptable encodings
					REQUEST_HEADER_ACCEPTLANGUAGE,           ///< acceptable languages for response
					REQUEST_HEADER_ACCEPTRANGES,             ///< allows the server to indicate its acceptance of range requests for a resource
					REQUEST_HEADER_AUTHORIZATION,            ///< authentication credentials for HTTP authentication
					REQUEST_HEADER_PROXYAUTHORIZATION,       ///< authentication credentials for HTTP proxy authentication
					REQUEST_HEADER_CONNECTION,               ///< what type of connection the user-agent would prefer
					REQUEST_HEADER_DATE,                     ///< the date and time that the message was sent
					REQUEST_HEADER_IFMODIFIEDSINCE,          ///< allows a 304 Not Modified to be returned
					REQUEST_HEADER_USERAGENT,                ///< the user agent string of the user agent
					REQUEST_HEADER_COOKIE,                   ///< the user agent string of the user agent

					REQUEST_HEADER_ENUMSIZE
				};

				/**
				 * @enum http::responseHeaderEnum defines HTTP response headers
				 */
				enum responseHeaderEnum {
					RESPONSE_HEADER_ACCEPTRANGES,            ///< what partial content range types this server supports
					RESPONSE_HEADER_AGE,                     ///< the age the object has been in a proxy cache in seconds
					RESPONSE_HEADER_ALLOW,                   ///< valid actions for a specified resource. To be used for a 405 Method not allowed
					RESPONSE_HEADER_CACHECONTROL,            ///< tells all caching mechanisms from server to client whether they may cache this object
					RESPONSE_HEADER_CONTENTENCODING,         ///< the type of encoding used on the data
					RESPONSE_HEADER_CONTENTLANGUAGE,         ///< the language the content is in
					RESPONSE_HEADER_CONTENTLENGTH,           ///< the length of the content in bytes
					RESPONSE_HEADER_CONTENTLOCATION,         ///< an alternate location for the returned data
					RESPONSE_HEADER_CONTENTDISPOSITION,      ///< an opportunity to raise a "File Download" dialog box for a known MIME type
					RESPONSE_HEADER_CONTENTMD5,              ///< an MD5 sum of the content of the response
					RESPONSE_HEADER_CONTENTRANGE,            ///< where in a full body message this partial message belongs
					RESPONSE_HEADER_CONTENTTYPE,             ///< the mime type of this content
					RESPONSE_HEADER_DATE,                    ///< the date and time that the message was sent
					RESPONSE_HEADER_LASTMODIFIED,            ///< the last modified date for the requested object
					RESPONSE_HEADER_LOCATION,                ///< used in redirection
					RESPONSE_HEADER_SERVER,                  ///< a name for the server
					RESPONSE_HEADER_TRANSFERENCODING,        ///< encoding of the response
					RESPONSE_HEADER_WWWAUTHENTICATE,         ///< authentication request
					RESPONSE_HEADER_PROXYAUTHENTICATE,       ///< proxy authentication request
					RESPONSE_HEADER_XPOWEREDBY,              ///< cgi provider

					RESPONSE_HEADER_ENUMSIZE
				};

				/**
				 * @class response
				 * @brief defines HTTP response
				 */
				class response {
				  public:

					/**
					 * constructor
					 */
					response();

					dodoMap<short, dodoString> headers;     ///< response headers, @see http::responseHeaderEnum
					dodoArray<cgi::cookie> cookies;         ///< cookies from the server
					dodoString data;                        ///< response data
					short code;                             ///< response code
					bool redirected;                        ///< true if redirection was performeed
				};

				/**
				 * @class file
				 * @brief defines file for POST request
				 */
				class file {
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
				class client {
				  private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					client(client &);

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
					~client();

#ifdef OPENSSL_EXT
					/**
					 * set certificates information
					 * @param certs defines certificates information
					 */
					void setSertificates(const io::ssl::__certificates__ &certs) const;
#endif

					mutable dodoMap<short, dodoString> requestHeaders; ///< headers that will be sent with request, @see http::requestHeaderEnum

					/**
					 * @param url defines Url
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					void setUrl(const dodoString &url) const;

					/**
					 * @param host defines proxy ip
					 * @param port defines proxy port
					 * @param user defines proxy user for authentication
					 * @param password defines proxy password for authentication
					 */
					void setProxy(const dodoString &host,
										  unsigned int     port = 3128,
										  const dodoString &user = __dodostring__,
										  const dodoString &password = __dodostring__) const;

					/**
					 * set cookies for the request
					 * @param cookies defines cookies
					 */
					void setCookies(const dodoStringMap &cookies) const;

					/**
					 * perform GET request
					 * @return server response
					 * @param url defines Url
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					response GET(const dodoString &url) const;

					/**
					 * perform GET request
					 */
					response GET() const;

					/**
					 * perform POST request
					 * @return server response
					 * @param url defines Url
					 * @param data defines POST data
					 * @param type defines content type of the POST request
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					response POST(const dodoString &url,
										  const dodoString &data,
										  const dodoString &type) const;

					/**
					 * perform POST request
					 * @param data defines POST data
					 * @param type defines content type of the POST request
					 */
					response POST(const dodoString &data,
										  const dodoString &type) const;

					/**
					 * perform POST request
					 * @return server response
					 * @param url defines Url
					 * @param arguments defines request arguments
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					response POST(const dodoString    &url,
										  const dodoStringMap &arguments) const;

					/**
					 * perform POST request
					 * @param arguments defines request arguments
					 */
					response POST(const dodoStringMap &arguments) const;

					/**
					 * perform POST request
					 * @return server response
					 * @param url defines Url
					 * @param arguments defines request arguments
					 * @param files defines files for POST request
					 * @note authentification may be as a part of URL or defined by setHttpAuthentification method
					 */
					response POST(const dodoString &url,
										  const dodoStringMap &arguments,
										  const dodoMap<dodoString, file> &files) const;

					/**
					 * perform POST request
					 * @param arguments defines request arguments
					 * @param files defines files for POST request
					 */
					response POST(const dodoStringMap &arguments,
										  const dodoMap<dodoString, file> &files) const;

					/**
					 * set HTTP authentification information
					 * @param user defines user name for HTTP authentification
					 * @param password defines password for HTTP authentification
					 */
					void setAuth(const dodoString &user,
								 const dodoString &password) const;

					bool followRedirection;     ///< if true follow the `Location` header; true by default

					bool cacheAuthentification; ///< if true cache authentification HTTP and proxy info; true by default

				  private:

					/**
					 * @return cookie parse from the header
					 * @param header defines `Set-Cookie` value
					 */
					cgi::cookie parseCookie(const dodoString &header) const;

					/**
					 * @enum getContentStatusEnum defines status of getContent routine
					 */
					enum getContentStatusEnum {
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
					short getContent(exchange *ex,
											 response &response) const;

					/**
					 * get proxy CONNECT response data
					 * @return flag of authentification status
					 * @param data defines buffer to store response
					 * @param ex defines network connection
					 * @param response defines response data to be filled
					 */
					short getProxyConnectResponse(exchange *ex,
														  response &response) const;

					mutable unsigned short authTries;                                                               ///< autherization request counter

					static const dodoString requestHeaderStatements[REQUEST_HEADER_ENUMSIZE];                  ///< HTTP request headers, @see http::requestHeaderEnum
					static const dodoString responseHeaderStatements[RESPONSE_HEADER_ENUMSIZE];                ///< HTTP response headers, @see http::responseHeaderEnum

					mutable __url__ urlComponents;                                                           ///< HTTP URL components
					mutable dodoString urlQuery;                                                                    ///< HTTP URL query
					mutable dodoString urlBasePath;                                                                 ///< HTTP URL base path

					/**
					 * @return status code from header
					 */
					short getStatusCode(const dodoString &header) const;

					/**
					 * @return offset of HTTP body in data or zero if headers are not complete
					 * @param data defines response data
					 * @param headers defines data with headers
					 */
					unsigned int extractHeaders(const dodoString &data,
														dodoString       &headers) const;

					/**
					 * fetch headers
					 * @param headers defines data with headers
					 * @param response defines response data to be filled
					 */
					void getHeaders(const dodoString &headers,
											response         &response) const;

					/**
					 * @param responseHeader defines what header create[REQUEST_HEADER_AUTHORIZATION or REQUEST_HEADER_PROXYAUTHORIZATION]
					 * @param user defines user name
 					 * @param password defines user password
					 */
					void makeBasicAuth(short            responseHeader,
											   const dodoString &user,
											   const dodoString &password) const;

					/**
					 * @param requestHeader defines what header parse[RESPONSE_HEADER_WWWAUTHENTICATE or RESPONSE_HEADER_PROXYAUTHENTICATE]
					 * @param responseHeader defines what header create[REQUEST_HEADER_AUTHORIZATION or REQUEST_HEADER_PROXYAUTHORIZATION]
					 * @param method defines request method
					 * @param user defines user name
					 * @param password defines user password
					 * @param response defines response data to be filled
					 */
					void makeDigestAuth(short            requestHeader,
												short            responseHeader,
												const dodoString &method,
												const dodoString &user,
												const dodoString &password,
												response         &response) const;

					/**
					 * clear authentification/cookies information
					 */
					void clear() const;

					/**
					 * @enum schemeEnum defines URI scheme of connection
					 */
					enum schemeEnum {
						SCHEME_HTTP,

#ifdef OPENSSL_EXT
						SCHEME_HTTPS,
#endif
					};

					mutable short scheme;                       ///< URI scheme of connection

					/**
					 * @enum proxyAuthEnum defines proxy authentification type
					 */
					enum proxyAuthEnum {
						PROXY_AUTH_NONE,
						PROXY_AUTH_BASIC,
						PROXY_AUTH_DIGEST
					};

					/**
					 * @struct __proxy_auth__
					 * @brief defines proxy authentication information
					 */
					struct __proxy_auth__ {
						dodoString   user;                      ///< user name
						dodoString   password;                  ///< user password
						dodoString   host;                      ///< proxy ip address
						unsigned int port;                      ///< proxy port
						short        authType;                  ///< type of proxy authentication, @see http::proxyAuthEnum
					};

					mutable __proxy_auth__ proxyAuthInfo;    ///< proxy authentication information

					mutable dodoStringMap httpAuth;             ///< cached HTTP auth info

#ifdef OPENSSL_EXT
					mutable io::ssl::__certificates__ certs;    ///< SSL certificates
					mutable bool certsSet;
#endif
				};
			};
		};
	};
};
#endif
