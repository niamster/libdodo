/***************************************************************************
 *            cgiFastServerEx.h
 *
 *  Mon Dec 31 01:51:55 2007
 *  Copyright  2007  Ni@m
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

#ifndef _CGIFASTSERVEREX_H_
#define _CGIFASTSERVEREX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace cgi
	{
		namespace fast
		{
			/**
			 * libdodo defined errors
			 */
			enum serverExR
			{
				SERVEREX_ACCEPTFAILED,
				SERVEREX_ISCGI,
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define CGIFASTSERVEREX_ACCEPTFAILED_STR "Accept call failed."
#define CGIFASTSERVEREX_ISCGI_STR        "Called as a CGI."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum serverFunctionsID
			{
				SERVEREX_LISTEN,
				SERVEREX_STACKTHREAD,
			};
		}
	};
};

#endif
