/***************************************************************************
 *            cgiDialogue.h
 *
 *  Sat Sep  17 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

namespace dodo {
    namespace cgi {
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
        class dialogue {
          private:

            /**
             * copy constructor
             * @note to prevent copying
             */
            dialogue(dialogue &);

          public:

            /**
             * constructor
             * @param io defines I/O interface
             * @param postSize defines of the limit of the 'multipart/form-data' CGI POST request for processing
             * @param autocleanFiles defines whether to clean POST files in destructor
             * @param filesInMem defines place of POST files[disk or memory]
             * @param tmpDir defines directory for POST files if on they are saved on the disk
             * @note header won't be printed until print/printString/desctructor is called
             */
            dialogue(exchange      &io,
                     unsigned long postSize = POST_SIZE_LIMIT,
                     bool          autocleanFiles = true,
                     bool          filesInMem = true,
                     dodo::string    tmpDir = "/tmp/");

            /**
             * constructor
             * @param io defines I/O interface
             * @param headers defines response headers
             * @param postSize defines of the limit of the 'multipart/form-data' CGI POST request for processing
             * @param autocleanFiles defines whether to clean POST files in destructor
             * @param filesInMem defines place of POST files[disk or memory]
             * @param tmpDir defines directory for POST files if on they are saved on the disk
             * @note headers will be printed in contructor
             */
            dialogue(exchange &io,
                     dodoMap<short, dodo::string> &headers,
                     unsigned long postSize = POST_SIZE_LIMIT,
                     bool autocleanFiles = true,
                     bool filesInMem = true,
                     dodo::string tmpDir = "/tmp/");

            /**
             * destructor
             */
            ~dialogue();

            /**
             * @return authentication response
             */
            cgi::__auth__ authenticationResponse();

            /**
             * request the authentication
             * @param realm defines authentication request string
             * @param type defines type of authentication, @see cgi::authEnum
             */
            void requestAuthentication(const dodo::string &realm,
                                       short            type = AUTH_DIGEST);

            /**
             * check authentication
             * @return true if authentication successfull
             * @param user defines user for authentication
             * @param password defined user's password for authentication
             */
            bool isAuthenticated(const dodo::string &user,
                                 const dodo::string &password);

            /**
             * set response code and message
             * @param code defines return code, @see cgi::statusCodeEnum
             */
            void setResponseStatus(short code);

            /**
             * @return method type, @see cgi::requestMethodEnum
             */
            int method() const;

            /**
             * @return array of request variables of given method
             * @param method defines defines type of array of request variables to return, @see cgi::requestMethodEnum
             * @note example: dialogue[cgi::REQUEST_METHOD_POST]["name"]
             */
            const dodoStringMap &operator[](short method);

            /**
             * @return value of the requested variable from POST and GET
             * @param name defines name of the variable
             * @note searches in GET first
             */
            dodo::string operator[](const dodo::string &name);


            /**
             * specific variables (from POST, GET, ENV or COOKIE)
             */
            dodoStringMap POST;                                         ///< POST variables
            dodoStringMap GET;                                          ///< GET variables
            mutable dodoMap<short, dodo::string> ENVIRONMENT;             ///< environment variables, @see cgi::environmentEnum
            dodoStringMap COOKIES;                                      ///< cookies sent by browser
            dodoMap<dodo::string, file, dodoMapStringCompare> FILES;      ///< POST files

            dodo::string content;                                         ///< contents of the stdin for the POST request

            dodoMap<short, dodo::string> HEADERS;                         ///< headers that will be printed with printHeaders method

            /**
             * print data to the output
             * @return amount in bytes of successfully printed data
             * @param data defines data that would be printed
             */
            unsigned long print(const dodo::string &data);

            /**
             * print data to the output
             * @return amount in bytes of successfully printed data
             * @param data defines data that would be printed
             * @note print until '\n' is reached
             */
            unsigned long printString(const dodo::string &data);

            /**
             * flush output
             */
            void flush() const;

            /**
             * set cookie
             * @param cookie defines the cookie
             * @note cookies are printed with printHeaders method
             */
            void setCookie(const cookie &cookie);

            /**
             * @return charset of the request
             */
            dodo::string charset();

            /**
             * cast to exchange *
             * @return I/O interface
             * @note headers are printed before the cast
             */
            operator exchange*();

          private:

            /**
             * fetch auth information
             */
            void makeAuth();

            /**
             * write detected method to method class property
             */
            void detectMethod();

            /**
             * fill POST variables and files if defined
             */
            void makePost();

            /**
             * get contents of stdin for the POST request
             * @param postSize defines of the limit of the 'multipart/form-data' CGI POST request for processing
             */
            void makeContent(unsigned long postSize);

            /**
             * get environment variables
             */
            void makeEnv();

            /**
             * initiate HEADERS class property with given tuples name->value
             * @param headers defines init headers
             */
            void initHeaders(dodoMap<short, dodo::string> &headers);

            /**
             * process serialized string of tuples key->value
             * @param values defines map that will be filled with processed tuples key->value
             * @param request defines request string to process
             * @param delim defines delimiter
             * @note
             * from : name1=value1`delim`name2=value2
             * to : val["name1"]=value1; val["name2"]=value2;
             */
            void makeKeyValue(dodoStringMap    &values,
                              const dodo::string &request,
                              const char       *delim = "&");

            /**
             * print cgi headers
             * @note prints cookies also
             */
            void printHeaders() const;

            short returnCode;                                                                           ///< HTTP return code
            dodo::string returnMessage;                                                                   ///< HTTP return message

            bool postFilesInMem;                                                                        ///< place of POST files
            bool autocleanFiles;                                                                        ///< defines whether to clean POST files in destructor
            dodo::string postFilesTmpDir;                                                                 ///< directory for POST files if on they are saved on the disk

            dodo::slList<cookie> cookies;                                                                   ///< cookies
            int requestMethod;                                                                          ///< request method

            dodoStringArray contenTypeExtensions;                                                       ///< contains contentype extension[boundary, modification-date, etc]

            /**
             * delete temp POST files that have been created
             */
            void cleanTmp();

            exchange &io;                                                                               ///< CGI I/O instance

            mutable bool headersPrinted;                                                                ///< true if headers have been printed

            static const char *environmentStatements[ENVIRONMENT_ENUMSIZE];                             ///< names of environment variables, @see cgi::environmentEnum
            static const dodo::string responseHeaderStatements[RESPONSE_HEADER_ENUMSIZE];                 ///< HTTP response headers, @see cgi::responseHeaderEnum
            static const dodo::string responseStatusStatements[STATUS_CODE_ENUMSIZE];                     ///< HTTP response headers, @see cgi::statusCodeEnum

            /**
             * @struct __auth__ defines authenfication information
             */
            struct __auth__ {
                dodo::string user;                                                                        ///< user name
                dodo::string password;                                                                    ///< user password
                dodo::string realm;                                                                       ///< explanation of the authentication request
                dodo::string nonce;                                                                       ///< dialogue-specified uniquely generated data
                dodo::string opaque;                                                                      ///< dialogue-specified uniquely generated data, which should be returned by the dialogue unchanged
                dodo::string cnonce;                                                                      ///< dialogue-specified uniquely generated data
                dodo::string nonceCount;                                                                  ///< hexadecimal count of the number of requests (including the current request) that the server has sent with the nonce value in this request
                dodo::string uri;                                                                         ///< URI from Request-URI
                dodo::string qop;                                                                         ///< quality of protection
                dodo::string response;                                                                    ///< 32 hex digits. which proves that the user knows a password
                short      type;                                                                        ///< authenfication type, @see cgi::authEnum
            };

            __auth__ authInfo;                                                                          ///< authentication information
        };
    };
};
#endif
