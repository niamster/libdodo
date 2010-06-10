/***************************************************************************
 *            dataBaseSqlite.h
 *
 *  Sat Dec 10 2005
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

#ifndef _DATABASESQLITE_H_
#define _DATABASESQLITE_H_ 1

#include <libdodo/directives.h>

#ifdef SQLITE3_EXT
#include <libdodo/types.h>
#include <libdodo/dataBaseSqlConstructor.h>

namespace dodo {
    namespace data {
        namespace base {
            struct __sqlite__;

            /**
             * @class sqlite
             * @brief provides an interface to sqlite db
             */
            class sqlite : public sql::constructor {
              public:

                /**
                 * @struct __connection_options__
                 * @brief defines connection options for the server
                 */
                struct __connection_options__ : public data::base::__connection_options__ {
                    /**
                     * constructor
                     */
                    __connection_options__();

                    /**
                     * constructor
                     * @param path defines path to db or unix socket
                     */
                    __connection_options__(const            dodo::string &path);

                    dodo::string   path;      ///< path to db or unix socket
                };

              private:

                /**
                 * copy constructor
                 * @note to prevent copying
                 */
                sqlite(sqlite &);

              public:

                /**
                 * constructor
                 */
                sqlite();

                /**
                 * constructor
                 * @param info defines information for connection to db
                 */
                sqlite(const data::base::__connection_options__ &info);

                /**
                 * destructor
                 */
                virtual ~sqlite();

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
                 * @return amount of affected rows from the evaluated request
                 */
                virtual unsigned int affectedRows() const;

                /**
                 * @return amount of received rows from the evaluated request
                 */
                virtual unsigned int fetchedRows() const;

                /**
                 * @param rows defines rows got from the request
                 */
                virtual void fetchedRows(data::base::rows &rows) const;

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
                 * @param rows defines rows for insertion into data base
                 * @param condition defines row insertion condition
                 */
                virtual void insert(const data::base::rows      &rows,
                                    const data::base::condition &condition);

                /**
                 * @param rows defines values of row for update
                 * @param condition defines row update condition
                 */
                virtual void update(const data::base::rows      &rows,
                                    const data::base::condition &condition);

              protected:

                /**
                 * @return insert SQL statement
                 */
                virtual dodo::string insert();

                /**
                 * @return update SQL statement
                 */
                virtual dodo::string update();

                /**
                 * @struct __blob__
                 * @brief defines blob value
                 */
                struct __blob__ {
                    unsigned int     reference;     ///< reference in request
                    const dodo::string *value;        ///< pointer to blob value
                };

                dodoList<__blob__> blobs;           ///< references to blob data

              private:

                __sqlite__ *handle;                 ///< DB handle

                __connection_options__ info; ///< DB connection information
            };
        };
    };
};
#endif
#endif
