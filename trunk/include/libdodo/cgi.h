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

#ifndef _CGI_H_
#define _CGI_H_

#include <libdodo/directives.h>

#include <string.h>
#include <stdlib.h>

#include <libdodo/cgiEx.h>
#include <libdodo/tools.h>
#include <libdodo/types.h>
#include <libdodo/dodoMap.h>
#include <libdodo/stringTools.h>
#include <libdodo/cgiFast.h>
#include <libdodo/ioSTD.h>

namespace dodo
{

#define CGI_ENVIRONMENTSTATEMENTS    38

	/**
	 * @enum cgiEnvironmentEnum defines CGI environment
	 */
	enum cgiEnvironmentEnum
	{
		CGI_ENVIRONMENT_REQUESTMETHOD,
		CGI_ENVIRONMENT_REQUESTURI,
		CGI_ENVIRONMENT_QUERYSTRING,
		CGI_ENVIRONMENT_CONTENTTYPE,
		CGI_ENVIRONMENT_CONTENTLENGTH,
		CGI_ENVIRONMENT_CONTENTTRANSFERENCODING,
		CGI_ENVIRONMENT_HTTPAUTHORIZATION,///< authentication credentials for HTTP authentication
		CGI_ENVIRONMENT_HTTPHOST,
		CGI_ENVIRONMENT_HTTPUSERAGENT,///< the user agent string of the user agent
		CGI_ENVIRONMENT_HTTPCOOKIE,///< the user agent string of the user agent
		CGI_ENVIRONMENT_HTTPIFMODIFIEDSINCE,///< allows a 304 Not Modified to be returned
		CGI_ENVIRONMENT_HTTPDATE,///< the date and time that the message was sent
		CGI_ENVIRONMENT_HTTPACCEPT,///< content-types that are acceptable by client
		CGI_ENVIRONMENT_HTTPACCEPTLANGUAGE,///< acceptable languages for response
		CGI_ENVIRONMENT_HTTPACCEPTENCODING,///< acceptable encodings
		CGI_ENVIRONMENT_HTTPACCEPTCHARSET,///< character sets that are acceptable
		CGI_ENVIRONMENT_HTTPACCEPTRANGES,///< character sets that are acceptable
		CGI_ENVIRONMENT_HTTPKEEPALIVE,
		CGI_ENVIRONMENT_HTTPCONNECTION,///< what type of connection the user-agent would prefer
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
		CGI_ENVIRONMENT_SERVERSIGNATURE,
		CGI_ENVIRONMENT_SERVERSOFTWARE,
		CGI_ENVIRONMENT_SERVERNAME,
		CGI_ENVIRONMENT_SERVERADDR,
		CGI_ENVIRONMENT_SERVERPORT,
		CGI_ENVIRONMENT_SERVERADMIN,
		CGI_ENVIRONMENT_SERVERPROTOCOL,
		CGI_ENVIRONMENT_SCRIPTFILENAME,
		CGI_ENVIRONMENT_SCRIPTNAME,
	};

#define CGI_RESPONSEHEADERSTATEMENTS 18
	
	/**
	 * @enum cgiResponseHeaderEnum defines HTTP response headers
	 */
	enum cgiResponseHeaderEnum
	{
		CGI_RESPONSEHEADER_ACCEPTRANGES,///< what partial content range types this server supports
		CGI_RESPONSEHEADER_AGE,///< the age the object has been in a proxy cache in seconds
		CGI_RESPONSEHEADER_ALLOW,///< valid actions for a specified resource. To be used for a 405 Method not allowed
		CGI_RESPONSEHEADER_CACHECONTROL,///< tells all caching mechanisms from server to client whether they may cache this object
		CGI_RESPONSEHEADER_CONTENTENCODING,///< the type of encoding used on the data
		CGI_RESPONSEHEADER_CONTENTLANGUAGE,///< the language the content is in
		CGI_RESPONSEHEADER_CONTENTLENGTH,///< the length of the content in bytes
		CGI_RESPONSEHEADER_CONTENTLOCATION,///< an alternate location for the returned data
		CGI_RESPONSEHEADER_CONTENTDISPOSITION,///< an opportunity to raise a "File Download" dialog box for a known MIME type
		CGI_RESPONSEHEADER_CONTENTMD5,///< an MD5 sum of the content of the response
		CGI_RESPONSEHEADER_CONTENTRANGE,///< where in a full body message this partial message belongs
		CGI_RESPONSEHEADER_CONTENTTYPE,///< the mime type of this content
		CGI_RESPONSEHEADER_DATE,///< the date and time that the message was sent
		CGI_RESPONSEHEADER_LASTMODIFIED,///< the last modified date for the requested object
		CGI_RESPONSEHEADER_LOCATION,///< used in redirection
		CGI_RESPONSEHEADER_SERVER,///< a name for the server
		CGI_RESPONSEHEADER_WWWAUTHENTICATE,///< auuthentification request
		CGI_RESPONSEHEADER_XPOWEREDBY,///< cgi provider
	};

#define CGI_STATUSSTATEMENTS 40
	
