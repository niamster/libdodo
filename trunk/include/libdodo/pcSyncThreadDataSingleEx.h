/***************************************************************************
 *            pcSyncThreadDataSingleEx.h
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

#ifndef _PCSYNCTHREADDATASINGLEEX_H_
#define _PCSYNCTHREADDATASINGLEEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace pc
	{
		namespace sync
		{
			namespace thread
			{
				namespace data
				{
					/**
					 * libdodo defined errors
					 */
					enum singleExR
					{
						SINGLEEX_ISALREADYLOCKED,
						SINGLEEX_CANNOTLOCK
					};

					/**
					 * explanations for libdodo defined errors
					 */
#define PCSYNCTHREADDATASINGLEEX_ISALREADYLOCKED_STR "The share is currently locked. Please wait."
#define PCSYNCTHREADDATASINGLEEX_CANNOTLOCK_STR      "The share is currently locked. Timeout exhousted."

					/**
					 * IDs of functions where exception might be thrown
					 */
					enum singleFunctionsID
					{
						SINGLEEX_DEL,
						SINGLEEX_SET,
						SINGLEEX_LOCK,
						SINGLEEX_UNLOCK,
						SINGLEEX_SIGNLE,
					};
				};
			};
		};
	};
};

#endif
