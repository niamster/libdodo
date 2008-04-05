/***************************************************************************
 *            systemProcessSharedDataGuardEx.h
 *
 *  Sun Jul 22 23:14:14 2007
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

#ifndef _SYSTEMPROCESSSHAREDDATAGUARDEX_H_
#define _SYSTEMPROCESSSHAREDDATAGUARDEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum systemProcessSharedDataGuardExR
	{
		SYSTEMPROCESSSHAREDDATAGUARDEX_ISALREADYLOCKED,
		SYSTEMPROCESSSHAREDDATAGUARDEX_CANNOTLOCK
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define SYSTEMPROCESSSHAREDDATAGUARDEX_ISALREADYLOCKED_STR "The share is currently locked. Please wait."
#define SYSTEMPROCESSSHAREDDATAGUARDEX_CANNOTLOCK_STR "The share is currently locked. Timeout exhousted."

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum systemProcessSharedDataGuardFunctionsID
	{
		SYSTEMPROCESSSHAREDDATAGUARDEX_DEL,
		SYSTEMPROCESSSHAREDDATAGUARDEX_SET,
		SYSTEMPROCESSSHAREDDATAGUARDEX_LOCK,
		SYSTEMPROCESSSHAREDDATAGUARDEX_UNLOCK,
	};
};

#endif

