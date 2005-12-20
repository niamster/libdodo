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

#include <directives.h>

#include <types.h>
#include <flush.h>
#include <flushSocketEx.h>
#include <flushDisk.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/un.h>
#include <sys/socket.h>

#include <net/if.h>
#include <sys/ioctl.h>

namespace dodo
{	
	
	/**
	 * @enum flushSocketOperationTypeEnum describes type of operation for hook
	 */
	enum flushSocketOperationTypeEnum
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
	 * @enum socketTransferTypeEnum type of socket to use
	 */
	enum socketTransferTypeEnum
	{
		TRANSFER_TYPE_STREAM,///< Sequenced, reliable, connection-based byte streams
		TRANSFER_TYPE_DATAGRAM,///< Connectionless, unreliable datagrams of fixed maximum length
	};
	
	/**
	 * @enum socketProtoFamilyEnum describes type of domain of socket to use
	 */
	enum socketProtoFamilyEnum
	{
		PROTO_FAMILY_IPV4,
		PROTO_FAMILY_IPV6,		
		PROTO_FAMILY_UNIX_SOCKET,
	};
	
	/**
	 * @enum socketOptionsEnum defines options for socket
	 */
	 enum socketOptionsEnum
	 {
	 	SOCKET_KEEP_ALIVE=1,///< Keeps  connections  active by enabling the periodic transmission of messages, if this is supported by the protocol.
	 	SOCKET_REUSE_ADDRESS,///<  should allow reuse of local addresses	
	 	SOCKET_DONOT_USE_GATEWAY,///< Requests  that outgoing messages bypass the standard routing facilities.	
	 	SOCKET_BROADCAST,///< Permits  sending of broadcast messages, if this is supported by the protocol. 	
	 	SOCKET_OOB_INLINE,///< out-of-band(marked urgent) data keep inline in recieve operation
	 	#ifdef SO_REUSEPORT
		 	SOCKET_REUSE_PORT,
		#endif
	 };
	 
	 /**
	  * @enum socketLingerOption defines linger options for socket
	  */
	 enum socketLingerOption
	 { 
	 	SOCKET_GRACEFUL_CLOSE,///< close returns immediately, but any unsent data is transmitted (after close returns).
	 	SOCKET_HARD_CLOSE,///< close returns immediately, and any unsent data is discarded.
	 	SOCKET_WAIT_CLOSE,///< (*default*) close does not return until all unsent data is transmitted (or the connection is closed by the remote system).
	 };
	 
	 /**
	  * @struct __ifInfo describes interface info
	  */
	 struct __ifInfo
	 {
	 	std::string address;///< ip address of the interface
	 	std::string breadcast;///< broadcast address of the interface
	 	std::string netmask;///< netmask of the interface
	 	std::string hwaddr;///< harware address of the interface(MAC)
	 	
	 	bool up;///< true if interface is up
	 	bool loop;///< true is a loopback
	 };
	 
	 /**
	  * @struct __hostInfo describes info about host
	  */
	 struct __hostInfo
	 {
	 	std::string name;///< original name of the host
	 	stringArr aliases;///< aliases of the host
	 	stringArr addresses;///< addresses of the host
	 };
	 
	/**
	 * @struct __servInfo describes info about service
	 */
	struct __servInfo
	{
	 	std::string name;///< original name of the service
	 	stringArr aliases;///< sevice's aliases
		int port;///< port of service
	};
	
	/**
	 * @struct __connInfo describes connections info
	 * you may use it for connect() method or accept()[info about remote connected host]
	 */
	struct __connInfo
	{
		std::string host;
		int port;
	};
	
	/**
	 * @class flushSocketOptions defines options for socket connections
	 */
	class flushSocketOptions
	{
		
		friend class flushSocket;
		friend class flushSocketExchange;
		
		protected:
		
			/**
			 * constructor
			 * @param family is family of the socket
			 * @param type is type of the socket
			 */
			flushSocketOptions(socketProtoFamilyEnum family, socketTransferTypeEnum type);
			
