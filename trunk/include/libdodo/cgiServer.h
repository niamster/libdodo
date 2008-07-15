/***************************************************************************
 *            cgiServer.h
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

#ifndef _CGISERVER_H_
#define _CGISERVER_H_

#include <libdodo/directives.h>

#include <string.h>
#include <stdlib.h>

#include <libdodo/cgiServerEx.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/cgiFastServer.h>
#include <libdodo/ioStdio.h>

namespace dodo
{
	namespace rpc
	{
		namespace xml
		{
			class cgiServer;
		};

		namespace json
		{
			class cgiServer;
		};
	};

	namespace cgi
	{
#define SERVER_ENVIRONMENTSTATEMENTS    38

		/**
		 * @enum serverEnvironmentEnum defines SERVER environment
		 */
		enum serverEnvironmentEnum
		{
			SERVER_ENVIRONMENT_REQUESTMETHOD,
			SERVER_ENVIRONMENT_REQUESTURI,
			SERVER_ENVIRONMENT_QUERYSTRING,
			SERVER_ENVIRONMENT_CONTENTTYPE,
			SERVER_ENVIRONMENT_CONTENTLENGTH,
			SERVER_ENVIRONMENT_CONTENTTRANSFERENCODING,
			SERVER_ENVIRONMENT_HTTPAUTHORIZATION,               ///< authentication credentials for HTTP authentication
			SERVER_ENVIRONMENT_HTTPHOST,
			SERVER_ENVIRONMENT_HTTPUSERAGENT,                   ///< the user agent string of the user agent
			SERVER_ENVIRONMENT_HTTPCOOKIE,                      ///< the user agent string of the user agent
			SERVER_ENVIRONMENT_HTTPIFMODIFIEDSINCE,             ///< allows a 304 Not Modified to be returned
			SERVER_ENVIRONMENT_HTTPDATE,                        ///< the date and time that the message was sent
			SERVER_ENVIRONMENT_HTTPACCEPT,                      ///< content-types that are acceptable by server
			SERVER_ENVIRONMENT_HTTPACCEPTLANGUAGE,              ///< acceptable languages for response
			SERVER_ENVIRONMENT_HTTPACCEPTENCODING,              ///< acceptable encodings
			SERVER_ENVIRONMENT_HTTPACCEPTCHARSET,               ///< character sets that are acceptable
			SERVER_ENVIRONMENT_HTTPACCEPTRANGES,                ///< character sets that are acceptable
			SERVER_ENVIRONMENT_HTTPKEEPALIVE,
			SERVER_ENVIRONMENT_HTTPCONNECTION,                  ///< what type of connection the user-agent would prefer
			SERVER_ENVIRONMENT_HTTPREFERER,
			SERVER_ENVIRONMENT_HTTPVIA,
			SERVER_ENVIRONMENT_HTTPXFORWARDEDFOR,
			SERVER_ENVIRONMENT_REMOTEADDR,
			SERVER_ENVIRONMENT_REMOTEPORT,
			SERVER_ENVIRONMENT_REDIRECTSTATUS,
			SERVER_ENVIRONMENT_REDIRECTQUERYSTRING,
			SERVER_ENVIRONMENT_REDIRECTURL,
			SERVER_ENVIRONMENT_GATEWAYINTERFACE,
			SERVER_ENVIRONMENT_PATH,
			SERVER_ENVIRONMENT_SERVERSIGNATURE,
			SERVER_ENVIRONMENT_SERVERSOFTWARE,
			SERVER_ENVIRONMENT_SERVERNAME,
			SERVER_ENVIRONMENT_SERVERADDR,
			SERVER_ENVIRONMENT_SERVERPORT,
			SERVER_ENVIRONMENT_SERVERADMIN,
			SERVER_ENVIRONMENT_SERVERPROTOCOL,
			SERVER_ENVIRONMENT_SCRIPTFILENAME,
			SERVER_ENVIRONMENT_SCRIPTNAME,
		};

