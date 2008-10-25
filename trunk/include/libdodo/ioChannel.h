/***************************************************************************
 *            ioChannel.h
 *
 *  Tue Oct 11 00:19:57 2005
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

#ifndef _IOCHANNEL_H_
#define _IOCHANNEL_H_

#include <libdodo/directives.h>

#include <libdodo/xexec.h>
#include <libdodo/types.h>
#include <libdodo/ioEventInfo.h>
#include <libdodo/pcSyncThreadStack.h>

namespace dodo
{
	namespace io
	{
		/**
		 * @enum exchangeOperationTypeEnum describes type of operation for hook
		 */
		enum exchangeOperationTypeEnum
		{
			IO_OPERATION_READ,
			IO_OPERATION_READSTRING,
			IO_OPERATION_READSTREAM,
			IO_OPERATION_READSTREAMSTRING,
			IO_OPERATION_WRITE,
			IO_OPERATION_WRITESTRING,
			IO_OPERATION_WRITESTREAM,
			IO_OPERATION_WRITESTREAMSTRING,
		};

#ifndef IO_WO_XEXEC

		/**
		 * @class __xexecIoChannelCollectedData
		 * @brief defines data that could be retrieved from io::channel(to modificate)
		 */
		class __xexecIoChannelCollectedData : public __xexecCollectedData
		{
			public:

				/**
				 * constructor
				 * @param executor defines class that executed hook
				 * @param execObject defines type of object that executed a hook[see xexecObjectTypeEnum]
				 */
				__xexecIoChannelCollectedData(xexec *executor, short execObject);

				dodoString buffer;                                      ///< data buffer
		};

#endif

		/**
		 * @class channel
		 * @brief implements an interface for I/O operations
		 */
		class channel : virtual public eventInfo,
						virtual public pc::sync::thread::stack
#ifndef IO_WO_XEXEC
						,
						public xexec
#endif
		{
			public:

				/**
				 * constructor
				 */
				channel();

				/**
				 * destructor
				 */
				virtual ~channel();

				/**
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 */
				virtual void readString(dodoString &data);

				/**
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 */
				virtual void read(char * const data);

				/**
				 * read from stream - '\0' or '\n' - terminated string
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 */
				virtual void readStreamString(dodoString &data);

				/**
				 * read from stream - '\0' or '\n' - terminated string
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 */
				virtual void readStream(char * const data);

				/**
				 * @param data defines data that will be written
				 */
				virtual void writeString(const dodoString &data);

				/**
				 * @param data defines data that will be written
				 */
				virtual void write(const char * const data);

				/**
				 * write to stream - '\0' - terminated string
				 * @param data defines data that will be written
				 */
				virtual void writeStreamString(const dodoString &data);

				/**
				 * write to stream - '\0' - terminated string
				 * @param data defines data that will be written
				 */
				virtual void writeStream(const char * const data);

				/**
				 * flush output
				 */
				virtual void flush() = 0;

				unsigned long inSize;                                   ///< size of data block for read* operations
				unsigned long outSize;                                  ///< size of data block for write* operations

			protected:

				/**
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 */
				virtual void _read(char * const data) = 0;

				/**
				 * read from stream - '\0' or '\n' - terminated string
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 */
				virtual unsigned long _readStream(char * const data) = 0;

				/**
				 * @param data defines data that will be written
				 */
				virtual void _write(const char * const data) = 0;

				/**
				 * write to stream - '\0' - terminated string
				 * @param data defines data that will be written
				 */
				virtual void _writeStream(const char * const data) = 0;

				bool opened;                                 ///< true if I/O *session* is opened

#ifndef IO_WO_XEXEC

				__xexecIoChannelCollectedData collectedData;                                   ///< data collected for xexec

#endif
		};
	};
};

#endif
