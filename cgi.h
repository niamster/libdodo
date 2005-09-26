/***************************************************************************
 *            cgi.h
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

#include "tools.h"
#include "types.h"
#include "regexp.h"

#ifndef _CGI_H_
#define _CGI_H_

namespace dodo
{
	/**
	 * these variables will be in ENV
	 */
	__statements HTTP_ENV[30] = 
	{		
		"REQUEST_METHOD",
		"REQUEST_URI",
		
		"QUERY_STRING",
		
		"CONTENT_TYPE",
		"CONTENT_LENGTH",
				
		"HTTP_HOST",
		"HTTP_USER_AGENT",
		"HTTP_ACCEPT",
		"HTTP_ACCEPT_LANGUAGE",
		"HTTP_ACCEPT_ENCODING",
		"HTTP_ACCEPT_CHARSET",
		"HTTP_KEEP_ALIVE",
		"HTTP_CONNECTION",
		"HTTP_REFERER",
		
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
	 * structure that defines node about uploaded file
	 */
	struct cgiFilesUp
	{
		std::string name;
		std::string type;
		std::string tmp_name;
		unsigned long size;
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
			 * it's usefull if u want to modify them; defoult headers are in headers array
			 * if headers will pass to contructor u may silent set to false and modyfied headers will be printed
			 */
			cgipp(bool silent = false, assocArr &headers = assocArr());
			virtual ~cgipp();
			
			/**
			 * get cgiFilesUp by name from post request
			 */
			//virtual cgiFilesUp getFile(const std::string &varName);
			/**
			 * get method type; see requestMethodEnum
			 */ 
			virtual int getMethod();
			/**
			 * encode/decode URL
			 */
			//virtual std::string encodeURL(const std::string &url);
			//virtual std::string decodeURL(const std::string &url);
			
			/**
			 * to have access like classObj[POST]["name"]
			 * if GET_POST specified from GET will be searched first
			 * if not GET/POST/GET_POST - GET will be placed
			 */
			//virtual std::string operator[](int method);
			
			/**
			 * get specific variables (from POST, GET or ENV)
			 */
			__method METHOD_POST;
			__method METHOD_GET;
			__method ENVIRONMENT;
			
			/**
			 * merges results from POST and GET;
			 * if first is set to GET - from GET will be searched for; any else - from POST;
			 * it can prevent if 'variable names' are same as in POST as in GET
			 */
			 //virtual std::string request(const std::string &varName, int first = GET);
			 
			/**
			 * print cgi headers; u can change 'em modyfing HEADERS array
			 */
			virtual void printHeaders();
			
			assocArr HEADERS;
			/**
			 * base64 decode
			 */
			static std::string decode64(const std::string &string);
			static char hexToChar(const char &first,const char &second);	
		protected:
			
			int method;///can be any from requestMethodEnum
			std::map<std::string, cgiFilesUp> postFiles;///holds array of cgiFilesUp if one or more files were uploaded
			
			virtual void detectMethod();///writes detected method to method property
			
			/**
			 * fills properties of the class
			 */
			virtual void makeGet();
			virtual void makePost();///also fills postFiles if detected
			virtual void makeEnv();
			virtual void initHeaders(assocArr &a_headers);
			
			/**
			 * deletes temp files that were created if POST_FILES were present
			 */
			virtual void cleanTmp();
	};

}

#endif /*CGI_H_*/
