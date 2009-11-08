/***************************************************************************
 *            ioMemory.h
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

#ifndef _IOMEMORY_H_
#define _IOMEMORY_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioBlockChannel.h>

namespace dodo {
    namespace io {
        /**
         * @class memory
         * @brief provides memory I/O manipulations
         */
        class memory : virtual public block::channel {
          public:

            /*
             * @enum flagsEnum defines flags of memory serviced by io::memory interface
             */
            enum flagsEnum {
                FLAGS_NORMAL = 1,
                FLAGS_EXTERN = 2,
                FLAGS_FIXED_LENGTH = 4,
            };

            /**
             * constructor
             * @param data defines pointer to memory region
             * @param size defines memory region size
             * @param flags defines memory region flags, @see io::memory::flagsEnum
             * @param protection defines flags of IO protection, @see io::channel::protectionEnum
             */
            memory(char          *data,
                   unsigned long size,
                   short         flags = FLAGS_NORMAL,
                   short         protection = channel::PROTECTION_PROCESS);

            /**
             * constructor
             * @param protection defines flags of IO protection, @see io::channel::protectionEnum
             */
            memory(short protection = channel::PROTECTION_PROCESS);

            /**
             * copy constructor
             * @note xexec object is not copied
             */
            memory(const memory &fd);

            /**
             * constructor
             * @param data defines initial data for the interface
             * @param protection defines flags of IO protection, @see io::channel::protectionEnum
             */
            memory(const dodoString &data,
                   short            protection = channel::PROTECTION_PROCESS);

            /**
             * destructor
             */
            virtual ~memory();

            /**
             * clone file object
             * @param fd defines object to clone
             * @note xexec object is not copied
             */
            void clone(const memory &fd);

            /**
             * erase node on position
             */
            virtual void erase();

            /**
             * flush output
             */
            virtual void flush() const;

            /**
             * clear buffer
             */
            virtual void clear();

            /**
             * @return memory
             */
            operator const char*();

          protected:

            /**
             * @return descriptor of the input stream
             */
            virtual int inDescriptor() const;

            /**
             * @return descriptor of the output stream
             */
            virtual int outDescriptor() const;

            /**
             * @return amount in bytes of successfully read data
             * @param data defines buffer that will be filled
             * @note not more then bs(including null)
             */
            virtual unsigned long _read(char * const data) const;

            /**
             * read null- or newline- terminated memory
             * @return amount in bytes of successfully read data
             * @param data defines buffer that will be filled
             * @note not more then bs(including null)
             */
            virtual unsigned long _readString(char * const data) const;

            /**
             * @return amount in bytes of successfully written data
             * @param data defines data that will be written
             */
            virtual unsigned long _write(const char * const data) const;

            /**
             * write null- terminated memory
             * @return amount in bytes of successfully written data
             * @param data defines data that will be written
             * @note write only to the end of the file(append)
             */
            virtual unsigned long _writeString(const char * const data) const;

          protected:

            mutable char *data;         ///< data
            mutable unsigned long size; ///< size of data

            short flags;

            mutable bool nullEnd;       ///< true if null is set up in the end
        };
    };
};
#endif
