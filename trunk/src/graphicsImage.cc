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

#include <libdodo/graphicsImage.h>

#ifdef IMAGEMAGICK_EXT

namespace dodo
{
	namespace graphics
	{
		__image_init__ __image_init_object__;
	};
};

//-------------------------------------------------------------------

using namespace dodo::graphics;

#ifndef GRAPHICS_WO_XEXEC

__xexecImageCollectedData::__xexecImageCollectedData(xexec *executor,
                                                     short execObject) : __xexecCollectedData(executor, execObject)
{
}

#endif

//-------------------------------------------------------------------

__image_init__::__image_init__() : initialized(false)
{
	if (IsMagickInstantiated() == MagickFalse)
	{
		initialized = true;

		MagickCoreGenesis(NULL, MagickFalse);
	}
}

//-------------------------------------------------------------------

__image_init__::~__image_init__()
{
	if (initialized)
		MagickCoreTerminus();
}

//-------------------------------------------------------------------

const char *image::mappingStArr[] =
{
	"RGB",
	"RGBA",
};

//-------------------------------------------------------------------

const ImageType image::typeStArr[] =
{
	TrueColorMatteType,
	GrayscaleMatteType,
};

//-------------------------------------------------------------------

const StorageType image::pixelSizeStArr[] =
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

const CompressionType image::compressionStArr[] =
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

image::image()

#ifndef GRAPHICS_WO_XEXEC

: collectedData(this, XEXEC_OBJECT_GRAPHICSIMAGE)

#endif
{
	collectedData.imHandle = NULL;

	collectedData.imInfo = AcquireImageInfo();
	exInfo = AcquireExceptionInfo();
}

//-------------------------------------------------------------------

image::image(const dodoString &str)

#ifndef GRAPHICS_WO_XEXEC

: collectedData(this, XEXEC_OBJECT_GRAPHICSIMAGE)

#endif
{
	collectedData.imHandle = NULL;

	collectedData.imInfo = AcquireImageInfo();
	exInfo = AcquireExceptionInfo();

	unsigned long size = str.size() + 1;

	if (size < MaxTextExtent)
	{
		GetImageInfo(collectedData.imInfo);

		strncpy(collectedData.imInfo->filename, str.c_str(), size);

		collectedData.imHandle = ReadImage(collectedData.imInfo, exInfo);
		if (collectedData.imHandle != NULL)
		{
			collectedData.imInfo->compression = collectedData.imHandle->compression;
			collectedData.imInfo->quality = collectedData.imHandle->quality;

			strcpy(collectedData.imInfo->magick, collectedData.imHandle->magick);
		}
	}
}

//-------------------------------------------------------------------

image::~image()
{
	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	DestroyImageInfo(collectedData.imInfo);
	DestroyExceptionInfo(exInfo);
}

//-------------------------------------------------------------------

void
image::read(const dodoString &str)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_READ;
	performXExec(preExec);
#endif

	unsigned long size = str.size() + 1;

	if (size >= MaxTextExtent)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_LIBDODO, IMAGEEX_LONGPATH, GRAPHICSIMAGEEX_LONGPATH_STR, __LINE__, __FILE__);

	GetImageInfo(collectedData.imInfo);

	strncpy(collectedData.imInfo->filename, str.c_str(), size);

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	collectedData.imHandle = ReadImage(collectedData.imInfo, exInfo);
	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	collectedData.imInfo->compression = collectedData.imHandle->compression;
	collectedData.imInfo->quality = collectedData.imHandle->quality;

	strcpy(collectedData.imInfo->magick, collectedData.imHandle->magick);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::read(const unsigned char * const data,
			unsigned long size)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_READ;
	performXExec(preExec);
#endif

	GetImageInfo(collectedData.imInfo);

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	collectedData.imHandle = BlobToImage(collectedData.imInfo, data, size, exInfo);
	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	collectedData.imInfo->compression = collectedData.imHandle->compression;
	collectedData.imInfo->quality = collectedData.imHandle->quality;

	strcpy(collectedData.imInfo->magick, collectedData.imHandle->magick);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::read(const __imageInfo &info)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_READ;
	performXExec(preExec);
#endif

	if (info.mapping < 0 || info.mapping >= IMAGE_MAPPINGSTATEMENTS || info.pixelSize < 0 || info.pixelSize >= IMAGE_PIXELSIZESTATEMENTS)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);

	GetImageInfo(collectedData.imInfo);

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	collectedData.imHandle = ConstituteImage(info.width, info.height, mappingStArr[info.mapping], pixelSizeStArr[info.pixelSize], info.data, exInfo);
	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	collectedData.imInfo->compression = collectedData.imHandle->compression;
	collectedData.imInfo->quality = collectedData.imHandle->quality;

	strcpy(collectedData.imInfo->magick, collectedData.imHandle->magick);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::create(unsigned long width,
              unsigned long height,
              const __color &background,
              unsigned short backgroundDepth)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_CREATE;
	performXExec(preExec);
