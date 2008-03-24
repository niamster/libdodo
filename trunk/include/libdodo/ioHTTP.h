/***************************************************************************
 *            ioHTTP.h
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

#ifndef _IOHTTP_H_
#define _IOHTTP_H_

#include <libdodo/directives.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libdodo/tools.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioHTTPEx.h>
#include <libdodo/types.h>

namespace dodo
{	
#define IOHTTP_REQUESTHEADERSTATEMENTS_SIZE 11

	/**
	 * @enum ioHTTPRequestHeaderEnum defines HTTP request headers
	 */
	enum ioHTTPRequestHeaderEnum
	{
		IOHTTP_REQUESTHEADER_ACCEPT,///< content-types that are acceptable
		IOHTTP_REQUESTHEADER_ACCEPTCHARSET,///< character sets that are acceptable
		IOHTTP_REQUESTHEADER_ACCEPTENCODING,///< acceptable encodings
		IOHTTP_REQUESTHEADER_ACCEPTLANGUAGE,///< acceptable languages for response
		IOHTTP_REQUESTHEADER_ACCEPTRANGES,///< allows the server to indicate its acceptance of range requests for a resource
		IOHTTP_REQUESTHEADER_AUTHORIZATION,///< authentication credentials for HTTP authentication
		IOHTTP_REQUESTHEADER_CONNECTION,///< what type of connection the user-agent would prefer
		IOHTTP_REQUESTHEADER_DATE,///< the date and time that the message was sent
		IOHTTP_REQUESTHEADER_HOST,///< the domain name of the server (for virtual hosting)
		IOHTTP_REQUESTHEADER_IFMODIFIEDSINCE,///< allows a 304 Not Modified to be returned
		IOHTTP_REQUESTHEADER_USERAGENT,///< the user agent string of the user agent
	};

#define IOHTTP_RESPONSEHEADERSTATEMENTS_SIZE 16
	
	/**
	 * @enum ioHTTPResponseHeaderEnum defines HTTP response headers
	 */
	enum ioHTTPResponseHeaderEnum
	{
		IOHTTP_RESPONSEHEADER_ACCEPTRANGES,///< what partial content range types this server supports
		IOHTTP_RESPONSEHEADER_AGE,///< the age the object has been in a proxy cache in seconds
		IOHTTP_RESPONSEHEADER_ALLOW,///< valid actions for a specified resource. To be used for a 405 Method not allowed
		IOHTTP_RESPONSEHEADER_CACHECONTROL,///< tells all caching mechanisms from server to client whether they may cache this object
		IOHTTP_RESPONSEHEADER_CONTENTENCODING,///< the type of encoding used on the data
		IOHTTP_RESPONSEHEADER_CONTENTLANGUAGE,///< the language the content is in
		IOHTTP_RESPONSEHEADER_CONTENTLENGTH,///< the length of the content in bytes
		IOHTTP_RESPONSEHEADER_CONTENTLOCATION,///< an alternate location for the returned data
		IOHTTP_RESPONSEHEADER_CONTENTDISPOSITION,///< an opportunity to raise a "File Download" dialog box for a known MIME type
		IOHTTP_RESPONSEHEADER_CONTENTMD5,///< an MD5 sum of the content of the response
		IOHTTP_RESPONSEHEADER_CONTENTRANGE,///< where in a full body message this partial message belongs
		IOHTTP_RESPONSEHEADER_CONTENTTYPE,///< the mime type of this content
		IOHTTP_RESPONSEHEADER_DATE,///< the date and time that the message was sent
		IOHTTP_RESPONSEHEADER_LASTMODIFIED,///< the last modified date for the requested object
		IOHTTP_RESPONSEHEADER_LOCATION,///< used in redirection
		IOHTTP_RESPONSEHEADER_SERVER,///< a name for the server
	};

	/**
	 * @class ioHTTP provides disk I/O manipulations
	 */
	class ioHTTP
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			ioHTTP(ioHTTP &fd);

		public:

			/**
			 * constructor
			 */
			ioHTTP();

			/**
			 * destructor
			 */
			virtual ~ioHTTP();

			dodoMap<short, dodoString> requestHeaders; ///< headers that will be sent with request[see ioHTTPRequestHeaderEnum]
			dodoMap<short, dodoString> responseHeaders; ///< headers that will be received with response[see ioHTTPResponseHeaderEnum]
			
			/**
			 * @return server response
			 * @param url defines URL
			 */
			virtual dodoString GET(const __url &url);
			
			/**
			 * @return server response
			 * @param url defines URL
			 */
			virtual dodoString GET(const dodoString &url);

		private:
			
			ioNetworkExchange ex;
			
			static const dodoString requestHeaderStatements[IOHTTP_REQUESTHEADERSTATEMENTS_SIZE];///< HTTP request headers[see ioHTTPRequestHeaderEnum]
			static const dodoString responseHeaderStatements[IOHTTP_RESPONSEHEADERSTATEMENTS_SIZE];///< HTTP response headers[see ioHTTPResponseHeaderEnum]
	};

};

#endif
