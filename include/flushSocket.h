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

#include <flushSocketEx.h>
#include <types.h>
#include <flushSocketOptions.h>
#include <flushSocketExchange.h>
#include <xexec.h>

namespace dodo
{	
	
	/**
	 * @enum flushSocketOperationTypeEnum describes type of operation for hook
	 */
	enum flushSocketOperationTypeEnum
	{
		FLUSHSOCKET_OPER_CONNECT,
		FLUSHSOCKET_OPER_CONNECT_UNIX,
		FLUSHSOCKET_OPER_BINDNLISTEN,
		FLUSHSOCKET_OPER_BINDNLISTEN_UNIX,
		FLUSHSOCKET_OPER_ACCEPT,
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
	 * @class flushSocket performs communication actions!!
	 * exchange of data is flushSocketExchange class' task; ou init it with connect or accept methods
	 */
	class flushSocket : public flushSocketOptions	
	
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
			 * @param family is family of the socket[see socketProtoFamilyEnum]
			 * @param type is type of the socket[see socketTransferTypeEnum]
			 */
			flushSocket(bool server, short family, short type);
			
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
					 * @param toInit indicates data that will path to initialize function
					 */			
					virtual xexecCounts addExec(const std::string &module, void *data, void *toInit = NULL) const;
							
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 * @param toInit indicates data that will path to initialize function
					 */
					virtual int addPostExec(const std::string &module, void *data, void *toInit = NULL) const;
					
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 * @param toInit indicates data that will path to initialize function
					 */
					virtual int addPreExec(const std::string &module, void *data, void *toInit = NULL) const;
						
				#endif		
			
			#endif
						
			/**
			 * connect from specific address. for client part
			 * @param host is ip address where to connect
			 * @param port is port where to connect
			 * @param exchange is reference to oject that will perform communication actions
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connectFrom(const std::string &local, const std::string &host, int port, flushSocketExchange &exchange);
						
			/**
			 * connect from specific address. for client part
			 * @param destinaton is structure that describes destination
			 * @param exchange is reference to oject that will perform communication actions
			 * the same as previous, but more pretty
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connectFrom(const std::string &local, const __connInfo &destinaton, flushSocketExchange &exchange);
						
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
			
			bool blockInherited;///< if true - children(flushSocketExchange) became unblocked, if parent(flushSocket) in unblocked; false by default
			
		protected:
			
			/**
			 * creates socket with given data
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							makeSocket();
	 
			mutable bool server;///< indicates whether server object or not
			
			mutable std::string unixSock;///to remember, 'cos have to unlink in destructor
	};
	
};

#endif
