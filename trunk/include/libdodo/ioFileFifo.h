/***************************************************************************
 *            ioFileFifo.h
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

#ifndef _IOFILEFIFO_H_
#define _IOFILEFIFO_H_ 1

#include <libdodo/directives.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioFileFifoEx.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>

namespace dodo
{
	namespace io
	{
		namespace file
		{
#ifndef IO_WO_XEXEC

			/**
			 * @enum fileOperationTypeEnum defines type of operation for hook
			 */
			enum fileOperationTypeEnum
			{
				FIFO_OPERATION_OPEN = 128,
				FIFO_OPERATION_CLOSE
			};

#endif

			/**
			 * @enum fifoOpenmodeEnum defines modes to open file
			 */
			enum fifoOpenmodeEnum
			{
				FIFO_OPENMODE_READ,                                        ///< normaly blocks until some other process opens the same FIFO for writing
				FIFO_OPENMODE_READ_OPENNONBLOCK,                           ///< normaly does't block until some other process opens the same FIFO for writing, further I/O is in blocked mode
				FIFO_OPENMODE_WRITE,                                       ///< normaly blocks until some other process opens the same FIFO for reading
			};

			/**
			 * @class fifo
			 * @brief provides I/O manipulations with fifo files
			 * @note writeStream* put extra '\n' to the end of the string, so no need to add it manually
			 */
			class fifo : virtual public channel
			{
				public:

					/**
					 * constructor
					 */
					fifo();

					/**
					 * constructor
					 * @param path defines path to the file
					 * @param mode defines mode to open file[see fifoOpenmodeEnum]
					 */
					fifo(const dodoString &path, short mode);

					/**
					 * copy constructor
					 * @note xexec object is not copied
					 */
					fifo(const fifo &fd);

					/**
					 * destructor
					 */
					virtual ~fifo();

					/**
					 * clone file object
					 * @param fd defines object to clone
					 * @note xexec object is not copied
					 */
					virtual void clone(const fifo &fd);

					/**
					 * open file
					 * @param path defines path to the file
					 * @param mode defines mode to open file[see fifoOpenmodeEnum]
					 */
					virtual void open(const dodoString &path, short mode);

					/**
					 * close file
					 */
					virtual void close();

					/**
					 * flush output
					 */
					virtual void flush();

					/**
					 * @return true if stream is blocked
					 */
					virtual bool isBlocked();

					/**
					 * blocks/unblocks stream
					 * @param flag indicates whether to block or unblock stream
					 */
					virtual void block(bool flag);

					int inFileFifoBuffer;                                           ///< input buffer
					int outFileFifoBuffer;                                          ///< output buffer

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
					 */
					virtual void _read(char * const data);

					/**
					 * read from stream - '\0' or '\n' - terminated string
					 * @param data defines buffer that will be filled
					 * @note not more then inSize(including '\0')
					 */
					virtual unsigned long _readStream(char * const data);

					/**
					 * @param data defines data that will be written
					 */
					virtual void _write(const char * const data);

					/**
					 * write to stream - '\0' - terminated string
					 * @param data defines data that will be written
					 * @note puts extra '\n' to the end of the string
					 */
					virtual void _writeStream(const char * const data);

				private:

					dodoString path;                                                                        ///< file path
					short mode;                                                                             ///< file open mode[see fileOpenmodeEnum]

					FILE *handler;                                                                          ///< file handler

					bool blocked;                                           ///< true if stream is blocked
			};
		};
	};
};

#endif
