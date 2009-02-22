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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioMemoryEx.h>

namespace dodo
{
	namespace io
	{
		/**
		 * @class memory
		 * @brief provides memory I/O manipulations
		 * @note if blockOffset is false then read/write position is amount of bytes from the beginning, if true then:
		 * writeStream, writeStream write only to the end of the file(append)
		 * write offset for write, write is calculated as pos*outSize
		 * read offset for read, read is calculated as pos*inSize
		 * read offset for readStream, readStream is calculated as pos*'# of \n - terminated memorys'
		 */
		class memory : virtual public channel
		{
		  public:

			/**
			 * constructor
			 * @param protection defines type of IO protection[see channelProtectionTypeEnum]
			 */
			memory(short protection = CHANNEL_PROTECTION_PROCESS);

			/**
			 * copy constructor
			 * @note xexec object is not copied
			 */
			memory(const memory &fd);

			/**
			 * constructor
			 * @param protection defines type of IO protection[see channelProtectionTypeEnum]
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
			virtual void flush();

			unsigned long pos;  ///< read/write/erase position[0 by default]
			bool blockOffset;   ///<  if blockOffset is false then read/write position is amount of bytes from the beginning, if true then read/write position calculated by defined rules[true by default]
			bool append;        ///< append to the end[false by default]

			/**
			 * @return memory
			 */
			operator const dodoString &();

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
			 * if blockOffset is true read offset is calculated as pos*inSize otherwise offset it taken pos bytes from the beginning
			 */
			virtual void _read(char * const data);

			/**
			 * read from stream - '\0' or '\n' - terminated memory
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 * if blockOffset is true read offset is calculated as pos*'# of \n - terminated memorys' otherwise offset it taken pos bytes from the beginning
			 */
			virtual unsigned long _readStream(char * const data);

			/**
			 * @param data defines data that will be written
			 * @note if blockOffset is true write offset is calculated as pos*outSize otherwise offset it taken pos bytes from the beginning
			 */
			virtual void _write(const char * const data);

			/**
			 * write to stream - '\0' - terminated memory
			 * @param data defines data that will be written
			 * @note write only to the end of the file(append)
			 */
			virtual void _writeStream(const char * const data);

		  protected:

			dodoString buffer; ///< object data
		};
	};
};

#endif
