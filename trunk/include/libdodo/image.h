/***************************************************************************
 *            image.h
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

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <libdodo/directives.h>

#ifdef IMAGEMAGICK_EXT

#include <libdodo/types.h>
#include <libdodo/imageEx.h>

#include <magick/MagickCore.h>

namespace dodo
{
	/**
	 * @class image for simple image manipulations
	 */
	class image
	{
		public:

	};

	/**
	 * @class __image_init__ initializes image evironment
	 */
	class __image_init__
	{
		public:

			/**
			 * contructor
			 */
			__image_init__();

			/**
			 * destructor
			 */
			~__image_init__();

		private:

			bool initialized; ///< indicates if environment was initialized in the object
	};

	extern __image_init__ __image_init_object__;
};

#endif

#endif
