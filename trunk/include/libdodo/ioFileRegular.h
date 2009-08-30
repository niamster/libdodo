/***************************************************************************
 *            ioFileRegular.h
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

#ifndef _IOFILEREGULAR_H_
#define _IOFILEREGULAR_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioBlockChannel.h>

namespace dodo {
	namespace io {
		struct __file__;

		namespace file {
			/**
			 * @class regular
			 * @brief provides file I/O manipulations
			 */
			class regular : virtual public block::channel {
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
			 * @enum file::regular::openModeEnum defines modes to open file
			 */
			enum openModeEnum {
				OPEN_MODE_READ_ONLY,             ///< reading from the file
				OPEN_MODE_READ_WRITE,            ///< reading and writing[creates if not exists]
				OPEN_MODE_READ_WRITE_TRUNCATE,   ///< file will be truncated if exists
			};

				/**
				 * constructor
				 * @param protection defines type of IO protection, @see io::channel::protectionEnum
				 */
				regular(short protection = channel::PROTECTION_PROCESS);

				/**
				 * constructor
				 * @param path defines path to the file
				 * @param mode defines mode to open file, @see file::regular::openModeEnum
				 * @param protection defines type of IO protection, @see io::channel::protectionEnum
				 */
				regular(const dodoString &path,
						short            mode,
						short            protection = channel::PROTECTION_PROCESS);

				/**
				 * copy constructor
				 * @note xexec object is not copied
				 */
				regular(const regular &);

				/**
				 * destructor
				 */
				virtual ~regular();

				/**
				 * clone file object
				 * @param file defines object to clone
				 * @note xexec object is not copied
				 */
				void clone(const regular &file);

				/**
				 * open file
				 * @param path defines path to the file
				 * @param mode defines mode to open file, @see file::regular::openModeEnum
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
				 * erase node on position
				 */
				virtual void erase();

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
				 * @note not more then inSize(including null)
				 * if block is true read offset is calculated as pos*inSize otherwise offset it taken pos bytes from the beginning
				 */
				virtual void _read(char * const data) const;

				/**
				 * read from stream null- or newline- terminated string
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including null)
				 * if block is true read offset is calculated as pos*'# of \n terminated strings' otherwise offset it taken pos bytes from the beginning
				 */
				virtual unsigned long _readString(char * const data) const;

				/**
				 * @param data defines data that will be written
				 * @note if block is true write offset is calculated as pos*outSize otherwise offset it taken pos bytes from the beginning
				 */
				virtual void _write(const char * const data) const;

				/**
				 * write to stream null- terminated string
				 * @param data defines data that will be written
				 * @note write only to the end of the file(append)
				 */
				virtual void _writeString(const char * const data) const;

			  private:

				dodoString path;        ///< file path
				short mode;             ///< file open mode, @see file::regular::openModeEnum

				__file__ *handle;       ///< file handle
			};
		};
	};
};
#endif
