/***************************************************************************
 *            cgiDialogue.h
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

#ifndef _CGIDIALOGUE_H_
#define _CGIDIALOGUE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/cgi.h>

namespace dodo
{
	namespace cgi
	{
		class exchange;

		/**
		 * @note
		 * headers that will be printed calling prinHeaders method:
		 * "Content-type",
		 * "X-Powered-By"
		 * you can change them or add extra using HEADERS dialogue class property
		 */

		/**
		 * @class dialogue
		 * @brief provides CGI request-response functionality
		 * @note do not read stdin before contructing object of this class!
		 * object of this class should be contructed in the early begin
		 */
		class dialogue
		{
		  public:

			/**
			 * constructor
			 * @param io defines I/O interface
			 * @param autocleanFiles defines whether to clean POST files in destructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 * @note header won't be printed until print/printStream/desctructor is called
			 */
			dialogue(exchange   &io,
					 bool       autocleanFiles = true,
					 bool       postFilesInMem = true,
					 dodoString postFilesTmpDir = "/tmp/");

			/**
			 * constructor
			 * @param io defines I/O interface
			 * @param silent defines whether to print headers in constructor or not
			 * @param autocleanFiles defines whether to clean POST files in destructor
			 * @param postFilesInMem defines place of POST files[disk or memory]
			 * @param postFilesTmpDir defines directory for POST files if on they are saved on the disk
			 * @note headers will be printed in contructor
			 */
			dialogue(exchange &io,
					 dodoMap<short, dodoString> &headers,
					 bool autocleanFiles = true,
					 bool postFilesInMem = true,
					 dodoString postFilesTmpDir = "/tmp/");

			/**
			 * destructor
			 */
			virtual ~dialogue();

			/**
			 * @return authentication info
			 */
			virtual __cgiAuthInfo__ getAuthenticationInfo();

			/**
			 * request the authentication
			 * @param realm defines authentication request string
			 * @param type defines type of authentication[see cgiAuthTypeEnum]
			 */
			virtual void requestAuthentication(const dodoString &realm,
											   short            type = CGI_AUTHTYPE_DIGEST);

			/**
			 * check authentication
			 * @return true if authentication successfull
			 * @param user defines user for authentication
			 * @param password defined user's password for authentication
			 */
			virtual bool isAuthenticated(const dodoString &user,
										 const dodoString &password);

			/**
			 * set response code and message
			 * @param code defines return code[see cgiStatusCodeEnum]
			 */
			virtual void setResponseStatus(short code);

			/**
			 * @return method type[see cgiRequestMethodEnum]
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
			dodoStringMap POST;                                         ///< POST variables
			dodoStringMap GET;                                          ///< GET variables
			mutable dodoMap<short, dodoString> ENVIRONMENT;             ///< environment variables[see cgiEnvironmentEnum]
			dodoStringMap COOKIES;                                      ///< cookies sent by browser
			dodoMap<dodoString, file, dodoMapStringCompare> FILES; ///< POST files

			dodoString content;                                         ///< contents of the stdin for the POST request

			dodoMap<short, dodoString> HEADERS;                         ///< headers that will be printed with printHeaders method

			/**
			 * @return value of the requested variable from POST and GET
			 * @param varName defines name of the variable
			 * @note searches in GET first
			 */
			virtual dodoString request(const dodoString &varName);

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
			virtual void flush() const;

			/**
			 * set cookie
			 * @param cookie defines the cookie
			 * @note cookies are printed with printHeaders method
			 */
			virtual void setCookie(const cookie &cookie);

			/**
			 * @return charset of the request
			 */
			virtual dodoString getCharset();

			/**
			 * cast to exchange *
			 * @return I/O interface
			 * @note headers are printed before the cast
			 */
			virtual operator exchange*();

		  private:

			/**
			 * copy constructor
			 * @note to prevent copying
			 */
			dialogue(dialogue &);

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
			virtual void makeKeyValue(dodoStringMap    &val,
									  const dodoString &string,
									  const char       *delim = "&");

			/**
			 * print cgi headers
			 * @note prints cookies also
			 */
			virtual void printHeaders() const;

		  private:

			short returnCode;                                                                   ///< HTTP return code
			dodoString returnMessage;                                                           ///< HTTP return message

			bool postFilesInMem;                                                                ///< place of POST files
			bool autocleanFiles;                                                                ///< defines whether to clean POST files in destructor
			dodoString postFilesTmpDir;                                                         ///< directory for POST files if on they are saved on the disk

			dodoList<cookie> cookies;                                                    ///< cookies
			int method;                                                                         ///< request method

			dodoStringArray contenTypeExtensions;                                               ///< contains contentype extension[boundary, modification-date, etc]

			/**
			 * delete temp POST files that have been created
			 */
			virtual void cleanTmp();

			exchange &cgiIO;                                                                    ///< CGI I/O instance

			mutable bool headersPrinted;                                                        ///< true if headers have been printed

			static const char *environmentStatements[CGI_ENVIRONMENTSTATEMENTS];                ///< names of environment variables[see cgiEnvironmentEnum]
			static const dodoString responseHeaderStatements[CGI_RESPONSEHEADERSTATEMENTSD];    ///< HTTP response headers[see cgiResponseHeaderEnum]
			static const dodoString responseStatusStatements[CGI_STATUSSTATEMENTS];             ///< HTTP response headers[see cgiStatusCodeEnum]

			/**
			 * @struct __authInfo__ defines authenfication information
			 */
			struct __authInfo__
			{
				dodoString user;                                                                ///< user name
				dodoString password;                                                            ///< user password
				dodoString realm;                                                               ///< explanation of the authentication request
				dodoString nonce;                                                               ///< dialogue-specified uniquely generated data
				dodoString opaque;                                                              ///< dialogue-specified uniquely generated data, which should be returned by the dialogue unchanged
				dodoString cnonce;                                                              ///< dialogue-specified uniquely generated data
				dodoString nonceCount;                                                          ///< hexadecimal count of the number of requests (including the current request) that the server has sent with the nonce value in this request
				dodoString uri;                                                                 ///< URI from Request-URI
				dodoString qop;                                                                 ///< quality of protection
				dodoString response;                                                            ///< 32 hex digits. which proves that the user knows a password
				short      type;                                                                ///< authenfication type[see cgiAuthTypeEnum]
			};

			__authInfo__ authInfo;                                                        ///< authentication information
		};
	};
};

#endif
