/***************************************************************************
 *            pcSyncProcessDataCollectionEx.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _PCSYNCPROCESSDATACOLLECTIONEX_H_
#define _PCSYNCPROCESSDATACOLLECTIONEX_H_

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
					enum collectionExR
					{
						COLLECTIONEX_NOTFOUND,
					};

					/**
					 * explanations for libdodo defined errors
					 */
#define PCSYNCPROCESSDATACOLLECTIONEX_NOTFOUND_STR "Share not found."

					/**
					 * IDs of functions where exception might be thrown
					 */
					enum collectionFunctionsID
					{
						COLLECTIONEX_DEL,
						COLLECTIONEX_SET,
						COLLECTIONEX_GET,
					};
				};
			};
		};
	};
};

#endif
