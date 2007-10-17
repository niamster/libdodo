/***************************************************************************
 *            systemTools.h
 *
 *  Sat Nov 19 15:19:57 2005
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _SYSTEMEX_H_
#define _SYSTEMEX_H_

#include "directives.h"
#include "baseEx.h"

#ifndef NO_EX

	namespace dodo
	{	
		
		/**
		 * libdodo defined errors
		 */
		enum systemExR
		{
			SYSTEMTOOLS_MEMORY_OVER = 1,
			SYSTEMTOOLS_WRONG_PARAMETHER,
		};
		
		/**
		 * libdodo defined errors' explanation
		 */				
		#define SYSTEMTOOLS_WRONG_PARAMETHER_STR "Wrong paramather passed to function."
		
		/**
		 * ID of function where exception was thrown
		 */					
		enum systemFunctionsID
		{
			SYSTEMTOOLS_SETWORKINGDIR,
			SYSTEMTOOLS_GETWORKINGDIR,
			SYSTEMTOOLS_GETUSAGEINFO,
			SYSTEMTOOLS_GETLIMIT,
			SYSTEMTOOLS_SETLIMIT,
			SYSTEMTOOLS_GETPRIORITY,
			SYSTEMTOOLS_SETPRIORITY,
			SYSTEMTOOLS_GETUID,
			SYSTEMTOOLS_SETUID,
			SYSTEMTOOLS_GETGID,
			SYSTEMTOOLS_SETGID,
			SYSTEMTOOLS_GETUSERINFO,
			SYSTEMTOOLS_GETUSERS,
			SYSTEMTOOLS_GETGROUPINFO,
			SYSTEMTOOLS_GETGROUPS,
			SYSTEMTOOLS_CHANGEROOT
		};
	};

#endif	
	
#endif
