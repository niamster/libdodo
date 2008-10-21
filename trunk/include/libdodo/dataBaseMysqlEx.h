/***************************************************************************
 *            dataBaseMysqlEx.h
 *
 *  Thu Jul  7 00:25:19 2005
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

#ifndef _DATABASEMYSQLEX_H_
#define _DATABASEMYSQLEX_H_

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace data
	{
		namespace base
		{
			/**
			 * IDs of functions where exception might be thrown
			 */
			enum mysqlFunctionsID
			{
				MYSQLEX_CONNECT,
				MYSQLEX_EXEC,
				MYSQLEX_GETFIELDSTYPES,
			};
		};
	};
};

#endif
