/***************************************************************************
 *            ioNetworkHttpEx.h
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

#ifndef _IONETWORKHTTPEX_H_
#define _IONETWORKHTTPEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum ioNetworkHttpExR
	{
		IONETWORKHTTPEX_CANNOTCONNECT,
		IONETWORKHTTPEX_NOTAUTHORIZED,
		IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE,
		IONETWORKHTTPEX_UNKNOWNPROXYAUTHTYPE,
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define IONETWORKHTTPEX_CANNOTCONNECT_STR "Can't connect to the host(s)."
#define IONETWORKHTTPEX_NOTAUTHORIZED_STR "Aythorization required."
#define IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE_STR "Unknown HTTP authentification type."
#define IONETWORKHTTPEX_UNKNOWNPROXYAUTHTYPE_STR "Unknown proxy authentification type."

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum ioNetworkHttpFunctionsID
	{
		IONETWORKHTTPEX_POST,
		IONETWORKHTTPEX_GET,
		IONETWORKHTTPEX_GETCONTENT,
	};
};

#endif
