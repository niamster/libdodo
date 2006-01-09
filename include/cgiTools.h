/***************************************************************************
 *            cgiTools.h
 *
 *  Sat Sep  17 21:55:19 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _CGITOOLS_H_
#define _CGITOOLS_H_

#include <string.h>
#include <stdlib.h>

#include <directives.h>

#include <tools.h>
#include <types.h>
#include <dodoMap.h>

namespace dodo
{
	/**
	 * @var array of names that would be in ENV array[environment of http demon]
	 */
	__statements HTTP_ENV[33] = 
	{		
		"REQUEST_METHOD",
		"REQUEST_URI",
		
		"QUERY_STRING",
		
		"CONTENT_TYPE",
		"CONTENT_LENGTH",
				
		"HTTP_HOST",
		"HTTP_USER_AGENT",
		"HTTP_COOKIE",
		"HTTP_ACCEPT",
		"HTTP_ACCEPT_LANGUAGE",
		"HTTP_ACCEPT_ENCODING",
		"HTTP_ACCEPT_CHARSET",
		"HTTP_KEEP_ALIVE",
		"HTTP_CONNECTION",
		"HTTP_REFERER",
		"HTTP_VIA",
		"HTTP_X_FORWARDED_FOR",
		
		"REMOTE_ADDR",
		"REMOTE_PORT",
				
		"REDIRECT_STATUS",
		"REDIRECT_QUERY_STRING",
		"REDIRECT_URL",
		
		"GATEWAY_INTERFACE",
				
		"PATH",
		
		"SERVER_SIGNATURE",
		"SERVER_SOFTWARE",
		"SERVER_NAME",
		"SERVER_ADDR",
		"SERVER_PORT",
		"SERVER_ADMIN",
		"SERVER_PROTOCOL",
		
		"SCRIPT_FILENAME",
		"SCRIPT_NAME"

	};
 
	/**
	 * @enum errPostFileEnum describes errors that could happen durin' savin' POST file
	 */ 
	enum errPostFileEnum
	{
		NONE,///< everythin' is OK
		ACCESS_DENY,///< permissions error
		BAD_FILE_NAME,///< filename too long
		NO_SPACE///< not enough space to save the file
	}; 
	
	/**
	 * @struct __cgiFilesUp
	 * defines node about uploaded POST file
	 */
	struct __cgiFilesUp
	{
		std::string name;///< real name of the file
		std::string type;///< MIME type
		std::string tmp_name;///< path where it was temporary saved
		unsigned long size;///< size of the file
		int error;///< indicates if error occured
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
		__cookies(const std::string &name, const std::string &value, const std::string &exDate, const std::string &path, const std::string &domain, bool secure);
		
		std::string name;///< name of cookie
		std::string value;///< value of cookie
		
		std::string exDate;///< date of expire
		std::string path;///< cookie's path
		std::string domain;///< cookie's domain
		bool secure;///< indicates if cookie is secure; false by default
	}; 

	static char *post_files_tmp_dir = "/tmp";///< path of dir, where POST files will be temporary saved

	/**
	 * @enum requestMethodEnum indicates request type
	 */
	enum requestMethodEnum
	{
		GET,
		POST,
		GET_POST///< if POST but data tranferred in ?name=value&.... format
	};

	
	/**
	 * @note
	 * headers that will be printed calling prinHeaders method:
	 * "Content-type",
	 * "X-Powered-By"
	 * u can change them or add yours modifin' HEADERS array;
	 */

	/**
	 * @class cgiTools
	 * provides simple work with info that passes to cgi program
	 * u can get POST/GET variables; any ENV variable;
	 * @note when usin' POST:
	 * 		POST is in stdin; 
	 * 		do not read stdin before contructin' object of this class!
	 * 		please contruct object of this class in the begin of your program; 
	 * 		it will prevent any situations;
	 */
	class cgiTools
	{
		
		private:
			
			/**
			 * copy constructor
			 * to prevent copying
			 */
			cgiTools(cgiTools &ct);
		
		public:
					
			/**
			 * constructor
			 * @param silent [false by default];if is set to true, no header will be printed during constructing; you may call printHeaders method later.
			 * @param headers contains array of headers that would pe printed; it's usefull if you set silent=false[prin headers during contructing]
			 * 
			 * @note you cant print headers after they have been printed with printHeaders method 
			 */
			cgiTools(bool silent = false, assocArr &headers = __assocarray__);
			
			/**
			 * destructor
			 */
			virtual ~cgiTools();
			
			/**
			 * @return cgiFilesUp
			 * @param varName is name of `input` from POST request
			 */
			virtual __cgiFilesUp getFile(const std::string &varName) const;
			
			/**
			 * @return method type
			 */ 
			virtual int getMethod() const;
			
			/**
			 * @return reference to array with given method
			 * @param method is indicates what reference would be returned
			 * example: classObj[POST]["name"]
			 */
			virtual dodoMap &operator[](requestMethodEnum method) const;
			
			/**
			 * specific variables (from POST, GET, ENV or COOKIE)
			 */
			mutable dodoMap METHOD_POST;///< array of POST variables
			mutable dodoMap METHOD_GET;///< array of GET variables
			mutable dodoMap ENVIRONMENT;///< environment variables
			mutable dodoMap COOKIES;///< coockes sent by browser
			
			/**
			 * @return value of requested variable from POST or GET
			 * @param varName name of the variable
			 * @param first indicates what array will be searched first
			 */
			 virtual std::string request(const std::string &varName, requestMethodEnum first = GET) const;
			 
			/**
			 * prints cgi headers; 
			 * u can change them modyfing HEADERS array or passing into constructor
			 * cookies prints too
			 */
			virtual void printHeaders() const;
			
			mutable assocArr HEADERS;///< array of header that will be printed with printHeaders method
			
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
			virtual void setCookie(const std::string &name, const std::string &value, const std::string &exDate=__string__, const std::string &path=__string__, const std::string &domain=__string__, bool secure=false);
		
			virtual void setCookie(const __cookies &cookie);
		
		protected:
					
			/**
			 * writes detected method to method property
			 */		
			virtual void detectMethod() const;
			
			/**
			 * fills POST variable [also files if defined]
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							makePost() const;
			
			/**
			 * gets info about environment
			 */				
			virtual void makeEnv() const;
			
			/**
			 * initiates headers with given headers; printed with printHeaders method
			 */
			virtual void initHeaders(assocArr &a_headers) const;

			/**
			 * processes :
			 * from : smth1=1`delim`smth2=2 
			 * to : val["smth1"] = 1; val["smth2"] = 2;
			 * 
			 * @param val will be filled with processed values
			 * @param string contains string to process
			 * @param delim indicates format of delimiter
			 */
			virtual void make(assocArr &val, const std::string &string, char *delim = "&") const;
		
		private:	
		
			mutable std::vector<__cookies> cookiesSet;///< array of cookies nodes
			mutable int method;///< method that recieved program
			mutable std::map<std::string, __cgiFilesUp> postFiles;///< array of POST files, if one or more files were uploaded
		
			/**
			 * deletes temp files that were created if POST files were present
			 */
			virtual void cleanTmp() const;
			
			mutable assocArr::iterator i;///< iterator
			mutable assocArr::iterator j;///< iterator
			
			mutable stringArr getPair;///< temporary storage for URL encoding
			mutable stringArr::iterator l;///<iterator for array of strings
			mutable stringArr::iterator m;///<iterator for array of strings
			
			mutable std::string met0;///< temporary stores method for request class method
			mutable std::string met1;///< temporary stores method for request class method
	};

};

#endif /*CGI_H_*/
