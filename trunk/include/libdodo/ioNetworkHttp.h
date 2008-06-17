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
#define _IONETWORKHTTP_H_

#include <libdodo/directives.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/ioNetworkClient.h>
#include <libdodo/ioNetworkSslClient.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/ioNetworkHttpEx.h>
#include <libdodo/types.h>
#include <libdodo/regexp.h>
#include <libdodo/cgiServer.h>

namespace dodo
{
	namespace io
	{
		namespace network
		{
#define HTTP_REQUESTHEADERSTATEMENTS 12

			/**
			 * @enum httpRequestHeaderEnum defines HTTP request headers
			 */
			enum httpRequestHeaderEnum
			{
				HTTP_REQUESTHEADER_ACCEPT,                              ///< content-types that are acceptable
				HTTP_REQUESTHEADER_ACCEPTCHARSET,                       ///< character sets that are acceptable
				HTTP_REQUESTHEADER_ACCEPTENCODING,                      ///< acceptable encodings
				HTTP_REQUESTHEADER_ACCEPTLANGUAGE,                      ///< acceptable languages for response
				HTTP_REQUESTHEADER_ACCEPTRANGES,                        ///< allows the server to indicate its acceptance of range requests for a resource
				HTTP_REQUESTHEADER_AUTHORIZATION,                       ///< authentication credentials for HTTP authentication
				HTTP_REQUESTHEADER_PROXYAUTHORIZATION,                  ///< authentication credentials for HTTP proxy authentication
				HTTP_REQUESTHEADER_CONNECTION,                          ///< what type of connection the user-agent would prefer
				HTTP_REQUESTHEADER_DATE,                                ///< the date and time that the message was sent
				HTTP_REQUESTHEADER_IFMODIFIEDSINCE,                     ///< allows a 304 Not Modified to be returned
				HTTP_REQUESTHEADER_USERAGENT,                           ///< the user agent string of the user agent
				HTTP_REQUESTHEADER_COOKIE,                              ///< the user agent string of the user agent
			};

#define HTTP_RESPONSEHEADERSTATEMENTS 19

			/**
			 * @enum httpResponseHeaderEnum defines HTTP response headers
			 */
			enum httpResponseHeaderEnum
			{
				HTTP_RESPONSEHEADER_ACCEPTRANGES,                       ///< what partial content range types this server supports
				HTTP_RESPONSEHEADER_AGE,                                ///< the age the object has been in a proxy cache in seconds
				HTTP_RESPONSEHEADER_ALLOW,                              ///< valid actions for a specified resource. To be used for a 405 Method not allowed
				HTTP_RESPONSEHEADER_CACHECONTROL,                       ///< tells all caching mechanisms from server to client whether they may cache this object
				HTTP_RESPONSEHEADER_CONTENTENCODING,                    ///< the type of encoding used on the data
				HTTP_RESPONSEHEADER_CONTENTLANGUAGE,                    ///< the language the content is in
				HTTP_RESPONSEHEADER_CONTENTLENGTH,                      ///< the length of the content in bytes
				HTTP_RESPONSEHEADER_CONTENTLOCATION,                    ///< an alternate location for the returned data
				HTTP_RESPONSEHEADER_CONTENTDISPOSITION,                 ///< an opportunity to raise a "File Download" dialog box for a known MIME type
				HTTP_RESPONSEHEADER_CONTENTMD5,                         ///< an MD5 sum of the content of the response
				HTTP_RESPONSEHEADER_CONTENTRANGE,                       ///< where in a full body message this partial message belongs
				HTTP_RESPONSEHEADER_CONTENTTYPE,                        ///< the mime type of this content
				HTTP_RESPONSEHEADER_DATE,                               ///< the date and time that the message was sent
				HTTP_RESPONSEHEADER_LASTMODIFIED,                       ///< the last modified date for the requested object
				HTTP_RESPONSEHEADER_LOCATION,                           ///< used in redirection
				HTTP_RESPONSEHEADER_SERVER,                             ///< a name for the server
				HTTP_RESPONSEHEADER_WWWAUTHENTICATE,                    ///< authentication request
				HTTP_RESPONSEHEADER_PROXYAUTHENTICATE,                  ///< proxy authentication request
				HTTP_RESPONSEHEADER_XPOWEREDBY,                         ///< cgi provider
			};

			/**
			 * @struct __httpResponse defines HTTP response
			 */
			struct __httpResponse
			{
				/**
				 * constructor
				 */
				__httpResponse();

				dodoMap<short, dodoString> headers;                     ///< response headers[see httpResponseHeaderEnum]
				dodoArray<cgi::__serverCookie> cookies;                 ///< cookies from the server
				dodoString data;                                        ///< response data
				short code;                                             ///< response code
				bool redirected;                                        ///< true if redirection was performeed
			};

			/**
			 * @class http provides disk I/O manipulations
			 * FIXME: fetch with HTTP/1.1 for non-proxy mode and correctly parse chanked requests
			 */
			class http
			{
				private:

					/**
					 * copy constructor
					 * to prevent copying
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
					
					/**
					 * set certificates information
					 * @param certs defines certificates information 
					 */
					virtual void setSertificates(const ssl::__certificates &certs);

					dodoMap<short, dodoString> requestHeaders;                     ///< headers that will be sent with request[see httpRequestHeaderEnum]

					/**
					 * @return server response
					 */
					virtual __httpResponse getResponse();

