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
	 * @enum imageMappingEnum describes the order of pixels 
	 */
	enum imageMappingEnum
	{
		IMAGE_MAP_RGB,
		IMAGE_MAP_RGBA,
		IMAGE_MAP_CMYK
	};
	
	/**
	 * @enum imagePixelSizeEnum describes pixel's size
	 */
	enum imagePixelSizeEnum
	{
		IMAGE_PS_CHAR,///< 8 bits
		IMAGE_PS_SHORT,///< 16 bits
		IMAGE_PS_INT,///< 32(16 on some CPUs) bits
		IMAGE_PS_LONG,///< 32(32 on some CPUs) bits
		IMAGE_PS_FLOAT,///< 32 bits
		IMAGE_PS_DOUBLE,///< 64 bits
	};
	
	enum imageEncoderEnum
	{
		IMAGE_ENC_PNG,
		IMAGE_ENC_JPEG,
		IMAGE_ENC_RGB
	};

	/**
	 * @struct __imageInfo defines image information 
	 */
	struct __imageInfo
	{
		void *data;///< 2D array of pixels
		unsigned long width;///< width of the image
		unsigned long height;///< height of the image
		short mapping;///< type of mapping[see imageMappingEnum]
		short pixelSize;///< type of pixel
	};

	/**
	 * @class image for simple image manipulations
	 */
	class image
	{
		public:

			/**
			 * contructor
			 */
			image();

			/**
			 * destructor
			 */
			~image();
			
			/**
			 * reads image
			 * @param path describes path to image
			 */
			void read(const dodoString &path);
			
			/**
			 * reads image
			 * @param info describes image info
			 */
			void read(const __imageInfo &info);
			
			/**
			 * reads image
			 * @param data describes image data
			 * @param size describes image data size
			 */
			void read(const unsigned char * const data, unsigned long size);
			
			/**
			 * writes image
			 * @param path describes path to image
			 */
			void write(const dodoString &path);
			
			/**
			 * writes image
			 * @param data describes pointer to image
			 * @param size describes size of data
			 */
			void write(const unsigned char *data, unsigned int &size);
			
			/**
			 * @param encoder describes codec to encode image[see imageEncoderEnum]
			 */
			void setEncoder(short encoder);
			
			/**
			 * @param width describes width of the image
			 * @param height describes height of the image
			 */
			void scale(unsigned long width, unsigned long height);
			
		protected:
			
			ImageInfo *imInfo;///< image info handler
			Image *im;///< image handler
			ExceptionInfo *exInfo;///< exception info handler
		
		private:
			
			static const __statements mappingStArr[3];///< image mapping statements
			static const StorageType pixelSizeStArr[6];///< pixel type statements
			static const __statements encoderStArr[6];///< image encoder
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
