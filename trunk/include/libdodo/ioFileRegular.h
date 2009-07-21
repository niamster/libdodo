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
#ifndef IO_WO_XEXEC
			/**
			 * @enum regularOperationTypeEnum defines type of operation for hook
			 */
			enum regularOperationTypeEnum {
				REGULAR_OPERATION_OPEN = 128,
				REGULAR_OPERATION_CLOSE
			};
#endif

			/**
			 * @enum regularOpenmodeEnum defines modes to open file
			 */
			enum regularOpenmodeEnum {
				REGULAR_OPENMODE_READ_ONLY,             ///< reading from the file
				REGULAR_OPENMODE_READ_WRITE,            ///< reading and writing[creates if not exists]
				REGULAR_OPENMODE_READ_WRITE_TRUNCATE,   ///< file will be truncated if exists
			};

			/**
			 * @class regular
			 * @brief provides file I/O manipulations
			 */
			class regular : virtual public block::channel {
			  public:

				/**
				 * constructor
				 * @param protection defines type of IO protection[see channelProtectionTypeEnum]
				 */
				regular(short protection = CHANNEL_PROTECTION_PROCESS);

				/**
				 * constructor
				 * @param path defines path to the file
				 * @param mode defines mode to open file[see regularOpenmodeEnum]
				 * @param protection defines type of IO protection[see channelProtectionTypeEnum]
				 */
				regular(const dodoString &path,
						short            mode,
						short            protection = CHANNEL_PROTECTION_PROCESS);

				/**
				 * copy constructor
				 * @note xexec object is not copied
				 */
				regular(const regular &fd);

				/**
				 * destructor
				 */
				virtual ~regular();

				/**
				 * clone file object
				 * @param fd defines object to clone
				 * @note xexec object is not copied
				 */
				virtual void clone(const regular &fd);

				/**
				 * open file
				 * @param path defines path to the file
				 * @param mode defines mode to open file[see regularOpenmodeEnum]
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
				 * read from stream - '\0' or '\n' - terminated string
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 * if block is true read offset is calculated as pos*'# of \n - terminated strings' otherwise offset it taken pos bytes from the beginning
				 */
				virtual unsigned long _readStream(char * const data) const;

				/**
				 * @param data defines data that will be written
				 * @note if block is true write offset is calculated as pos*outSize otherwise offset it taken pos bytes from the beginning
				 */
				virtual void _write(const char * const data) const;

				/**
				 * write to stream - '\0' - terminated string
				 * @param data defines data that will be written
				 * @note write only to the end of the file(append)
				 */
				virtual void _writeStream(const char * const data) const;

			  private:

				dodoString path;        ///< file path
				short mode;             ///< file open mode[see fileOpenmodeEnum]

				__file__ *handle;       ///< file handle
			};
		};
	};
};

#endif
