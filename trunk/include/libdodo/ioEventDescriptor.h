/***************************************************************************
 *            ioEventDescriptor.h
 *
 *  Fri Jan 18 2008
 *  Copyright  2008  Ni@m
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

#ifndef _IOEVENTDESCRIPTOR_H_
#define _IOEVENTDESCRIPTOR_H_ 1

#include <libdodo/directives.h>

namespace dodo {
    namespace io {
        namespace event {
            /**
             * @class descriptor
             * @brief provides interface for event to get descriptors for io::event::manager
             */
            class descriptor {
                friend class manager;

              protected:

                /**
                 * destructor
                 */
                virtual ~descriptor() = 0;

                /**
                 * @return descriptor of input stream
                 */
                virtual int inDescriptor() const = 0;

                /**
                 * @return descriptor of output stream
                 */
                virtual int outDescriptor() const = 0;
            };
        };
    };
};
#endif
