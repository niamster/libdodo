/***************************************************************************
 *            ioEventManager.h
 *
 *  Thu Sep 09 2006
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

#ifndef _IOEVENTMANAGER_H_
#define _IOEVENTMANAGER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace pc {
        namespace sync {
            class protector;
        };
    };

    namespace io {
        namespace event {
            class descriptor;

            /**
             * @class manager
             * @brief provides information if in/output stream is readable/writable
             */
            class manager {
              private:

                /**
                 * copy constructor
                 * @note to prevent copying
                 */
                manager(manager &);

              public:

                /**
                 * constructor
                 */
                manager();

                /**
                 * destructor
                 */
                ~manager();

                /**
                 * @return identificator of channel pair
                 * @param fl defines a stream[file, stdio, exchange, ...]
                 */
                int add(const descriptor &fl);

                /**
                 * delete channel pair from watchers
                 * @param pos defines identificator of stream pair
                 */
                void remove(int pos);

                /**
                 * @return true if channel is readable
                 * @param pos defines identificator of stream pair
                 * @param timeout defines amount of time to wait for result[in milliseconds]
                 * @note if timeout is negative - infinit timeout
                 */
                bool isReadable(int pos,
                                int timeout = 100) const;

                /**
                 * @return collection of boolean values[value is true if channel is readable]
                 * @param pos defines collection of identificators of stream pairs
                 * @param timeout defines amount of time to wait for result[in milliseconds]
                 * @note if timeout is negative - infinit timeout
                 */
                dodoArray<bool> isReadable(const dodoArray<int> &pos,
                                           int                  timeout = 100) const;

                /**
                 * @return true if channel is writable
                 * @param pos defines identificator of stream pair
                 * @param timeout defines amount of time to wait for result[in milliseconds]
                 * @note if timeout is negative - infinit timeout
                 */
                bool isWritable(int pos,
                                int timeout = 100) const;

                /**
                 * @return collection of boolean values[value is true if channel is writable]
                 * @param pos defines collection of identificators of stream pairs
                 * @param timeout defines amount of time to wait for result[in milliseconds]
                 * @note if timeout is negative - infinit timeout
                 */
                dodoArray<bool> isWritable(const dodoArray<int> &pos,
                                           int                  timeout = 100) const;

              protected:

                /**
                 * @struct __descriptors__
                 * @brief defines input and output descriptors of the io::* classes
                 */
                struct __descriptors__ {
                    int position;                                   ///< identificator of the pair

                    int in;                                         ///< input stream
                    int out;                                        ///< output stream
                };

                dodoArray<__descriptors__> desc;                    ///< stream pairs

                int descs;                                          ///< descriptors counter

                pc::sync::protector *keeper;                        ///< section locker
            };
        };
    };
};
#endif
