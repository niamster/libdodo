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

#include <libdodo/directives.h>

#include <libdodo/ioChannel.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioStreamChannel.h>

namespace dodo {
	namespace io {
		struct __file__;

		/**
		 * @class pipe
		 * @brief provides interface for PIPE I/O operations
		 * @note writeStream* put extra newline to the end of the string, so no need to add it manually
		 */
		class pipe : virtual public stream::channel {
		  public:

			/**
			 * @enum operationEnum defines type of operation for xexec
			 */
			enum operationEnum {
				OPERATION_OPEN = 128,
				OPERATION_CLOSE
			};

			/**
			 * copy constructor
			 * @note xexec object is not copied
			 */
			pipe(const pipe &);

			/**
			 * constructor
			 * @param open defines whether pipe should be opened in constructor
			 * @param protection defines type of IO protection, @see io::channel::protectionEnum
			 */
			pipe(bool  open = false,
				 short protection = channel::PROTECTION_PROCESS);

			/**
			 * destructor
			 */
			virtual ~pipe();

			/**
			 * clone pipe object
			 * @param pipe defines object to clone
			 * @note xexec object is not copied
			 */
			void clone(const pipe &pipe);

			/**
			 * @return info about source of inputting
			 * @note it can be used to get info about peer if you ar using inetd
			 */
			network::exchange::__peer__ peer();

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

			int inPipeBuffer;   ///< input buffer
			int outPipeBuffer;  ///< output buffer

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
			 */
			virtual void _read(char * const data) const;

			/**
			 * read from stream null or newline terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including null)
			 */
			virtual unsigned long _readStream(char * const data) const;

			/**
			 * @param data defines data that will be written
			 */
			virtual void _write(const char * const data) const;

			/**
			 * write to stream null terminated string
			 * @param data defines data that will be written
			 * @note puts extra newline to the end of the string
			 */
			virtual void _writeStream(const char * const data) const;

			__file__ *in;       ///< input stream descriptor
			__file__ *out;      ///< output stream descriptor

		  private:

			bool blocked;       ///< true if stream is blocked
		};
	};
};
#endif
