/***************************************************************************
 *            dbSqlBaseEx.h
 *
 *  Mon Jul 18 20:25:14 2005
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

#ifndef _DBSQLBASEEX_H_
#define _DBSQLBASEEX_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/baseEx.h>

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum dbSqlBaseExR
	{
		DBSQLBASEEX_EMPTYREQUEST
	};

	/**
	 * libdodo defined errors' explanation
	 */
#define DBSQLBASEEX_EMPTYREQUEST_STR "Query is empty. are you sure?"

	/**
	 * ID of function where exception was thrown
	 */
	enum dbSqlBaseFunctionsID
	{
		DBSQLBASEEX_QUERYCOLLECT

	};

};

#endif
