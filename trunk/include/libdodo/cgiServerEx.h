/***************************************************************************
 *            cgiServerEx.h
 *
 *  Wed Mar 12 01:01:14 2008
 *  Copyright  2008  Ni@m
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

#ifndef _CGISERVEREX_H_
#define _CGISERVEREX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace cgi
	{
		/**
		 * libdodo defined errors
		 */
		enum serverExR
		{
			SERVEREX_WRONGSTATUSCODE,
		};

		/**
		 * explanations for libdodo defined errors
		 */
#define CGISERVEREX_WRONGSTATUSCODE_STR "Wrong status code."

		/**
		 * IDs of functions where exception might be thrown
		 */
		enum serverFunctionsID
		{
			SERVEREX_MAKEPOST,
			SERVEREX_SETRESPONSESTATUS,
		};
	};
};

#endif
