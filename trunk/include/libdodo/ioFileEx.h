/***************************************************************************
 *            ioFileEx.h
 *
 *  Mon Feb 21 03:03:47 2005
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

#ifndef _IOFILEEX_H_
#define _IOFILEEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace io
	{
		/**
		 * libdodo defined errors
		 */
		enum fileExR
		{
			FILEEX_WRONGFILENAME,
			FILEEX_CANNOTOVEWRITE,
			FILEEX_FILEISSHORTERTHANGIVENPOSITION,
		};

		/**
		 * explanations for libdodo defined errors
		 */
	#define FILEEX_WRONGFILENAME_STR "Probably wrong filename, type of file is wrong or `path` is empty!"
	#define FILEEX_CANNOTOVEWRITE_STR "You set option not to overwrite, but try to do that =)"
	#define FILEEX_FILEISSHORTERTHANGIVENPOSITION_STR "Given file couldn't reach given position. File is short."

		/**
		 * IDs of functions where exception might be thrown
		 */
		enum fileFunctionsID
		{
			FILEEX_CLOSE,
			FILEEX_OPEN,
			FILEEX__READ,
			FILEEX__WRITE,
			FILEEX__READSTREAM,
			FILEEX__WRITESTREAM,
			FILEEX_FLUSH,
		};
	};
};

#endif