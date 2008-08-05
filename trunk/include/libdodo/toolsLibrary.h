/***************************************************************************
 *            toolsLibrary.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _TOOLSLIBRARY_H_
#define _TOOLSLIBRARY_H_

#include <libdodo/directives.h>

#ifdef DL_EXT

#ifdef BFD_EXT

#include <bfd.h>

#endif

#include <libdodo/toolsLibraryEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>

namespace dodo
{
	namespace tools
	{
		/**
		 * @class library provides handling object from static and dynamic libraries
		 */
		class library
		{
			public:

				/**
				 * consructor
				 */
				library();

				/**
				 * consructor
				 * @param path defines path to the library[if not in ldconfig db] or library name
				 */
				library(const dodoString &path);

				/**
				 * destructor
				 */
				virtual ~library();

				/**
				 * open library
				 * @param path defines path to the library[if not in ldconfig db] or library name
				 */
				virtual void open(const dodoString &path);

				/**
				 * close library
				 * @note function exported from the library will be inaccessible
				 */
				virtual void close();

				/**
				 * get function from the library
				 * @param name defines function name
				 */
				virtual void *get(const dodoString &name);

				/**
				 * get function from the library
				 * @param name defines function name
				 */
				virtual void *operator[](const dodoString &name);

#ifdef BFD_EXT

				/**
				 * get symbols from the library
				 * @return symbols of the library
				 * @param path defines path to the library[if not in ldconfig db] or library name
				 */
				static dodoStringArray getSymbols(const dodoString &path);

#endif

			protected:

				void *handle;                                ///< handle to library
		};
	};
};

#endif

#endif
