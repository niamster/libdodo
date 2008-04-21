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

#ifndef _IONETWORKHTTP_H_
#define _IONETWORKHTTP_H_

#include <libdodo/directives.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libdodo/tools.h>
#include <libdodo/ioNetworkClient.h>
#include <libdodo/ioNetworkTools.h>
#include <libdodo/ioNetworkHttpEx.h>
#include <libdodo/types.h>
#include <libdodo/regexp.h>
#include <libdodo/cgi.h>

namespace dodo
{	
#define IONETWORKHTTP_REQUESTHEADERSTATEMENTS 12

	/**
	 * @enum ioNetworkHttpRequestHeaderEnum defines HTTP request headers
	 */
	enum ioNetworkHttpRequestHeaderEnum
	{
		IONETWORKHTTP_REQUESTHEADER_ACCEPT,///< content-types that are acceptable
		IONETWORKHTTP_REQUESTHEADER_ACCEPTCHARSET,///< character sets that are acceptable
		IONETWORKHTTP_REQUESTHEADER_ACCEPTENCODING,///< acceptable encodings
		IONETWORKHTTP_REQUESTHEADER_ACCEPTLANGUAGE,///< acceptable languages for response
		IONETWORKHTTP_REQUESTHEADER_ACCEPTRANGES,///< allows the server to indicate its acceptance of range requests for a resource
		IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION,///< authentication credentials for HTTP authentication
		IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION,///< authentication credentials for HTTP proxy authentication
		IONETWORKHTTP_REQUESTHEADER_CONNECTION,///< what type of connection the user-agent would prefer
		IONETWORKHTTP_REQUESTHEADER_DATE,///< the date and time that the message was sent
		IONETWORKHTTP_REQUESTHEADER_IFMODIFIEDSINCE,///< allows a 304 Not Modified to be returned
		IONETWORKHTTP_REQUESTHEADER_USERAGENT,///< the user agent string of the user agent
		IONETWORKHTTP_REQUESTHEADER_COOKIE,///< the user agent string of the user agent
	};

#define IONETWORKHTTP_RESPONSEHEADERSTATEMENTS 19
	
	/**
	 * @enum ioNetworkHttpResponseHeaderEnum defines HTTP response headers
	 */
	enum ioNetworkHttpResponseHeaderEnum
	{
		IONETWORKHTTP_RESPONSEHEADER_ACCEPTRANGES,///< what partial content range types this server supports
		IONETWORKHTTP_RESPONSEHEADER_AGE,///< the age the object has been in a proxy cache in seconds
		IONETWORKHTTP_RESPONSEHEADER_ALLOW,///< valid actions for a specified resource. To be used for a 405 Method not allowed
		IONETWORKHTTP_RESPONSEHEADER_CACHECONTROL,///< tells all caching mechanisms from server to client whether they may cache this object
		IONETWORKHTTP_RESPONSEHEADER_CONTENTENCODING,///< the type of encoding used on the data
		IONETWORKHTTP_RESPONSEHEADER_CONTENTLANGUAGE,///< the language the content is in
		IONETWORKHTTP_RESPONSEHEADER_CONTENTLENGTH,///< the length of the content in bytes
		IONETWORKHTTP_RESPONSEHEADER_CONTENTLOCATION,///< an alternate location for the returned data
		IONETWORKHTTP_RESPONSEHEADER_CONTENTDISPOSITION,///< an opportunity to raise a "File Download" dialog box for a known MIME type
		IONETWORKHTTP_RESPONSEHEADER_CONTENTMD5,///< an MD5 sum of the content of the response
		IONETWORKHTTP_RESPONSEHEADER_CONTENTRANGE,///< where in a full body message this partial message belongs
		IONETWORKHTTP_RESPONSEHEADER_CONTENTTYPE,///< the mime type of this content
		IONETWORKHTTP_RESPONSEHEADER_DATE,///< the date and time that the message was sent
		IONETWORKHTTP_RESPONSEHEADER_LASTMODIFIED,///< the last modified date for the requested object
		IONETWORKHTTP_RESPONSEHEADER_LOCATION,///< used in redirection
		IONETWORKHTTP_RESPONSEHEADER_SERVER,///< a name for the server
		IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE,///< authentification request
		IONETWORKHTTP_RESPONSEHEADER_PROXYAUTHENTICATE,///< proxy authentification request
		IONETWORKHTTP_RESPONSEHEADER_XPOWEREDBY,///< cgi provider
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
		
		dodoMap<short, dodoString> headers; ///< response headers[see ioNetworkHttpResponseHeaderEnum]
		dodoArray<__cookie> cookies;///< cookies from the server
		dodoString data; ///< response data
		short code; ///< response code
		bool redirected;///< true if redirection was performeed
	};
	
	/**
	 * @class ioNetworkHttp provides disk I/O manipulations
	 */
	class ioNetworkHttp
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			ioNetworkHttp(ioNetworkHttp &fd);

		public:

			/**
			 * constructor
			 */
			ioNetworkHttp();

			/**
			 * destructor
			 */
			virtual ~ioNetworkHttp();

			dodoMap<short, dodoString> requestHeaders; ///< headers that will be sent with request[see ioNetworkHttpRequestHeaderEnum]
			
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
			 * @param user defines proxy user for authentification
			 * @param password defines proxy password for authentification
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
			
			bool followRedirection;///< if true follow the `Location` header; true by default

		private:
			
			/**
			 * @return cookie parse from the header
			 * @param header defines `Set-Cookie` value
			 */
			virtual __cookie parseCookie(const dodoString &header);
			
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
			virtual short getContent(dodoString &data, ioNetworkExchange &ex);
			
			unsigned short authTries;///< autherization request counter
			
			static const dodoString requestHeaderStatements[IONETWORKHTTP_REQUESTHEADERSTATEMENTS];///< HTTP request headers[see ioNetworkHttpRequestHeaderEnum]
			static const dodoString responseHeaderStatements[IONETWORKHTTP_RESPONSEHEADERSTATEMENTS];///< HTTP response headers[see ioNetworkHttpResponseHeaderEnum]
			
			regexp httpStatusRE;///< parser for HTTP response status code
			
			__httpResponse response;///< HTTP response data
			__url urlComponents;///< HTTP Url
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
			 * @param responseHeader defines what header create[IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION or IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION]
			 * @param user defines user name 
			 * @param password defines user password 
			 */
			virtual void makeBasicAuth(short responseHeader, const dodoString &user, const dodoString &password);
			
			/**
			 * @param requestHeader defines what header parse[IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE or IONETWORKHTTP_RESPONSEHEADER_PROXYAUTHENTICATE]
			 * @param responseHeader defines what header create[IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION or IONETWORKHTTP_REQUESTHEADER_PROXYAUTHORIZATION]
			 * @param method defines request method
			 * @param user defines user name 
			 * @param password defines user password 
			 */
			virtual void makeDigestAuth(short requestHeader, short responseHeader, const dodoString &method, const dodoString &user, const dodoString &password);
			
			/**
			 * @struct __proxyAuthInfo defines proxy authentification information
			 */
			struct __proxyAuthInfo
			{
				dodoString user;///< user name
				dodoString password;///< user password
				dodoString host;///< proxy ip address
				unsigned int port;///< proxy port
				bool enabled;///< if true proxy settings are enabled
				bool authRequired;///< if true proxy authentification is required
			};
			
			__proxyAuthInfo proxyAuthInfo;///< proxy authentification information 
	};

};

#endif
