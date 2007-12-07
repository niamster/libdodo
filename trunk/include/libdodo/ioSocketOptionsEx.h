/***************************************************************************
 *            ioSocketOptionsEx.h
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

#ifndef _IOSOCKETOPTIONSEX_H_
#define _IOSOCKETOPTIONSEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * next for ioSocketOptions
	 */

	/**
	 * libdodo defined errors
	 */
	enum ioSocketOptionsExR
	{
		IOSOCKETOPTIONS_NO_IOSOCKETOPTIONS_SOCKET_CREATED,
		IOSOCKETOPTIONS_WRONG_PARAMETHER,
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define IOSOCKETOPTIONS_NO_IOSOCKETOPTIONS_SOCKET_CREATED_STR    "No socket was created =(."
		#define IOSOCKETOPTIONS_WRONG_PARAMETHER_STR     "Wrong paramather passed to function."

	/**
	 * ID of function where exception was thrown
	 */
	enum ioSocketOptionsFunctionsID
	{
		IOSOCKETOPTIONS_SETINBUFFERSIZE,
		IOSOCKETOPTIONS_SETOUTBUFFERSIZE,
		IOSOCKETOPTIONS_SETINTIMEOUT,
		IOSOCKETOPTIONS_SETOUTTIMEOUT,
		IOSOCKETOPTIONS_SETSOCKOPT,
		IOSOCKETOPTIONS_SETLINGERSOCKOPT,
		IOSOCKETOPTIONS_BLOCK,
		IOSOCKETOPTIONS__CLOSE,
	};

};

#endif
