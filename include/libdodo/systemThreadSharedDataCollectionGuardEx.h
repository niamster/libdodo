/***************************************************************************
 *            systemThreadSharedDataCollectionGuardEx.h
 *
 *  Wed Oct 5 16:25:14 2005
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

#ifndef _SYSTEMTHREADSHAREDDATACOLLECTIONGUARDEX_H_
#define _SYSTEMTHREADSHAREDDATACOLLECTIONGUARDEX_H_

#include <directives.h>

#include <baseEx.h>


#ifndef NO_EX

	namespace dodo
	{
		/**
		 * libdodo defined errors
		 */
		enum systemThreadSharedDataCollectionGuardExR
		{
			SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND,
		};
		
		/**
		 * libdodo defined errors' explanation
		 */
		#define SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND_STR "Share not found."
			
		/**
		 * ID of function where exception was thrown
		 */			
		enum systemThreadSharedDataCollectionGuardFunctionsID
		{
			SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_DEL,
			SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_SET,
			SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_GET,
		};
	};

#endif
#endif 

