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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _IOFILETEMPEX_H_
#define _IOFILETEMPEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace io {
		namespace file {
			/**
			 * libdodo defined errors
			 */
			enum tempExR {
				TEMPEX_CANNOTOVEWRITE,
				TEMPEX_FILEISSHORTERTHANGIVENPOSITION,
				TEMPEX_NOTOPENED
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define IOFILETEMPEX_CANNOTOVEWRITE_STR                 "You set option not to overwrite but tried to do that."
#define IOFILETEMPEX_FILEISSHORTERTHANGIVENPOSITION_STR "Given file couldn't reach given position. File is short."
#define IOFILETEMPEX_NOTOPENED_STR "File was not opened."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum tempFunctionsID {
				TEMPEX_TEMP,
				TEMPEX_CLOSE,
				TEMPEX_CLONE,
				TEMPEX_OPEN,
				TEMPEX__READ,
				TEMPEX__WRITE,
				TEMPEX__READSTREAM,
				TEMPEX__WRITESTREAM,
				TEMPEX_FLUSH,
				TEMPEX_GETINDESCRIPTOR,
				TEMPEX_GETOUTDESCRIPTOR,
			};
		};
	};
};

#endif
