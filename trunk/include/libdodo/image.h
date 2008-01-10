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

#include <libdodo/types.h>
#include <libdodo/imageEx.h>
#include <libdodo/xexec.h>

namespace dodo
{
	/**
	 * @enum imageOperTypeEnum describes type of operation for hook
	 */
	enum imageOperTypeEnum
	{
		IMAGE_OPERATION_WRITE,
		IMAGE_OPERATION_READ,
	};

	/**
	 * @struct __xexexImageCollectedData contains data that could be retrieved from class(to modificate)[contains references]
	 */
	struct __xexexImageCollectedData
	{
		/**
		 * constructor
		 * initiates references
		 */
		__xexexImageCollectedData(ImageInfo &imInfo,
								  Image &im,
								  int &operType,
								  void *executor);

		ImageInfo &imInfo;  ///< image info handler
		Image &im;          ///< image handler

		int &operType;      ///< operation type set by main action; can be used in hook to determine type of action

		void *executor;     ///< class that executed hook
	};

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
		IMAGE_PIXELSIZE_CHAR,   ///< 8 bits
		IMAGE_PIXELSIZE_SHORT,  ///< 16 bits
		IMAGE_PIXELSIZE_INT,    ///< 32(16 on some CPUs) bits
		IMAGE_PIXELSIZE_LONG,   ///< 32(32 on some CPUs) bits
		IMAGE_PIXELSIZE_FLOAT,  ///< 32 bits
		IMAGE_PIXELSIZE_DOUBLE, ///< 64 bits
	};

	/**
	 * @enum imageEncoderEnum describes image encoder
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
	 * @enum imageRotateDirAngEnum describes parameters for rotation
	 */
	enum imageRotateDirectionAngleEnum
	{
		IMAGE_ROTATEDIRECTIONANGLE_90CW = -90,
		IMAGE_ROTATEDIRECTIONANGLE_90CCW = 90,
		IMAGE_ROTATEDIRECTIONANGLE_180 = 180,
		IMAGE_ROTATEDIRECTIONANGLE_270CW = -270,
		IMAGE_ROTATEDIRECTIONANGLE_270CCW = 270,
	};

	/**
	 * @enum imageCompressionEnum describes type of compression
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
	 * @struct __imageSize defines image width/height
	 */
	struct __imageSize
	{
		unsigned long width;    ///< width of the image
		unsigned long height;   ///< height of the image
	};

	/**
	 * @class image for simple image manipulations
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
			 * reads image
			 * @param path describes path to image
			 */
			virtual void read(const dodoString &path);

			/**
			 * reads image
			 * @param info describes image info
			 */
			virtual void read(const __imageInfo &info);

			/**
			 * reads image
			 * @param data describes image data
			 * @param size describes image data size
			 */
			virtual void read(const unsigned char * const data, unsigned long size);

			/**
			 * writes image
			 * @param path describes path to image
			 */
			virtual void write(const dodoString &path);

			/**
			 * writes image
			 * @param data describes pointer to image
			 * @param size describes size of data
			 */
			virtual void write(unsigned char **data, unsigned int &size);

			/**
			 * closes access to image
			 */
			virtual void close();
			
			/**
			 * @return info about image
			 */
			virtual __imageSize getImageSize();

			/**
			 * sets image output encoder
			 * @param encoder describes codec to encode image[see imageEncoderEnum]
			 */
			virtual void setEncoder(short encoder);

			/**
			 * sets image compression type
			 * @param type describes type of image compression[see imageCompressionEnum]
			 */
			virtual void setCompression(short type);

			/**
			 * sets image quality
			 * @param quality describes quality of image
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
			 * scales image
			 * @param width describes width of the image
			 * @param height describes height of the image
			 */
			virtual void scale(unsigned long width, unsigned long height);

			/**
			 * rotates image
			 * @param angle describes the number of degrees to rotate the image[see also imageRotateDirAngEnum]
			 */
			virtual void rotate(double angle);

			/**
			 * destroys image data got from write
			 * @param data describes pointer to image
			 */
			virtual void destroyImageData(unsigned char **data);

			#ifndef IMAGE_WO_XEXEC

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * adds hook before the operation by callback
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 */
			virtual int addPreExec(inExec func, void *data);

			#ifdef DL_EXT

			/**
			 * set function from module that will be executed before/after the main action call
			 * the type of hook[pre/post] is defined in module
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual __xexecCounts addExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPostExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPreExec(const dodoString &module, void *data, void *toInit = NULL);

			#endif

			#endif

		protected:

			ImageInfo *imInfo;                          ///< image info handler
			Image *im;                                  ///< image handler
			ExceptionInfo *exInfo;                      ///< exception info handler

			__xexexImageCollectedData collectedData;    ///< data collected for xexec

		private:

			static const __statements mappingStArr[3];          ///< image mapping statements
			static const StorageType pixelSizeStArr[6];         ///< pixel type statements
			static const __statements encoderStArr[6];          ///< image encoder
			static const CompressionType compressionStArr[8];   ///< image encoder
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
