/***************************************************************************
 *            ioBlockChannel.h
 *
 *  Sat Jun 13 2009
 *  Copyright  2009  Dmytro Milinevskyy
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

#ifndef _IOBLOCKCHANNEL_H_
#define _IOBLOCKCHANNEL_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/ioChannel.h>

namespace dodo {
    namespace io {
        namespace block {
            /**
             * @class channel
             * @brief implements an interface for I/O operations on block sources
             */
            class channel : public io::channel {
              public:

                /**
                 * constructor
                 * @param protection defines type of IO protection, @see io::channel::protectionEnum
                 */
                channel(short protection);

                /**
                 * destructor
                 */
                virtual ~channel();

                /**
                 * @return read data
                 * @note not more than io::channel::bs
                 */
                virtual dodo::string read() const;

                /**
                 * @return amount in bytes of successfully written data
                 * @param data defines data that will be written
                 * @note not more than io::channel::bs
                 */
                virtual unsigned long write(const dodo::string &data) const;

                /**
                 * read null- or newline- terminated string
                 * @return read data
                 * @note not more than io::channel::bs
                 */
                virtual dodo::string readString() const;

                /**
                 * write null- terminated string
                 * @return amount in bytes of successfully written data
                 * @param data defines data that will be written
                 * @note not more than io::channel::bs
                 */
                virtual unsigned long writeString(const dodo::string &data) const;

                mutable unsigned long pos;  ///< read/write/erase position; incremented on read/write/erase[0 by default]

                bool append;                ///< append to the end[false by default]
            };
        };
    };
};
#endif