			/**
			 * constructor
			 */			
			flushSocketOptions();
			
			/**
			 * destructor
			 */
			virtual ~flushSocketOptions();
		
		public:
				
			/**
			 * set socket options
			 * @param options is option to set to socket
			 * @param flag indicates whether to set or unset option
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setSockOption(socketOptionsEnum option, bool flag);
			
			/**
			 * set linger option
			 * @param option is linger option
			 * @param seconds how long to wait(for SOCKET_WAIT_CLOSE only)
			 */				
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setLingerSockOption(socketLingerOption option, int seconds=1);
			
			/**
			 * @return linger option that was set
			 */
			virtual socketLingerOption getLingerOption() const;
			
			/**
			 * @return amount of seconds to wait(for SOCKET_WAIT_CLOSE only)
			 */
			virtual int getLingerPeriod() const;
			
			/**
			 * sets incoming buffer size of socket
			 * @param bytes is size of buffer in bytes
			 */ 
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setInBufferSize(int bytes);

			/**
			 * sets outgoing buffer size of socket
			 * @param bytes is size of buffer in bytes
			 */ 
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setOutBufferSize(int bytes);
			
			/**
			 * @return incoming buffer size of socket
			 */
			virtual int getInBufferSize() const;
			
			/**
			 * @return outgoing buffer size of socket
			 */
			virtual int getOutBufferSize() const;

			/**
			 * sets incomming operation timeout of socket
			 * @param microseconds is amount of time to wait for action
			 */ 
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setInTimeout(unsigned long microseconds);

			/**
			 * sets outgoing operation timeout of socket
			 * @param microseconds is amount of time to wait for action
			 */ 
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							setOutTimeout(unsigned long microseconds);
			
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
			
		protected:
			
			mutable socketProtoFamilyEnum family;///< socket family
			mutable socketTransferTypeEnum type;///< socket type
					
			mutable int socketOpts;///< socket options
			
			mutable socketLingerOption lingerOpts;///< socket linger option
			mutable int lingerSeconds;///< socket linger timeout
			
			mutable unsigned long inTimeout;///< incomming operation timeout of socket; in microseconds
			mutable unsigned long outTimeout;///< outgoing operation timeout of socket; in microseconds
			 
			mutable int inSocketBuffer;///< incoming buffer size of socket; in bytes
			mutable int outSocketBuffer;///< outgoing buffer size of socket; in bytes

			int socket;///< id of socket
	};
	
	/**
	 * @class __initialAccept holds info that passes to accept call, and then inits flushSocketExchange;
	 */
	class __initialAccept
	{
		friend class flushSocketExchange;
		friend class flushSocket;
		
		public:			
		
			/**
			 * constructor
			 */
			__initialAccept();
			
			/**
			 * copy constructor
			 * @note if you want to copy it, the object, from what was copy is not be able to init new session => you have to reinit it with accept method!
			 */
			__initialAccept(__initialAccept &init);
		
		
		private:
				
			int socket;///< id of socket
			
			socketProtoFamilyEnum family;///< socket family
			socketTransferTypeEnum type;///< socket type			
	};
		
		
	class flushSocketExchange;
	
	/**
	 * @class flushSocket performs communication actions!!
	 * exchange of data is flushSocketExchange class' task; ou init it with connect or accept methods
	 */
	class flushSocket : protected flush, public flushSocketOptions	
	
	#ifndef FLUSH_SOCKET_WO_XEXEC
								, public xexec
	#endif
	
