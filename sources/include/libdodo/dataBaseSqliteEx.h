/***************************************************************************
 *            dataBaseSqliteEx.h
 *
 *  Sat Dec 10 2005
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

#ifndef _DATABASESQLITEEX_H_
#define _DATABASESQLITEEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace data {
        namespace base {
            /**
             * libdodo defined errors
             */
            enum sqliteExR {
                SQLITEEX_NOTOPENED,
            };

            /**
             * explanations for libdodo defined errors
             */
#define DATABASESQLITEEX_NOTOPENED_STR      "Database not opened"

            /**
             * IDs of functions where exception might be thrown
             */
            enum sqliteFunctionsID {
                SQLITEEX_DISCONNECT,
                SQLITEEX_CONNECT,
                SQLITEEX_EXEC,
                SQLITEEX_SQLITE,
                SQLITEEX_FETCHEDROWS,
                SQLITEEX_AFFECTEDROWS,
                SQLITEEX_REQUESTFIELDSTYPES,
            };
        };
    };
};
#endif
