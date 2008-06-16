/***************************************************************************
 *            ipcThreadSharedDataGuardEx.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _IPCTHREADSHAREDDATAGUARDEX_H_
#define _IPCTHREADSHAREDDATAGUARDEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace ipc
	{
		namespace thread
		{
			namespace shared
			{
				/**
				 * libdodo defined errors
				 */
				enum dataGuardExR
				{
					DATAGUARDEX_ISALREADYLOCKED,
					DATAGUARDEX_CANNOTLOCK
				};

				/**
				 * explanations for libdodo defined errors
				 */
#define IPCTHREADSHAREDDATAGUARDEX_ISALREADYLOCKED_STR "The share is currently locked. Please wait."
#define IPCTHREADSHAREDDATAGUARDEX_CANNOTLOCK_STR      "The share is currently locked. Timeout exhousted."

				/**
				 * IDs of functions where exception might be thrown
				 */
				enum dataGuardFunctionsID
				{
					DATAGUARDEX_DEL,
					DATAGUARDEX_SET,
					DATAGUARDEX_LOCK,
					DATAGUARDEX_UNLOCK,
				};
			};
		};
	}
};

#endif
