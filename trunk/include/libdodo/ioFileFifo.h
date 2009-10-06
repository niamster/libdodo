/***************************************************************************
 *            ioFileFifo.h
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

#ifndef _IOFILEFIFO_H_
#define _IOFILEFIFO_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioStreamChannel.h>

namespace dodo {
	namespace io {
		struct __file__;

		namespace file {
			/**
			 * @class fifo
			 * @brief provides I/O manipulations with fifo files
			 * @note writeString* put extra newline- to the end of the string, so no need to add it manually
			 */
			class fifo : virtual public stream::channel {
			  public:

#ifndef IO_WO_XEXEC
				/**
				 * @enum operationEnum defines type of operation for xexec
				 */
				enum operationEnum {
					OPERATION_OPEN = 128,
					OPERATION_CLOSE
				};
#endif

				/**
				 * @enum openModeEnum defines modes to open file
				 */
				enum openModeEnum {
					OPEN_MODE_READ,                 ///< normaly blocks until some other process opens the same FIFO for writing
					OPEN_MODE_READ_OPENNONBLOCK,    ///< normaly does't block until some other process opens the same FIFO for writing, further I/O is in blocked mode
					OPEN_MODE_WRITE,                ///< normaly blocks until some other process opens the same FIFO for reading
				};

				/**
				 * constructor
				 * @param protection defines type of IO protection, @see io::channel::protectionEnum
				 */
				fifo(short protection = channel::PROTECTION_PROCESS);

				/**
				 * constructor
				 * @param path defines path to the file
				 * @param mode defines mode to open file, @see file::fifo::openModeEnum
				 * @param protection defines type of IO protection, @see io::channel::protectionEnum
				 */
				fifo(const dodoString &path,
					 short            mode,
					 short            protection = channel::PROTECTION_PROCESS);

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
				void clone(const fifo &fd);

				/**
				 * open file
				 * @param path defines path to the file
				 * @param mode defines mode to open file, @see file::fifo::openModeEnum
				 */
				virtual void open(const dodoString &path,
								  short            mode);

				/**
				 * close file
				 */
				virtual void close();

				/**
				 * flush output
				 */
				virtual void flush() const;

				/**
				 * @return true if stream is blocked
				 */
				virtual bool isBlocked();

				/**
				 * blocks/unblocks stream
				 * @param flag indicates whether to block or unblock stream
				 */
				virtual void block(bool flag);

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
				 * @param data defines buffer that will be filled
				 * @note not more then blockSize(including null)
				 */
				virtual void _read(char * const data) const;

				/**
				 * read from stream null- or newline- terminated string
				 * @param data defines buffer that will be filled
				 * @note not more then blockSize(including null)
				 */
				virtual unsigned long _readString(char * const data) const;

				/**
				 * @param data defines data that will be written
				 */
				virtual void _write(const char * const data) const;

				/**
				 * write to stream null- terminated string
				 * @param data defines data that will be written
				 * @note puts extra newline- to the end of the string
				 */
				virtual void _writeString(const char * const data) const;

			  private:

				dodoString path;        ///< file path
				short mode;             ///< file open mode, @see file::fifo::openModeEnum

				__file__ *handle;       ///< file handle

				bool blocked;           ///< true if stream is blocked
			};
		};
	};
};
#endif
