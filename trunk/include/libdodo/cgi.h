/***************************************************************************
 *            cgi.h
 *
 *  Sat Sep  17 21:55:19 2005
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

namespace dodo
{
	namespace cgi
	{
#define CGI_ENVIRONMENTSTATEMENTS    38

		/**
		 * @enum cgiEnvironmentEnum defines DIALOGUE environment
		 */
		enum cgiEnvironmentEnum
		{
			CGI_ENVIRONMENT_REQUESTMETHOD,
			CGI_ENVIRONMENT_REQUESTURI,
			CGI_ENVIRONMENT_QUERYSTRING,
			CGI_ENVIRONMENT_CONTENTTYPE,
			CGI_ENVIRONMENT_CONTENTLENGTH,
			CGI_ENVIRONMENT_CONTENTTRANSFERENCODING,
			CGI_ENVIRONMENT_HTTPAUTHORIZATION,      ///< authentication credentials for HTTP authentication
			CGI_ENVIRONMENT_HTTPHOST,
			CGI_ENVIRONMENT_HTTPUSERAGENT,          ///< the user agent string of the user agent
			CGI_ENVIRONMENT_HTTPCOOKIE,             ///< the user agent string of the user agent
			CGI_ENVIRONMENT_HTTPIFMODIFIEDSINCE,    ///< allows a 304 Not Modified to be returned
			CGI_ENVIRONMENT_HTTPDATE,               ///< the date and time that the message was sent
			CGI_ENVIRONMENT_HTTPACCEPT,             ///< content-types that are acceptable by dialogue
			CGI_ENVIRONMENT_HTTPACCEPTLANGUAGE,     ///< acceptable languages for response
			CGI_ENVIRONMENT_HTTPACCEPTENCODING,     ///< acceptable encodings
			CGI_ENVIRONMENT_HTTPACCEPTCHARSET,      ///< character sets that are acceptable
			CGI_ENVIRONMENT_HTTPACCEPTRANGES,       ///< character sets that are acceptable
			CGI_ENVIRONMENT_HTTPKEEPALIVE,
			CGI_ENVIRONMENT_HTTPCONNECTION,         ///< what type of connection the user-agent would prefer
			CGI_ENVIRONMENT_HTTPREFERER,
			CGI_ENVIRONMENT_HTTPVIA,
			CGI_ENVIRONMENT_HTTPXFORWARDEDFOR,
			CGI_ENVIRONMENT_REMOTEADDR,
			CGI_ENVIRONMENT_REMOTEPORT,
			CGI_ENVIRONMENT_REDIRECTSTATUS,
			CGI_ENVIRONMENT_REDIRECTQUERYSTRING,
			CGI_ENVIRONMENT_REDIRECTURL,
			CGI_ENVIRONMENT_GATEWAYINTERFACE,
			CGI_ENVIRONMENT_PATH,
			CGI_ENVIRONMENT_DIALOGUESIGNATURE,
			CGI_ENVIRONMENT_DIALOGUESOFTWARE,
			CGI_ENVIRONMENT_DIALOGUENAME,
			CGI_ENVIRONMENT_DIALOGUEADDR,
			CGI_ENVIRONMENT_DIALOGUEPORT,
			CGI_ENVIRONMENT_DIALOGUEADMIN,
			CGI_ENVIRONMENT_DIALOGUEPROTOCOL,
			CGI_ENVIRONMENT_SCRIPTFILENAME,
			CGI_ENVIRONMENT_SCRIPTNAME,
		};

