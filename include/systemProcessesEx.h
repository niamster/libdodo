/***************************************************************************
 *            systemProcessesEx.h
 *
 *  Tue Feb 27 08:42:14 2007
 *  Copyright  2007  Ni@m
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

#ifndef _SYSTEMPROCESSESEX_H_
#define _SYSTEMPROCESSESEX_H_

#include <directives.h>

#include <baseEx.h>

#ifndef NO_EX

	namespace dodo
	{
		/**
		 * libdodo defined errors
		 */
		enum systemProcessesExR
		{
			SYSTEMPROCESSES_,
		};
		
		/**
		 * libdodo defined errors' explanation
		 */
		#define SYSTEMTHREADS_ ""
			
		/**
		 * ID of function where exception was thrown
		 */			
		enum systemProcessesFunctionsID
		{
			SYSTEMPROCESSES_,
		};
	};

#endif
#endif 
