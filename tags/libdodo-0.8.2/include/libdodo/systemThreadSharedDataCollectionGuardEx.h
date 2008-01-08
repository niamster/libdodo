/***************************************************************************
 *            systemThreadSharedDataCollectionGuardEx.h
 *
 *  Sat Oct 20 02:00:55 2007
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

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum systemThreadSharedDataCollectionGuardExR
	{
		SYSTEMTHREADSHAREDDATACOLLECTIONGUARDEX_NOTFOUND,
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define SYSTEMTHREADSHAREDDATACOLLECTIONGUARDEX_NOTFOUND_STR    "Share not found."

	/**
	 * ID of function where exception was thrown
	 */
	enum systemThreadSharedDataCollectionGuardFunctionsID
	{
		SYSTEMTHREADSHAREDDATACOLLECTIONGUARDEX_DEL,
		SYSTEMTHREADSHAREDDATACOLLECTIONGUARDEX_SET,
		SYSTEMTHREADSHAREDDATACOLLECTIONGUARDEX_GET,
	};
};

#endif

