/***************************************************************************
 *            xexecEx.h
 *
 *  Wed Oct 5 2005
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

#ifndef _XEXECEX_H_
#define _XEXECEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    /**
     * libdodo defined errors
     */
    enum xexecExR {
        XEXECEX_WRONGTYPE,
    };

    /**
     * explanations for libdodo defined errors
     */
#define XEXECEX_WRONGTYPE_STR "Wrong xExec type"

    /**
     * IDs of functions where exception might be thrown
     */
    enum xexecFunctionsID {
        XEXECEX_ADDXEXEC,
#ifdef DL_EXT
        XEXECEX_MODULE,
        XEXECEX_REMOVEXEXEC,
#endif
    };
};
#endif
