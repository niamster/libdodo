/***************************************************************************
 *            ioFifo.h
 *
 *  Tue Jul 1 15:33:57 2008
 *  Copyright  2008  Ni@m
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

#ifndef _IOFIFO_H_
#define _IOFIFO_H_

#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <libdodo/directives.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/ioFifoEx.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/ipcThreadGuard.h>

namespace dodo
{
	namespace io
	{
		/**
		 * @enum fifoOperationTypeEnum defines type of operation for hook
		 */
		enum fifoOperationTypeEnum
		{
			FIFO_OPERATION_OPEN = 128,
			FIFO_OPERATION_CLOSE
		};

		/**
		 * @class fifo provides interface for FIFO I/O operations
		 */
		class fifo : virtual public channel
		{
			public:
				
				/**
				 * copy constructor
				 */
				fifo(fifo &fd);

				/**
				 * constructor
				 */
				fifo();

				/**
				 * destructor
				 */
				virtual ~fifo();

				/**
				 * @return info about source of inputting
				 * @note it can be used to get info foreign 'inputter' if you ar using inetd
				 */
				network::__peerInfo peerInfo();
				
				/**
				 * open fifo
				 */
				virtual void open();
				
				/**
				 * close fifo
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

				int inFifoBuffer;                        ///< input buffer
				int outFifoBuffer;                       ///< output buffer

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
				 */
				virtual void _writeStream(const char * const data);

				FILE *inHandle;                             ///< input stream descriptor
				FILE *outHandle;                             ///< output stream descriptor
			
			private:

				bool blocked;                           ///< true if stream is blocked
		};
	};
};

#endif
