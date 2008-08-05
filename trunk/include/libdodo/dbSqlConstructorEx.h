/***************************************************************************
 *            dbSqlConstructorEx.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DBSQLCONSTRUCTOREX_H_
#define _DBSQLCONSTRUCTOREX_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace db
	{
		namespace sql
		{
			/**
			 * libdodo defined errors
			 */
			enum sqlConstructorExR
			{
				SQLCONSTRUCTOREX_EMPTYREQUEST,
				SQLCONSTRUCTOREX_UNKNOWNJOINTYPE,
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define DBSQLCONSTRUCTOREX_EMPTYREQUEST_STR    "Query is empty. Are you sure?"
#define DBSQLCONSTRUCTOREX_UNKNOWNJOINTYPE_STR "Unknown join type."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum sqlConstructorFunctionsID
			{
				SQLCONSTRUCTOREX_QUERYCOLLECT,
				SQLCONSTRUCTOREX_JOINCOLLECT,
			};
		};
	};
};

#endif