	/**
	 * @enum cgiStatusCodeEnum defines HTTP response status code
	 */	
	enum cgiStatusCodeEnum
	{
		CGI_STATUSCODE_CONTINUE,///< 100
		CGI_STATUSCODE_SWITCHINGPROTOCOLS,///< 101
		CGI_STATUSCODE_OK,///< 200
		CGI_STATUSCODE_CREATED,///< 201
		CGI_STATUSCODE_ACCEPTED,///< 202
		CGI_STATUSCODE_NONAUTHORITATIVE,///< 203
		CGI_STATUSCODE_NOCONTENT,///< 204
		CGI_STATUSCODE_RESETCONTENT,///< 205
		CGI_STATUSCODE_PARTIALCONTENT,///< 206
		CGI_STATUSCODE_MULTIPLECHOISES,///< 300
		CGI_STATUSCODE_MOVEDPERMANENTLY,///< 301
		CGI_STATUSCODE_FOUND,///< 302
		CGI_STATUSCODE_SEEOTHER,///< 303
		CGI_STATUSCODE_NOTMODIFIED,///< 304
		CGI_STATUSCODE_USEPROXY,///< 305
		CGI_STATUSCODE_TEMPORARYREDIRECT,///< 307
		CGI_STATUSCODE_BADREQUEST,///< 400
		CGI_STATUSCODE_UNAUTHORIZED,///< 401
		CGI_STATUSCODE_PAYMENTREQUIRED,///< 402
		CGI_STATUSCODE_FORBIDDEN,///< 403
		CGI_STATUSCODE_NOTFOUND,///< 404
		CGI_STATUSCODE_METHODNOTALLOWED,///< 405
		CGI_STATUSCODE_NOTACCEPTABLE,///< 406
		CGI_STATUSCODE_PROXYAUTHENTIFICATIONREQUIRED,///< 407 
		CGI_STATUSCODE_REQUESTTIMEOUT,///< 408
		CGI_STATUSCODE_CONFLICT,///< 409
		CGI_STATUSCODE_GONE,///< 410 
		CGI_STATUSCODE_LENGTHREQUIRED,///< 411
		CGI_STATUSCODE_PRECONDITIONFAILED,///< 412
		CGI_STATUSCODE_REQUESTENTITYTOOLARGE,///< 413
		CGI_STATUSCODE_REQUESTURITOOLONG,///< 414
		CGI_STATUSCODE_UNSUPPORTEDMEDIATYPE,///< 415
		CGI_STATUSCODE_REQUESTRANGENOTSATISFIABLE,///< 416
		CGI_STATUSCODE_EXPECTATIONFAILED,///< 417
		CGI_STATUSCODE_INTERNALSERVERERROR,///< 500
		CGI_STATUSCODE_NOTIMPLEMENTED,///< 501
		CGI_STATUSCODE_BADGATEWAY,///< 502
		CGI_STATUSCODE_SERVICEUNAVAILABLE,///< 503
		CGI_STATUSCODE_GATEWAYTIMEOUT,///< 504
		CGI_STATUSCODE_HTTPVERSIONNOTSUPPORTED,///< 505
	};

