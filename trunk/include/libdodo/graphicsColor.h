/***************************************************************************
 *            graphicsColor.h
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

#ifndef _GRAPHICSCOLOR_H_
#define _GRAPHICSCOLOR_H_

#include <libdodo/directives.h>

namespace dodo
{
	namespace graphics
	{
		/**
		 * @struct __color
		 * @brief defines color of the primitive
		 */
		struct __color
		{
				unsigned short red;
				unsigned short green;
				unsigned short blue;
				unsigned short opacity;
		};

		/**
		 * @class color
		 * @brief defines default colors
		 */
		class color
		{
			public:

				static __color red;
				static __color green;
				static __color blue;
				static __color white;
				static __color black;
				static __color transparent;
		};
	};
};

#endif
