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
#include <libdodo/cgiFast.h>
#include <libdodo/ioSTD.h>

namespace dodo
{

#define HTTP_ENVIRONMENT_SIZE    34

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
		CGI_ENVIRONMENT_HTTPHOST,
		CGI_ENVIRONMENT_HTTPUSERAGENT,
		CGI_ENVIRONMENT_HTTPCOOKIE,
		CGI_ENVIRONMENT_HTTPACCEPT,
		CGI_ENVIRONMENT_HTTPACCEPTLANGUAGE,
		CGI_ENVIRONMENT_HTTPACCEPTENCODING,
		CGI_ENVIRONMENT_HTTPACCEPTCHARSET,
		CGI_ENVIRONMENT_HTTPKEEPALIVE,
		CGI_ENVIRONMENT_HTTPCONNECTION,
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
		dodoString buf;                 ///< contains file data[if file is stored in the memory]
	};

	/**
	 * @struct __cookies describes cookie that would be sent to the client browser
	 */
	struct __cookie
	{
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

		dodoString exDate;  ///< expiration date
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
		CGI_REQUESTMETHOD_GET_POST ///< if POST method and request URL also contains ?name=value&.... format data
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
			 * @param autocleanContent indicates whether to clean contents of stdin got for POST request in constructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 */
			cgi(dodoStringMap &headers, bool silent = false, bool autocleanContent = true, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");


			/**
			 * constructor
			 * @param silent defines whether to print headers in constructor or not
			 * @param autocleanFiles defines whether to clean POST files in destructor
			 * @param autocleanContent indicates whether to clean contents of stdin got for POST request in constructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 */
			cgi(bool silent = false, bool autocleanContent = true, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

#ifdef FCGI_EXT

			/**
			 * constructor
			 * @param cf defines ioCGIFast output interface
			 * @param silent defines whether to print headers in constructor or not
			 * @param autocleanFiles defines whether to clean POST files in destructor
			 * @param autocleanContent indicates whether to clean contents of stdin got for POST request in constructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 */
			cgi(ioCgiFastExchange *cf, bool silent = false, bool autocleanContent = true, bool autocleanFiles = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

			/**
			 * constructor
			 * @param cf defines ioCGIFast output interface
			 * @param headers defines headers that will be printed
			 * @param silent defines whether to print headers in constructor or not
			 * @param autocleanFiles defines whether to clean POST files in destructor
			 * @param autocleanContent indicates whether to clean contents of stdin got for POST request in constructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 */
			cgi(ioCgiFastExchange *cf, dodoStringMap &headers, bool silent = false, bool autocleanFiles = true, bool autocleanContent = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

#endif

			/**
			 * destructor
			 */
			virtual ~cgi();

			/**
			 * @return contents of stdin got for the POST request
			 */
			dodoString getContent();

			/**
			 * clears contents of stdin got for the POST request
			 */
			void clearContent();

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
			dodoMap<dodoString, __cgiFile, stringTools::equal> FILES;       ///< POST files

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

			dodoStringMap HEADERS; ///< headers that will be printed with printHeaders method

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
			 * write detected method to method class property
			 */
			virtual void detectMethod();

			/**
			 * fill POST variables and files if defined
			 */
			void makePost();

			/**
			 * get contents of stdin for the POST request
			 */
			void makeContent();

			/**
			 * get environment variables
			 */
			virtual void makeEnv();

			/**
			 * initiate HEADERS class property with given tuples name->value
			 * @param headers defines init headers
			 */
			virtual void initHeaders(dodoStringMap &headers);

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

			static const char *environmentStatements[HTTP_ENVIRONMENT_SIZE];///< names of environment variables 
	};
};

#endif
