/***************************************************************************
 *            ioMemory.h
 *
 *  Tue Oct 8 08:19:57 2005
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
		/*
		 * @enum memoryFlagsEnum defines flags of memory serviced by io::memory interface
		 */
		enum memoryFlagsEnum {
			MEMORYFLAGS_NORMAL = 1,
			MEMORYFLAGS_EXTERN = 2,
			MEMORYFLAGS_FIXED_LENGTH = 4,
		};

		/**
		 * @class memory
		 * @brief provides memory I/O manipulations
		 */
		class memory : virtual public block::channel {
		  public:

			/**
			 * constructor
			 * @param shared defines pointer to memory region
			 * @param shared defines memory region size
			 * @param shared defines memory region flags[see memoryFlagsEnum]
			 * @param protection defines flags of IO protection[see channelProtectionFlagsEnum]
			 */
			memory(char          *data,
				   unsigned long size,
				   short         flags = MEMORYFLAGS_NORMAL,
				   short         protection = CHANNEL_PROTECTION_PROCESS);

			/**
			 * constructor
			 * @param protection defines flags of IO protection[see channelProtectionFlagsEnum]
			 */
			memory(short protection = CHANNEL_PROTECTION_PROCESS);

			/**
			 * copy constructor
			 * @note xexec object is not copied
			 */
			memory(const memory &fd);

			/**
			 * constructor
			 * @param data defines initial data for the interface
			 * @param protection defines flags of IO protection[see channelProtectionFlagsEnum]
			 */
			memory(const dodoString &data,
				   short            protection = CHANNEL_PROTECTION_PROCESS);

			/**
			 * destructor
			 */
			virtual ~memory();

			/**
			 * clone file object
			 * @param fd defines object to clone
			 * @note xexec object is not copied
			 */
			virtual void clone(const memory &fd);

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
			virtual int getInDescriptor() const;

			/**
			 * @return descriptor of the output stream
			 */
			virtual int getOutDescriptor() const;

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 * if block is true read offset is calculated as pos*inSize otherwise offset it taken pos bytes from the beginning
			 */
			virtual void _read(char * const data) const;

			/**
			 * read from stream - '\0' or '\n' - terminated memory
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 * if block is true read offset is calculated as pos*'# of \n - terminated memorys' otherwise offset it taken pos bytes from the beginning
			 */
			virtual unsigned long _readStream(char * const data) const;

			/**
			 * @param data defines data that will be written
			 * @note if block is true write offset is calculated as pos*outSize otherwise offset it taken pos bytes from the beginning
			 */
			virtual void _write(const char * const data) const;

			/**
			 * write to stream - '\0' - terminated memory
			 * @param data defines data that will be written
			 * @note write only to the end of the file(append)
			 */
			virtual void _writeStream(const char * const data) const;

		  protected:

			mutable char *data;         ///< data
			mutable unsigned long size; ///< size of data

			short flags;
		};
	};
};

#endif