#define SERVER_RESPONSEHEADERSTATEMENTS 18

		/**
		 * @enum serverResponseHeaderEnum defines HTTP response headers
		 */
		enum serverResponseHeaderEnum
		{
			SERVER_RESPONSEHEADER_ACCEPTRANGES,                         ///< what partial content range types this server supports
			SERVER_RESPONSEHEADER_AGE,                                  ///< the age the object has been in a proxy cache in seconds
			SERVER_RESPONSEHEADER_ALLOW,                                ///< valid actions for a specified resource. To be used for a 405 Method not allowed
			SERVER_RESPONSEHEADER_CACHECONTROL,                         ///< tells all caching mechanisms from server to server whether they may cache this object
			SERVER_RESPONSEHEADER_CONTENTENCODING,                      ///< the type of encoding used on the data
			SERVER_RESPONSEHEADER_CONTENTLANGUAGE,                      ///< the language the content is in
			SERVER_RESPONSEHEADER_CONTENTLENGTH,                        ///< the length of the content in bytes
			SERVER_RESPONSEHEADER_CONTENTLOCATION,                      ///< an alternate location for the returned data
			SERVER_RESPONSEHEADER_CONTENTDISPOSITION,                   ///< an opportunity to raise a "File Download" dialog box for a known MIME type
			SERVER_RESPONSEHEADER_CONTENTMD5,                           ///< an MD5 sum of the content of the response
			SERVER_RESPONSEHEADER_CONTENTRANGE,                         ///< where in a full body message this partial message belongs
			SERVER_RESPONSEHEADER_CONTENTTYPE,                          ///< the mime type of this content
			SERVER_RESPONSEHEADER_DATE,                                 ///< the date and time that the message was sent
			SERVER_RESPONSEHEADER_LASTMODIFIED,                         ///< the last modified date for the requested object
			SERVER_RESPONSEHEADER_LOCATION,                             ///< used in redirection
			SERVER_RESPONSEHEADER_SERVER,                               ///< a name for the server
			SERVER_RESPONSEHEADER_WWWAUTHENTICATE,                      ///< auuthentification request
			SERVER_RESPONSEHEADER_XPOWEREDBY,                           ///< cgi provider
		};

