/***************************************************************************
 *            ioNetworkOptions.h
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

#ifndef _IONETWORKOPTIONS_H_
#define _IONETWORKOPTIONS_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <libdodo/ioDiskTools.h>
#include <libdodo/ioNetworkOptionsEx.h>
#include <libdodo/types.h>

namespace dodo
{

	/**
	 * @enum ioNetworkOptionsTransferTypeEnum defines type of socket
	 */
	enum ioNetworkOptionsTransferTypeEnum
	{
		IONETWORKOPTIONS_TRANSFER_TYPE_STREAM,   ///< Sequenced, reliable, connection-based byte streams
		IONETWORKOPTIONS_TRANSFER_TYPE_DATAGRAM, ///< Connectionless, unreliable datagrams of fixed maximum length
	};

	/**
	 * @enum ioNetworkOptionsProtoFamilyEnum defines type of domain of socket
	 */
	enum ioNetworkOptionsProtoFamilyEnum
	{
		IONETWORKOPTIONS_PROTO_FAMILY_IPV4,
		IONETWORKOPTIONS_PROTO_FAMILY_IPV6,
		IONETWORKOPTIONS_PROTO_FAMILY_UNIX_SOCKET,
	};

	/**
	 * @enum ioNetworkOptionsOptionEnum defines options for socket
	 */
	enum ioNetworkOptionsOptionEnum
	{
		IONETWORKOPTIONS_OPTION_KEEP_ALIVE = 1,      ///< Keeps  connections  active by enabling the periodic transmission of messages, if this is supported by the protocol.
		IONETWORKOPTIONS_OPTION_REUSE_ADDRESS,       ///< Should allow reuse of local addresses[it's accepted by default].
		IONETWORKOPTIONS_OPTION_DONOT_USE_GATEWAY,   ///< Requests  that outgoing messages bypass the standard routing facilities.
		IONETWORKOPTIONS_OPTION_BROADCAST,           ///< Permits  sending of broadcast messages, if this is supported by the protocol.
		IONETWORKOPTIONS_OPTION_OOB_INLINE,          ///< Out-of-band(marked urgent) data keep inline in recieve operation.
#ifdef SO_REUSEPORT
		IONETWORKOPTIONS_OPTION_REUSE_PORT,			///< Should allow reuse of local port.
#endif
	};

	/**
	 * @enum ioNetworkOptionsLingerOptionEnum defines linger options for socket
	 */
	enum ioNetworkOptionsLingerOptionEnum
	{
		IONETWORKOPTIONS_LINGEROPTION_GRACEFUL_CLOSE,  ///< close returns immediately, but any unsent data is transmitted (after close returns).
		IONETWORKOPTIONS_LINGEROPTION_HARD_CLOSE,      ///< close returns immediately, and any unsent data is discarded.
		IONETWORKOPTIONS_LINGEROPTION_WAIT_CLOSE,      ///< (*default*) close does not return until all unsent data is transmitted (or the connection is closed by the remote system).
	};

	/**
	 * @struct __connInfo defines connection information
	 */
	struct __connInfo
	{
		dodoString host;///< host of the box 
		int port;///< port of the box
	};
	
	/**
	 * @class ioNetworkOptions provides option manipulation for network connections
	 */
	class ioNetworkOptions
	{

		protected:

			/**
			 * constructor
			 * @param family defines transfer type of domain of the socket[see ioNetworkOptionsProtoFamilyEnum]
			 * @param type defines type of the socket[see ioNetworkOptionsProtoFamilyEnum]
			 */
			ioNetworkOptions(short family, short type);

			/**
			 * constructor
			 */
			ioNetworkOptions();

			/**
			 * destructor
			 */
			virtual ~ioNetworkOptions();

		public:

			/**
			 * set socket options
			 * @param option defines option that will be applied to the socket[see ioNetworkOptionsOptionEnum]
			 * @param flag defines state of option
			 */
			virtual void setOption(short option, bool flag);

			/**
			 * @return true if socket option is set[see ioNetworkOptionsOptionEnum]
			 */
			virtual bool getOption(int option) const;

			/**
			 * set linger option
			 * @param option is linger option[see ioNetworkOptionsLingerOptionEnum]
			 * @param seconds how long to wait
			 * @note for IONETWORKOPTIONS_LINGEROPTION_WAIT_CLOSE only
			 */
			virtual void setLingerOption(short option, int seconds = 1);

			/**
			 * @return linger option that was set[see ioNetworkOptionsLingerOptionEnum]
			 */
			virtual short getLingerOption() const;

			/**
			 * @return amount of seconds to wait
			 * @note for IONETWORKOPTIONS_LINGEROPTION_WAIT_CLOSE only
			 */
			virtual int getLingerPeriod() const;

			/**
			 * set input buffer size
			 * @param bytes defines size of the buffer in bytes
			 */
			virtual void setInBufferSize(unsigned long bytes);

			/**
			 * set output buffer size
			 * @param bytes defines size of the buffer in bytes
			 */
			virtual void setOutBufferSize(unsigned long bytes);

			/**
			 * @return input buffer size in bytes
			 */
			virtual unsigned long getInBufferSize() const;

			/**
			 * @return output buffer size in bytes
			 */
			virtual unsigned long getOutBufferSize() const;

			/**
			 * set input timeout value of socket
			 * @param microseconds defines amount of time of timeout
			 */
			virtual void setInTimeout(unsigned long microseconds);

			/**
			 * set output timeout value of socket
			 * @param microseconds defines amount of time of timeout
			 */
			virtual void setOutTimeout(unsigned long microseconds);

			/**
			 * @return input timeout value
			 */
			virtual unsigned long getInTimeout() const;

			/**
			 * @return output timeout value
			 */
			virtual unsigned long getOutTimeout() const;

			/**
			 * @return true if socked is blocked
			 */
			virtual bool isBlocked() const;

			/**
			 * block/unblock socket
			 * @param flag defines block statement
			 */
			virtual void block(bool flag);

		protected:

			/**
			 * close socket connection
			 * @param socket defines socket descriptor
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

			int socket;                     ///< socket descriptor

			bool blocked;                   ///< if true socket is blocked
	};

};

#endif
