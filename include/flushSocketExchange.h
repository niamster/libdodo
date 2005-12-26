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
 
#ifndef _FLUSHSOCKETEXCHANGE_H_
#define _FLUSHSOCKETEXCHANGE_H_

#include <directives.h>

#include <types.h>
#include <flush.h>
#include <flushSocketExchangeEx.h>
#include <flushSocketOptions.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

namespace dodo
{	
	
	/**
	 * @enum flushSocketOperationTypeEnum describes type of operation for hook
	 */
	enum flushSocketExchangeOperationTypeEnum
	{
		FLUSHSOCKETEXCHANGE_OPER_RECIEVE,
		FLUSHSOCKETEXCHANGE_OPER_SEND,
		FLUSHSOCKETEXCHANGE_OPER_CLOSE,
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
			
			bool blocked;///< if blocked
			bool blockInherited;///< if block inherited	
	};
		
	/**
	 * @class flushSocketExchange used for communication[send/receive data]
	 * you may use it's functions only after passing it to connect(accept)
	 * otherwise you'll receive exeptions about socket(or false) from all of this' class' methods
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
			 * receive
			 * @param data is data that would be received
			 * @param urgent -> receives out-of-band data
			 * @note receives no longer than inSize
			 * if outSize bigger than socket buffer size - receives with few iterations
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							receive(char * const data, bool urgent = false) const;
			
			/**
			 * receive
			 * @param data is string that would be received
			 * @param urgent -> receives out-of-band data
			 * @note receives no longer than inSize
			 * if outSize bigger than socket buffer size - receives with few iterations
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif				
							receiveString(std::string &data, bool urgent = false) const;///urgent = true -> Receipt  of out-of-band data
						
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
			 * @param blockInherited indicates whether to inherit block of parent
			 */		
			virtual void init(int socket, bool blockInherited);	

		private:
		
			/**
			 * @note share vars
			 */			
			mutable long iter;///< amount of iterations to do to operate with data
			mutable long rest;///< amount of data that is needed to operate at last	
	 };
};

#endif /*SOCKETPP_H_*/