#define SERVER_STATUSSTATEMENTS 40

		/**
		 * @enum serverStatusCodeEnum defines HTTP response status code
		 */
		enum serverStatusCodeEnum
		{
			SERVER_STATUSCODE_CONTINUE,                                         ///< 100
			SERVER_STATUSCODE_SWITCHINGPROTOCOLS,                               ///< 101
			SERVER_STATUSCODE_OK,                                               ///< 200
			SERVER_STATUSCODE_CREATED,                                          ///< 201
			SERVER_STATUSCODE_ACCEPTED,                                         ///< 202
			SERVER_STATUSCODE_NONAUTHORITATIVE,                                 ///< 203
			SERVER_STATUSCODE_NOCONTENT,                                        ///< 204
			SERVER_STATUSCODE_RESETCONTENT,                                     ///< 205
			SERVER_STATUSCODE_PARTIALCONTENT,                                   ///< 206
			SERVER_STATUSCODE_MULTIPLECHOISES,                                  ///< 300
			SERVER_STATUSCODE_MOVEDPERMANENTLY,                                 ///< 301
			SERVER_STATUSCODE_FOUND,                                            ///< 302
			SERVER_STATUSCODE_SEEOTHER,                                         ///< 303
			SERVER_STATUSCODE_NOTMODIFIED,                                      ///< 304
			SERVER_STATUSCODE_USEPROXY,                                         ///< 305
			SERVER_STATUSCODE_TEMPORARYREDIRECT,                                ///< 307
			SERVER_STATUSCODE_BADREQUEST,                                       ///< 400
			SERVER_STATUSCODE_UNAUTHORIZED,                                     ///< 401
			SERVER_STATUSCODE_PAYMENTREQUIRED,                                  ///< 402
			SERVER_STATUSCODE_FORBIDDEN,                                        ///< 403
			SERVER_STATUSCODE_NOTFOUND,                                         ///< 404
			SERVER_STATUSCODE_METHODNOTALLOWED,                                 ///< 405
			SERVER_STATUSCODE_NOTACCEPTABLE,                                    ///< 406
			SERVER_STATUSCODE_PROXYauthenticationREQUIRED,                      ///< 407
			SERVER_STATUSCODE_REQUESTTIMEOUT,                                   ///< 408
			SERVER_STATUSCODE_CONFLICT,                                         ///< 409
			SERVER_STATUSCODE_GONE,                                             ///< 410
			SERVER_STATUSCODE_LENGTHREQUIRED,                                   ///< 411
			SERVER_STATUSCODE_PRECONDITIONFAILED,                               ///< 412
			SERVER_STATUSCODE_REQUESTENTITYTOOLARGE,                            ///< 413
			SERVER_STATUSCODE_REQUESTURITOOLONG,                                ///< 414
			SERVER_STATUSCODE_UNSUPPORTEDMEDIATYPE,                             ///< 415
			SERVER_STATUSCODE_REQUESTRANGENOTSATISFIABLE,                       ///< 416
			SERVER_STATUSCODE_EXPECTATIONFAILED,                                ///< 417
			SERVER_STATUSCODE_INTERNALSERVERERROR,                              ///< 500
			SERVER_STATUSCODE_NOTIMPLEMENTED,                                   ///< 501
			SERVER_STATUSCODE_BADGATEWAY,                                       ///< 502
			SERVER_STATUSCODE_SERVICEUNAVAILABLE,                               ///< 503
			SERVER_STATUSCODE_GATEWAYTIMEOUT,                                   ///< 504
			SERVER_STATUSCODE_HTTPVERSIONNOTSUPPORTED,                          ///< 505
		};

		/**
		 * @enum serverPostFileErrEnum defines errors that might occur during fetching POST file
		 */
		enum serverPostFileErrEnum
		{
			SERVER_POSTFILEERR_NONE,                            ///< everything is OK
			SERVER_POSTFILEERR_ACCESS_DENY,                     ///< permissions error
			SERVER_POSTFILEERR_BAD_FILE_NAME,                   ///< filename is too long
			SERVER_POSTFILEERR_NO_SPACE,                        ///< not enough space to save the file
			SERVER_POSTFILEERR_NO_FILE,                         ///< file with such name not found
		};

		/**
		 * @struct __serverFile defines node about uploaded POST file
		 */
		struct __serverFile
		{
			/**
			 * constructor
			 */
			__serverFile();

			dodoString name;                                    ///< real name of the file
			dodoString mime;                                    ///< mimetype of the file
			dodoString tmp_name;                                ///< path where it was temporary saved[empty, if file is stored in the memory]
			unsigned long size;                                 ///< size of the file
			short error;                                        ///< error code[see serverPostFileErrEnum]
			dodoString data;                                    ///< contains file data[if file is stored in the memory]
		};

		/**
		 * @struct __serverCookie describes cookie that would be sent to the server browser
		 */
		struct __serverCookie
		{
			/**
			 * constructor
			 */
			__serverCookie();

			/**
			 * constructor
			 */
			__serverCookie(bool secure);

			/**
			 * constructor
			 * @note defines struct data with user data
			 */
			__serverCookie(const dodoString &name, const dodoString &value, const dodoString &exDate, const dodoString &path, const dodoString &domain, bool secure);

			dodoString name;                    ///< name of the cookie
			dodoString value;                   ///< value of the cookie

			dodoString expires;                 ///< expiration date
			dodoString path;                    ///< cookie path
			dodoString domain;                  ///< cookie domain
			bool secure;                        ///< defines cookie security; false by default
		};

		/**
		 * @enum serverRequestMethodEnum defines request type
		 */
		enum serverRequestMethodEnum
		{
			SERVER_REQUESTMETHOD_GET,
			SERVER_REQUESTMETHOD_POST,
			SERVER_REQUESTMETHOD_GET_POST             ///< if POST method and request Url also contains ?name=value&.... format data
		};

		/**
		 * @enum serverAuthTypeEnum defines authentication types
		 */
		enum serverAuthTypeEnum
		{
			SERVER_AUTHTYPE_NONE,
			SERVER_AUTHTYPE_BASIC,
			SERVER_AUTHTYPE_DIGEST
		};

		/**
		 * @struct __serverAuthInfo defines authenfication information
		 */
		struct __serverAuthInfo
		{
			dodoString user;                    ///< user name
			short type;                         ///< authenfication type[see cgiAuthTypeEnum]
		};

		/**
		 * @note
		 * headers that will be printed calling prinHeaders method:
		 * "Content-type",
		 * "X-Powered-By"
		 * you can change them or add extra using HEADERS cgi class property
		 */

		/**
		 * @class cgi provides SERVER functionality
		 * @note do not read stdin before contructing object of this class!
		 * object of this class should be contructed in the early begin
		 */
		class server
		{
			friend class rpc::xml::cgiServer;
			friend class rpc::json::cgiServer;

			private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				server(server &ct);

			public:

				/**
				 * constructor
				 * @param headers defines headers that will be printed
				 * @param silent defines whether to print headers in constructor or not
				 * @param autocleanFiles defines whether to clean POST files in destructor
				 * @param postFilesInMem defines place of POST files[disk or memory]
				 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
				 */
				server(dodoMap<short, dodoString> &headers, bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");


				/**
				 * constructor
				 * @param silent defines whether to print headers in constructor or not
				 * @param autocleanFiles defines whether to clean POST files in destructor
				 * @param postFilesInMem defines place of POST files[disk or memory]
				 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
				 */
				server(bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

#ifdef FASTCGI_EXT

				/**
				 * constructor
				 * @param cf defines cgi::fast::exchange I/O interface
				 * @param silent defines whether to print headers in constructor or not
				 * @param autocleanFiles defines whether to clean POST files in destructor
				 * @param postFilesInMem defines place of POST files[disk or memory]
				 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
				 */
				server(fast::exchange *cf, bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

				/**
				 * constructor
				 * @param cf defines cgi::fast::exchange I/O interface
				 * @param headers defines headers that will be printed
				 * @param silent defines whether to print headers in constructor or not
				 * @param autocleanFiles defines whether to clean POST files in destructor
				 * @param postFilesInMem defines place of POST files[disk or memory]
				 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
				 */
				server(fast::exchange *cf, dodoMap<short, dodoString> &headers, bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

#endif

				/**
				 * destructor
				 */
				virtual ~server();

				/**
				 * @return authentication info
				 */
				virtual __serverAuthInfo getAuthenticationInfo();

				/**
				 * request the authentication
				 * @param realm defines authentication request string
				 * @param type defines type of authentication[see cgiAuthTypeEnum]
				 */
				virtual void requestAuthentication(const dodoString &realm, short type = SERVER_AUTHTYPE_DIGEST);

				/**
				 * check authentication
				 * @return true if authentication successfull
				 * @param user defines user for authentication
				 * @param password defined user's password for authentication
				 */
				virtual bool isAuthenticated(const dodoString &user, const dodoString &password);

				/**
				 * @return contents of stdin got for the POST request
				 */
				virtual dodoString getContent();

				/**
				 * clears contents of stdin got for the POST request
				 */
				virtual void clearContent();

				/**
				 * set response code and message
				 * @param code defines return code[see cgiStatusCodeEnum]
				 */
				virtual void setResponseStatus(short code);

				/**
				 * @return method type
				 */
				virtual int getMethod() const;

				/**
				 * @return array of request variables of given method
				 * @param method defines defines type of array of request variables to return[see requestMethodEnum]
				 * @note example: classObj[SERVER_REQUESTMETHOD_POST]["name"]
				 */
				virtual const dodoStringMap &operator[](short method);

				/**
				 * specific variables (from POST, GET, ENV or COOKIE)
				 */
				dodoStringMap POST;                                                                     ///< POST variables
				dodoStringMap GET;                                                                      ///< GET variables
				mutable dodoMap<short, dodoString> ENVIRONMENT;                                         ///< environment variables[see cgiEnvironmentEnum]
				dodoStringMap COOKIES;                                                                  ///< cookies sent by browser
				dodoMap<dodoString, __serverFile, dodoMapStringCompare> FILES;                          ///< POST files
				dodoMap<short, dodoString> HEADERS;                                                     ///< headers that will be printed with printHeaders method

				/**
				 * @return value of the requested variable from POST or GET
				 * @param varName defines name of the variable
				 * @param first defines what array will be used first to search for the variable[see requestMethodEnum]
				 */
				virtual dodoString request(const dodoString &varName, short first = SERVER_REQUESTMETHOD_GET);

				/**
				 * print cgi headers
				 * @note print cookies also
				 */
				virtual void printHeaders() const;

				/**
				 * print data to the output
				 * @param data defines data that would be printed
				 */
				virtual void print(const dodoString &data);

				/**
				 * print data to the output
				 * @param data defines data that would be printed
				 * @note print until '\n' is reached
				 */
				virtual void printStream(const dodoString &data);

				/**
				 * flush output
				 */
				virtual void flush();

				/**
				 * set cookie
				 * @param name defines name of cookie
				 * @param value defines value of cookie
				 * @param exDate defines expiration date
				 * @param path defines cookie path
				 * @param domain defines cookie domain
				 * @param secure defines cookie security
				 * @note cookies are printed with printHeaders method
				 */
				virtual void setCookie(const dodoString &name, const dodoString &value, const dodoString &exDate = __dodostring__, const dodoString &path = __dodostring__, const dodoString &domain = __dodostring__, bool secure = false);

				/**
				 * set cookie
				 * @param cookie defines the cookie
				 * @note cookies are printed with printHeaders method
				 */
				virtual void setCookie(const __serverCookie &cookie);

				/**
				 * @return charset of the request
				 */
				virtual dodoString getCharset();

			protected:

				/**
				 * fetch auth information
				 */
				virtual void makeAuth();

				/**
				 * write detected method to method class property
				 */
				virtual void detectMethod();

				/**
				 * fill POST variables and files if defined
				 */
				virtual void makePost();

				/**
				 * get contents of stdin for the POST request
				 */
				virtual void makeContent();

				/**
				 * get environment variables
				 */
				virtual void makeEnv();

				/**
				 * initiate HEADERS class property with given tuples name->value
				 * @param headers defines init headers
				 */
				virtual void initHeaders(dodoMap<short, dodoString> &headers);

				/**
				 * process serialized string of tuples key->value
				 * @param val defines map that will be filled with processed tuples key->value
				 * @param string defines string to process
				 * @param delim defines delimite
				 * @note
				 * from : name1=value1`delim`name2=value2
				 * to : val["name1"]=value1; val["name2"]=value2;
				 */
				virtual void make(dodoStringMap &val, const dodoString &string, const char *delim = "&");

			private:

				short returnCode;                                       ///< HTTP return code
				dodoString returnMessage;                               ///< HTTP return message

				bool postFilesInMem;                                    ///< place of POST files

				bool autocleanFiles;                                    ///< defines whether to clean POST files in destructor

				dodoString postFilesTmpDir;                             ///< directory for POST files if on they are saved on the disk

				dodoList<__serverCookie> cookies;                       ///< cookies
				int method;                                             ///< request method

				dodoString content;                                     ///< contents of the stdin for the POST request

				dodoStringArray contenTypeExtensions;							///< contains contentype extension[boundary, modification-date, etc]

				/**
				 * delete temp POST files that have been created
				 */
				virtual void cleanTmp();

				/**
				 * @return trimmed string
				 * @param data defines string to trim
				 * @note uses trimSymbols to trim
				 */
				static dodoString trim(const dodoString &data);

#ifdef FASTCGI_EXT

				bool serverFastSet;                    ///< true if use ioSERVERFast

#endif

				io::channel *cgiIO;                                                                                     ///< SERVER I/O instance

				mutable bool headersPrinted;                                                                            ///< true if headers have been printed

				static const char *environmentStatements[SERVER_ENVIRONMENTSTATEMENTS];                                 ///< names of environment variables[see cgiEnvironmentEnum]
				static const dodoString responseHeaderStatements[SERVER_RESPONSEHEADERSTATEMENTS];                      ///< HTTP response headers[see cgiResponseHeaderEnum]
				static const dodoString responseStatusStatements[SERVER_STATUSSTATEMENTS];                              ///< HTTP response headers[see cgiStatusCodeEnum]

				/**
				 * @struct __authInfo defines authenfication information
				 */
				struct __internalAuthInfo
				{
					dodoString user;                            ///< user name
					dodoString password;                        ///< user password
					dodoString realm;                           ///< explanation of the authentication request
					dodoString nonce;                           ///< server-specified uniquely generated data
					dodoString opaque;                          ///< server-specified uniquely generated data, which should be returned by the server unchanged
					dodoString cnonce;                          ///< server-specified uniquely generated data
					dodoString nonceCount;                      ///< hexadecimal count of the number of requests (including the current request) that the server has sent with the nonce value in this request
					dodoString uri;                             ///< URI from Request-URI
					dodoString qop;                             ///< quality of protection
					dodoString response;                        ///< 32 hex digits. which proves that the user knows a password
					short type;                                 ///< authenfication type[see cgiAuthTypeEnum]
				};

				__internalAuthInfo authInfo;                 ///< authentication information
		};
	};
};

#endif
