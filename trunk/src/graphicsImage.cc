/***************************************************************************
 *            graphicsImage.cc
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

#include <libdodo/directives.h>

#ifdef IMAGEMAGICK_EXT
#ifndef IMAGEMAGICK_PRE_63
#include <magick/MagickCore.h>
#else
#include <magick/ImageMagick.h>
#endif

#include <math.h>

#include "graphicsImage.inline"

#include <libdodo/graphicsImage.h>
#include <libdodo/types.h>
#include <libdodo/graphicsColor.h>
#include <libdodo/graphicsImageEx.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace graphics
	{
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

		__image_init__ __image_init_object__;
	};
};


//-------------------------------------------------------------------

using namespace dodo::graphics;

#ifndef GRAPHICS_WO_XEXEC
__xexecImageCollectedData::__xexecImageCollectedData(xexec *executor,
													 short execObject) : __xexecCollectedData(executor, execObject),
																		 handle(new __imageHandle)
{
}
#else
__xexecImageCollectedData::__xexecImageCollectedData() : handle(new __imageHandle)
{
}
#endif

//-------------------------------------------------------------------

__xexecImageCollectedData::~__xexecImageCollectedData()
{
	delete handle;
}

//-------------------------------------------------------------------

__image_init__::__image_init__()
{
	if (IsMagickInstantiated() == MagickFalse)
	{
#ifndef IMAGEMAGICK_PRE_63
		MagickCoreGenesis(NULL, MagickFalse);
#else
		InitializeMagick(NULL);
#endif
	}

	SetFatalErrorHandler(imErrorHandler);
	SetErrorHandler(imErrorHandler);
	SetWarningHandler(imWarningHandler);
}

//-------------------------------------------------------------------

__image_init__::~__image_init__()
{
	if (IsMagickInstantiated() == MagickTrue)
	{
#ifndef IMAGEMAGICK_PRE_63
		MagickCoreTerminus();
#else
		DestroyMagick();
#endif
	}
}

//-------------------------------------------------------------------

void
__image_init__::imWarningHandler(const ExceptionType et,
								 const char          *reason,
								 const char          *description)
{
}

//-------------------------------------------------------------------

void
__image_init__::imErrorHandler(const ExceptionType et,
							   const char          *reason,
							   const char          *description)
{
	throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_IMERRORHANDLER, exception::ERRNO_LIBDODO, IMAGEEX_IMERROR, reason, __LINE__, __FILE__, description);
}

//-------------------------------------------------------------------

const char *image::mappingStArr[] =
{
	"RGB",
	"RGBA",
};

//-------------------------------------------------------------------

const int image::typeStArr[] =
{
	TrueColorMatteType,
	GrayscaleMatteType,
};

//-------------------------------------------------------------------

const int image::pixelSizeStArr[] =
{
	CharPixel,
	ShortPixel,
	IntegerPixel,
	LongPixel,
	FloatPixel,
	DoublePixel
};

//-------------------------------------------------------------------

const char *image::encoderStArr[] =
{
	"PNG",
	"JPEG",
	"GIF",
	"BMP",
	"XPM",
	"ICO"
};

//-------------------------------------------------------------------

const int image::compressionStArr[] =
{
	NoCompression,
	LZWCompression,
	RLECompression,
	ZipCompression,
	BZipCompression,
	JPEGCompression,
	LosslessJPEGCompression,
	JPEG2000Compression,
};

//-------------------------------------------------------------------

image::image(image &a_image)
#ifndef GRAPHICS_WO_XEXEC
	: collectedData(this, XEXEC_OBJECT_GRAPHICSIMAGE)
#endif
{
}

//-------------------------------------------------------------------

image::image() : exInfo(new ExceptionInfo)
#ifndef GRAPHICS_WO_XEXEC
			   ,
				 collectedData(this, XEXEC_OBJECT_GRAPHICSIMAGE)
#endif
{
	collectedData.handle->imHandle = NULL;

#ifndef IMAGEMAGICK_PRE_63
	collectedData.handle->imInfo = AcquireImageInfo();
#else
	collectedData.handle->imInfo = CloneImageInfo(NULL);
#endif
}

//-------------------------------------------------------------------

image::~image()
{
	if (collectedData.handle->imHandle != NULL)
	{
		DestroyImage(collectedData.handle->imHandle);
	}

	DestroyImageInfo(collectedData.handle->imInfo);
	DestroyExceptionInfo((ExceptionInfo *)exInfo);

	delete (ExceptionInfo *)exInfo;
}

//-------------------------------------------------------------------

void
image::readFile(const dodoString &str)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_READ;
	performXExec(preExec);
#endif

	unsigned long size = str.size() + 1;

	if (size >= MaxTextExtent)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_LIBDODO, IMAGEEX_LONGPATH, GRAPHICSIMAGEEX_LONGPATH_STR, __LINE__, __FILE__);
	}

	GetExceptionInfo((ExceptionInfo *)exInfo);
	GetImageInfo(collectedData.handle->imInfo);

	strncpy(collectedData.handle->imInfo->filename, str.c_str(), size);

	if (collectedData.handle->imHandle != NULL)
	{
		DestroyImage(collectedData.handle->imHandle);
	}

	collectedData.handle->imHandle = ReadImage(collectedData.handle->imInfo, (ExceptionInfo *)exInfo);
	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)exInfo)->error_number, ((ExceptionInfo *)exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)exInfo)->description);
	}

	collectedData.handle->imInfo->compression = collectedData.handle->imHandle->compression;
	collectedData.handle->imInfo->quality = collectedData.handle->imHandle->quality;

	strcpy(collectedData.handle->imInfo->magick, collectedData.handle->imHandle->magick);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::readMemory(const dodoString &data)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_READ;
	performXExec(preExec);
#endif

	GetExceptionInfo((ExceptionInfo *)exInfo);
	GetImageInfo(collectedData.handle->imInfo);

	if (collectedData.handle->imHandle != NULL)
	{
		DestroyImage(collectedData.handle->imHandle);
	}

	collectedData.handle->imHandle = BlobToImage(collectedData.handle->imInfo, data.data(), data.size(), (ExceptionInfo *)exInfo);
	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)exInfo)->error_number, ((ExceptionInfo *)exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)exInfo)->description);
	}

	collectedData.handle->imInfo->compression = collectedData.handle->imHandle->compression;
	collectedData.handle->imInfo->quality = collectedData.handle->imHandle->quality;

	strcpy(collectedData.handle->imInfo->magick, collectedData.handle->imHandle->magick);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::readMemory(const __imageInfo &info)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_READ;
	performXExec(preExec);
#endif

	if (info.mapping < 0 || info.mapping >= IMAGE_MAPPINGSTATEMENTS || info.pixelSize < 0 || info.pixelSize >= IMAGE_PIXELSIZESTATEMENTS)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);
	}

	GetExceptionInfo((ExceptionInfo *)exInfo);
	GetImageInfo(collectedData.handle->imInfo);

	if (collectedData.handle->imHandle != NULL)
	{
		DestroyImage(collectedData.handle->imHandle);
	}

	collectedData.handle->imHandle = ConstituteImage(info.width, info.height, mappingStArr[info.mapping], (StorageType)pixelSizeStArr[info.pixelSize], info.data, (ExceptionInfo *)exInfo);
	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)exInfo)->error_number, ((ExceptionInfo *)exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)exInfo)->description);
	}

	collectedData.handle->imInfo->compression = collectedData.handle->imHandle->compression;
	collectedData.handle->imInfo->quality = collectedData.handle->imHandle->quality;

	strcpy(collectedData.handle->imInfo->magick, collectedData.handle->imHandle->magick);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::create(unsigned long  width,
			  unsigned long  height,
			  const __color  &background,
			  unsigned short backgroundDepth)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_CREATE;
	performXExec(preExec);
#endif

	GetExceptionInfo((ExceptionInfo *)exInfo);
	GetImageInfo(collectedData.handle->imInfo);

	if (collectedData.handle->imHandle != NULL)
	{
		DestroyImage(collectedData.handle->imHandle);
	}

	MagickPixelPacket bg;

	bg.colorspace = RGBColorspace;
	bg.matte = MagickTrue;
	bg.fuzz = 0;
	bg.depth = backgroundDepth;

	bg.red = background.red;
	bg.green = background.green;
	bg.blue = background.blue;
	bg.opacity = background.opacity;
	bg.index = 0;

	collectedData.handle->imHandle = NewMagickImage(collectedData.handle->imInfo, width, height, &bg);
	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_CREATE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)exInfo)->error_number, ((ExceptionInfo *)exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)exInfo)->description);
	}

	collectedData.handle->imInfo->compression = collectedData.handle->imHandle->compression;
	collectedData.handle->imInfo->quality = collectedData.handle->imHandle->quality;

	strcpy(collectedData.handle->imInfo->magick, collectedData.handle->imHandle->magick);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::setType(short type)
{
	if (type < 0 || type >= IMAGE_TYPESTATEMENTS)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETTYPE, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);
	}

	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETTYPE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	if (SetImageType(collectedData.handle->imHandle, (ImageType)typeStArr[type]) == MagickFalse)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETTYPE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_CANNOTSETTYPE, GRAPHICSIMAGEEX_CANNOTSETTYPE_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
image::setAlpha()
{
	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETALPHA, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	collectedData.handle->imHandle->matte = MagickTrue;
}

//-------------------------------------------------------------------

void
image::removeAlpha()
{
	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_REMOVEALPHA, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	collectedData.handle->imHandle->matte = MagickFalse;
}

//-------------------------------------------------------------------

void
image::setOpacity(unsigned short opacity)
{
	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETOPACITY, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	SetImageOpacity(collectedData.handle->imHandle, opacity);
}

//-------------------------------------------------------------------

void
image::setBackgroundColor(__color background)
{
	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETBACKGROUNDCOLOR, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	collectedData.handle->imHandle->background_color.red = background.red;
	collectedData.handle->imHandle->background_color.green = background.green;
	collectedData.handle->imHandle->background_color.blue = background.blue;
	collectedData.handle->imHandle->background_color.opacity = background.opacity;

	SetImageBackgroundColor(collectedData.handle->imHandle);
}

//-------------------------------------------------------------------

void
image::close()
{
	if (collectedData.handle->imHandle != NULL)
	{
		DestroyImage(collectedData.handle->imHandle);

		collectedData.handle->imHandle = NULL;
	}
}

//-------------------------------------------------------------------

void
image::writeFile(const dodoString &str)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_WRITE;
	performXExec(preExec);
#endif

	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	unsigned long size = str.size() + 1;

	if (size >= MaxTextExtent)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_LIBDODO, IMAGEEX_LONGPATH, GRAPHICSIMAGEEX_LONGPATH_STR, __LINE__, __FILE__);
	}

	strncpy(collectedData.handle->imHandle->filename, str.c_str(), size);

	GetExceptionInfo((ExceptionInfo *)exInfo);

	if (WriteImage(collectedData.handle->imInfo, collectedData.handle->imHandle) == MagickFalse)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, collectedData.handle->imHandle->exception.error_number, ((ExceptionInfo *)exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)exInfo)->description);
	}

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::writeMemory(dodoString &data)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_WRITE;
	performXExec(preExec);
#endif

	if (collectedData.handle->imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	GetExceptionInfo((ExceptionInfo *)exInfo);

	unsigned long size = 0;
	unsigned char *imData = ImageToBlob(collectedData.handle->imInfo, collectedData.handle->imHandle, (size_t *)&size, (ExceptionInfo *)exInfo);
	if (imData == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)exInfo)->error_number, ((ExceptionInfo *)exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)exInfo)->description);
	}

	data.assign((char *)imData, size);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::setCompression(short type)
{
	if (type < 0 || type >= IMAGE_COMPRESSIONSTATEMENTS)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETENCODER, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);
	}

	collectedData.handle->imInfo->compression = (CompressionType)compressionStArr[type];
}

//-------------------------------------------------------------------

void
image::setQuality(short quality)
{
	collectedData.handle->imInfo->quality = quality;
}

//-------------------------------------------------------------------

void
image::setEncoder(short encoder)
{
	if (encoder < 0 || encoder >= IMAGE_ENCODERSTATEMENTS)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETENCODER, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);
	}

	strcpy(collectedData.handle->imInfo->magick, encoderStArr[encoder]);
}

//-------------------------------------------------------------------

short
image::getCompression()
{
	for (int i = 0; i < IMAGE_COMPRESSIONSTATEMENTS; ++i)
	{
		if (collectedData.handle->imInfo->compression == compressionStArr[i])
		{
			return i;
		}
	}
}

//-------------------------------------------------------------------

short
image::getQuality()
{
	return collectedData.handle->imInfo->quality;
}

//-------------------------------------------------------------------

short
image::getEncoder()
{
	for (int i = 0; i < IMAGE_ENCODERSTATEMENTS; ++i)
	{
		if (strcmp(collectedData.handle->imInfo->magick, encoderStArr[i]) == 0)
		{
			return i;
		}
	}
}

//-------------------------------------------------------------------

__imageSize
image::getImageSize()
{
	if (collectedData.handle->imHandle == NULL)
	{
		return __imageSize();
	}

	__imageSize info;

	info.height = collectedData.handle->imHandle->rows;
	info.width = collectedData.handle->imHandle->columns;

	return info;
}

#endif

//-------------------------------------------------------------------

