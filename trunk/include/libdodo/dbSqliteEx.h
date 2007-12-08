/***************************************************************************
 *            dbSqliteEx.h
 *
 *  Sat Dec 10 06:45:19 2005
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

#ifndef _DBSQLITEEX_H_
#define _DBSQLITEEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum dbSqliteExR
	{
		DBSQLITEEX_WRONG_HINT_USAGE,
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define DBSQLITEEX_WRONG_HINT_USAGE_STR    "Wrong hint usage. You used hint where it's not needed."

	/**
	 * ID of function where exception was thrown
	 */
	enum dbSqliteFunctionsID
	{
		DBSQLITEEX_DISCONNECT,
		DBSQLITEEX_CONNECT,
		DBSQLITEEX__EXEC,
		DBSQLITEEX_FETCHROW,
		DBSQLITEEX_FETCHASSOC
	};

};

#endif
