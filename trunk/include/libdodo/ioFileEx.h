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

	/**
	 * libdodo defined errors
	 */
	enum ioFileExR
	{
		IOFILEEX_WRONGFILENAME,
		IOFILEEX_CANNOTOVEWRITE,
		IOFILEEX_FILEISSHORTERTHANGIVENPOSITION,
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define IOFILEEX_WRONGFILENAME_STR "Probably wrong filename, type of file is wrong or `path` is empty!"
#define IOFILEEX_CANNOTOVEWRITE_STR "You set option not to overwrite, but try to do that =)"
#define IOFILEEX_FILEISSHORTERTHANGIVENPOSITION_STR "Given file couldn't reach given position. File is short."

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum ioFileFunctionsID
	{
		IOFILEEX_CLOSE,
		IOFILEEX_OPEN,
		IOFILEEX__READ,
		IOFILEEX__WRITE,
		IOFILEEX__READSTREAM,
		IOFILEEX__WRITESTREAM,
		IOFILEEX_FLUSH,
	};
};

#endif
