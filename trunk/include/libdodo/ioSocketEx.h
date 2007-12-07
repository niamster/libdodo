/***************************************************************************
 *            ioSocketEx.h
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

#ifndef _IOSOCKETEX_H_
#define _IOSOCKETEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * next for ioSocket*
	 */

	/**
	 * libdodo defined errors
	 */
	enum ioSocketExR
	{
		IOIOSOCKETOPTIONS_SOCKET_NO_IOSOCKETOPTIONS_SOCKET_CREATED,
		IOIOSOCKETOPTIONS_SOCKET_CANNOT_BIND,
		IOIOSOCKETOPTIONS_SOCKET_CANNOT_CONNECT,
		IOIOSOCKETOPTIONS_SOCKET_CANNOT_ACCEPT,
		IOIOSOCKETOPTIONS_SOCKET_ACCEPT_WO_BIND,
		IOIOSOCKETOPTIONS_SOCKET_WRONG_PARAMETHER,
		IOIOSOCKETOPTIONS_SOCKET_WRONG_FILENAME,
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define IOIOSOCKETOPTIONS_SOCKET_NO_IOSOCKETOPTIONS_SOCKET_CREATED_STR    "No socket was created =(."
		#define IOIOSOCKETOPTIONS_SOCKET_CANNOT_BIND_STR          "You cannot <bindNListen> with this constructor; choose another one!"
		#define IOIOSOCKETOPTIONS_SOCKET_CANNOT_CONNECT_STR       "You cannot <connect> with this constructor; choose another one!"
		#define IOIOSOCKETOPTIONS_SOCKET_CANNOT_ACCEPT_STR        "You cannot <accept> with this constructor; choose another one!"
		#define IOIOSOCKETOPTIONS_SOCKET_ACCEPT_WO_BIND_STR       "You try to accept without called bindNListen or bindNListen failed."
		#define IOIOSOCKETOPTIONS_SOCKET_WRONG_PARAMETHER_STR     "Wrong paramather passed to function."
		#define IOIOSOCKETOPTIONS_SOCKET_WRONG_FILENAME_STR       "Probably wrong filename, type of file is wrong or `path` is empty!"

	/**
	 * ID of function where exception was thrown
	 */
	enum ioSocketFunctionsID
	{
		IOIOSOCKETOPTIONS_SOCKET_MAKESOCKET,
		IOIOSOCKETOPTIONS_SOCKET_CONNECT,
		IOIOSOCKETOPTIONS_SOCKET_MAKEUNIXSOCKET,
		IOIOSOCKETOPTIONS_SOCKET_BINDNLISTEN,
		IOIOSOCKETOPTIONS_SOCKET_ACCEPT,
		IOIOSOCKETOPTIONS_SOCKET_SEND,
		IOIOSOCKETOPTIONS_SOCKET_RECIEVE,
		IOIOSOCKETOPTIONS_SOCKET_CONNECTFROM
	};

};

#endif
