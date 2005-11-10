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
		///reasons of exception
		enum flushSocketExR
		{
			FLUSHSOCKET_MEMORY_OVER = 1,
			FLUSHSOCKET_NO_SOCKET_CREATED,
			FLUSHSOCKET_CANNOT_BIND,
			FLUSHSOCKET_CANNOT_CONNECT,
		};
		
		#define FLUSHSOCKET_NO_SOCKET_CREATED_STR "No socket was created =(. Please try to run `init` function"
		#define FLUSHSOCKET_MEMORY_OVER_STR "Not anough memmory"
		#define FLUSHSOCKET_CANNOT_BIND_STR "You cannot <bindNListen> with this constructor; choose another one!"
		#define FLUSHSOCKET_CANNOT_CONNECT_STR "You cannot <connect> with this constructor; choose another one!"
		
		enum flushSocketFunctionsID
		{
			FLUSHSOCKET_MAKESOCKET,
			FLUSHSOCKET_CONNECT,
			FLUSHSOCKET_MAKEUNIXSOCKET,
			FLUSHSOCKET_SETINBUFFERSIZE,
			FLUSHSOCKET_SETOUTBUFFERSIZE,
			FLUSHSOCKET_SETINTIMEOUT,
			FLUSHSOCKET_SETOUTTIMEOUT,
			FLUSHSOCKET_SETSOCKOPT,
			FLUSHSOCKET_SETLINGERSOCKOPT,
			FLUSHSOCKET_GETHOSTINFO,
			FLUSHSOCKET_BINDNLISTEN,
			FLUSHSOCKET__CLOSE,		
			FLUSHSOCKET_CLOSE,
			FLUSHSOCKET_SETLOCALNAME
			
		};
		
	};

#endif	
	
#endif