	{
		friend class flushSocketExchange;
		
		private:
				
			/**
			 * connstructor
			 * to prevent copying
			 */		
			flushSocket(flushSocket &fs);
		
		public:
		
			/**
			 * constructors
			 * @param server indicates what type of oject will be
			 * @param family is family of the socket
			 * @param type is type of the socket
			 */
			flushSocket(bool server, socketProtoFamilyEnum family, socketTransferTypeEnum type);
			
			/**
			 * destructor
			 */
			virtual ~flushSocket();
			
			#ifndef FLUSH_SOCKET_WO_XEXEC
					
				/**
				 * adds hook after the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */			
				virtual int addPostExec(inExec func, void *data) const;
				
				/**
				 * adds hook before the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPreExec(inExec func, void *data) const;
				
				#ifdef DL_EXT
	
					/**
					 * set function from module that will be executed before/after the main action call
					 * the type of hook[pre/post] is defined in module
					 * @return number in list where function is set
					 * @param func is a pointer to function
					 * @param data is pointer to data toy want to pass to hook
					 */			
					virtual xexecCounts addExec(const std::string &module, void *data) const;
							
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPostExec(const std::string &module, void *data) const;
					
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPreExec(const std::string &module, void *data) const;
						
				#endif		
			
			#endif
						
			/**
			 * connect. for client part
			 * @param host is ip address where to connect
			 * @param port is port where to connect
			 * @param exchange is reference to oject that will perform communication actions
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connect(const std::string &host, int port, flushSocketExchange &exchange);
						
			/**
			 * connect. for client part
			 * @param destinaton is structure that describes destination
			 * @param exchange is reference to oject that will perform communication actions
			 * the same as previous, but more pretty
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connect(const __connInfo &destinaton, flushSocketExchange &exchange);
						
			/**
			 * connect. for client part
			 * @param path is path to unix socket
			 * @param exchange is reference to oject that will perform communication actions
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connect(const std::string &path, flushSocketExchange &exchange);///if socket is already created - nothin' will be done for creation. if file exists, but not socket - ex will be thrown (or false will be returned)!
			
			/**
			 * connect. for server part
			 * @param host is ip address that would be listen; can be '*' -> any address
			 * @param port is port where to listen
			 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							bindNListen(const std::string &host, int port, int numberOfConnections);
			
			/**
			 * connect. for server part
			 * @param destinaton is structure that describes destination
			 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
			 * the same as previous, but more pretty
			 */							
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							bindNListen(const __connInfo &destinaton, int numberOfConnections);

			/**
			 * connect. for server part
			 * @param path is path to unix socket
			 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
			 * @note if socket is already created and force = true and it's a socket - delete it!!
			 */						
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							bindNListen(const std::string &path, int numberOfConnections, bool force = false);	
			
			/**
			 * accepts incommin' connections(as for server)
			 * @return true on accept; with TRANSFER_TYPE_DATAGRAM is always returns true, so u should skip calling this function
			 * @param init will be filled with info that will init flushSocketExchange object
			 * @param info is info about connected host
			 * @note if was defined NO_EX - no way to detect error
			 * with PROTO_FAMILY_UNIX_SOCKET `info` will be always empty, so you may use second function
			 */
			virtual bool accept(__initialAccept &init, __connInfo &info) const;
			
			/**
			 * accepts incommin' connections(as for server)
			 * @return true on accept; with TRANSFER_TYPE_DATAGRAM is always returns true, so u should skip calling this function
			 * @param init will be filled with info that will init flushSocketExchange object
			 * @note if was defined NO_EX - no way to detect error
			 * if you don't want to know anythin' about remote; not just alias. a little bit faster!
			 */
			virtual bool accept(__initialAccept &init) const;
			
			/**
			 * @return info abou interface
			 */
			static __ifInfo getInterfaceInfo(const std::string &interface);
			
			/**
			 * @return info about given host
			 * @param host points to host about what info would be given
			 */
			static __hostInfo getHostInfo(const std::string &host);
			
			/**
			 * @return name of local host
			 */
			static std::string getLocalName();
			
			/**
			 * sets local name
			 * @param host is new  name of the host
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setLocalName(const std::string &host);
								
			/**
			 * @return info about service
			 * @param service is service name
			 * @param protocol specifies protocol of service(tcp, udp ..)
			 */
			static __servInfo getServiceInfo(const std::string &service, const std::string &protocol);
								
			/**
			 * @return info about service
			 * @param port is port of service
			 * @param protocol specifies protocol of service(tcp, udp ..)
			 */
			static __servInfo getServiceInfo(int port, const std::string &protocol);
			
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
			 * closes connection for socket
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			 
							_close(int socket); 
	 
			mutable bool server;///< indicates whether server object or not
			
			mutable std::string unixSock;///to remember, 'cos have to unlink in destructor
	};
	
