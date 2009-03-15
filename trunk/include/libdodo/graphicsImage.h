/***************************************************************************
 *            graphicsImage.h
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

#ifndef _GRAPHICSIMAGE_H_
#define _GRAPHICSIMAGE_H_ 1

#include <libdodo/directives.h>

#ifdef IMAGEMAGICK_EXT
#include <libdodo/types.h>
#include <libdodo/graphicsColor.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace graphics
	{
		/**
		 * @struct __imageHandle
		 * @brief defines internal handlers for image processing library
		 */
		struct __imageHandle;

		/**
		 * @enum imageOperTypeEnum defines type of operation for hook
		 */
		enum imageOperTypeEnum
		{
			IMAGE_OPERATION_WRITE,
			IMAGE_OPERATION_READ,
			IMAGE_OPERATION_CREATE,
		};

		/**
		 * @class __xexecImageCollectedData
		 * @brief defines data that could be retrieved from the image object
		 */
		class __xexecImageCollectedData
#ifndef GRAPHICS_WO_XEXEC
			: public __xexecCollectedData
#endif
		{
		  public:

			/**
			 * constructor
			 */
			__xexecImageCollectedData();

#ifndef GRAPHICS_WO_XEXEC
			/**
			 * constructor
			 * @param executor defines class that executed hook
			 * @param execObject defines type of object that executed a hook[see xexecObjectTypeEnum]
			 */
			__xexecImageCollectedData(xexec *executor,
									  short execObject);
#endif

			/**
			 * destructor
			 */
			~__xexecImageCollectedData();

			__imagecHandle *handle; ///< ImageMagic handle
		};

#define IMAGE_MAPPINGSTATEMENTS 2

		/**
		 * @enum imageMappingEnum defines the order of pixels
		 */
		enum imageMappingEnum
		{
			IMAGE_MAPPING_RGB,
			IMAGE_MAPPING_RGBA,
		};

#define IMAGE_TYPESTATEMENTS 2

		/**
		 * @enum imageTypeEnum defines the type of the image
		 */
		enum imageTypeEnum
		{
			IMAGE_TYPE_TRUECOLOR,
			IMAGE_TYPE_GRAYSCALE
		};

#define IMAGE_PIXELSIZESTATEMENTS 6

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

#define IMAGE_ENCODERSTATEMENTS 6

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

#define IMAGE_COMPRESSIONSTATEMENTS 8

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
		 * @struct __imageInfo
		 * @brief defines image information
		 */
		struct __imageInfo
		{
			void          *data;            ///< 2D array of pixels
			unsigned long width;            ///< width of the image
			unsigned long height;           ///< height of the image
			short         mapping;          ///< type of mapping[see imageMappingEnum]
			short         pixelSize;        ///< type of pixel
		};

		/**
		 * @struct __imageSize
		 * @brief defines image dimensions
		 */
		struct __imageSize
		{
			unsigned long width;            ///< width of the image
			unsigned long height;           ///< height of the image
		};

		/**
		 * @class image
		 * @brief provides simple image manipulations
		 */
		class image
#ifndef GRAPHICS_WO_XEXEC
			: public xexec
#endif
		{
			friend class transform;
			friend class draw;

		  private:

			/**
			 * copy constructor
			 * @note to prevent copying
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
			 * create image
			 * @param width defines image width
			 * @param height defines image height
			 * @param background defines image background
			 * @param backgroundDepth defines the color depth of the image background in bits
			 */
			virtual void create(unsigned long  width,
								unsigned long  height,
								const __color  &background = color::transparent,
								unsigned short backgroundDepth = 32);

			/**
			 * read image
			 * @param path defines path to image
			 */
			virtual void readFile(const dodoString &path);

			/**
			 * read image
			 * @param info defines image info
			 */
			virtual void readMemory(const __imageInfo &info);

			/**
			 * read image
			 * @param data defines image data
			 */
			virtual void readMemory(const dodoString &data);

			/**
			 * write image
			 * @param path describes path to image
			 */
			virtual void writeFile(const dodoString &path);

			/**
			 * write image
			 * @param data defines image data
			 */
			virtual void writeMemory(dodoString &data);

			/**
			 * close access to image
			 */
			virtual void close();

			/**
			 * @return info about image
			 */
			virtual __imageSize getImageSize();

			/**
			 * set image type
			 * @param type defines image type[see imageTypeEnum]
			 */
			virtual void setType(short type);

			/**
			 * set image alpha channel
			 */
			virtual void setAlpha();

			/**
			 * remove image alpha channel
			 */
			virtual void removeAlpha();

			/**
			 * set image opacity
			 * @param opacity define image opacity
			 * @note 0 is fully opaque and 65535 is fully transparent
			 */
			virtual void setOpacity(unsigned short opacity);

			/**
			 * set image background
			 * @param background define image background color
			 */
			virtual void setBackgroundColor(__color background);

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

		  protected:

			ExceptionInfo exInfo;                                                       ///< exception info handler

			__xexecImageCollectedData collectedData;                                    ///< data collected for xexec

		  private:

			static const char *mappingStArr[IMAGE_MAPPINGSTATEMENTS];                   ///< image mapping statements
			static const StorageType pixelSizeStArr[IMAGE_PIXELSIZESTATEMENTS];         ///< pixel type statements
			static const ImageType typeStArr[IMAGE_TYPESTATEMENTS];                     ///< color space statements
			static const char *encoderStArr[IMAGE_ENCODERSTATEMENTS];                   ///< image encoder
			static const CompressionType compressionStArr[IMAGE_COMPRESSIONSTATEMENTS]; ///< image compression
		};

		/**
		 * @class __image_init__
		 * @brief initializes image evironment
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

			/**
			 * ImageMagic error handler
			 * @param et defines error category
			 * @param reason defines the reason of the error
			 * @param description defines description to the reason
			 */
			static void imErrorHandler(const ExceptionType et,
									   const char          *reason,
									   const char          *description);

			/**
			 * ImageMagic warning handler
			 * @param et defines warning category
			 * @param reason defines the reason of the warning
			 * @param description defines description to the reason
			 */
			static void imWarningHandler(const ExceptionType et,
										 const char          *reason,
										 const char          *description);
		};

		extern __image_init__ __image_init_object__;
	};
};
#endif

#endif
