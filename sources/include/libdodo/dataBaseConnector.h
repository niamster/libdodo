/***************************************************************************
 *            dataBaseConnector.h
 *
 *  Sun Jan  15 2005
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

#ifndef _DATABASECONNECTOR_H_
#define _DATABASECONNECTOR_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/xexec.h>

namespace dodo {
    namespace data {
        namespace base {
            /**
             * @struct __connection_options__
             * @brief defines connection options for the server
             */
            struct __connection_options__ {

                /**
                 * destructor
                 */
                virtual ~__connection_options__() = 0;
            };

            /**
             * @class condition
             * @brief defines condition for data base operations
             */
            class condition {
              public:

                /**
                 * destructor
                 */
                virtual ~condition() = 0;
            };

            /**
             * @class row
             * @brief defines rows of data base
             */
            class rows {
              public:

                /**
                 * destructor
                 */
                virtual ~rows() = 0;
            };

            /**
             * @class query
             * @brief defines data base query
             */
            class query {
              public:

                /**
                 * destructor
                 */
                virtual ~query() = 0;
            };

            /**
             * @class connector
             * @brief implements data base interface
             */
            class connector
#ifndef DATABASE_WO_XEXEC
                : public xexec
#endif
            {
              public:

                /**
                 * @enum operationEnum defines type of operation for xexec
                 */
                enum operationEnum {
                    OPERATION_CONNECT,
                    OPERATION_DISCONNECT,
                    OPERATION_SELECT,
                    OPERATION_INSERT,
                    OPERATION_UPDATE,
                    OPERATION_REMOVE,
                    OPERATION_EXEC,
                    OPERATION_FETCHEDROWS,
                };

                /**
                 * constructor
                 */
                connector();

                /**
                 * destructor
                 */
                virtual ~connector() = 0;

                /**
                 * connect to the database
                 * @param info defines information for connection to db
                 */
                virtual void connect(const __connection_options__ &info) = 0;

                /**
                 * disconnect from the database
                 */
                virtual void disconnect() = 0;

                /**
                 * @param condition defines row selection condition
                 */
                virtual void select(const condition &condition) = 0;

                /**
                 * @param rows defines rows for insertion into data base
                 * @param condition defines row insertion condition
                 */
                virtual void insert(const rows      &rows,
                                    const condition &condition) = 0;

                /**
                 * @param rows defines values of row for update
                 * @param condition defines row update condition
                 */
                virtual void update(const rows      &rows,
                                    const condition &condition) = 0;

                /**
                 * @param condition defines row remove condition
                 */
                virtual void remove(const condition &condition) = 0;

                /**
                 * @param rows defines rows got from the request
                 */
                virtual void fetchedRows(rows &rows) const = 0;

                /**
                 * execute request for data base
                 * @param query contains query for data base
                 */
                virtual void exec(const query &query) = 0;

                /**
                 * execute constructed request for data base
                 */
                virtual void exec() = 0;

                bool reconnect; ///< if true tries to reconect in case when `exec` failed with connection error[true by default]
            };
        };
    };
};
#endif
