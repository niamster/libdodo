/***************************************************************************
 *            ioSocketOptions.h
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

#ifndef _IOSOCKETOPTIONS_H_
#define _IOSOCKETOPTIONS_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <libdodo/ioDiskTools.h>
#include <libdodo/tools.h>
#include <libdodo/ioSocketOptionsEx.h>
#include <libdodo/types.h>
#include <libdodo/io.h>

namespace dodo
{

	/**
	 * @enum socketTransferTypeEnum type of socket to use
	 */
	enum socketOptionsTransferTypeEnum
	{
		IOSOCKETOPTIONS_TRANSFER_TYPE_STREAM,   ///< Sequenced, reliable, connection-based byte streams
		IOSOCKETOPTIONS_TRANSFER_TYPE_DATAGRAM, ///< Connectionless, unreliable datagrams of fixed maximum length
	};

	/**
	 * @enum socketProtoFamilyEnum describes type of domain of socket to use
	 */
	enum socketOptionsProtoFamilyEnum
	{
		IOSOCKETOPTIONS_PROTO_FAMILY_IPV4,
		IOSOCKETOPTIONS_PROTO_FAMILY_IPV6,
		IOSOCKETOPTIONS_PROTO_FAMILY_UNIX_SOCKET,
	};

	/**
	 * @enum socketOptionsEnum defines options for socket
	 */
	enum socketOptionsOptionsEnum
	{
		IOSOCKETOPTIONS_SOCKET_KEEP_ALIVE = 1,      ///< Keeps  connections  active by enabling the periodic transmission of messages, if this is supported by the protocol.
		IOSOCKETOPTIONS_SOCKET_REUSE_ADDRESS,       ///<  should allow reuse of local addresses[it's accepted by default]
		IOSOCKETOPTIONS_SOCKET_DONOT_USE_GATEWAY,   ///< Requests  that outgoing messages bypass the standard routing facilities.
		IOSOCKETOPTIONS_SOCKET_BROADCAST,           ///< Permits  sending of broadcast messages, if this is supported by the protocol.
		IOSOCKETOPTIONS_SOCKET_OOB_INLINE,          ///< out-of-band(marked urgent) data keep inline in recieve operation
#ifdef SO_REUSEPORT
		IOSOCKETOPTIONS_SOCKET_REUSE_PORT,
#endif
	};

	/**
	 * @enum socketLingerOption defines linger options for socket
	 */
	enum socketOptionsLingerOptionEnum
	{
		IOSOCKETOPTIONS_SOCKET_GRACEFUL_CLOSE,  ///< close returns immediately, but any unsent data is transmitted (after close returns).
		IOSOCKETOPTIONS_SOCKET_HARD_CLOSE,      ///< close returns immediately, and any unsent data is discarded.
		IOSOCKETOPTIONS_SOCKET_WAIT_CLOSE,      ///< (*default*) close does not return until all unsent data is transmitted (or the connection is closed by the remote system).
	};

	/**
	 * @class ioSocketOptions defines options for socket connections
	 */
	class ioSocketOptions
	{

		protected:

			/**
			 * constructor
			 * @param family is family of the socket
			 * @param type is type of the socket
			 */
			ioSocketOptions(short family, short type);

			/**
			 * constructor
			 */
			ioSocketOptions();

			/**
			 * destructor
			 */
			virtual ~ioSocketOptions();

		public:

			/**
			 * set socket options
			 * @param options is option to set to socket
			 * @param flag indicates whether to set or unset option
			 */
			virtual void setSockOption(short option, bool flag);

			/**
			 * set linger option
			 * @param option is linger option[see socketLingerOption]
			 * @param seconds how long to wait(for IOSOCKETOPTIONS_SOCKET_WAIT_CLOSE only)
			 */
			virtual void setLingerSockOption(short option, int seconds = 1);

			/**
			 * @return linger option that was set[see socketLingerOption]
			 */
			virtual short getLingerOption() const;

			/**
			 * @return amount of seconds to wait(for IOSOCKETOPTIONS_SOCKET_WAIT_CLOSE only)
			 */
			virtual int getLingerPeriod() const;

			/**
			 * sets incoming buffer size of socket
			 * @param bytes is size of buffer in bytes
			 */
			virtual void setInBufferSize(unsigned long bytes);

			/**
			 * sets outgoing buffer size of socket
			 * @param bytes is size of buffer in bytes
			 */
			virtual void setOutBufferSize(unsigned long bytes);

			/**
			 * @return incoming buffer size of socket
			 */
			virtual unsigned long  getInBufferSize() const;

			/**
			 * @return outgoing buffer size of socket
			 */
			virtual unsigned long  getOutBufferSize() const;

			/**
			 * sets incomming operation timeout of socket
			 * @param microseconds is amount of time to wait for action
			 */
			virtual void setInTimeout(unsigned long microseconds);

			/**
			 * sets outgoing operation timeout of socket
			 * @param microseconds is amount of time to wait for action
			 */
			virtual void setOutTimeout(unsigned long microseconds);

			/**
			 * @return incomming operation timeout of socket
			 */
			virtual unsigned long getInTimeout() const;

			/**
			 * @return outgoing operation timeout of socket
			 */
			virtual unsigned long getOutTimeout() const;


			/**
			 * @return true if socket option was set
			 */
			virtual bool getSocketOpts(int option) const;

			/**
			 * @return true if socked is blocked
			 */
			virtual bool isBlocked() const;

			/**
			 * blocks/unblocks socket
			 * @param flag indicates ehether to block or unblock socket
			 */
			virtual void block(bool flag);

		protected:

			/**
			 * closes connection for socket
			 */
			static void _close(int socket);

			short family;                   ///< socket family
			short type;                     ///< socket type

			int socketOpts;                 ///< socket options

			short lingerOpts;               ///< socket linger option
			int lingerSeconds;              ///< socket linger timeout

			unsigned long inTimeout;        ///< incomming operation timeout of socket; in microseconds
			unsigned long outTimeout;       ///< outgoing operation timeout of socket; in microseconds

			unsigned long inSocketBuffer;   ///< incoming buffer size of socket; in bytes
			unsigned long outSocketBuffer;  ///< outgoing buffer size of socket; in bytes

			int socket;                     ///< id of socket

			bool blocked;                   ///< indicates, whether blocked or not;
	};

};

#endif
