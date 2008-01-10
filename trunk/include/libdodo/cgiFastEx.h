/***************************************************************************
 *            cgiFastEx.h
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

#ifndef _CGIFASTEX_H_
#define _CGIFASTEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * next for cgiFast
	 */

	/**
	 * libdodo defined errors
	 */
	enum cgiFastFunctionsExR
	{
		CGIFASTIOEX_FAILEDTOFLUSH,
		CGIFASTIOEX_FAILEDTOPRINTSTRING,
		CGIFASTEX_ACCEPTFAILED,
		CGIFASTEX_ISCGI,
	};

	/**
	 * libdodo defined errors' explanation
	 */
#define CGIFASTIOEX_FAILEDTOFLUSH_STR "Failed to flush."
#define CGIFASTIOEX_FAILEDTOPRINTSTRING_STR "Failed to print string."
#define CGIFASTEX_ACCEPTFAILED_STR "Accept call failed."
#define CGIFASTEX_ISCGI_STR "Called as a CGI."


	/**
	 * ID of function where exception was thrown
	 */
	enum cgiFastFunctionsID
	{
		CGIFASTIOEX_FLUSH,
		CGIFASTIOEX_PRINT,
		CGIFASTEX_LISTEN,
		CGIFASTEX_STACKTHREAD,
	};

};

#endif
