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

#include <libdodo/tools.h>
#include <libdodo/types.h>
#include <libdodo/dodoMap.h>
#include <libdodo/cgiFast.h>
#include <libdodo/ioSTD.h>

namespace dodo
{

	#define HTTP_ENV_SIZE    34

	/**
	 * @var array of names that would be in ENV array[environment of http demon]
	 */
	const __statements HTTP_ENV[HTTP_ENV_SIZE] =
	{
		{ "REQUEST_METHOD"            },
		{ "REQUEST_URI"               },

		{ "QUERY_STRING"              },

		{ "CONTENT_TYPE"              },
		{ "CONTENT_LENGTH"            },
		{ "CONTENT_IOSOCKETOPTIONS_TRANSFER_ENCODING" },

		{ "HTTP_HOST"                 },
		{ "HTTP_USER_AGENT"           },
		{ "HTTP_COOKIE"               },
		{ "HTTP_ACCEPT"               },
		{ "HTTP_ACCEPT_LANGUAGE"      },
		{ "HTTP_ACCEPT_ENCODING"      },
		{ "HTTP_ACCEPT_CHARSET"       },
		{ "HTTP_KEEP_ALIVE"           },
		{ "HTTP_CONNECTION"           },
		{ "HTTP_REFERER"              },
		{ "HTTP_VIA"                  },
		{ "HTTP_X_FORWARDED_FOR"      },

		{ "REMOTE_ADDR"               },
		{ "REMOTE_PORT"               },

		{ "REDIRECT_STATUS"           },
		{ "REDIRECT_QUERY_STRING"     },
		{ "REDIRECT_URL"              },

		{ "GATEWAY_INTERFACE"         },

		{ "PATH"                      },

		{ "SERVER_SIGNATURE"          },
		{ "SERVER_SOFTWARE"           },
		{ "SERVER_NAME"               },
		{ "SERVER_ADDR"               },
		{ "SERVER_PORT"               },
		{ "SERVER_ADMIN"              },
		{ "SERVER_PROTOCOL"           },

		{ "SCRIPT_FILENAME"           },
		{ "SCRIPT_NAME"               }

	};

	/**
	 * @enum errPostFileEnum describes errors that could happen durin' savin' POST file
	 */
	enum postFileErrEnum
	{
		CGI_POSTFILEERR_NONE,           ///< everythin' is OK
		CGI_POSTFILEERR_ACCESS_DENY,    ///< permissions error
		CGI_POSTFILEERR_BAD_FILE_NAME,  ///< filename too long
		CGI_POSTFILEERR_NO_SPACE        ///< not enough space to save the file
	};

	/**
	 * @struct __cgiFilesUp
	 * defines node about uploaded POST file
	 */
	struct __cgiFile
	{
		/**
		 * constructor
		 */
		__cgiFile();
		dodoString name;        ///< real name of the file
		dodoString type;        ///< MIME type
		FILE *fp;               ///< pointer to opened file[any case - in memory or on disk]
		dodoString tmp_name;    ///< path where it was temporary saved[empty, if file stored in memory]
		unsigned long size;     ///< size of the file
		int error;              ///< indicates if error occured
		void *buf;              ///< contains file data[if file stored in memory]
	};

	/**
	 * @struct __cookies
	 * describes cookie that would be sent to browser
	 */
	struct __cookies
	{
		/**
		 * constructor
		 */
		__cookies(bool secure);

		/**
		 * constructor
		 * @note  defines struct data with user data
		 */
		__cookies(const dodoString &name, const dodoString &value, const dodoString &exDate, const dodoString &path, const dodoString &domain, bool secure);

		dodoString name;    ///< name of cookie
		dodoString value;   ///< value of cookie

		dodoString exDate;  ///< date of expire
		dodoString path;    ///< cookie's path
		dodoString domain;  ///< cookie's domain
		bool secure;        ///< indicates if cookie is secure; false by default
	};

	/**
	 * @enum requestMethodEnum indicates request type
	 */
	enum requestMethodEnum
	{
		CGI_REQUESTMETHOD_GET,
		CGI_REQUESTMETHOD_POST,
		CGI_REQUESTMETHOD_GET_POST ///< if POST but data tranferred in ?name=value&.... format
	};

	/**
	 * @note
	 * headers that will be printed calling prinHeaders method:
	 * "Content-type",
	 * "X-Powered-By"
	 * u can change them or add yours modifin' HEADERS array;
	 */

