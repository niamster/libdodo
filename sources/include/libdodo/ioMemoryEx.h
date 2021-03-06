/***************************************************************************
 *            ioMemoryEx.h
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

#ifndef _IOMEMORYEX_H_
#define _IOMEMORYEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace io {
        /**
         * libdodo defined errors
         */
        enum memoryExR {
            MEMORYEX_CANTBEUSEDWITHIOEVENT,
            MEMORYEX_OUTOFBOUNDS,
            MEMORYEX_APPENDTOFIXED,
            MEMORYEX_EXTENDFIXED,
            MEMORYEX_WRONGFLAGS,
        };

        /**
         * explanations for libdodo defined errors
         */
#define IOMEMORYEX_CANTBEUSEDWITHIOEVENT_STR        "io::memory can not be used with io::event"
#define IOMEMORYEX_OUTOFBOUNDS_STR                  "Access data out of bounds"
#define IOMEMORYEX_APPENDTOFIXED_STR                "Append to fixed-size region"
#define IOMEMORYEX_EXTENDFIXED_STR                  "Extend fixed-size region"
#define IOMEMORYEX_WRONGFLAGS_STR                   "Wrong flags set"

        /**
         * IDs of functions where exception might be thrown
         */
        enum memoryFunctionsID {
            MEMORYEX__READ,
            MEMORYEX__WRITE,
            MEMORYEX__WRITESTRING,
            MEMORYEX_ERASE,
            MEMORYEX_INDESCRIPTOR,
            MEMORYEX_OUTDESCRIPTOR,
            MEMORYEX_MEMORY
        };
    };
};
#endif