	/**
	 * @enum postFileErrEnum defines errors that might occur during fetching POST file
	 */
	enum postFileErrEnum
	{
		CGI_POSTFILEERR_NONE,           ///< everything is OK
		CGI_POSTFILEERR_ACCESS_DENY,    ///< permissions error
		CGI_POSTFILEERR_BAD_FILE_NAME,  ///< filename is too long
		CGI_POSTFILEERR_NO_SPACE,        ///< not enough space to save the file
		CGI_POSTFILEERR_NO_FILE,        ///< file with such name not found
	};

	/**
	 * @struct __cgiFile defines node about uploaded POST file
	 */
	struct __cgiFile
	{
		/**
		 * constructor
		 */
		__cgiFile();
		
		dodoString name;                ///< real name of the file
		dodoString type;                ///< MIME type
		dodoString tmp_name;            ///< path where it was temporary saved[empty, if file is stored in the memory]
		unsigned long size;             ///< size of the file
		postFileErrEnum error;          ///< error code[see postFileErrEnum]
		dodoString data;                 ///< contains file data[if file is stored in the memory]
	};

	/**
	 * @struct __cookies describes cookie that would be sent to the client browser
	 */
	struct __cookie
	{
		/**
		 * constructor
		 */
		__cookie();
		
		/**
		 * constructor
		 */
		__cookie(bool secure);

		/**
		 * constructor
		 * @note defines struct data with user data
		 */
		__cookie(const dodoString &name, const dodoString &value, const dodoString &exDate, const dodoString &path, const dodoString &domain, bool secure);

		dodoString name;    ///< name of the cookie
		dodoString value;   ///< value of the cookie

		dodoString expires;  ///< expiration date
		dodoString path;    ///< cookie path
		dodoString domain;  ///< cookie domain
		bool secure;        ///< defines cookie security; false by default
	};

	/**
	 * @enum requestMethodEnum defines request type
	 */
	enum requestMethodEnum
	{
		CGI_REQUESTMETHOD_GET,
		CGI_REQUESTMETHOD_POST,
		CGI_REQUESTMETHOD_GET_POST ///< if POST method and request Url also contains ?name=value&.... format data
	};
	
	/**
	 * @enum cgiAuthTypeEnum defines authentification types
	 */
	enum cgiAuthTypeEnum
	{
		CGI_AUTHTYPE_NONE,
		CGI_AUTHTYPE_BASIC,
		CGI_AUTHTYPE_DIGEST
	};
	
	/**
	 * @struct __authInfo defines authenfication information
	 */
	struct __authInfo
	{
		dodoString user;///< user name
		short type;///< authenfication type[see cgiAuthTypeEnum]
	};

	/**
	 * @note
	 * headers that will be printed calling prinHeaders method:
	 * "Content-type",
	 * "X-Powered-By"
	 * you can change them or add extra using HEADERS cgi class property
	 */

	/**
	 * @class cgi provides CGI functionality
	 * @note do not read stdin before contructing object of this class!
	 * object of this class should be contructed in the early begin
	 */
	class cgi
	{
		friend class rpcXmlCgiServer;
		
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			cgi(cgi &ct);

		public:

