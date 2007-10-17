/***************************************************************************
 *            flushSocketOptionsEx.h
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

#ifndef _FLUSHSOCKETOPTIONSEX_H_
#define _FLUSHSOCKETOPTIONSEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * next for flushSocketOptions
	 */

	/**
	 * libdodo defined errors
	 */
	enum flushSocketOptionsExR
	{
		FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED,
		FLUSHSOCKETOPTIONS_WRONG_PARAMETHER,
	};

	/**
	 * libdodo defined errors' explanation
	 */
        #define FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED_STR    "No socket was created =(."
        #define FLUSHSOCKETOPTIONS_WRONG_PARAMETHER_STR     "Wrong paramather passed to function."

	/**
	 * ID of function where exception was thrown
	 */
	enum flushSocketOptionsFunctionsID
	{
		FLUSHSOCKETOPTIONS_SETINBUFFERSIZE,
		FLUSHSOCKETOPTIONS_SETOUTBUFFERSIZE,
		FLUSHSOCKETOPTIONS_SETINTIMEOUT,
		FLUSHSOCKETOPTIONS_SETOUTTIMEOUT,
		FLUSHSOCKETOPTIONS_SETSOCKOPT,
		FLUSHSOCKETOPTIONS_SETLINGERSOCKOPT,
		FLUSHSOCKETOPTIONS_BLOCK,
		FLUSHSOCKETOPTIONS__CLOSE,
	};

};

#endif
