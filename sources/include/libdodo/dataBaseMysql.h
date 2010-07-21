/***************************************************************************
 *            dataBaseMysql.h
 *
 *  Thu Apr  30 2005
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

#ifndef _DATABASEMYSQL_H_
#define _DATABASEMYSQL_H_ 1

#include <libdodo/directives.h>

#ifdef MYSQL_EXT
#include <libdodo/types.h>
#include <libdodo/dataBaseSqlConstructor.h>

namespace dodo {
    namespace data {
        namespace base {
            struct __mysql__;

            /**
             * @class mysql
             * @brief provides an interface to MySQL db
             */
            class mysql : public sql::constructor {
              public:

                /**
                 * @struct __connection_options__
                 * @brief defines connection options for the server
                 */
                struct __connection_options__ : public data::base::__connection_options__ {
                    /**
                     * @enum typeEnum defines connection type
                     */
                    enum typeEnum {
                        TYPE_COMPRESS = 2,
                        TYPE_MULTI_STATEMENTS = 4,
                    };

                    /**
                     * @struct __ssl_options__
                     * @brief defines SSL mySQL options
                     */
                    struct __ssl_options__ {
                        dodo::string key;     ///< pathname to the key file
                        dodo::string cert;    ///< pathname to the certificate file
                        dodo::string ca;      ///< pathname to the certificate authority file
                        dodo::string capath;  ///< pathname to a directory that contains trusted SSL CA certificates in pem format
                        dodo::string cipher;  ///< allowed SSL ciphers
                    };

                    /**
                     * constructor
                     */
                    __connection_options__();

                    /**
                     * constructor
                     * @param db defines name of db
                     * @param host defines host
                     * @param user defines user
                     * @param password defines password
                     * @param path defines path to db or unix socket
                     * @param port defines port
                     * @param type defines type of connection
                     */
                    __connection_options__(const dodo::string &db,
                                           const            dodo::string &host,
                                           const            dodo::string &user,
                                           const            dodo::string &password,
                                           const            dodo::string &path = __dodostring__,
                                           unsigned int              port = 0,
                                           unsigned long    type = TYPE_MULTI_STATEMENTS);

                    unsigned long type; ///< type of connection [@see __connection_options__::typeEnum]

                    dodo::string   db;        ///< database name
                    dodo::string   host;      ///< hostname
                    dodo::string   user;      ///< username
                    dodo::string   password;  ///< password
                    dodo::string   path;      ///< path to db or unix socket
                    unsigned int port;      ///< port
                    __ssl_options__ ssl;    ///< SSL options
                };

              private:

                /**
                 * copy constructor
                 * @note to prevent copying
                 */
                mysql(mysql &);

              public:

                /**
                 * constructor
                 */
                mysql();

                /**
                 * constructor
                 * @param info defines information for connection to db
                 */
                mysql(const data::base::__connection_options__ &info);

                /**
                 * destructor
                 */
                virtual ~mysql();

                /**
                 * connect to the database
                 * @param info defines information for connection to db
                 */
                virtual void connect(const data::base::__connection_options__ &info);

                /**
                 * disconnect from the database
                 */
                virtual void disconnect();

                /**
                 * automaticaly detect fields types
                 * @param table defines table for which rules will be applied
                 */
                virtual void requestFieldsTypes(const dodo::string &table);

                /**
                 * @param rows defines rows got from the request
                 */
                virtual void fetchedRows(data::base::rows &rows) const;

                /**
                 * @return amount of affected rows from the evaluated request
                 */
                virtual unsigned int affectedRows() const;

                /**
                 * @return amount of received rows from the evaluated request
                 */
                virtual unsigned int fetchedRows() const;

                /**
                 * execute request for data base
                 * @param query contains query for data base
                 */
                virtual void exec(const data::base::query &query);

                /**
                 * execute collected request for data base
                 */
                virtual void exec();

                /**
                 * set sessions charset
                 * @param charset defines charset
                 */
                void setCharset(const dodo::string &charset);

                /**
                 * @return current session charset
                 */
                dodo::string charset() const;

                /**
                 * set connection timeout
                 * @param time defines connection timeout in seconds
                 */
                void setConnectionTimeout(unsigned int time);

              private:

                __mysql__ *handle;      ///< DB handle

                __connection_options__ info; ///< DB connection information
            };
        };
    };
};
#endif
#endif
