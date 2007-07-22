/***************************************************************************
 *            systemThreadShareDataGuardEx.h
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

#ifndef _SYSTEMTHREADSHAREDATAGUARDEX_H_
#define _SYSTEMTHREADSHAREDATAGUARDEX_H_

#include <directives.h>

#include <baseEx.h>

#ifndef NO_EX

	namespace dodo
	{
		/**
		 * libdodo defined errors
		 */
		enum systemThreadShareDataGuardExR
		{
			SYSTEMTHREADSHAREDATAGUARD_ISALREADYLOCKED,
			SYSTEMTHREADSHAREDATAGUARD_CANNOTLOCK
		};
		
		/**
		 * libdodo defined errors' explanation
		 */
		#define SYSTEMTHREADSHAREDATAGUARD_ISALREADYLOCKED_STR "The share is currently locked. Please wait."
		#define SYSTEMTHREADSHAREDATAGUARD_CANNOTLOCK_STR "The share is currently locked. Timeout exhousted."
			
		/**
		 * ID of function where exception was thrown
		 */			
		enum systemThreadShareDataGuardFunctionsID
		{
			SYSTEMTHREADSHAREDATAGUARD_DEL,
			SYSTEMTHREADSHAREDATAGUARD_SET,
			SYSTEMTHREADSHAREDATAGUARD_LOCK,
			SYSTEMTHREADSHAREDATAGUARD_UNLOCK,
		};
	};

#endif
#endif 

