/***************************************************************************
 *            cgi.h
 *
 *  Sat Sep  17 2005
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

#ifndef _CGI_H_
#define _CGI_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
	namespace cgi {
		/**
		 * @enum environmentEnum defines DIALOGUE environment
		 */
		enum environmentEnum {
			ENVIRONMENT_REQUESTMETHOD,
			ENVIRONMENT_REQUESTURI,
			ENVIRONMENT_QUERYSTRING,
			ENVIRONMENT_CONTENTTYPE,
			ENVIRONMENT_CONTENTLENGTH,
			ENVIRONMENT_CONTENTTRANSFERENCODING,
			ENVIRONMENT_HTTPAUTHORIZATION,      ///< authentication credentials for HTTP authentication
			ENVIRONMENT_HTTPHOST,
			ENVIRONMENT_HTTPUSERAGENT,          ///< the user agent string of the user agent
			ENVIRONMENT_HTTPCOOKIE,             ///< the user agent string of the user agent
			ENVIRONMENT_HTTPIFMODIFIEDSINCE,    ///< allows a 304 Not Modified to be returned
			ENVIRONMENT_HTTPDATE,               ///< the date and time that the message was sent
			ENVIRONMENT_HTTPACCEPT,             ///< content-types that are acceptable by dialogue
			ENVIRONMENT_HTTPACCEPTLANGUAGE,     ///< acceptable languages for response
			ENVIRONMENT_HTTPACCEPTENCODING,     ///< acceptable encodings
			ENVIRONMENT_HTTPACCEPTCHARSET,      ///< character sets that are acceptable
			ENVIRONMENT_HTTPACCEPTRANGES,       ///< character sets that are acceptable
			ENVIRONMENT_HTTPKEEPALIVE,
			ENVIRONMENT_HTTPCONNECTION,         ///< what type of connection the user-agent would prefer
			ENVIRONMENT_HTTPREFERER,
			ENVIRONMENT_HTTPVIA,
			ENVIRONMENT_HTTPXFORWARDEDFOR,
			ENVIRONMENT_REMOTEADDR,
			ENVIRONMENT_REMOTEPORT,
			ENVIRONMENT_REDIRECTSTATUS,
			ENVIRONMENT_REDIRECTQUERYSTRING,
			ENVIRONMENT_REDIRECTURL,
			ENVIRONMENT_GATEWAYINTERFACE,
			ENVIRONMENT_PATH,
			ENVIRONMENT_DIALOGUESIGNATURE,
			ENVIRONMENT_DIALOGUESOFTWARE,
			ENVIRONMENT_DIALOGUENAME,
			ENVIRONMENT_DIALOGUEADDR,
			ENVIRONMENT_DIALOGUEPORT,
			ENVIRONMENT_DIALOGUEADMIN,
			ENVIRONMENT_DIALOGUEPROTOCOL,
			ENVIRONMENT_SCRIPTFILENAME,
			ENVIRONMENT_SCRIPTNAME,

			ENVIRONMENT_ENUMSIZE
		};

		/**
		 * @enum responseHeaderEnum defines HTTP response headers
		 */
		enum responseHeaderEnum {
			RESPONSE_HEADER_ACCEPTRANGES,            ///< what partial content range types this dialogue supports
			RESPONSE_HEADER_AGE,                     ///< the age the object has been in a proxy cache in seconds
			RESPONSE_HEADER_ALLOW,                   ///< valid actions for a specified resource. To be used for a 405 Method not allowed
			RESPONSE_HEADER_CACHECONTROL,            ///< tells all caching mechanisms from dialogue to dialogue whether they may cache this object
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
			RESPONSE_HEADER_DIALOGUE,                ///< a name for the dialogue
			RESPONSE_HEADER_WWWAUTHENTICATE,         ///< auuthentification request
			RESPONSE_HEADER_XPOWEREDBY,              ///< cgi provider

			RESPONSE_HEADER_ENUMSIZE
		};

		/**
		 * @enum statusCodeEnum defines HTTP response status code
		 */
		enum statusCodeEnum {
			STATUS_CODE_CONTINUE,                    ///< 100
			STATUS_CODE_SWITCHINGPROTOCOLS,          ///< 101
			STATUS_CODE_OK,                          ///< 200
			STATUS_CODE_CREATED,                     ///< 201
			STATUS_CODE_ACCEPTED,                    ///< 202
			STATUS_CODE_NONAUTHORITATIVE,            ///< 203
			STATUS_CODE_NOCONTENT,                   ///< 204
			STATUS_CODE_RESETCONTENT,                ///< 205
			STATUS_CODE_PARTIALCONTENT,              ///< 206
			STATUS_CODE_MULTIPLECHOISES,             ///< 300
			STATUS_CODE_MOVEDPERMANENTLY,            ///< 301
			STATUS_CODE_FOUND,                       ///< 302
			STATUS_CODE_SEEOTHER,                    ///< 303
			STATUS_CODE_NOTMODIFIED,                 ///< 304
			STATUS_CODE_USEPROXY,                    ///< 305
			STATUS_CODE_TEMPORARYREDIRECT,           ///< 307
			STATUS_CODE_BADREQUEST,                  ///< 400
			STATUS_CODE_UNAUTHORIZED,                ///< 401
			STATUS_CODE_PAYMENTREQUIRED,             ///< 402
			STATUS_CODE_FORBIDDEN,                   ///< 403
			STATUS_CODE_NOTFOUND,                    ///< 404
			STATUS_CODE_METHODNOTALLOWED,            ///< 405
			STATUS_CODE_NOTACCEPTABLE,               ///< 406
			STATUS_CODE_PROXYauthenticationREQUIRED, ///< 407
			STATUS_CODE_REQUESTTIMEOUT,              ///< 408
			STATUS_CODE_CONFLICT,                    ///< 409
			STATUS_CODE_GONE,                        ///< 410
			STATUS_CODE_LENGTHREQUIRED,              ///< 411
			STATUS_CODE_PRECONDITIONFAILED,          ///< 412
			STATUS_CODE_REQUESTENTITYTOOLARGE,       ///< 413
			STATUS_CODE_REQUESTURITOOLONG,           ///< 414
			STATUS_CODE_UNSUPPORTEDMEDIATYPE,        ///< 415
			STATUS_CODE_REQUESTRANGENOTSATISFIABLE,  ///< 416
			STATUS_CODE_EXPECTATIONFAILED,           ///< 417
			STATUS_CODE_INTERNALDIALOGUEERROR,       ///< 500
			STATUS_CODE_NOTIMPLEMENTED,              ///< 501
			STATUS_CODE_BADGATEWAY,                  ///< 502
			STATUS_CODE_SERVICEUNAVAILABLE,          ///< 503
			STATUS_CODE_GATEWAYTIMEOUT,              ///< 504
			STATUS_CODE_HTTPVERSIONNOTSUPPORTED,     ///< 505

			STATUS_CODE_ENUMSIZE
		};

		/**
		 * @enum fileErrorEnum defines errors that might occur during fetching POST file
		 */
		enum fileErrorEnum {
			FILE_ERROR_NONE,                       ///< everything is OK
			FILE_ERROR_ACCESS_DENY,                ///< permissions error
			FILE_ERROR_BAD_FILE_NAME,              ///< filename is too long
			FILE_ERROR_NO_SPACE,                   ///< not enough space to save the file
			FILE_ERROR_NO_FILE,                    ///< file with such name not found
		};

		/**
		 * @class file
		 * @brief defines node about uploaded POST file
		 */
		class file {
		  public:

			/**
			 * constructor
			 */
			file();

			dodoString name;                            ///< real name of the file
			dodoString mime;                            ///< mimetype of the file
			dodoString path;                        ///< path where it was temporary saved[empty, if file is stored in the memory]
			unsigned long size;                         ///< size of the file
			short error;                                ///< error code, @see cgi::ileErrorEnum
			dodoString data;                            ///< contains file data[if file is stored in the memory]
		};

		/**
		 * @class cookie
		 * @brief describes cookie that would be sent to the browser
		 */
		class cookie {
		  public:

			/**
			 * constructor
			 */
			cookie();

			/**
			 * constructor
			 * @param name defines name of the cookie
			 * @param value defines value of the cookie
			 * @param expires defines expiration date of the cookie
			 * @param path defines path of the cookie
			 * @param domain defines domain of the cookie
			 * @param secure defines cookie security
			 */
			cookie(const dodoString &name,
				   const dodoString &value,
				   const dodoString &expires = __dodostring__,
				   const dodoString &path = __dodostring__,
				   const dodoString &domain = __dodostring__,
				   bool             secure = false);

			dodoString name;            ///< name of the cookie
			dodoString value;           ///< value of the cookie

			dodoString expires;         ///< expiration date
			dodoString path;            ///< cookie path
			dodoString domain;          ///< cookie domain
			bool secure;                ///< defines cookie security; false by default
		};

		/**
		 * @enum requestMethodEnum defines request type
		 */
		enum requestMethodEnum {
			REQUEST_METHOD_GET,
			REQUEST_METHOD_POST,
			REQUEST_METHOD_GET_POST  ///< if POST method and request Url also contains ?name=value&.... format data
		};

		/**
		 * @enum authEnum defines authentication type
		 */
		enum authEnum {
			AUTH_NONE,
			AUTH_BASIC,
			AUTH_DIGEST
		};

		/**
		 * @struct __auth__
		 * @brief defines authenfication information
		 */
		struct __auth__ {
			dodoString user;            ///< user name
			short      type;            ///< authenfication type, @see cgi::authEnum
		};
	};
};

#include <libdodo/cgiBasicExchange.h>
#include <libdodo/cgiBasicServer.h>
#include <libdodo/cgiDialogue.h>
#include <libdodo/cgiDialogueEx.h>
#include <libdodo/cgiExchange.h>
#include <libdodo/cgiFastExchange.h>
#include <libdodo/cgiFastExchangeEx.h>
#include <libdodo/cgiFastServer.h>
#include <libdodo/cgiFastServerEx.h>
#include <libdodo/cgiServer.h>
#endif
