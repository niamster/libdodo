/***************************************************************************
 *            graphicsImageEx.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _GRAPHICSIMAGEEX_H_
#define _GRAPHICSIMAGEEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace graphics
	{
		/**
		 * libdodo defined errors
		 */
		enum imageExR
		{
			IMAGEEX_IMERROR,
			IMAGEEX_BADINFO,
			IMAGEEX_EMPTYIMAGE,
			IMAGEEX_LONGPATH,
			IMAGEEX_CANNOTSETTYPE,
		};

		/**
		 * explanations for libdodo defined errors
		 */
#define GRAPHICSIMAGEEX_BADINFO_STR    "Bad information for image was provided."
#define GRAPHICSIMAGEEX_EMPTYIMAGE_STR "The image is empty. It was either closed or not opened."
#define GRAPHICSIMAGEEX_LONGPATH_STR "Path to image is too long."
#define GRAPHICSIMAGEEX_CANNOTSETTYPE_STR "Can not set image type."

		/**
		 * IDs of functions where exception might be thrown
		 */
		enum imageFunctionsID
		{
			IMAGEEX_IMERRORHANDLER,
			IMAGEEX_READ,
			IMAGEEX_CREATE,
			IMAGEEX_WRITE,
			IMAGEEX_SETENCODER,
			IMAGEEX_SETTYPE,
			IMAGEEX_SETALPHA,
			IMAGEEX_REMOVEALPHA,
			IMAGEEX_SETOPACITY,
			IMAGEEX_SETBACKGROUNDCOLOR,
		};
	};
};

#endif
