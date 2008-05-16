/***************************************************************************
 *            ioNetworkClientEx.h
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

#ifndef _IONETWORKCLIENTEX_H_
#define _IONETWORKCLIENTEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace io
	{
		namespace network
		{
			/**
			 * libdodo defined errors
			 */
			enum clientExR
			{
				CLIENTEX_WRONGPARAMETER,
			};

			/**
			 * explanations for libdodo defined errors
			 */
		#define IONETWORKCLIENTEX_WRONGPARAMETER_STR "Wrong paramather passed to function."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum clientFunctionsID
			{
				CLIENTEX_MAKESOCKET,
				CLIENTEX_CONNECT,
				CLIENTEX_CONNECTFROM,
			};
		};
	};
};

#endif
