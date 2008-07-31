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
#define _IONETWORKEXCHANGE_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <poll.h>

#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioNetworkExchangeEx.h>
#include <libdodo/types.h>
#include <libdodo/xexec.h>
#include <libdodo/ipcThreadGuard.h>

namespace dodo
{
	namespace io
	{
		namespace network
		{
			/**
			 * @enum exchangeOperationTypeEnum defines type of operation for hook
			 */
			enum exchangeOperationTypeEnum
			{
				EXCHANGE_OPERATION_CLOSE = 128,
			};

			/**
			 * @class __initialAccept holds info that passes to accept call, and then inits exchange;
			 */
			class __initialAccept
			{
				friend class exchange;
				friend class client;
				friend class server;

				public:

					/**
					 * constructor
					 */
					__initialAccept();

					/**
					 * copy constructor
					 * @note if you want to copy it, the object, from what has been copied is not more able to init new session: you have to reinit it with ::accept method
					 */
					__initialAccept(__initialAccept &init);

				private:

					int socket;                                 ///< socket

					bool blocked;                               ///< true if blocked
					bool blockInherited;                        ///< true if block flag is inherited
			};

			/**
			 * @class exchange provides communication interface[send/receive data]
			 * @note readStreamString: if length of read data is inSize, data will contain exact inSize, no '\0' will be set in the end - this is specific only for network sessions
			 */
			class exchange : public connection,
							 virtual public channel
			{
				friend class server;
				friend class client;
				friend class http;

				public:

					/**
					 * constructor
					 */
					exchange();

					/**
					 * copy constructor
					 * @note the object that has inited the object of current instance is unusable anymore and can be used for another connections
					 * xexec object is not copied
					 */
					exchange(exchange &fse);

					/**
					 * constructor
					 * @param init is initial data[got from the ::accept method]
					 * @note the object that has inited the object of current instance can be used for another connections
					 */
					exchange(__initialAccept &init);

					/**
					 * destructor
					 */
					virtual ~exchange();
				
					/**
					 * init object
					 * @param init defines initial data[got from ::accept method]
					 */
					virtual void init(__initialAccept &init);

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
					virtual void init(int socket, bool blocked, bool blockInherited);
					
					/**
					 * flush output
					 * @note does nothing for network connections
					 */
					virtual void flush();

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
			};
		};
	};

};

#endif