	/**
	 * @class flushSocketExchange used for communication[send/recieve data]
	 * you may use it's functions only after passing it to connect(accept)
	 * otherwise you'll recieve exeptions about socket(or false) from all of this' class' methods
	 * if you'll init this class again with another connection = previous will be closed
	 */
	class flushSocketExchange : public flush, public flushSocketOptions	
	
	#ifndef FLUSH_SOCKETEXCHANGE_WO_XEXEC
								, public xexec
	#endif
	
	 
	 {
	 	
	 	friend class flushSocket;
	 	
		public:
											
			/**
			 * constructor
			 */	
			flushSocketExchange();
			
			/**
			 * copy constructor
			 * @note object that inited new object of this class you can use for future connections; 
			 * you can safely pass it to the functions;
			 */	
			flushSocketExchange(flushSocketExchange &fse);

			/**
			 * constructor
			 * @param init is initial data[got from accept method]
			 * @note object that inited new object of this class you can use for future connections; 
			 */	
			flushSocketExchange(__initialAccept &init);
			
			/**
			 * destructor
			 */
			virtual ~flushSocketExchange();
			
			#ifndef FLUSH_SOCKETEXCHANGE_WO_XEXEC
					
				/**
				 * adds hook after the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */			
				virtual int addPostExec(inExec func, void *data) const;
				
				/**
				 * adds hook before the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPreExec(inExec func, void *data) const;
				
				#ifdef DL_EXT
	
					/**
					 * set function from module that will be executed before/after the main action call
					 * the type of hook[pre/post] is defined in module
					 * @return number in list where function is set
					 * @param func is a pointer to function
					 * @param data is pointer to data toy want to pass to hook
					 */			
					virtual xexecCounts addExec(const std::string &module, void *data) const;
							
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPostExec(const std::string &module, void *data) const;
					
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPreExec(const std::string &module, void *data) const;
					
				#endif	
			
			#endif
				
			/**
			 * init oject with given data
			 * @param init is initial data[got from accept method]
			 */
			virtual void init(__initialAccept &init);
		
			/**
			 * @return true if connection is alive
			 */
			virtual bool alive() const;
			
			/**
			 * send
			 * @param data is data that would be sent
			 * @param urgent -> send out-of-band data
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 */			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							send(const char * const data, bool urgent = false) const;
			
			/**
			 * send
			 * @param data is string that would be sent
			 * @param urgent -> send out-of-band data
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif				
							sendString(const std::string &data, bool urgent = false) const;
			
			/**
			 * recieve
			 * @param data is data that would be recieved
			 * @param urgent -> recieves out-of-band data
			 * @note recieves no longer than inSize
			 * if outSize bigger than socket buffer size - recieves with few iterations
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							recieve(char * const data, bool urgent = false) const;
			
			/**
			 * recieve
			 * @param data is string that would be recieved
			 * @param urgent -> recieves out-of-band data
			 * @note recieves no longer than inSize
			 * if outSize bigger than socket buffer size - recieves with few iterations
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif				
							recieveString(std::string &data, bool urgent = false) const;///urgent = true -> Receipt  of out-of-band data
						
			/**
			 * closes this socket
			 */			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							close() const;
			
		protected:	
					
			/**
			 * inits this class' data
			 * @param socket is id of socket
			 */		
			virtual void init(int socket);	

			/**
			 * @note share vars
			 */			
			mutable long iter;///< amount of iterations to do to operate with data
			mutable long rest;///< amount of data that is needed to operate at last	
	 };
};

#endif /*SOCKETPP_H_*/
