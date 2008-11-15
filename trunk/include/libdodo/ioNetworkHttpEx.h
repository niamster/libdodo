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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _IONETWORKHTTPEX_H_
#define _IONETWORKHTTPEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace io
	{
		namespace network
		{
			/**
			 * libdodo defined errors
			 */
			enum httpExR
			{
				HTTPEX_CANNOTCONNECT,
				HTTPEX_NOTAUTHORIZED,
				HTTPEX_UNKNOWNWWWAUTHTYPE,
				HTTPEX_UNKNOWNPROXYAUTHTYPE,
				HTTPEX_UNSUPPORTEDSURICHEME,
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define IONETWORKHTTPEX_CANNOTCONNECT_STR        "Can't connect to the host(s)."
#define IONETWORKHTTPEX_NOTAUTHORIZED_STR        "Aythorization required."
#define IONETWORKHTTPEX_UNKNOWNWWWAUTHTYPE_STR   "Unknown HTTP authentication type."
#define IONETWORKHTTPEX_UNKNOWNPROXYAUTHTYPE_STR "Unknown proxy authentication type."
#define IONETWORKHTTPEX_UNSUPPORTEDSURICHEME_STR "Unsupported URI scheme."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum httpFunctionsID
			{
				HTTPEX_POST,
				HTTPEX_GET,
				HTTPEX_GETCONTENT,
				HTTPEX_SETURL,
				HTTPEX_GETPROXYCONNECTRESPONSE,
			};
		};
	};
};

#endif
