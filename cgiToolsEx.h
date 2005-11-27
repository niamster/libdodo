/***************************************************************************
 *            cgiToolsEx.h
 *
 *  Wed Oct 5 16:25:14 2005
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

#ifndef _CGITOOLSEX_H_
#define _CGITOOLSEX_H_

#include "directives.h"
#include "baseEx.h"

#ifndef NO_EX

	namespace dodo
	{
		
		///reasons of exception
		enum cgiToolsExR
		{
			CGITOOLS_MEMORY_OVER = 1
		};
		
		#define CGITOOLS_MEMORY_OVER_STR "Not anough memmory"
		
		enum cgiToolsFunctionsID
		{
			CGITOOLS_MAKEPOST,
		};
	};

#endif
#endif /*CGITOOLSEX_H_*/