#endif

	GetImageInfo(collectedData.imInfo);

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

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

	collectedData.imHandle = NewMagickImage(collectedData.imInfo, width, height, &bg);
	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_CREATE, exception::ERRNO_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	collectedData.imInfo->compression = collectedData.imHandle->compression;
	collectedData.imInfo->quality = collectedData.imHandle->quality;

	strcpy(collectedData.imInfo->magick, collectedData.imHandle->magick);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif

}

//-------------------------------------------------------------------

void
image::setType(short type)
{
	if (type < 0 || type >= IMAGE_TYPESTATEMENTS)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETTYPE, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);

	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETTYPE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	if (SetImageType(collectedData.imHandle, typeStArr[type]) == MagickFalse)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETTYPE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_CANNOTSETTYPE, GRAPHICSIMAGEEX_CANNOTSETTYPE_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
image::setAlpha()
{
	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETALPHA, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	collectedData.imHandle->matte = MagickTrue;
}

//-------------------------------------------------------------------

void
image::removeAlpha()
{
	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_REMOVEALPHA, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	collectedData.imHandle->matte = MagickFalse;
}

//-------------------------------------------------------------------

void
image::setOpacity(unsigned short opacity)
{
	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETOPACITY, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	SetImageOpacity(collectedData.imHandle, opacity);
}


//-------------------------------------------------------------------

void
image::setBackgroundColor(__color background)
{
	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETBACKGROUNDCOLOR, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	collectedData.imHandle->background_color.red = background.red;
	collectedData.imHandle->background_color.green = background.green;
	collectedData.imHandle->background_color.blue = background.blue;
	collectedData.imHandle->background_color.opacity = background.opacity;

	SetImageBackgroundColor(collectedData.imHandle);
}

//-------------------------------------------------------------------

void
image::close()
{
	if (collectedData.imHandle != NULL)
	{
		DestroyImage(collectedData.imHandle);

		collectedData.imHandle = NULL;
	}
}

//-------------------------------------------------------------------

void
image::write(const dodoString &str)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_WRITE;
	performXExec(preExec);
#endif

	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	unsigned long size = str.size() + 1;

	if (size >= MaxTextExtent)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_LIBDODO, IMAGEEX_LONGPATH, GRAPHICSIMAGEEX_LONGPATH_STR, __LINE__, __FILE__);

	strncpy(collectedData.imHandle->filename, str.c_str(), size);

	if (WriteImage(collectedData.imInfo, collectedData.imHandle) == MagickFalse)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, collectedData.imHandle->exception.error_number, exInfo->reason, __LINE__, __FILE__);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::write(unsigned char **data,
			 unsigned int &size)
{
#ifndef GRAPHICS_WO_XEXEC
	operType = IMAGE_OPERATION_WRITE;
	performXExec(preExec);
#endif

	if (collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	size = 0;
	*data = ImageToBlob(collectedData.imInfo, collectedData.imHandle, (size_t *)&size, exInfo);
	if (data == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

#ifndef GRAPHICS_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::setCompression(short type)
{
	if (type < 0 || type >= IMAGE_COMPRESSIONSTATEMENTS)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETENCODER, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);

	collectedData.imInfo->compression = compressionStArr[type];
}

//-------------------------------------------------------------------

void
image::setQuality(short quality)
{
	collectedData.imInfo->quality = quality;
}

//-------------------------------------------------------------------

void
image::setEncoder(short encoder)
{
	if (encoder < 0 || encoder >= IMAGE_ENCODERSTATEMENTS)
		throw exception::basic(exception::ERRMODULE_GRAPHICSIMAGE, IMAGEEX_SETENCODER, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);

	strcpy(collectedData.imInfo->magick, encoderStArr[encoder]);
}

//-------------------------------------------------------------------

short
image::getCompression()
{
	for (int i = 0; i < IMAGE_COMPRESSIONSTATEMENTS; ++i)
		if (collectedData.imInfo->compression == compressionStArr[i])
			return i;
}

//-------------------------------------------------------------------

short
image::getQuality()
{
	return collectedData.imInfo->quality;
}

//-------------------------------------------------------------------

short
image::getEncoder()
{
	for (int i = 0; i < IMAGE_ENCODERSTATEMENTS; ++i)
		if (strcmp(collectedData.imInfo->magick, encoderStArr[i]) == 0)
			return i;
}

//-------------------------------------------------------------------

__imageSize
image::getImageSize()
{
	if (collectedData.imHandle == NULL)
		return __imageSize();

	__imageSize info;

	info.height = collectedData.imHandle->rows;
	info.width = collectedData.imHandle->columns;

	return info;
}

//-------------------------------------------------------------------

void
image::destroyImageData(unsigned char **data)
{
	free(*data);
}

#endif

//-------------------------------------------------------------------