	/**
	 * @class cgi
	 * provides simple work with info that passes to cgi program
	 * u can get POST/GET variables; any ENV variable;
	 * @note when usin' POST:
	 * 		POST is in stdin;
	 * 		do not read stdin before contructin' object of this class!
	 * 		please contruct object of this class in the begin of your program;
	 * 		it will prevent any situations;
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
			 * @param silent [false by default];if is set to true, no header will be printed during constructing; you may call printHeaders method later.
			 * @param headers contains array of headers that would pe printed; it's usefull if you set silent=false[prin headers during contructing]
			 * @param autoclearContent indicates whether to clean content of request in constructor
			 * @param postFilesInMem indicates where to place POST files[disk or memory]
			 * @param postFilesTmpDir indicates where to place POST files if on disk
			 *
			 * @note you cant print headers after they have been printed with printHeaders method
			 */
			cgi(bool silent = false, dodoStringMap &headers = __dodostringmap__, bool autoclearContent = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

			#ifdef FCGI_EXT

			/**
			 * constructor
			 * @param cf describes output interface
			 * @param silent [false by default];if is set to true, no header will be printed during constructing; you may call printHeaders method later.
			 * @param headers contains array of headers that would pe printed; it's usefull if you set silent=false[prin headers during contructing]
			 * @param autoclearContent indicates whether to clean content of request in constructor
			 * @param postFilesInMem indicates where to place POST files[disk or memory]
			 * @param postFilesTmpDir indicates where to place POST files if on disk
			 *
			 * @note you cant print headers after they have been printed with printHeaders method
			 */
			cgi(cgiFastIO *cf, bool silent = false, dodoStringMap &headers = __dodostringmap__, bool autoclearContent = true, bool postFilesInMem = true, dodoString postFilesTmpDir = "/tmp/");

			#endif

			/**
			 * destructor
			 */
			virtual ~cgi();

			/**
			 * @return content of the request
			 */
			dodoString getContent();

			/**
			 * clears content of the request
			 */
			void clearContent();

			/**
			 * @return method type
			 */
			virtual int getMethod() const;

			/**
			 * @return reference to array with given method[see requestMethodEnum]
			 * @param method is indicates what reference would be returned
			 * example: classObj[POST]["name"]
			 */
			virtual const dodoStringMap &operator[](short method);

			/**
			 * specific variables (from POST, GET, ENV or COOKIE)
			 */
			dodoStringMap METHOD_POST;                                      ///< array of POST variables
			dodoStringMap METHOD_GET;                                       ///< array of GET variables
			dodoStringMap ENVIRONMENT;                                      ///< environment variables
			dodoStringMap COOKIES;                                          ///< coockes sent by browser
			dodoMap<dodoString, __cgiFile, stringTools::equal> FILES;    ///< array of POST files, if one or more files were uploaded

			/**
			 * @return value of requested variable from POST or GET
			 * @param varName name of the variable
			 * @param first indicates what array will be searched first[see requestMethodEnum]
			 */
			virtual dodoString request(const dodoString &varName, short first = CGI_REQUESTMETHOD_GET);

			/**
			 * prints cgi headers;
			 * u can change them modyfing HEADERS array or passing into constructor
			 * cookies prints too
			 */
			virtual void printHeaders() const;

			dodoStringMap HEADERS; ///< array of header that will be printed with printHeaders method

			/**
			 * sends buf to output
			 * @param buf describes what to send to user
			 */
			virtual void print(const dodoString &buf);

			/**
			 * flushes output
			 */
			virtual void flush();

			/**
			 * sets cookie. the cookies are printed with printHeaders method
			 *
			 * @param name is name of cookie
			 * @param value is value of cookie
			 * @param exDate is date of expire
			 * @param path is cookie's path
			 * @param domain is cookie's domain
			 * @param secure is indicates if cookie is secure; false by default
			 */
			virtual void setCookie(const dodoString &name, const dodoString &value, const dodoString &exDate = __dodostring__, const dodoString &path = __dodostring__, const dodoString &domain = __dodostring__, bool secure = false);

			virtual void setCookie(const __cookies &cookie);

		protected:

			/**
			 * writes detected method to method property
			 */
			virtual void detectMethod();

			/**
			 * fills POST variable [also files if defined]
			 */
			void makePost();

			/**
			 * gets contents of the request
			 */
			void makeContent();

			/**
			 * gets info about environment
			 */
			virtual void makeEnv();

			/**
			 * initiates headers with given headers; printed with printHeaders method
			 */
			virtual void initHeaders(dodoStringMap &headers);

			/**
			 * processes :
			 * from : smth1=1`delim`smth2=2
			 * to : val["smth1"]=1; val["smth2"]=2;
			 *
			 * @param val will be filled with processed values
			 * @param string contains string to process
			 * @param delim indicates format of delimiter
			 */
			virtual void make(dodoStringMap &val, const dodoString &string, const char *delim = "&");

		private:

			bool postFilesInMem;                ///< where POST files stored

			bool autoclearContent;              ///< clear content of the request after processing

			dodoString postFilesTmpDir;         ///< path of dir, where POST files will be temporary saved

			std::list<__cookies> cookiesSet;    ///< array of cookies nodes
			int method;                         ///< method that received program

			dodoString content;                 ///< content of the request

			/**
			 * deletes temp files that were created if POST files were present
			 */
			virtual void cleanTmp();

			#ifdef FCGI_EXT

			bool cgiFastSet;    ///< indicates whether cgiFast was set

			cgiFastIO *cf;     ///< pointer to cgiFast class

			#endif

			ioSTD *fstd;
	};

};

#endif
