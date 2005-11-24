/***************************************************************************
 *            flushSocket.h
 *
 *  Thu Oct 04 02:02:24 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _FLUSHSOCKET_H_
#define _FLUSHSOCKET_H_

#include "types.h"
#include "directives.h"
#include "flush.h"
#include "flushSocketEx.h"
#include "flushDisk.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/un.h>
#include <sys/socket.h>

namespace dodo
{	
	#define UNIX_SOCKET_PERM (OWNER_READ_ACCESS|OWNER_WRITE_ACCESS)
	
	enum flushSocketOperationTypeEnum///for xExec
	{
		FLUSHSOCKET_OPER_RECIEVE,
		FLUSHSOCKET_OPER_SEND,
		FLUSHSOCKET_OPER_CONNECT,
		FLUSHSOCKET_OPER_CONNECT_UNIX,
		FLUSHSOCKET_OPER_BINDNLISTEN,
		FLUSHSOCKET_OPER_BINDNLISTEN_UNIX,
		FLUSHSOCKET_OPER_CLOSE,
		FLUSHSOCKET_OPER_ACCEPT,
	};	
	
	/**
	 * type of socket to use
	 */
	enum socketTransferTypeEnum
	{
		TRANSFER_TYPE_STREAM,///Sequenced, reliable, connection-based byte streams
		TRANSFER_TYPE_DATAGRAM,///Connectionless, unreliable datagrams of fixed maximum length
	};
	
	/**
	 * type of domain
	 * as response for more type of protocol and address famaly will increase - i'll add. i don't think that u use appletalk or ipx! as for start - these !
	 */
	enum socketProtoFamilyEnum
	{
		PROTO_FAMILY_IPV4,
		PROTO_FAMILY_IPV6,		
		PROTO_FAMILY_UNIX_SOCKET,
	};
	
	/**
	 * Options for socket
	 */
	 enum socketOptionsEnum
	 {
	 	SOCKET_KEEP_ALIVE=2,///Keeps  connections  active by enabling the periodic transmission of messages, if this is supported by the protocol.
	 	SOCKET_REUSE_ADDRESS=4,/// 	
	 	SOCKET_DONOT_USE_GATEWAY=8, 	
	 	SOCKET_BROADCAST=16,///Permits  sending of broadcast messages, if this is supported by the protocol. 	
	 	SOCKET_OOB_INLINE=32,///out-of-band(marked urgent) data keep inline in recieve operation
	 	#ifdef SO_REUSEPORT
		 	SOCKET_REUSE_PORT=512,
		#endif
	 };
	 /**
	  * send unsent messages in socket queue if close called
	  */
	 enum socketLingerOption
	 { 
	 	SOCKET_GRACEFUL_CLOSE,///close returns immediately, but any unsent data is transmitted (after close returns).
	 	SOCKET_HARD_CLOSE,///close returns immediately, and any unsent data is discarded.
	 	SOCKET_WAIT_CLOSE,///(*default*) close does not return until all unsent data is transmitted (or the connection is closed by the remote system).
	 };
	 
	 /**
	  * info about host
	  */
	 struct __hostInfo
	 {
	 	std::string name;
	 	stringArr aliases;
	 	stringArr addresses;
	 };
	 
	/**
	 * info about service
	 */
	struct __servInfo
	{
	 	std::string name;
	 	stringArr aliases;
		int port;
	};
	
	/**
	 * you may use it for connect() method or accept()[info about remote connected host]
	 */
	struct __connInfo
	{
		std::string host;
		int port;
	};
	
	/**
	 * class that have same options as for flushSocketExchange as for flushSocket
	 */
	class flushSocketOptions
	{
		
		friend class flushSocket;
		friend class flushSocketExchange;
		
		protected:
		
			///constructor
			flushSocketOptions(socketProtoFamilyEnum family, socketTransferTypeEnum type);
			flushSocketOptions();
			virtual ~flushSocketOptions();
		
		public:
				
			/**
			 * set socket options.
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setSockOption(socketOptionsEnum option, bool flag);
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setLingerSockOption(socketLingerOption option, int seconds=1);///seconds is used only for SOCKET_WAIT_CLOSE
			
			virtual socketLingerOption getLingerOption();
			virtual int getLingerPeriod();///only for SOCKET_WAIT_CLOSE
			
			/**
			 * The maximum buffer size for stream sockets is 262144 bytes
			 */ 
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setInBufferSize(int bytes);///accept value to socket; size of socket buffer!
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setOutBufferSize(int bytes);///accept value to socket; size of socket buffer!
			
			virtual int getInBufferSize();
			virtual int getOutBufferSize();
			
				
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setInTimeout(unsigned long microseconds);///accept value to socket; timeout for operation
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setOutTimeout(unsigned long microseconds);///accept value to socket; timeout for operation
			
			virtual unsigned long getInTimeout();
			virtual unsigned long getOutTimeout();
			
			
			/**
			 * socketOptionsEnum
			 */
			virtual bool getSocketOpts(int option);
			
		protected:
			
			socketProtoFamilyEnum family;
			socketTransferTypeEnum type;
					
			int socketOpts;
			socketLingerOption lingerOpts;
			int lingerSeconds;
			
			unsigned long inTimeout;///in microseconds
			unsigned long outTimeout;///in microseconds
			 
			int inSocketBuffer;
			int outSocketBuffer;

			int socket;///id of socket
				
			bool block;///now is only *nix compatible
	};
	
	/**
	 * it's passes to accept call, and then inits flushSocketExchange;
	 */
	class __initialAccept
	{
		friend class flushSocketExchange;
		friend class flushSocket;
		
		public:			
		
			__initialAccept();
			__initialAccept(__initialAccept &init);///if you want to copy it, the object, from what was copy is not be able to init new session; you have to reinit it with accept call!
		
		
		private:
				
			int socket;
			socketProtoFamilyEnum family;
			socketTransferTypeEnum type;			
	};
		
		
	class flushSocketExchange;	
	/**
	 * class that takes ugly routine with sockets;
	 * exchange of data is flushSocketExchange class' task
	 * this class can establish connections; the below class can send/recieve data!
	 */
	class flushSocket : protected flush, public flushSocketOptions
	{
		friend class flushSocketExchange;
		
		private:
				
			/**
			 * to prevent copying (pass to functions ...)
			 */		
			flushSocket(flushSocket &fs);
		
		public:
			
			/**
			 * for xExec
			 */			
			virtual int addPostExec(inExec func, void *data) const;
			virtual int addPreExec(inExec func, void *data) const;	
			virtual int addPostExec(const std::string &, void *data) const;
			virtual int addPreExec(const std::string &, void *data) const;	
											
			/**
			 * return self, casted to base class - dodoBase; usefull to cast from child to parent;
			 */		
			virtual dodoBase * const getSelf();
					
			/**
			 * constructors/destructors
			 */
			flushSocket(unsigned long numberOfConn, socketProtoFamilyEnum family, socketTransferTypeEnum type);///for server; for TRANSFER_TYPE_DATAGRAM internally number of connections overrides to 1!
			flushSocket(socketProtoFamilyEnum family, socketTransferTypeEnum type);///for client
			virtual ~flushSocket();
			
			/**
			 * connect. for client part
			 * host - ip address; call getHostInfo to get address' for it.
			 * first - net connection
			 * second - local connectioin
			 * if u use unix-socket - it will create it.
			 * u do not have to construct flushSocketExchange!! the function will construct it!
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connect(const std::string &host, int port, flushSocketExchange &exchange);
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connect(const __connInfo &destinaton, flushSocketExchange &exchange);///the same as previous, but more pretty. alias.
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connect(const std::string &path, flushSocketExchange &exchange);///if socket is already created - nothin' will be done for creation. if file exists, but not socket - ex will be thrown (or false will be returned)!
			
			/**
			 * connect. for server part
			 * host - ip address; call getHostInfo to get address' for it.
			 * first - net connection
			 * second - local connectioin
			 * if u use unix-socket - it will create it. 
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							bindNListen(const std::string &host, int port);///host - can be '*' -> any address
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							bindNListen(const __connInfo &destinaton);///the same as previous, but more pretty. alias.
						
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							bindNListen(const std::string &path, bool force = false);///if socket is already created and force = true and it's a socket - delete it!!			
			
			/**
			 * accepts incommin' connectins(as for server)
			 * on accept - return true;
			 * if was defined NO_EX - no way to detect error
			 * also returns info about connected host
			 * with PROTO_FAMILY_UNIX_SOCKET `host` will be always empty, so you may use second function
			 * with TRANSFER_TYPE_DATAGRAM is always returns true, so u should skip calling this function
			 */
			virtual bool accept(__initialAccept &init, __connInfo &info);
			virtual bool accept(__initialAccept &init);///if you don't want to know anythin' about remote; not just alias. a little bit faster!
			
			/**
			 * get info about given host
			 */
			static __hostInfo getHostInfo(const std::string &host);
			
			/**
			 * get name of localhost
			 */
			static std::string getLocalName();
			
			/**
			 * sets local name
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setLocalName(const std::string &host);
								
			/**
			 * get info about service(port, protocol...)
			 */
			static __servInfo getServiceInfo(const std::string &service, const std::string &protocol);
			static __servInfo getServiceInfo(int port, const std::string &protocol);
			
			/*
			virtual bool getBlock();

			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif						
							setBlock();*/
			
		private:
			
			/**
			 * creates socket with given data
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							makeSocket();

			/**
			 * closes main socket for server part(called only in destructor)
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			 
							_close(int socket); 
			/**
			 * number of connections that can recieve
			 */			 
			long numberOfConn;///number of connection for client = -1
			long accepted;///how many was accepted
			
			std::string unixSock;///to remember, 'cos have to unlink in destructor
	};
	
	/**
	 * class used for send/recieve data
	 * you may use it's functions only after passing it to connect(accept)
	 * otherwise you'll recieve exeptions about socket(or false) from all of this' class' methods
	 * if you'll init this class again with another connection = previous will be closed
	 */
	 class flushSocketExchange : public flush, public flushSocketOptions
	 {
	 	
	 	friend class flushSocket;
	 	
		public:
					
			/**
			 * return self, casted to base class - dodoBase; usefull to cast from child to parent;
			 */		
			virtual dodoBase * const getSelf();
			
			/**
			 * for xExec
			 */			
			virtual int addPostExec(inExec func, void *data) const;
			virtual int addPreExec(inExec func, void *data) const;	
			virtual int addPostExec(const std::string &, void *data) const;
			virtual int addPreExec(const std::string &, void *data) const;	
											
			/**
			 * constructors/destructors
			 */	
			flushSocketExchange();
			flushSocketExchange(flushSocketExchange &fse);///init with the same object; object that inited new u can use for future connections; u can safely pass it to the functions;
			flushSocketExchange(__initialAccept &init);///init with accept given data
			virtual ~flushSocketExchange();
	
			/**
			 * init 
			 */
			virtual void init(__initialAccept &init);///init with accept given data
		
			/**
			 * indicates, whether connection alive or not
			 */
			virtual bool alive();
			
			/**
			 * send, recieve
			 * 
			 * sends no longer than inSize,outSize;
			 * if inSize,outSize bigger than socket buf - sends with few iterations
			 * 
			 */			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							send(const char * const data, bool urgent = false);///urgent = true -> Sends out-of-band data
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif				
							sendString(const std::string &data, bool urgent = false);///urgent = true -> Sends out-of-band data
			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							recieve(char * const data, bool urgent = false);///urgent = true -> Receipt  of out-of-band data
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif				
							recieveString(std::string &data, bool urgent = false);///urgent = true -> Receipt  of out-of-band data
						
			/**
			 * closes this stream
			 */			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							close();
			
		protected:	
					
			/**
			 * inits this class' data
			 */		
			virtual void init(int socket);		
	 };
};

#endif /*SOCKETPP_H_*/
