/***************************************************************************
 *            cgiClient.h
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

#ifndef _CGICLIENT_H_
#define _CGICLIENT_H_

#include <libdodo/directives.h>

#include <string.h>
#include <stdlib.h>

#include <libdodo/cgiClientEx.h>
#include <libdodo/tools.h>
#include <libdodo/types.h>
#include <libdodo/dodoMap.h>
#include <libdodo/toolsString.h>
#include <libdodo/cgiFastClient.h>
#include <libdodo/ioSTD.h>

namespace dodo
{
	class rpcXmlCgiServer;

	namespace cgi
	{
#define CLIENT_ENVIRONMENTSTATEMENTS    38
	
		/**
		 * @enum clientEnvironmentEnum defines CLIENT environment
		 */
		enum clientEnvironmentEnum
		{
			CLIENT_ENVIRONMENT_REQUESTMETHOD,
			CLIENT_ENVIRONMENT_REQUESTURI,
			CLIENT_ENVIRONMENT_QUERYSTRING,
			CLIENT_ENVIRONMENT_CONTENTTYPE,
			CLIENT_ENVIRONMENT_CONTENTLENGTH,
			CLIENT_ENVIRONMENT_CONTENTTRANSFERENCODING,
			CLIENT_ENVIRONMENT_HTTPAUTHORIZATION,///< authentication credentials for HTTP authentication
			CLIENT_ENVIRONMENT_HTTPHOST,
			CLIENT_ENVIRONMENT_HTTPUSERAGENT,///< the user agent string of the user agent
			CLIENT_ENVIRONMENT_HTTPCOOKIE,///< the user agent string of the user agent
			CLIENT_ENVIRONMENT_HTTPIFMODIFIEDSINCE,///< allows a 304 Not Modified to be returned
			CLIENT_ENVIRONMENT_HTTPDATE,///< the date and time that the message was sent
			CLIENT_ENVIRONMENT_HTTPACCEPT,///< content-types that are acceptable by client
			CLIENT_ENVIRONMENT_HTTPACCEPTLANGUAGE,///< acceptable languages for response
			CLIENT_ENVIRONMENT_HTTPACCEPTENCODING,///< acceptable encodings
			CLIENT_ENVIRONMENT_HTTPACCEPTCHARSET,///< character sets that are acceptable
			CLIENT_ENVIRONMENT_HTTPACCEPTRANGES,///< character sets that are acceptable
			CLIENT_ENVIRONMENT_HTTPKEEPALIVE,
			CLIENT_ENVIRONMENT_HTTPCONNECTION,///< what type of connection the user-agent would prefer
			CLIENT_ENVIRONMENT_HTTPREFERER,
			CLIENT_ENVIRONMENT_HTTPVIA,
			CLIENT_ENVIRONMENT_HTTPXFORWARDEDFOR,
			CLIENT_ENVIRONMENT_REMOTEADDR,
			CLIENT_ENVIRONMENT_REMOTEPORT,
			CLIENT_ENVIRONMENT_REDIRECTSTATUS,
			CLIENT_ENVIRONMENT_REDIRECTQUERYSTRING,
			CLIENT_ENVIRONMENT_REDIRECTURL,
			CLIENT_ENVIRONMENT_GATEWAYINTERFACE,
			CLIENT_ENVIRONMENT_PATH,
			CLIENT_ENVIRONMENT_SERVERSIGNATURE,
			CLIENT_ENVIRONMENT_SERVERSOFTWARE,
			CLIENT_ENVIRONMENT_SERVERNAME,
			CLIENT_ENVIRONMENT_SERVERADDR,
			CLIENT_ENVIRONMENT_SERVERPORT,
			CLIENT_ENVIRONMENT_SERVERADMIN,
			CLIENT_ENVIRONMENT_SERVERPROTOCOL,
			CLIENT_ENVIRONMENT_SCRIPTFILENAME,
			CLIENT_ENVIRONMENT_SCRIPTNAME,
		};
	
	#define CLIENT_RESPONSEHEADERSTATEMENTS 18
		
		/**
		 * @enum clientResponseHeaderEnum defines HTTP response headers
		 */
		enum clientResponseHeaderEnum
		{
			CLIENT_RESPONSEHEADER_ACCEPTRANGES,///< what partial content range types this server supports
			CLIENT_RESPONSEHEADER_AGE,///< the age the object has been in a proxy cache in seconds
			CLIENT_RESPONSEHEADER_ALLOW,///< valid actions for a specified resource. To be used for a 405 Method not allowed
			CLIENT_RESPONSEHEADER_CACHECONTROL,///< tells all caching mechanisms from server to client whether they may cache this object
			CLIENT_RESPONSEHEADER_CONTENTENCODING,///< the type of encoding used on the data
			CLIENT_RESPONSEHEADER_CONTENTLANGUAGE,///< the language the content is in
			CLIENT_RESPONSEHEADER_CONTENTLENGTH,///< the length of the content in bytes
			CLIENT_RESPONSEHEADER_CONTENTLOCATION,///< an alternate location for the returned data
			CLIENT_RESPONSEHEADER_CONTENTDISPOSITION,///< an opportunity to raise a "File Download" dialog box for a known MIME type
			CLIENT_RESPONSEHEADER_CONTENTMD5,///< an MD5 sum of the content of the response
			CLIENT_RESPONSEHEADER_CONTENTRANGE,///< where in a full body message this partial message belongs
			CLIENT_RESPONSEHEADER_CONTENTTYPE,///< the mime type of this content
			CLIENT_RESPONSEHEADER_DATE,///< the date and time that the message was sent
			CLIENT_RESPONSEHEADER_LASTMODIFIED,///< the last modified date for the requested object
			CLIENT_RESPONSEHEADER_LOCATION,///< used in redirection
			CLIENT_RESPONSEHEADER_SERVER,///< a name for the server
			CLIENT_RESPONSEHEADER_WWWAUTHENTICATE,///< auuthentification request
			CLIENT_RESPONSEHEADER_XPOWEREDBY,///< cgi provider
		};
	
	#define CLIENT_STATUSSTATEMENTS 40
		
		/**
		 * @enum clientStatusCodeEnum defines HTTP response status code
		 */	
		enum clientStatusCodeEnum
		{
			CLIENT_STATUSCODE_CONTINUE,///< 100
			CLIENT_STATUSCODE_SWITCHINGPROTOCOLS,///< 101
			CLIENT_STATUSCODE_OK,///< 200
			CLIENT_STATUSCODE_CREATED,///< 201
			CLIENT_STATUSCODE_ACCEPTED,///< 202
			CLIENT_STATUSCODE_NONAUTHORITATIVE,///< 203
			CLIENT_STATUSCODE_NOCONTENT,///< 204
			CLIENT_STATUSCODE_RESETCONTENT,///< 205
			CLIENT_STATUSCODE_PARTIALCONTENT,///< 206
			CLIENT_STATUSCODE_MULTIPLECHOISES,///< 300
			CLIENT_STATUSCODE_MOVEDPERMANENTLY,///< 301
			CLIENT_STATUSCODE_FOUND,///< 302
			CLIENT_STATUSCODE_SEEOTHER,///< 303
			CLIENT_STATUSCODE_NOTMODIFIED,///< 304
			CLIENT_STATUSCODE_USEPROXY,///< 305
			CLIENT_STATUSCODE_TEMPORARYREDIRECT,///< 307
			CLIENT_STATUSCODE_BADREQUEST,///< 400
			CLIENT_STATUSCODE_UNAUTHORIZED,///< 401
			CLIENT_STATUSCODE_PAYMENTREQUIRED,///< 402
			CLIENT_STATUSCODE_FORBIDDEN,///< 403
			CLIENT_STATUSCODE_NOTFOUND,///< 404
			CLIENT_STATUSCODE_METHODNOTALLOWED,///< 405
			CLIENT_STATUSCODE_NOTACCEPTABLE,///< 406
			CLIENT_STATUSCODE_PROXYAUTHENTIFICATIONREQUIRED,///< 407 
			CLIENT_STATUSCODE_REQUESTTIMEOUT,///< 408
			CLIENT_STATUSCODE_CONFLICT,///< 409
			CLIENT_STATUSCODE_GONE,///< 410 
			CLIENT_STATUSCODE_LENGTHREQUIRED,///< 411
			CLIENT_STATUSCODE_PRECONDITIONFAILED,///< 412
			CLIENT_STATUSCODE_REQUESTENTITYTOOLARGE,///< 413
			CLIENT_STATUSCODE_REQUESTURITOOLONG,///< 414
			CLIENT_STATUSCODE_UNSUPPORTEDMEDIATYPE,///< 415
			CLIENT_STATUSCODE_REQUESTRANGENOTSATISFIABLE,///< 416
			CLIENT_STATUSCODE_EXPECTATIONFAILED,///< 417
			CLIENT_STATUSCODE_INTERNALSERVERERROR,///< 500
			CLIENT_STATUSCODE_NOTIMPLEMENTED,///< 501
			CLIENT_STATUSCODE_BADGATEWAY,///< 502
			CLIENT_STATUSCODE_SERVICEUNAVAILABLE,///< 503
			CLIENT_STATUSCODE_GATEWAYTIMEOUT,///< 504
			CLIENT_STATUSCODE_HTTPVERSIONNOTSUPPORTED,///< 505
		};
	
		/**
		 * @enum clientPostFileErrEnum defines errors that might occur during fetching POST file
		 */
		enum clientPostFileErrEnum
		{
			CLIENT_POSTFILEERR_NONE,           ///< everything is OK
			CLIENT_POSTFILEERR_ACCESS_DENY,    ///< permissions error
			CLIENT_POSTFILEERR_BAD_FILE_NAME,  ///< filename is too long
			CLIENT_POSTFILEERR_NO_SPACE,        ///< not enough space to save the file
			CLIENT_POSTFILEERR_NO_FILE,        ///< file with such name not found
		};
	
		/**
		 * @struct __clientFile defines node about uploaded POST file
		 */
		struct __clientFile
		{
			/**
			 * constructor
			 */
			__clientFile();
			
			dodoString name;                ///< real name of the file
			dodoString type;                ///< MIME type
			dodoString tmp_name;            ///< path where it was temporary saved[empty, if file is stored in the memory]
			unsigned long size;             ///< size of the file
			short error;          			///< error code[see clientPostFileErrEnum]
			dodoString data;                 ///< contains file data[if file is stored in the memory]
		};
	
		/**
		 * @struct __clientCookie describes cookie that would be sent to the client browser
		 */
		struct __clientCookie
		{
			/**
			 * constructor
			 */
			__clientCookie();
			
			/**
			 * constructor
			 */
			__clientCookie(bool secure);
	
			/**
			 * constructor
			 * @note defines struct data with user data
			 */
			__clientCookie(const dodoString &name, const dodoString &value, const dodoString &exDate, const dodoString &path, const dodoString &domain, bool secure);
	
			dodoString name;    ///< name of the cookie
			dodoString value;   ///< value of the cookie
	
			dodoString expires;  ///< expiration date
			dodoString path;    ///< cookie path
			dodoString domain;  ///< cookie domain
			bool secure;        ///< defines cookie security; false by default
		};
	
		/**
		 * @enum clientRequestMethodEnum defines request type
		 */
		enum clientRequestMethodEnum
		{
			CLIENT_REQUESTMETHOD_GET,
			CLIENT_REQUESTMETHOD_POST,
			CLIENT_REQUESTMETHOD_GET_POST ///< if POST method and request Url also contains ?name=value&.... format data
		};
		
		/**
		 * @enum cgiAuthTypeEnum defines authentification types
		 */
		enum clientAuthTypeEnum
		{
			CLIENT_AUTHTYPE_NONE,
			CLIENT_AUTHTYPE_BASIC,
			CLIENT_AUTHTYPE_DIGEST
		};
		
		/**
		 * @struct __authInfo defines authenfication information
		 */
		struct __clientAuthInfo
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
		 * @class cgi provides CLIENT functionality
		 * @note do not read stdin before contructing object of this class!
		 * object of this class should be contructed in the early begin
		 */
		class client
		{
			friend class dodo::rpcXmlCgiServer;
			
			private:
	
				/**
				 * copy constructor
				 * to prevent copying
				 */
				client(client &ct);
	
			public:
	
				/**
				 * constructor
				 * @param headers defines headers that will be printed
				 * @param silent defines whether to print headers in constructor or not
				 * @param autocleanFiles defines whether to clean POST files in destructor
				 * @param postFilesInMem defines place of POST files[disk or memory]
				 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
				 */
				client(dodoMap<short, dodoString> &headers, bool silent = false, bool autocleanContent = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");
	
	
				/**
				 * constructor
				 * @param silent defines whether to print headers in constructor or not
				 * @param autocleanFiles defines whether to clean POST files in destructor
				 * @param postFilesInMem defines place of POST files[disk or memory]
				 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
				 */
				client(bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");
	
	#ifdef FASTCGI_EXT
	
				/**
				 * constructor
				 * @param cf defines ioCLIENTFast output interface
				 * @param silent defines whether to print headers in constructor or not
				 * @param autocleanFiles defines whether to clean POST files in destructor
				 * @param postFilesInMem defines place of POST files[disk or memory]
				 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
				 */
				client(fastClientExchange *cf, bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");
	
				/**
				 * constructor
				 * @param cf defines ioCLIENTFast output interface
				 * @param headers defines headers that will be printed
				 * @param silent defines whether to print headers in constructor or not
				 * @param autocleanFiles defines whether to clean POST files in destructor
				 * @param postFilesInMem defines place of POST files[disk or memory]
				 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
				 */
				client(fastClientExchange *cf, dodoMap<short, dodoString> &headers, bool silent = false, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");
	
	#endif
	
				/**
				 * destructor
				 */
				virtual ~client();
	
				/**
				 * @return authentification info
				 */
				virtual __clientAuthInfo getAuthentificationInfo();
				
				/**
				 * request the authentification
				 * @param realm defines authentification request string
				 * @param type defines type of authentification[see cgiAuthTypeEnum]
				 */
				virtual void requestAuthentification(const dodoString &realm, short type=CLIENT_AUTHTYPE_DIGEST);
				
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
				 * @note example: classObj[CLIENT_REQUESTMETHOD_POST]["name"]
				 */
				virtual const dodoStringMap &operator[](short method);
	
				/**
				 * specific variables (from POST, GET, ENV or COOKIE)
				 */
				dodoStringMap POST;                                      ///< POST variables
				dodoStringMap GET;                                       ///< GET variables
				mutable dodoMap<short, dodoString> ENVIRONMENT;                  ///< environment variables[see cgiEnvironmentEnum]
				dodoStringMap COOKIES;                                          ///< cookies sent by browser
				dodoMap<dodoString, __clientFile, dodoMapStringCompare> FILES;       ///< POST files
				dodoMap<short, dodoString> HEADERS; ///< headers that will be printed with printHeaders method
	
				/**
				 * @return value of the requested variable from POST or GET
				 * @param varName defines name of the variable
				 * @param first defines what array will be used first to search for the variable[see requestMethodEnum]
				 */
				virtual dodoString request(const dodoString &varName, short first = CLIENT_REQUESTMETHOD_GET);
	
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
				virtual void setCookie(const __clientCookie &cookie);
	
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
	
				dodoList<__clientCookie> cookies;     ///< cookies
				int method;                         ///< request method
	
				dodoString content;                 ///< contents of the stdin for the POST request
	
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
	
				bool clientFastSet;    ///< true if use ioCLIENTFast
	
	#endif
	
				io *cgiIO;///< CLIENT I/O instance
	
				mutable bool headersPrinted;///< true if headers have been printed 
	
				static const char *environmentStatements[CLIENT_ENVIRONMENTSTATEMENTS];///< names of environment variables[see cgiEnvironmentEnum]
				static const dodoString responseHeaderStatements[CLIENT_RESPONSEHEADERSTATEMENTS];///< HTTP response headers[see cgiResponseHeaderEnum]
				static const dodoString responseStatusStatements[CLIENT_STATUSSTATEMENTS];///< HTTP response headers[see cgiStatusCodeEnum]
				
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
};

#endif
