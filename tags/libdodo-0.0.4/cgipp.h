/***************************************************************************
 *            cgipp.h
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

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "tools.h"
#include "types.h"
#include "cgippEx.h"

#ifndef _CGIPP_H_
#define _CGIPP_H_

namespace dodo
{
	/**
	 * these variables will be in ENV
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
		"SCRIPT_NAME",

	};
	/**
	 * headers that will be printed calling prinHeaders;
	 * u can change them or add yours modifin' HEADERS array;
	 * "Content-type",
	 * "X-Powered-By"
	 */
	/**
	 * errors that could happen durin' savin' postFile
	 */ 
	enum errPostFileEnum
	{
		NONE,///everythin' is OK
		ACCESS_DENY,///no permissions to create file
		BAD_FILE_NAME,///too long
		NO_SPACE
	}; 
	/**
	 * structure that defines node about uploaded file
	 */
	struct __cgiFilesUp
	{
		std::string name;
		std::string type;
		std::string tmp_name;
		unsigned long size;
		int error;///see 
	};
	/**
	 * 
	 */
	struct __cookies
	{
		__cookies(bool secure);
		
		std::string name;
		std::string value;
		
		std::string exDate;///date of expire
		std::string path;
		std::string domain;
		bool secure;///false by default
	}; 
	/**
	 * do not forget trailing symbol!!! for OS comliance!
	 */
	static char *post_files_tmp_dir = "/tmp/";

	/**
	 * request type
	 */
	enum requestMethodEnum
	{
		GET,
		POST,
		GET_POST///if POST but page was callesd with ?name=value&....
	};

	/**
	 * class to provide simple work with info that passes to cgi
	 * u can get POST/GET variable; any ENV variable;
	 * BE CAREFULL:
	 * 	when usin' POST:
	 * 		POST, whaen script was called contains in stdin; 
	 * 		do not read stdin before contructin' object of this class!
	 * 		please contruct object of this class in the begin of your program; it will prevent any situations;
	 */
	class cgipp
	{
		friend class cgippEx;
		public:
			
			/**
			 * class like a duck for std::map<std::string, std::string> but controlling varName
			 * if varName's value is not defined - '_undefined_' will be returned
			 */
			class __method
			{
				friend class cgipp;
				public:
					/**
					 * overloaded operator
					 */
					virtual std::string operator[](const std::string &varName);
					
				protected:
				
					assocArr methodArr;
			};
		
			/**
			 * constructor/destructor
			 */
			/**
			 * override/add some headers to outputtin'
			 * if silent set to true, no header will be printed durin' constructin'; u may call printHeadrs later.
			 * it's usefull if u want to modify them; default headers are in headers array; or u want to set cookies;
			 * if headers will pass to contructor u may silent set to false and modyfied headers will be printed
			 */
			cgipp(bool silent = false, assocArr &headers = assocArr());
			virtual ~cgipp();
			
			/**
			 * get cgiFilesUp by name from post request
			 */
			virtual __cgiFilesUp getFile(const std::string &varName) const;
			/**
			 * get method type; see requestMethodEnum
			 */ 
			virtual int getMethod() const;
			
			/**
			 * to have access like classObj[POST]["name"]
			 */
			virtual dodo::cgipp::__method &operator[](int method) const;
			
			/**
			 * get specific variables (from POST, GET, ENV or COOKIE)
			 */
			mutable __method METHOD_POST;
			mutable __method METHOD_GET;
			mutable __method ENVIRONMENT;
			mutable __method COOKIES;
			
			/**
			 * merges results from POST and GET;
			 * if first is set to GET - from GET will be searched for; any else - from POST;
			 * it can prevent if 'variable names' are same as in POST as in GET
			 */
			 virtual std::string request(const std::string &varName, int first = GET) const;
			 
			/**
			 * print cgi headers; u can change 'em modyfing HEADERS array
			 * cookie header's part is prits too!!!
			 */
			virtual void printHeaders() const;
			
			mutable assocArr HEADERS;
			/**
			 * base64 decode
			 */
			static std::string decode64(const std::string &string);
			static std::string encode64(const std::string &string);
			static char hexToChar(const char &first, const char &second);
			static std::string charToHex(const char &first);
			
			/**
			 * sets cookie;
			 */
			virtual void setCookie(const std::string &name, const std::string &value, const std::string &exDate=__string__, const std::string &path=__string__, const std::string &domain=__string__, bool secure=false) const;
		protected:
			/**
			 * cookies specied data
			 */
			mutable std::vector<__cookies> cookiesSet;
			mutable int method;///can be any from requestMethodEnum
			mutable std::map<std::string, __cgiFilesUp> postFiles;///holds array of cgiFilesUp if one or more files were uploaded
			
			virtual void detectMethod() const;///writes detected method to method property
			
			/**
			 * fills properties of the class
			 */
			virtual void makePost() const;///also fills postFiles if detected
			virtual void makeEnv() const;
			virtual void initHeaders(assocArr &a_headers) const;
			
			/**
			 * deletes temp files that were created if POST_FILES were present
			 */
			virtual void cleanTmp() const;
			
			virtual void make(__method &val,const std::string &string, char *delim = "&") const;
	};

}

#endif /*CGI_H_*/
