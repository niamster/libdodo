/***************************************************************************
 *            imageEx.h
 *
 *  Thu Nov 23 00:19:57 2007
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

#ifndef _IMAGEEX_H_
#define _IMAGEEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum imageFunctionsExR
	{
		IMAGEEX_BADINFO,
		IMAGEEX_EMPTYIMAGE
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define IMAGEEX_BADINFO_STR "Bad information for image was provided."
#define IMAGEEX_EMPTYIMAGE_STR "The image is empty. It was either closed or not opened."

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum imageFunctionsID
	{
		IMAGEEX_READ,
		IMAGEEX_WRITE,
		IMAGEEX_SETENCODER,
		IMAGEEX_SCALE,
		IMAGEEX_ROTATE,
	};

};

#endif