#define CGI_RESPONSEHEADERSTATEMENTSD 18

		/**
		 * @enum cgiResponseHeaderEnum defines HTTP response headers
		 */
		enum cgiResponseHeaderEnum
		{
			CGI_RESPONSEHEADER_ACCEPTRANGES,            ///< what partial content range types this dialogue supports
			CGI_RESPONSEHEADER_AGE,                     ///< the age the object has been in a proxy cache in seconds
			CGI_RESPONSEHEADER_ALLOW,                   ///< valid actions for a specified resource. To be used for a 405 Method not allowed
			CGI_RESPONSEHEADER_CACHECONTROL,            ///< tells all caching mechanisms from dialogue to dialogue whether they may cache this object
			CGI_RESPONSEHEADER_CONTENTENCODING,         ///< the type of encoding used on the data
			CGI_RESPONSEHEADER_CONTENTLANGUAGE,         ///< the language the content is in
			CGI_RESPONSEHEADER_CONTENTLENGTH,           ///< the length of the content in bytes
			CGI_RESPONSEHEADER_CONTENTLOCATION,         ///< an alternate location for the returned data
			CGI_RESPONSEHEADER_CONTENTDISPOSITION,      ///< an opportunity to raise a "File Download" dialog box for a known MIME type
			CGI_RESPONSEHEADER_CONTENTMD5,              ///< an MD5 sum of the content of the response
			CGI_RESPONSEHEADER_CONTENTRANGE,            ///< where in a full body message this partial message belongs
			CGI_RESPONSEHEADER_CONTENTTYPE,             ///< the mime type of this content
			CGI_RESPONSEHEADER_DATE,                    ///< the date and time that the message was sent
			CGI_RESPONSEHEADER_LASTMODIFIED,            ///< the last modified date for the requested object
			CGI_RESPONSEHEADER_LOCATION,                ///< used in redirection
			CGI_RESPONSEHEADER_DIALOGUE,                ///< a name for the dialogue
			CGI_RESPONSEHEADER_WWWAUTHENTICATE,         ///< auuthentification request
			CGI_RESPONSEHEADER_XPOWEREDBY,              ///< cgi provider
		};

