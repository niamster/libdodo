/***************************************************************************
 *            graphicsDraw.h
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

#ifndef _GRAPHICSDRAW_H_
#define _GRAPHICSDRAW_H_

#include <libdodo/directives.h>

#ifdef IMAGEMAGICK_EXT

#include <magick/MagickCore.h>
#include <math.h>

#include <libdodo/types.h>
#include <libdodo/graphicsImage.h>
#include <libdodo/graphicsColor.h>
#include <libdodo/graphicsDrawEx.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace graphics
	{
		/**
		 * @class draw
		 * @brief provides simple image drawing
		 */
		class draw
		{
			private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				draw(draw &im);

			public:

				/**
				 * contructor
				 */
				draw();

				/**
				 * contructor
				 * @param im defines image for drawations
				 */
				draw(image *im);

				/**
				 * destructor
				 */
				virtual ~draw();

				/**
				 * set image for drawations
				 * @param im defines image for drawations
				 */
				virtual void setImage(image *im);

				/**
				 * draw circle
				 * @param x	defines x position of circle center
				 * @param y	defines y position of circle center
				 * @param radius defines circle radius
				 * @param fillColor defines circle fill color
				 * @param borderColor defines circle border color
				 * @param lineWidth defines the width of the line of the circle
				 */
				virtual void circle(unsigned long x, unsigned long y, unsigned long radius, const __color &fillColor, const __color &borderColor, double lineWidth = 1);

			protected:

				image *im;                ///< image to perform drawing
		};
	};
};

#endif

#endif
