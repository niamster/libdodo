/***************************************************************************
 *            ioFileFifoEx.h
 *
 *  Mon Feb 21 2005
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

#ifndef _IOFILEFIFOEX_H_
#define _IOFILEFIFOEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace io {
        namespace file {
            /**
             * libdodo defined errors
             */
            enum fifoExR {
                FIFOEX_WRONGFILENAME,
                FIFOEX_NOTOPENED
            };

            /**
             * explanations for libdodo defined errors
             */
#define IOFILEFIFOEX_WRONGFILENAME_STR "Wrong filename"
#define IOFILEFIFOEX_NOTOPENED_STR "Fifo not opened"

            /**
             * IDs of functions where exception might be thrown
             */
            enum fifoFunctionsID {
                FIFOEX_FIFO,
                FIFOEX_CLOSE,
                FIFOEX_CLONE,
                FIFOEX_OPEN,
                FIFOEX__READ,
                FIFOEX__WRITE,
                FIFOEX__READSTREAM,
                FIFOEX__WRITESTREAM,
                FIFOEX_FLUSH,
                FIFOEX_BLOCK,
                FIFOEX_INDESCRIPTOR,
                FIFOEX_OUTDESCRIPTOR,
            };
        };
    };
};
#endif
