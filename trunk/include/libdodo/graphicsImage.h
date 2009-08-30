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

namespace dodo {
	namespace graphics {
		struct __image__;

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
			image(image &);

		  public:

			/**
			 * @enum mappingEnum defines the order of pixels
			 */
			enum mappingEnum {
				MAPPING_RGB,
				MAPPING_RGBA,

				MAPPING_ENUMSIZE
			};

			/**
			 * @enum colorSpecificationEnum defines the type of the image
			 */
			enum colorSpecificationEnum {
				COLOR_SPECIFICATION_TRUECOLOR,
				COLOR_SPECIFICATION_GRAYSCALE,

				COLOR_SPECIFICATION_ENUMSIZE
			};

			/**
			 * @enum pixelSizeEnum defines size of pixel
			 */
			enum pixelSizeEnum {
				PIXEL_SIZE_CHAR,   ///< 8 bits
				PIXEL_SIZE_SHORT,  ///< 16 bits
				PIXEL_SIZE_INT,    ///< 32(16 on some CPUs) bits
				PIXEL_SIZE_LONG,   ///< 32(32 on some CPUs) bits
				PIXEL_SIZE_FLOAT,  ///< 32 bits
				PIXEL_SIZE_DOUBLE, ///< 64 bits

				PIXEL_SIZE_ENUMSIZE
			};

			/**
			 * @enum encoderEnum defines image encoder
			 */
			enum encoderEnum {
				ENCODER_PNG,
				ENCODER_JPEG,
				ENCODER_GIF,
				ENCODER_BMP,
				ENCODER_XPM,
				ENCODER_ICO,

				ENCODER_ENUMSIZE
			};

			/**
			 * @enum compressionEnum defines type of compression
			 */
			enum compressionEnum {
				COMPRESSION_NO,
				COMPRESSION_BZIP,
				COMPRESSION_LZW,
				COMPRESSION_RLE,
				COMPRESSION_ZIP,
				COMPRESSION_JPEG,         ///< only for JPEG
				COMPRESSION_LOSSLESSJPEG, ///< only for JPEG
				COMPRESSION_JPEG2000,     ///< only for JPEG

				COMPRESSION_ENUMSIZE
			};

			/**
			 * @struct __info__
			 * @brief defines image information
			 */
			struct __info__ {
				void          *data;            ///< 2D array of pixels
				unsigned long width;            ///< width of the image
				unsigned long height;           ///< height of the image
				short         mapping;          ///< type of mapping, @see image::mappingEnum
				short         pixelSize;        ///< type of pixel
			};

			/**
			 * @struct __size__
			 * @brief defines image dimensions
			 */
			struct __size__ {
				unsigned long width;            ///< width of the image
				unsigned long height;           ///< height of the image
			};

			/**
			 * @enum operationEnum defines type of operation for xexec
			 */
			enum operationEnum {
				OPERATION_WRITE,
				OPERATION_READ,
				OPERATION_CREATE,
			};

			/**
			 * @class __collected_data__
			 * @brief defines data that could be retrieved from the image object
			 */
			class __collected_data__
#ifndef GRAPHICS_WO_XEXEC
				: public xexec::__collected_data__
#endif
			{
			  public:

#ifndef GRAPHICS_WO_XEXEC
				/**
				 * constructor
				 * @param executor defines class that executed hook
				 * @param execObject defines type of object that executed a hook, @see xexec::objectEnum
				 */
				__collected_data__(xexec *executor,
									   short execObject);
#else
				/**
				 * constructor
				 */
				__collected_data__();
#endif

				/**
				 * destructor
				 */
				~__collected_data__();

				__image__ *handle; ///< ImageMagic handle
			};

			/**
			 * contructor
			 */
			image();

			/**
			 * destructor
			 */
			~image();

			/**
			 * create image
			 * @param width defines image width
			 * @param height defines image height
			 * @param background defines image background
			 * @param backgroundDepth defines the color depth of the image background in bits
			 */
			void create(unsigned long   width,
								unsigned long   height,
								const __color__ &background = color::transparent,
								unsigned short  backgroundDepth = 32);

			/**
			 * read image
			 * @param path defines path to image
			 */
			void readFile(const dodoString &path);

			/**
			 * read image
			 * @param info defines image info
			 */
			void readMemory(const __info__ &info);

			/**
			 * read image
			 * @param data defines image data
			 */
			void readMemory(const dodoString &data);

			/**
			 * write image
			 * @param path describes path to image
			 */
			void writeFile(const dodoString &path);

			/**
			 * write image
			 * @param data defines image data
			 */
			void writeMemory(dodoString &data);

			/**
			 * close access to image
			 */
			void close();

			/**
			 * @return info about image
			 */
			__size__ imageSize();

			/**
			 * set image color specification
			 * @param type defines image type, @see image::colorSpecificationEnum
			 */
			void setColorSpecification(short type);

			/**
			 * set image alpha channel
			 */
			void setAlpha();

			/**
			 * remove image alpha channel
			 */
			void removeAlpha();

			/**
			 * set image opacity
			 * @param opacity define image opacity
			 * @note 0 is fully opaque and 65535 is fully transparent
			 */
			void setOpacity(unsigned short opacity);

			/**
			 * set image background
			 * @param background define image background color
			 */
			void setBackgroundColor(__color__ background);

			/**
			 * set image output encoder
			 * @param encoder defines image encoder, @see image::encoderEnum
			 */
			void setEncoder(short encoder);

			/**
			 * set image compression type
			 * @param type defines type of image compression, @see image::compressionEnum
			 */
			void setCompression(short type);

			/**
			 * set image quality
			 * @param quality defines image quality
			 */
			void setQuality(short quality);

			/**
			 * @return image output encoder
			 */
			short encoder();

			/**
			 * @return image compression type
			 */
			short compression();

			/**
			 * @return image quality
			 */
			short quality();

		  protected:

			void *exInfo;                                                                   ///< exception info handler

			__collected_data__ collectedData;                                      ///< data collected for xexec

		  private:

			static const char *mappingStatements[MAPPING_ENUMSIZE];                       ///< image mapping statements
			static const int pixelSizeStatements[PIXEL_SIZE_ENUMSIZE];                     ///< pixel type statements
			static const int typeStatements[COLOR_SPECIFICATION_ENUMSIZE];                               ///< color space statements
			static const char *encoderStatements[ENCODER_ENUMSIZE];                       ///< image encoder
			static const int compressionStatements[COMPRESSION_ENUMSIZE];                 ///< image compression
		};
	};
};
#endif
#endif
