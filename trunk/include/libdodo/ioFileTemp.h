/***************************************************************************
 *            ioFileTemp.h
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

#ifndef _IOFILETEMP_H_
#define _IOFILETEMP_H_ 1

#include <libdodo/directives.h>

#include <libdodo/ioChannel.h>
#include <libdodo/ioBlockChannel.h>

namespace dodo {
	namespace io {
		struct __file__;

		namespace file {
			/**
			 * @class temp
			 * @brief provides I/O manipulations with temporary file
			 * @note  if block is false then read/write position is amount of bytes from the beginning, if true then:
			 * writeStream, writeStream write only to the end of the file(append)
			 * write offset for write, write is calculated as pos*outSize
			 * read offset for read, read is calculated as pos*inSize
			 * read offset for readStream, readStream is calculated as pos*'# of \n terminated strings'
			 */
			class temp : virtual public block::channel {
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
				 * constructor
				 * @param open defines whether temp file should be opened in constructor
				 * @param protection defines type of IO protection[see protectionEnum]
				 */
				temp(bool  open = false,
					 short protection = channel::PROTECTION_PROCESS);

				/**
				 * copy constructor
				 * @note xexec object is not copied
				 */
				temp(const temp &);

				/**
				 * destructor
				 */
				virtual ~temp();

				/**
				 * clone file object
				 * @param fd defines object to clone
				 * @note xexec object is not copied
				 */
				void clone(const temp &fd);

				/**
				 * open file
				 */
				virtual void open();

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

				bool block;         ///<  if block is false then read/write position is amount of bytes from the beginning, if true then read/write position calculated by defined rules[false by default]
				bool append;        ///< append to the end[false by default]

				bool overwrite;     ///< if true block will be overwritten; for regular and temp files only[false by default]

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
				virtual unsigned long _readStream(char * const data) const;

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
				virtual void _writeStream(const char * const data) const;

			  private:

				__file__ *handle; ///< file handle
			};
		};
	};
};
#endif
