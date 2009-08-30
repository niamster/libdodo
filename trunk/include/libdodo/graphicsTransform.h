/***************************************************************************
 *            graphicsTransform.h
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

#ifndef _GRAPHICSTRANSFORM_H_
#define _GRAPHICSTRANSFORM_H_ 1

#include <libdodo/directives.h>

#ifdef IMAGEMAGICK_EXT
namespace dodo {
	namespace graphics {
		class image;

		/**
		 * @class transform
		 * @brief provides simple image transformations
		 */
		class transform {
		  public:

			/**
			 * @enum rotateDirectionAngleEnum defines rotation options
			 */
			enum rotateDirectionAngleEnum {
				ROTATE_DIRECTION_ANGLE_90CW = -90,
				ROTATE_DIRECTION_ANGLE_90CCW = 90,
				ROTATE_DIRECTION_ANGLE_180 = 180,
				ROTATE_DIRECTION_ANGLE_270CW = -270,
				ROTATE_DIRECTION_ANGLE_270CCW = 270,
			};

			/**
			 * scale transform
			 * @param image defines image on which perform transformation
			 * @param width defines width of the transform
			 * @param height defines height of the transform
			 */
			static void scale(image &image,
							  unsigned long width,
							   unsigned long height);

			/**
			 * scale transform
			 * @param image defines image on which perform transformation
			 * @param size defines size of the transform
			 * @note scales transform by reducing size of the greater side saving proportions
			 */
			static void scale(image &image,
							  unsigned long size);

			/**
			 * rotate transform
			 * @param image defines image on which perform transformation
			 * @param angle defines rotation degree[see also transformRotateDirectionAngleEnum]
			 */
			static void rotate(image &image,
							  double angle);
		};
	};
};
#endif
#endif
