/***************************************************************************
 *            dataFormatJsonNodeEx.h
 *
 *  Mon Oct 20 2007
 *  Copyright  2007  Dmytro Milinevskyy
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

#ifndef _DATAFORMATJSONNODEEX_H_
#define _DATAFORMATJSONNODEEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace data {
        namespace format {
            namespace json {
                /**
                 * libdodo defined errors
                 */
                enum nodeExR {
                    NODEEX_WRONGTYPEREQUESTED,
                    NODEEX_ARRAYOUTOFRANGE,
                    NODEEX_MALFORMEDJSON,
                };

                /**
                 * explanations for libdodo defined errors
                 */
#define DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR "Wrong type was requested"
#define DATAFORMATJSONNODEEX_ARRAYOUTOFRANGE_STR    "Array key is out of range"
#define DATAFORMATJSONNODEEX_MALFORMEDJSON_STR      "Malformed json object"

                /**
                 * IDs of functions where exception might be thrown
                 */
                enum nodeFunctionsID {
                    NODEEX_BROPERATORSTRING,
                    NODEEX_BROPERATORNUMERIC,
                    NODEEX_GETSTRING,
                    NODEEX_GETBOOLEAN,
                    NODEEX_GETNUMERIC,
                    NODEEX_GETARRAY,
                    NODEEX_GETOBJECT,
                };
            };
        };
    };
};
#endif
