/***************************************************************************
 *            dbPostgresqlEx.h
 *
 *  Fri Jan  13 19:25:19 2006
 *  Copyright  2006  Ni@m
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

#ifndef _DBPOSTGRESQLEX_H_
#define _DBPOSTGRESQLEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum dbPostgresqlExR
	{
		DBPOSTGRESQLEX_WRONGHINTUSAGE,
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define DBPOSTGRESQLEX_WRONGHINTUSAGE_STR    "Wrong hint usage. You used hint where it's not needed."

	/**
	 * ID of function where exception was thrown
	 */
	enum dbPostgresqlFunctionsID
	{
		DBPOSTGRESQLEX_CONNECT,
		DBPOSTGRESQLEX__EXEC,
		DBPOSTGRESQLEX_SETCHARSET,
	};
};

#endif