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
#define _GRAPHICSTRANSFORM_H_

#include <libdodo/directives.h>

#ifdef IMAGEMAGICK_EXT

#include <magick/MagickCore.h>
#include <math.h>

#include <libdodo/types.h>
#include <libdodo/graphicsImage.h>
#include <libdodo/graphicsTransformEx.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace graphics
	{
		/**
		 * @enum transformRotateDirectionAngleEnum defines rotation options
		 */
		enum transformRotateDirectionAngleEnum
		{
			TRANSFORM_ROTATEDIRECTIONANGLE_90CW = -90,
			TRANSFORM_ROTATEDIRECTIONANGLE_90CCW = 90,
			TRANSFORM_ROTATEDIRECTIONANGLE_180 = 180,
			TRANSFORM_ROTATEDIRECTIONANGLE_270CW = -270,
			TRANSFORM_ROTATEDIRECTIONANGLE_270CCW = 270,
		};

		/**
		 * @class transform provides simple image transformations
		 */
		class transform
		{
			private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				transform(transform &im);

			public:

				/**
				 * contructor
				 */
				transform();

				/**
				 * contructor
				 * @param im defines image for transformations
				 */
				transform(image *im);

				/**
				 * destructor
				 */
				virtual ~transform();

				/**
				 * set image for transformations
				 * @param im defines image for transformations
				 */
				virtual void setImage(image *im);

				/**
				 * scale transform
				 * @param width defines width of the transform
				 * @param height defines height of the transform
				 */
				virtual void scale(unsigned long width, unsigned long height);

				/**
				 * scale transform
				 * @param size defines size of the transform
				 * @note scales transform by reducing size of the greater side saving proportions
				 */
				virtual void scale(unsigned long size);

				/**
				 * rotate transform
				 * @param angle defines rotation degree[see also transformRotateDirectionAngleEnum]
				 */
				virtual void rotate(double angle);

			protected:

				image *im;                ///< image to perform transformations
		};

	};
};

#endif

#endif
