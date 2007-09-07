/***************************************************************************
 *            flushSocketEx.h
 *
 *  Mon Feb 21 03:03:47 2005
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

#ifndef _FLUSHSOCKETEX_H_
#define _FLUSHSOCKETEX_H_

#include <directives.h>

#include <baseEx.h>

namespace dodo
{
	/**
	 * next for flushSocket*
	 */

	/**
	 * libdodo defined errors
	 */
	enum flushSocketExR
	{
		FLUSHSOCKET_NO_SOCKET_CREATED,
		FLUSHSOCKET_CANNOT_BIND,
		FLUSHSOCKET_CANNOT_CONNECT,
		FLUSHSOCKET_CANNOT_ACCEPT,
		FLUSHSOCKET_ACCEPT_WO_BIND,
		FLUSHSOCKET_WRONG_PARAMETHER,
		FLUSHSOCKET_WRONG_FILENAME,
	};

	/**
	 * libdodo defined errors' explanation
	 */
        #define FLUSHSOCKET_NO_SOCKET_CREATED_STR    "No socket was created =(."
        #define FLUSHSOCKET_CANNOT_BIND_STR          "You cannot <bindNListen> with this constructor; choose another one!"
        #define FLUSHSOCKET_CANNOT_CONNECT_STR       "You cannot <connect> with this constructor; choose another one!"
        #define FLUSHSOCKET_CANNOT_ACCEPT_STR        "You cannot <accept> with this constructor; choose another one!"
        #define FLUSHSOCKET_ACCEPT_WO_BIND_STR       "You try to accept without called bindNListen or bindNListen failed."
        #define FLUSHSOCKET_WRONG_PARAMETHER_STR     "Wrong paramather passed to function."
        #define FLUSHSOCKET_WRONG_FILENAME_STR       "Probably wrong filename, type of file is wrong or `path` is empty!"

	/**
	 * ID of function where exception was thrown
	 */
	enum flushSocketFunctionsID
	{
		FLUSHSOCKET_MAKESOCKET,
		FLUSHSOCKET_CONNECT,
		FLUSHSOCKET_MAKEUNIXSOCKET,
		FLUSHSOCKET_BINDNLISTEN,
		FLUSHSOCKET_ACCEPT,
		FLUSHSOCKET_SEND,
		FLUSHSOCKET_RECIEVE,
		FLUSHSOCKET_CONNECTFROM
	};

};

#endif
