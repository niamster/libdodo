/***************************************************************************
 *            libraryLoaderEx.h
 *
 *  Thu Mar 13 20:50:55 2008
 *  Copyright  2008  Ni@m
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

#ifndef _IPCLIBRARYLOADEREX_H_
#define _IPCLIBRARYLOADEREX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace ipc
	{
		/**
		 * libdodo defined errors
		 */
		enum libraryLoaderExR
		{
			LIBRARYLOADEREX_LIBRARYNOTOPENED,
		};
		
		/**
		 * explanations for libdodo defined errors
		 */
	#define LIBRARYLOADEREX_LIBRARYNOTOPENED_STR "Library was not opened."
		/**
		 * IDs of functions where exception might be thrown
		 */
		enum libraryLoaderFunctionsID
		{
			LIBRARYLOADEREX_OPEN,
			LIBRARYLOADEREX_CLOSE,
			LIBRARYLOADEREX_GET,
			LIBRARYLOADEREX_BROPERATORSTRING,
			LIBRARYLOADEREX_GETSYMBOLS,
		};
	};
};

#endif
