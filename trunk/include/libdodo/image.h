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

#include <magick/MagickCore.h>
#include <math.h>

#include <libdodo/types.h>
#include <libdodo/imageEx.h>
#include <libdodo/xexec.h>

namespace dodo
{
	/**
	 * @enum imageOperTypeEnum defines type of operation for hook
	 */
	enum imageOperTypeEnum
	{
		IMAGE_OPERATION_WRITE,
		IMAGE_OPERATION_READ,
	};

	/**
	 * @struct __xexexImageCollectedData defines data that could be retrieved from the image object
	 */
	struct __xexexImageCollectedData
	{
		/**
		 * constructor
		 * @param operType defines xexec operation
		 * @param executor defines class that executed hook
		 */
		__xexexImageCollectedData(int &operType,
					  void *executor);

		ImageInfo *imInfo;                                      ///< image info handler
		Image *imHandle;                                        ///< image handler

		int &operType;                                          ///< xexec operation

		void *executor;                                         ///< class that executed hook
	};

#define IMAGE_MAPPINGS 3

	/**
	 * @enum imageMappingEnum defines the order of pixels
	 */
	enum imageMappingEnum
	{
		IMAGE_MAP_RGB,
		IMAGE_MAP_RGBA,
		IMAGE_MAP_CMYK
	};

#define IMAGE_PIXELSIZES 6

	/**
	 * @enum imagePixelSizeEnum defines size of pixel
	 */
	enum imagePixelSizeEnum
	{
		IMAGE_PIXELSIZE_CHAR,   ///< 8 bits
		IMAGE_PIXELSIZE_SHORT,  ///< 16 bits
		IMAGE_PIXELSIZE_INT,    ///< 32(16 on some CPUs) bits
		IMAGE_PIXELSIZE_LONG,   ///< 32(32 on some CPUs) bits
		IMAGE_PIXELSIZE_FLOAT,  ///< 32 bits
		IMAGE_PIXELSIZE_DOUBLE, ///< 64 bits
	};

#define IMAGE_ENCODERS 6

	/**
	 * @enum imageEncoderEnum defines image encoder
	 */
	enum imageEncoderEnum
	{
		IMAGE_ENCODER_PNG,
		IMAGE_ENCODER_JPEG,
		IMAGE_ENCODER_GIF,
		IMAGE_ENCODER_BMP,
		IMAGE_ENCODER_XPM,
		IMAGE_ENCODER_ICO,
	};

	/**
	 * @enum imageRotateDirAngEnum defines rotation options
	 */
	enum imageRotateDirectionAngleEnum
	{
		IMAGE_ROTATEDIRECTIONANGLE_90CW = -90,
		IMAGE_ROTATEDIRECTIONANGLE_90CCW = 90,
		IMAGE_ROTATEDIRECTIONANGLE_180 = 180,
		IMAGE_ROTATEDIRECTIONANGLE_270CW = -270,
		IMAGE_ROTATEDIRECTIONANGLE_270CCW = 270,
	};

#define IMAGE_COMPRESSIONS

	/**
	 * @enum imageCompressionEnum defines type of compression
	 */
	enum imageCompressionEnum
	{
		IMAGE_COMPRESSION_NO,
		IMAGE_COMPRESSION_BZIP,
		IMAGE_COMPRESSION_LZW,
		IMAGE_COMPRESSION_RLE,
		IMAGE_COMPRESSION_ZIP,
		IMAGE_COMPRESSION_JPEG,         ///< only for JPEG
		IMAGE_COMPRESSION_LOSSLESSJPEG, ///< only for JPEG
		IMAGE_COMPRESSION_JPEG2000,     ///< only for JPEG
	};

	/**
	 * @struct __imageInfo defines image information
	 */
	struct __imageInfo
	{
		void *data;             ///< 2D array of pixels
		unsigned long width;    ///< width of the image
		unsigned long height;   ///< height of the image
		short mapping;          ///< type of mapping[see imageMappingEnum]
		short pixelSize;        ///< type of pixel
	};

	/**
	 * @struct __imageSize defines image dimensions
	 */
	struct __imageSize
	{
		unsigned long width;    ///< width of the image
		unsigned long height;   ///< height of the image
	};

	/**
	 * @class image provides simple image manipulations
	 */
	class image : public xexec
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			image(image &im);

		public:

			/**
			 * contructor
			 */
			image();

			/**
			 * destructor
			 */
			virtual ~image();

			/**
			 * read image
			 * @param path defines path to image
			 */
			virtual void read(const dodoString &path);

			/**
			 * read image
			 * @param info defines image info
			 */
			virtual void read(const __imageInfo &info);

			/**
			 * read image
			 * @param data defines image data
			 * @param size defines size of image data
			 */
			virtual void read(const unsigned char * const data, unsigned long size);

			/**
			 * write image
			 * @param path describes path to image
			 */
			virtual void write(const dodoString &path);

			/**
			 * write image
			 * @param data defines image data
			 * @param size defines size of image data
			 */
			virtual void write(unsigned char **data, unsigned int &size);

			/**
			 * close access to image
			 */
			virtual void close();

			/**
			 * @return info about image
			 */
			virtual __imageSize getImageSize();

			/**
			 * set image output encoder
			 * @param encoder defines image encoder[see imageEncoderEnum]
			 */
			virtual void setEncoder(short encoder);

			/**
			 * set image compression type
			 * @param type defines type of image compression[see imageCompressionEnum]
			 */
			virtual void setCompression(short type);

			/**
			 * set image quality
			 * @param quality defines image quality
			 */
			virtual void setQuality(short quality);

			/**
			 * @return image output encoder
			 */
			virtual short getEncoder();

			/**
			 * @return image compression type
			 */
			virtual short getCompression();

			/**
			 * @return image quality
			 */
			virtual short getQuality();

			/**
			 * scale image
			 * @param width defines width of the image
			 * @param height defines height of the image
			 */
			virtual void scale(unsigned long width, unsigned long height);

			/**
			 * scale image
			 * @param width defines size of the image
			 * @note scales image by reducing size of the greater side saving proportions
			 */
			virtual void scale(unsigned long size);

			/**
			 * rotate image
			 * @param angle defines rotation degree[see also imageRotateDirectionAngleEnum]
			 */
			virtual void rotate(double angle);

			/**
			 * destroy image data got from write
			 * @param data defines image data
			 */
			virtual void destroyImageData(unsigned char **data);

		protected:

			ExceptionInfo *exInfo;                          ///< exception info handler

			__xexexImageCollectedData collectedData;        ///< data collected for xexec

		private:

			static const char *mappingStArr[IMAGE_MAPPINGS];                        ///< image mapping statements
			static const StorageType pixelSizeStArr[IMAGE_PIXELSIZES];              ///< pixel type statements
			static const char *encoderStArr[IMAGE_ENCODERS];                        ///< image encoder
			static const CompressionType compressionStArr[IMAGE_COMPRESSIONS];      ///< image compression
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

			bool initialized; ///< true if environment was initialized in the object
	};

	extern __image_init__ __image_init_object__;
};

#endif

#endif
