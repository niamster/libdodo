/***************************************************************************
 *            ioFileGeneralEx.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _IOFILEGENERALEX_H_
#define _IOFILEGENERALEX_H_

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace io
	{
		namespace file
		{
			/**
			 * libdodo defined errors
			 */
			enum generalExR
			{
				GENERALEX_WRONGFILENAME,
				GENERALEX_CANNOTOVEWRITE,
				GENERALEX_FILEISSHORTERTHANGIVENPOSITION,
				GENERALEX_NOTOPENED
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define IOFILEGENERALEX_WRONGFILENAME_STR                  "Probably wrong filename, type of file is wrong or `path` is empty."
#define IOFILEGENERALEX_CANNOTOVEWRITE_STR                 "You set option not to overwrite but tried to do that."
#define IOFILEGENERALEX_FILEISSHORTERTHANGIVENPOSITION_STR "Given file couldn't reach given position. File is short."
#define IOFILEGENERALEX_NOTOPENED_STR 						"File was not opened."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum generalFunctionsID
			{
				GENERALEX_CLOSE,
				GENERALEX_CLONE,
				GENERALEX_OPEN,
				GENERALEX__READ,
				GENERALEX__WRITE,
				GENERALEX__READSTREAM,
				GENERALEX__WRITESTREAM,
				GENERALEX_FLUSH,
				GENERALEX_GETINDESCRIPTOR,
				GENERALEX_GETOUTDESCRIPTOR,
			};
		};
	};
};

#endif