			/**
			 * constructor
			 * @param headers defines headers that will be printed
			 * @param silent defines whether to print headers in constructor or not
			 * @param autocleanFiles defines whether to clean POST files in destructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 */
			cgi(dodoMap<short, dodoString> &headers, bool silent = false, bool autocleanContent = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");


			/**
			 * constructor
			 * @param silent defines whether to print headers in constructor or not
			 * @param autocleanFiles defines whether to clean POST files in destructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 */
			cgi(bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

#ifdef FCGI_EXT

			/**
			 * constructor
			 * @param cf defines ioCGIFast output interface
			 * @param silent defines whether to print headers in constructor or not
			 * @param autocleanFiles defines whether to clean POST files in destructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 */
			cgi(cgiFastExchange *cf, bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

			/**
			 * constructor
			 * @param cf defines ioCGIFast output interface
			 * @param headers defines headers that will be printed
			 * @param silent defines whether to print headers in constructor or not
			 * @param autocleanFiles defines whether to clean POST files in destructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 */
			cgi(cgiFastExchange *cf, dodoMap<short, dodoString> &headers, bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

#endif

			/**
			 * destructor
			 */
			virtual ~cgi();

			/**
			 * @return authentification info
			 */
			virtual __authInfo getAuthentificationInfo();
			
			/**
			 * request the authentification
			 * @param realm defines authentification request string
			 * @param type defines type of authentification[see cgiAuthTypeEnum]
			 */
			virtual void requestAuthentification(const dodoString &realm, short type=CGI_AUTHTYPE_DIGEST);
			
			/**
			 * check basic authentification
			 * @param user defines user for authentification
			 * @param password defined user's password for authentification
			 */
			virtual bool checkAuthentification(const dodoString &user, const dodoString &password);

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
			 * @note example: classObj[CGI_REQUESTMETHOD_POST]["name"]
			 */
			virtual const dodoStringMap &operator[](short method);

			/**
			 * specific variables (from POST, GET, ENV or COOKIE)
			 */
			dodoStringMap POST;                                      ///< POST variables
			dodoStringMap GET;                                       ///< GET variables
			dodoMap<short, dodoString> ENVIRONMENT;                  ///< environment variables[see cgiEnvironmentEnum]
			dodoStringMap COOKIES;                                          ///< cookies sent by browser
			dodoMap<dodoString, __cgiFile, dodoMapStringCompare> FILES;       ///< POST files
			dodoMap<short, dodoString> HEADERS; ///< headers that will be printed with printHeaders method

			/**
			 * @return value of the requested variable from POST or GET
			 * @param varName defines name of the variable
			 * @param first defines what array will be used first to search for the variable[see requestMethodEnum]
			 */
			virtual dodoString request(const dodoString &varName, short first = CGI_REQUESTMETHOD_GET);

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
			 * flushe output
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
			virtual void setCookie(const __cookie &cookie);

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
			
			short returnCode;///< HTTP return code
			dodoString returnMessage;///< HTTP return message

			bool postFilesInMem;                ///< place of POST files

			bool autocleanFiles;                ///< defines whether to clean POST files in destructor

			dodoString postFilesTmpDir;         ///< directory for POST files if on they are saved on the disk

			dodoList<__cookie> cookies;     ///< cookies
			int method;                         ///< request method

			dodoString content;                 ///< contents of the stdin for the POST request

			/**
			 * delete temp POST files that have been created
			 */
			virtual void cleanTmp();

#ifdef FCGI_EXT

			bool cgiFastSet;    ///< true if use ioCGIFast

#endif

			io *cgiIO;///< CGI I/O instance

			mutable bool headersPrinted;///< true if headers have been printed 

			static const char *environmentStatements[CGI_ENVIRONMENTSTATEMENTS];///< names of environment variables[see cgiEnvironmentEnum]
			static const dodoString responseHeaderStatements[CGI_RESPONSEHEADERSTATEMENTS];///< HTTP response headers[see cgiResponseHeaderEnum]
			static const dodoString responseStatusStatements[CGI_STATUSSTATEMENTS];///< HTTP response headers[see cgiStatusCodeEnum]
			
			/**
			 * @struct __authInfo defines authenfication information
			 */
			struct __internalAuthInfo
			{
				dodoString user;///< user name
				dodoString password;///< user password
				dodoString realm;///< explanation of the authentification request
				dodoString nonce;///< server-specified uniquely generated data
				dodoString opaque;///< server-specified uniquely generated data, which should be returned by the client unchanged
				dodoString cnonce;///< client-specified uniquely generated data
				dodoString nonceCount;///< hexadecimal count of the number of requests (including the current request) that the client has sent with the nonce value in this request
				dodoString uri;///< URI from Request-URI
				dodoString qop;///< quality of protection
				dodoString response;///< 32 hex digits. which proves that the user knows a password
				short type;///< authenfication type[see cgiAuthTypeEnum]
			};
		
			__internalAuthInfo authInfo;///< authentification information
	};
};

#endif
