/***************************************************************************
 *            mysqlppEx.h
 *
 *  Thu Jul  7 00:25:19 2005
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
 
#ifndef _MYSQLPPEX_H_
#define _MYSQLPPEX_H_

#include "directives.h"
#include "baseEx.h"

#ifndef NO_EX

	namespace dodo
	{
		
		enum mysqlppFunctionsID
		{
			MYSQLPP_CONNECT,
			MYSQLPP_EXEC,
		};		
	};

#endif

#endif /* _MYSQLINTERFACEEX_H */