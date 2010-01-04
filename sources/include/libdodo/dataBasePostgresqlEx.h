/***************************************************************************
 *            dataBasePostgresqlEx.h
 *
 *  Fri Jan  13 2006
 *  Copyright  2006  Ni@m
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

#ifndef _DATABASEPOSTGRESQLEX_H_
#define _DATABASEPOSTGRESQLEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace data {
        namespace base {
            /**
             * libdodo defined errors
             */
            enum postgresqlExR {
                POSTGRESQLEX_NOTOPENED,
            };

            /**
             * explanations for libdodo defined errors
             */
#define DATABASEPOSTGRESQLEX_NOTOPENED_STR      "Database not opened"

            /**
             * IDs of functions where exception might be thrown
             */
            enum postgresqlFunctionsID {
                POSTGRESQLEX_CONNECT,
                POSTGRESQLEX_EXEC,
                POSTGRESQLEX_POSTGRESQL,
                POSTGRESQLEX_SETCHARSET,
                POSTGRESQLEX_CHARSET,
                POSTGRESQLEX_REQUESTFIELDSTYPES,
                POSTGRESQLEX_FETCHEDROWS,
                POSTGRESQLEX_AFFECTEDROWS,
            };
        };
    };
};
#endif
