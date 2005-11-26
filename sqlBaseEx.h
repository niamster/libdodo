/***************************************************************************
 *            sqlBaseEx.h
 *
 *  Mon Jul 18 20:25:14 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _SQLBASEEX_H_
#define _SQLBASEEX_H_

#include "directives.h"
#include "baseEx.h"
#include <vector>
#include <string>

#ifndef NO_EX

	namespace dodo
	{
		
		/**
		 * libdodo defined errors
		 */
		enum sqlBaseExR
		{
			SQLBASE_MEMORY_OVER = 1,///not enough memmory
			SQLBASE_EMPTY_REQUEST
		};
		
		/**
		 * libdodo defined errors' explanation
		 */			
		#define SQLBASE_MEMORY_OVER_STR "Not anough memmory"
		#define SQLBASE_EMPTY_REQUEST_STR "Query is empty. are you sure?"
		
		/**
		 * ID of function where exception was thrown
		 */			
		enum flushSocketFunctionsID
		{
			SQLBASE_SELECTCOLLECT,
			SQLBASE_INSERTCOLLECT,
			SQLBASE_INSERTSELECTCOLLECT,
			SQLBASE_UPDATECOLLECT,
			SQLBASE_DELCOLLECT,
			SQLBASE_QUERYCOLLECT
			
		};
		
	};

#endif	

#endif /* _SQLBASEEX_H */
