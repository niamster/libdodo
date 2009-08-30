/***************************************************************************
 *            graphicsDrawEx.h
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

#ifndef _GRAPHICSDRAWEX_H_
#define _GRAPHICSDRAWEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace graphics {
		/**
		 * libdodo defined errors
		 */
		enum drawExR {
			DRAWEX_EMPTYIMAGE,
			DRAWEX_CANNOTDRAWPRIMITIVE
		};

		/**
		 * explanations for libdodo defined errors
		 */
#define GRAPHICSDRAWEX_EMPTYIMAGE_STR "Image is empty"
#define GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR "Can not draw a primitive"

		/**
		 * IDs of functions where exception might be thrown
		 */
		enum drawFunctionsID {
			DRAWEX_PRIMITIVE,
			DRAWEX_POINT,
			DRAWEX_TEXT,
			DRAWEX_IMAGE,
		};
	};
};
#endif
