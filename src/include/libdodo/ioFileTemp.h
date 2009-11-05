/***************************************************************************
 *            ioFileTemp.h
 *
 *  Tue Oct 8 2005
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

#ifndef _IOFILETEMP_H_
#define _IOFILETEMP_H_ 1

#include <libdodo/directives.h>

#include <libdodo/ioChannel.h>
#include <libdodo/ioBlockChannel.h>
#include <libdodo/ioFileRegular.h>

namespace dodo {
    namespace io {
        struct __file__;

        namespace file {
            /**
             * @class temp
             * @brief provides I/O manipulations with temporary file
             */
            class temp : virtual public block::channel,
                         public regular {
              public:

#ifndef IO_WO_XEXEC
                /**
                 * @enum operationEnum defines type of operation for xexec
                 */
                enum operationEnum {
                    OPERATION_OPEN = 128,
                };
#endif

                /**
                 * constructor
                 * @param open defines whether temp file should be opened in constructor
                 * @param protection defines type of IO protection, @see io::channel::protectionEnum
                 */
                temp(bool  open = false,
                     short protection = channel::PROTECTION_PROCESS);

                /**
                 * copy constructor
                 * @note xexec object is not copied
                 */
                temp(const temp &);

                /**
                 * destructor
                 */
                virtual ~temp();

                /**
                 * clone file object
                 * @param file defines object to clone
                 * @note xexec object is not copied
                 */
                void clone(const temp &file);


                /**
                 * open file
                 */
                void open();
            };
        };
    };
};
#endif