					/**
					 * @param url defines Url
					 */
					virtual void setUrl(const dodoString &url);

					/**
					 * @param host defines proxy ip
					 * @param port defines proxy port
					 * @param user defines proxy user for authentication
					 * @param password defines proxy password for authentication
					 */
					virtual void setProxyInformation(const dodoString &host, unsigned int port = 3128, const dodoString &user = __dodostring__, const dodoString &password = __dodostring__);

					/**
					 * disable proxy usage
					 */
					virtual void disableProxy();

					/**
					 * set cookies for the request
					 * @param cookies defines cookies
					 */
					virtual void setCookies(const dodoStringMap &cookies);

					/**
					 * perform GET request
					 * @return server response
					 * @param url defines Url
					 */
					virtual __httpResponse GET(const dodoString &url);

					/**
					 * perform GET request
					 */
					virtual void GET();

					/**
					 * perform POST request
					 * @return server response
					 * @param url defines Url
					 * @param data defines POST data
					 * @param type defines content type of the POST request
					 */
					virtual __httpResponse POST(const dodoString &url, const dodoString &data, const dodoString &type);

					/**
					 * perform POST request
					 * @param data defines POST data
					 * @param type defines content type of the POST request
					 */
					virtual void POST(const dodoString &data, const dodoString &type);

					/**
					 * perform POST request
					 * @return server response
					 * @param url defines Url
					 * @param arguments defines request arguments
					 */
					virtual __httpResponse POST(const dodoString &url, const dodoStringMap &arguments);

					/**
					 * perform POST request
					 * @param arguments defines request arguments
					 */
					virtual void POST(const dodoStringMap &arguments);

					/**
					 * perform POST request
					 * @return server response
					 * @param url defines Url
					 * @param arguments defines request arguments
					 * @param files defines path to files
					 */
					virtual __httpResponse POST(const dodoString &url, const dodoStringMap &arguments, const dodoStringMap &files);

					/**
					 * perform POST request
					 * @param arguments defines request arguments
					 * @param files defines path to files
					 */
					virtual void POST(const dodoStringMap &arguments, const dodoStringMap &files);

					bool followRedirection;                     ///< if true follow the `Location` header; true by default

				private:

					/**
					 * @return cookie parse from the header
					 * @param header defines `Set-Cookie` value
					 */
					virtual cgi::__serverCookie parseCookie(const dodoString &header);

					/**
					 * @enum getContentStatus defines status of getContent routine
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
					 * @return true if redirection is going to be performed
					 * @param data defines buffer to store response
					 * @param ex defines network connection
					 */
					virtual short getContent(dodoString &data, exchange &ex);

					unsigned short authTries;                                                                                   ///< autherization request counter

					static const dodoString requestHeaderStatements[HTTP_REQUESTHEADERSTATEMENTS];                              ///< HTTP request headers[see httpRequestHeaderEnum]
					static const dodoString responseHeaderStatements[HTTP_RESPONSEHEADERSTATEMENTS];                            ///< HTTP response headers[see httpResponseHeaderEnum]

					regexp httpStatusRE;                                                                                        ///< parser for HTTP response status code

					__httpResponse response;                                                                                    ///< HTTP response data
					tools::__url urlComponents;                                                                                 ///< HTTP Url
					dodoString url;

					/**
					 * @return true if no more headers should be processed
					 * @param data defines response data
					 * @param headers defines data with headers
					 */
					virtual bool extractHeaders(const dodoString &data, dodoString &headers);

					/**
					 * fetch headers
					 * @param headers defines data with headers
					 */
					virtual void getHeaders(const dodoString &headers);

					/**
					 * @return trimmed string
					 * @param data defines string to trim
					 * @note uses trimSymbols to trim
					 */
					static dodoString trim(const dodoString &data);

					/**
					 * @param responseHeader defines what header create[HTTP_REQUESTHEADER_AUTHORIZATION or HTTP_REQUESTHEADER_PROXYAUTHORIZATION]
					 * @param user defines user name
					 * @param password defines user password
					 */
					virtual void makeBasicAuth(short responseHeader, const dodoString &user, const dodoString &password);

					/**
					 * @param requestHeader defines what header parse[HTTP_RESPONSEHEADER_WWWAUTHENTICATE or HTTP_RESPONSEHEADER_PROXYAUTHENTICATE]
					 * @param responseHeader defines what header create[HTTP_REQUESTHEADER_AUTHORIZATION or HTTP_REQUESTHEADER_PROXYAUTHORIZATION]
					 * @param method defines request method
					 * @param user defines user name
					 * @param password defines user password
					 */
					virtual void makeDigestAuth(short requestHeader, short responseHeader, const dodoString &method, const dodoString &user, const dodoString &password);

					/**
					 * @struct __proxyAuthInfo defines proxy authentication information
					 */
					struct __proxyAuthInfo
					{
						dodoString user;                                ///< user name
						dodoString password;                            ///< user password
						dodoString host;                                ///< proxy ip address
						unsigned int port;                              ///< proxy port
						bool enabled;                                   ///< if true proxy settings are enabled
						bool authRequired;                              ///< if true proxy authentication is required
					};

					__proxyAuthInfo proxyAuthInfo;                     ///< proxy authentication information
					
					ssl::__certificates certs;///< SSL certificates
			};
		};
	};

};

#endif
