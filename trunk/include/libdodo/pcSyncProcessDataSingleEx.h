/***************************************************************************
 *            pcSyncProcessDataSingleEx.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _PCSYNCPROCESSDATASINGLEEX_H_
#define _PCSYNCPROCESSDATASINGLEEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace pc
	{
		namespace sync
		{
			namespace process
			{
				namespace data
				{
					/**
					 * libdodo defined errors
					 */
					enum singleExR
					{
						SINGLEEX_CANNOTLOCK,
					};

					/**
					 * explanations for libdodo defined errors
					 */
#define PCSYNCPROCESSDATASINGLEEX_CANNOTLOCK_STR      "The share is currently locked. Timeout exhousted."

					/**
					 * IDs of functions where exception might be thrown
					 */
					enum singleFunctionsID
					{
						SINGLEEX_SET,
						SINGLEEX_ACQUIRE,
						SINGLEEX_RELEASE,
						SINGLEEX_REMOVE,
						SINGLEEX_SIGNLE,
					};
				};
			};
		};
	};
};

#endif

