/***************************************************************************
 *            ioNetworkExchange.h
 *
 *  Thu Oct 04 02:02:24 2005
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

#ifndef _IONETWORKEXCHANGE_H_
#define _IONETWORKEXCHANGE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioStreamChannel.h>
#include <libdodo/xexec.h>

namespace dodo {
	namespace io {
		namespace network {
			/**
			 * @enum exchangeOperationTypeEnum defines type of operation for hook
			 */
			enum exchangeOperationTypeEnum {
				EXCHANGE_OPERATION_CLOSE = 128,
			};

			/**
			 * @class __initialAccept__
			 * @brief holds info that passes to accept call, and then inits exchange;
			 */
			class __initialAccept__ {
				friend class exchange;
				friend class client;
				friend class server;

			  public:

				/**
				 * constructor
				 */
				__initialAccept__();

				/**
				 * copy constructor
				 * @note if you want to copy it, the object, from what has been copied is not more able to init new session: you have to reinit it with ::accept method
				 */
				__initialAccept__(__initialAccept__ &init);

			  private:

				int socket;             ///< socket

				bool blocked;           ///< true if blocked
				bool blockInherited;    ///< true if block flag is inherited
			};

			/**
			 * @class exchange
			 * @brief provides communication interface[send/receive data]
			 * @note readStream*: if length of read data is inSize, data will contain exact inSize, no '\0' will be set in the end - this is specific only for network sessions
			 * writesStream* put extra '\0' in the end of the string
			 */
			class exchange : public connection,
							 virtual public stream::channel {
				friend class server;
				friend class client;
				friend class http::client;

			  private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				exchange(exchange &fse);

			  public:

				/**
				 * constructor
				 * @param protection defines type of IO protection[see channelProtectionTypeEnum]
				 */
				exchange(short protection = CHANNEL_PROTECTION_PROCESS);

				/**
				 * constructor
				 * @param init is initial data[got from the ::accept method]
				 * @param protection defines type of IO protection[see channelProtectionTypeEnum]
				 * @note the object that has inited the object of current instance can be used for another connections
				 */
				exchange(__initialAccept__ &init,
						 short             protection = CHANNEL_PROTECTION_PROCESS);

				/**
				 * destructor
				 */
				virtual ~exchange();

				/**
				 * @return true if connection is alive
				 */
				virtual bool isAlive();

				/**
				 * close connection
				 */
				virtual void close();

			  protected:

				/**
				 * @return descriptor of input stream
				 */
				virtual int getInDescriptor() const;

				/**
				 * @return descriptor of output stream
				 */
				virtual int getOutDescriptor() const;

				/**
				 * init current instance
				 * @param socket defines socket
				 * @param blocked defines the connection block status
				 * @param blockInherited defines block flag inheritance
				 */
				virtual void init(int  socket,
								  bool blocked,
								  bool blockInherited);

				/**
				 * flush output
				 * @note does nothing for network connections
				 */
				virtual void flush() const;

				/**
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 */
				virtual void _read(char * const data) const;

				/**
				 * read from stream - '\0' or '\n' - terminated string
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 */
				virtual unsigned long _readStream(char * const data) const;

				/**
				 * @param data defines data that will be written
				 */
				virtual void _write(const char * const data) const;

				/**
				 * write to stream - '\0' - terminated string
				 * @param data defines data that will be written
				 * @note puts extra '\0' in the end of the string
				 */
				virtual void _writeStream(const char * const data) const;
			};
		};
	};
};

#endif
