/***************************************************************************
 *            toolsNetworkEx.h
 *
 *  Mon Feb 21 2005
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

#ifndef _TOOLSNETWORKEX_H_
#define _TOOLSNETWORKEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace tools {
        /**
         * libdodo defined errors
         */
        enum networkExR {
            NETWORKEX_BADMAILHELO,
            NETWORKEX_BADMAILAUTH,
        };

        /**
         * explanations for libdodo defined errors
         */
#define TOOLSNETWORKEX_BADMAILHELO_STR    "Error occurd while sending EHLO"
#define TOOLSNETWORKEX_BADMAILAUTH_STR    "Error during authentication"

        /**
         * IDs of functions where exception might be thrown
         */
        enum networkFunctionsID {
            NETWORKEX_HOST,
            NETWORKEX_HOSTPRIMARYIP,
            NETWORKEX_SETLOCALNAME,
            NETWORKEX_LOCALNAME,
            NETWORKEX_SERVICE,
            NETWORKEX_INTERFACE,
            NETWORKEX_INTERFACESNAMES,
            NETWORKEX_MAIL,
        };
    };
};
#endif