#define CGI_STATUSSTATEMENTS 40

		/**
		 * @enum cgiStatusCodeEnum defines HTTP response status code
		 */
		enum cgiStatusCodeEnum
		{
			CGI_STATUSCODE_CONTINUE,                    ///< 100
			CGI_STATUSCODE_SWITCHINGPROTOCOLS,          ///< 101
			CGI_STATUSCODE_OK,                          ///< 200
			CGI_STATUSCODE_CREATED,                     ///< 201
			CGI_STATUSCODE_ACCEPTED,                    ///< 202
			CGI_STATUSCODE_NONAUTHORITATIVE,            ///< 203
			CGI_STATUSCODE_NOCONTENT,                   ///< 204
			CGI_STATUSCODE_RESETCONTENT,                ///< 205
			CGI_STATUSCODE_PARTIALCONTENT,              ///< 206
			CGI_STATUSCODE_MULTIPLECHOISES,             ///< 300
			CGI_STATUSCODE_MOVEDPERMANENTLY,            ///< 301
			CGI_STATUSCODE_FOUND,                       ///< 302
			CGI_STATUSCODE_SEEOTHER,                    ///< 303
			CGI_STATUSCODE_NOTMODIFIED,                 ///< 304
			CGI_STATUSCODE_USEPROXY,                    ///< 305
			CGI_STATUSCODE_TEMPORARYREDIRECT,           ///< 307
			CGI_STATUSCODE_BADREQUEST,                  ///< 400
			CGI_STATUSCODE_UNAUTHORIZED,                ///< 401
			CGI_STATUSCODE_PAYMENTREQUIRED,             ///< 402
			CGI_STATUSCODE_FORBIDDEN,                   ///< 403
			CGI_STATUSCODE_NOTFOUND,                    ///< 404
			CGI_STATUSCODE_METHODNOTALLOWED,            ///< 405
			CGI_STATUSCODE_NOTACCEPTABLE,               ///< 406
			CGI_STATUSCODE_PROXYauthenticationREQUIRED, ///< 407
			CGI_STATUSCODE_REQUESTTIMEOUT,              ///< 408
			CGI_STATUSCODE_CONFLICT,                    ///< 409
			CGI_STATUSCODE_GONE,                        ///< 410
			CGI_STATUSCODE_LENGTHREQUIRED,              ///< 411
			CGI_STATUSCODE_PRECONDITIONFAILED,          ///< 412
			CGI_STATUSCODE_REQUESTENTITYTOOLARGE,       ///< 413
			CGI_STATUSCODE_REQUESTURITOOLONG,           ///< 414
			CGI_STATUSCODE_UNSUPPORTEDMEDIATYPE,        ///< 415
			CGI_STATUSCODE_REQUESTRANGENOTSATISFIABLE,  ///< 416
			CGI_STATUSCODE_EXPECTATIONFAILED,           ///< 417
			CGI_STATUSCODE_INTERNALDIALOGUEERROR,       ///< 500
			CGI_STATUSCODE_NOTIMPLEMENTED,              ///< 501
			CGI_STATUSCODE_BADGATEWAY,                  ///< 502
			CGI_STATUSCODE_SERVICEUNAVAILABLE,          ///< 503
			CGI_STATUSCODE_GATEWAYTIMEOUT,              ///< 504
			CGI_STATUSCODE_HTTPVERSIONNOTSUPPORTED,     ///< 505
		};

		/**
		 * @enum cgiPostFileErrEnum defines errors that might occur during fetching POST file
		 */
		enum cgiPostFileErrEnum
		{
			CGI_POSTFILEERR_NONE,                       ///< everything is OK
			CGI_POSTFILEERR_ACCESS_DENY,                ///< permissions error
			CGI_POSTFILEERR_BAD_FILE_NAME,              ///< filename is too long
			CGI_POSTFILEERR_NO_SPACE,                   ///< not enough space to save the file
			CGI_POSTFILEERR_NO_FILE,                    ///< file with such name not found
		};

		/**
		 * @struct __cgiFile
		 * @brief defines node about uploaded POST file
		 */
		struct __cgiFile
		{
			/**
			 * constructor
			 */
			__cgiFile();

			dodoString    name;                         ///< real name of the file
			dodoString    mime;                         ///< mimetype of the file
			dodoString    tmp_name;                     ///< path where it was temporary saved[empty, if file is stored in the memory]
			unsigned long size;                         ///< size of the file
			short         error;                        ///< error code[see dialoguePostFileErrEnum]
			dodoString    data;                         ///< contains file data[if file is stored in the memory]
		};

		/**
		 * @struct __cgiCookie
		 * @brief describes cookie that would be sent to the dialogue browser
		 */
		struct __cgiCookie
		{
			/**
			 * constructor
			 */
			__cgiCookie();

			/**
			 * constructor
			 */
			__cgiCookie(bool secure);

			/**
			 * constructor
			 * @note defines struct data with user data
			 */
			__cgiCookie(const dodoString &name, const dodoString &value, const dodoString &exDate, const dodoString &path, const dodoString &domain, bool secure);

			dodoString name;                            ///< name of the cookie
			dodoString value;                           ///< value of the cookie

			dodoString expires;                         ///< expiration date
			dodoString path;                            ///< cookie path
			dodoString domain;                          ///< cookie domain
			bool       secure;                          ///< defines cookie security; false by default
		};

		/**
		 * @enum cgiRequestMethodEnum defines request type
		 */
		enum cgiRequestMethodEnum
		{
			CGI_REQUESTMETHOD_GET,
			CGI_REQUESTMETHOD_POST,
			CGI_REQUESTMETHOD_GET_POST                  ///< if POST method and request Url also contains ?name=value&.... format data
		};

		/**
		 * @enum cgiAuthTypeEnum defines authentication types
		 */
		enum cgiAuthTypeEnum
		{
			CGI_AUTHTYPE_NONE,
			CGI_AUTHTYPE_BASIC,
			CGI_AUTHTYPE_DIGEST
		};

		/**
		 * @struct __cgiAuthInfo
		 * @brief defines authenfication information
		 */
		struct __cgiAuthInfo
		{
			dodoString user;                            ///< user name
			short      type;                            ///< authenfication type[see cgiAuthTypeEnum]
		};
	};
};

#endif
