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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioFileTempEx.h>
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
			 * @enum tempOperationTypeEnum defines type of operation for hook
			 */
			enum tempOperationTypeEnum
			{
				TEMP_OPERATION_OPEN = 128,
				TEMP_OPERATION_CLOSE
			};

#endif

			/**
			 * @class temp
			 * @brief provides I/O manipulations with temporary file
			 * @note writeString* write only to the end of the file(appends)
			 */
			class temp : virtual public channel
			{
				public:

					/**
					 * constructor
					 */
					temp();

					/**
					 * copy constructor
					 * @note xexec object is not copied
					 */
					temp(const temp &fd);

					/**
					 * destructor
					 */
					virtual ~temp();

					/**
					 * clone file object
					 * @param fd defines object to clone
					 * @note xexec object is not copied
					 */
					virtual void clone(const temp &fd);

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
					virtual void flush();

					/**
					 * erase node on position
					 */
					virtual void erase();

					unsigned long pos;                                              ///< read/write/erase position; if pos==-1, append to the end[0 by default]

					bool overwrite;                                                 ///< if true block will be overwritten; for regular and temp files only[false by default]

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
					 * @note writes only to the end of the file(appends)
					 */
					virtual void _writeStream(const char * const data);

				private:

					FILE *handler;                                                                          ///< file handler
			};
		};
	};
};

#endif
