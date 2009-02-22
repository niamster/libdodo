/***************************************************************************
 *            ioPipe.h
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

#ifndef _IOPIPE_H_
#define _IOPIPE_H_ 1

#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <libdodo/directives.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/ioPipeEx.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/pcSyncThreadStack.h>

namespace dodo
{
	namespace io
	{
		/**
		 * @enum pipeOperationTypeEnum defines type of operation for hook
		 */
		enum pipeOperationTypeEnum
		{
			PIPE_OPERATION_OPEN = 128,
			PIPE_OPERATION_CLOSE
		};

		/**
		 * @class pipe
		 * @brief provides interface for PIPE I/O operations
		 * @note writeStream* put extra '\n' to the end of the string, so no need to add it manually
		 */
		class pipe : virtual public channel
		{
		  public:

			/**
			 * copy constructor
			 * @note xexec object is not copied
			 */
			pipe(const pipe &fd);

			/**
			 * constructor
			 * @param open defines whether pipe should be opened in constructor
			 */
			pipe(bool open = false);

			/**
			 * destructor
			 */
			virtual ~pipe();

			/**
			 * clone pipe object
			 * @param fd defines object to clone
			 * @note xexec object is not copied
			 */
			virtual void clone(const pipe &fd);

			/**
			 * @return info about source of inputting
			 * @note it can be used to get info foreign 'inputter' if you ar using inetd
			 */
			network::__peerInfo peerInfo();

			/**
			 * open pipe
			 */
			virtual void open();

			/**
			 * close pipe
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

			int inPipeBuffer;   ///< input buffer
			int outPipeBuffer;  ///< output buffer

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

			FILE *inHandle;     ///< input stream descriptor
			FILE *outHandle;    ///< output stream descriptor

		  private:

			bool blocked;       ///< true if stream is blocked
		};
	};
};

#endif
