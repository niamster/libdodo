/***************************************************************************
 *            ioNetworkSslExchange.h
 *
 *  Tue Jun 10 16:45:47 2008
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

#ifndef _IONETWORKSSLEXCHANGE_H_
#define _IONETWORKSSLEXCHANGE_H_

#include <libdodo/directives.h>

#ifdef OPENSSL_EXT

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
#include <openssl/ssl.h>

#include <libdodo/ioNetworkOptions.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioNetworkSslExchangeEx.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioSsl.h>
#include <libdodo/types.h>
#include <libdodo/xexec.h>
#include <libdodo/ipcThreadGuard.h>

namespace dodo
{
	namespace io
	{
		namespace network
		{
			namespace ssl
			{
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

						int socket;                                     ///< socket

						SSL *sslHandle;                                 ///< SSL connection handle

						bool blocked;                                   ///< true if blocked
						bool blockInherited;                            ///< true if block flag is inherited
				};

				/**
				 * @class exchange provides communication interface[send/receive data]
				 */
				class exchange : public network::exchange
				{
					friend class server;
					friend class client;
					friend class network::http;

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

						SSL *sslHandle;                         ///< SSL connection handle

						/**
						 * close socket connection
						 * @param socket defines socket descriptor
						 * @param sslHandle defines SSL handle
						 */
						virtual void _close(int socket, SSL *sslHandle);

						/**
						 * init current instance
						 * @param socket defines socket
						 * @param handle defines SSL handle
						 * @param blocked defines the connection block status
						 * @param blockInherited defines block flag inheritance
						 */
						virtual void init(int socket, SSL *handle, bool blocked, bool blockInherited);

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
				};
			};
		};
	};
};

#endif

#endif
