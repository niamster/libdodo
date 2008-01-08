/***************************************************************************
 *            systemThreadSharedDataGuardEx.h
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

#ifndef _SYSTEMTHREADSHAREDDATAGUARDEX_H_
#define _SYSTEMTHREADSHAREDDATAGUARDEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum systemThreadSharedDataGuardExR
	{
		SYSTEMTHREADSHAREDDATAGUARDEX_ISALREADYLOCKED,
		SYSTEMTHREADSHAREDDATAGUARDEX_CANNOTLOCK
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define SYSTEMTHREADSHAREDDATAGUARDEX_ISALREADYLOCKED_STR    "The share is currently locked. Please wait."
		#define SYSTEMTHREADSHAREDDATAGUARDEX_CANNOTLOCK_STR         "The share is currently locked. Timeout exhousted."

	/**
	 * ID of function where exception was thrown
	 */
	enum systemThreadSharedDataGuardFunctionsID
	{
		SYSTEMTHREADSHAREDDATAGUARDEX_DEL,
		SYSTEMTHREADSHAREDDATAGUARDEX_SET,
		SYSTEMTHREADSHAREDDATAGUARDEX_LOCK,
		SYSTEMTHREADSHAREDDATAGUARDEX_UNLOCK,
	};
};

#endif
