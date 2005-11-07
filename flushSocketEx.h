/***************************************************************************
 *            flushSocketEx.h
 *
 *  Mon Feb 21 03:03:47 2005
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _FLUSHSOCKETEX_H_
#define _FLUSHSOCKETEX_H_

#include "directives.h"
#include "baseEx.h"

#ifndef NO_EX

	namespace dodo
	{
		/**
		* class to catch and resolve exceptions in flush
		* by default it does nothing
		* but u can define functions for resolving
		* for example to resolve problem with NOT_ENOUGH_FREE_SPACE:
		*		void someF(stringEx *strEx)
		*		{
		*			///do smth
		*		}
		*		setResolve(NOT_ENOUGH_FREE_SPACE, someF);
		*	u also can combine reasons with '|'
		*   if u want to reset to default use NULL;
		*/	
		
		///reasons of exception
		enum flushSocketExR
		{
			FLUSHSOCKET_NOT_SUPPORTED_ADDR_FAMILY = 1,
			FLUSHSOCKET_ACCESS_DENIED,
			FLUSHSOCKET_UNKNOWN_PROTOCOL,
			FLUSHSOCKET_TOO_MANY_OPEN_FILE,
			FLUSHSOCKET_PROTO_NOT_SUPPORTED_WITHIN_DOMAIN,
			FLUSHSOCKET_MEMORY_OVER,
			FLUSHSOCKET_NO_FREE_PORTS,
			FLUSHSOCKET_ALREADY_USED,
			FLUSHSOCKET_CONNECTION_REFUSED,
			FLUSHSOCKET_NETWORK_UNREACHABLE,
			FLUSHSOCKET_ALREADY_CONNECTED,
			FLUSHSOCKET_SYSTEM_FAULT,///also: when use unix_socket - file exists, but not a socket
			FLUSHSOCKET_NOT_A_SOCKET,
			FLUSHSOCKET_NOT_FOUND,
			FLUSHSOCKET_NAMESERVER_ERR,
			FLUSHSOCKET_UNKNOWN
		};
		
		class flushSocket;
		class flushSocketEx;
			
		typedef void (*flushSocketExResolver)(flushSocketEx*);
		
		class flushSocketEx :virtual public baseEx
		{
			public:
	
				virtual baseEx *getSelf();
				virtual int getExID();
	
				flushSocketEx(flushSocketExR reason, flushSocket *obj, unsigned long line, std::string file);
				virtual ~flushSocketEx();
				virtual void setResolve(flushSocketExR,flushSocketExResolver);///try to resolve by own function
				virtual void resolve();//try to resolve
			
				flushSocketExR reason;///reason
			
				flushSocket *obj;///where exception appeared	
			
			private:
				
				flushSocketExResolver resolvers[16];///functions - solvers			
		};
	};

#endif	
	
#endif
