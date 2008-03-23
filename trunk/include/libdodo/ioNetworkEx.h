/***************************************************************************
 *            ioNetworkEx.h
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

#ifndef _IONETWORKEX_H_
#define _IONETWORKEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum ioNetworkExR
	{
		IONETWORKEX_NOSOCKETCREATED,
		IONETWORKEX_CANNOTBIND,
		IONETWORKEX_CANNOTCONNECT,
		IONETWORKEX_CANNOTACCEPT,
		IONETWORKEX_ACCEPTWOBIND,
		IONETWORKEX_WRONGPARAMETER,
		IONETWORKEX_WRONGFILENAME,
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define IONETWORKEX_NOSOCKETCREATED_STR "No socket was created =(."
#define IONETWORKEX_CANNOTBIND_STR "You cannot <bindNListen> with this constructor; choose another one!"
#define IONETWORKEX_CANNOTCONNECT_STR "You cannot <connect> with this constructor; choose another one!"
#define IONETWORKEX_CANNOTACCEPT_STR "You cannot <accept> with this constructor; choose another one!"
#define IONETWORKEX_ACCEPTWOBIND_STR "You try to accept without called bindNListen or bindNListen failed."
#define IONETWORKEX_WRONGPARAMETER_STR "Wrong paramather passed to function."
#define IONETWORKEX_WRONGFILENAME_STR "Probably wrong filename, type of file is wrong or `path` is empty!"

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum ioNetworkFunctionsID
	{
		IONETWORKEX_MAKESOCKET,
		IONETWORKEX_CONNECT,
		IONETWORKEX_MAKEUNIXSOCKET,
		IONETWORKEX_BINDNLISTEN,
		IONETWORKEX_ACCEPT,
		IONETWORKEX_SEND,
		IONETWORKEX_RECIEVE,
		IONETWORKEX_CONNECTFROM
	};

};

#endif
