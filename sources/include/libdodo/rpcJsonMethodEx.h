/***************************************************************************
 *            rpcJsonMethodEx.h
 *
 *  Mon Jul 14 2008
 *  Copyright  2008  Dmytro Milinevskyy
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

#ifndef _RPCJSONMETHODEX_H_
#define _RPCJSONMETHODEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace rpc {
        namespace json {
            /**
             * libdodo defined errors
             */
            enum methodExR {
                METHODEX_ROOTNOTANOBJECT,
                METHODEX_PARAMSNOTANARRAY,
            };

            /**
             * explanations for libdodo defined errors
             */
#define RPCJSONMETHODEX_ROOTNOTANOBJECT_STR "Root node is not an object"
#define RPCJSONMETHODEX_PARAMSNOTANARRAY_STR "Parameters are not an array"

            /**
             * IDs of functions where exception might be thrown
             */
            enum methodFunctionsID {
                METHODEX_JSONTOMETHOD,
            };
        };
    };
};
#endif
