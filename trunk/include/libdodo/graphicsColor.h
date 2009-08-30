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
#define _GRAPHICSCOLOR_H_ 1

#include <libdodo/directives.h>

#ifdef IMAGEMAGICK_EXT
namespace dodo {
	namespace graphics {
		/**
		 * @struct __color__
		 * @brief defines color of the primitive
		 */
		struct __color__ {
			unsigned short red;     ///< in range [0; 65535]
			unsigned short green;   ///< in range [0; 65535]
			unsigned short blue;    ///< in range [0; 65535]
			unsigned short opacity; ///< 0 is fully opaque and 65535 is fully transparent
		};

		/**
		 * @class color
		 * @brief defines default colors
		 */
		class color {
		  public:

			static __color__ red;
			static __color__ green;
			static __color__ blue;
			static __color__ white;
			static __color__ black;
			static __color__ transparent;
		};
	};
};
#endif
#endif
