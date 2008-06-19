/***************************************************************************
 *            image.cc
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

#include <libdodo/image.h>

#ifdef IMAGEMAGICK_EXT

namespace dodo
{
	__image_init__ __image_init_object__;
};

//-------------------------------------------------------------------

using namespace dodo;

__xexexImageCollectedData::__xexexImageCollectedData(int &a_operType,
													 void *a_executor) : operType(a_operType),
																		 executor(a_executor)
{
}

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
	"CMYK"
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

image::image(image &a_image) : collectedData(operType,
											 (void *) this)
{
}

//-------------------------------------------------------------------

image::image() : collectedData(operType,
							   (void *) this)
{
#ifndef IMAGE_WO_XEXEC

	execObject = XEXEC_OBJECT_IMAGE;
	execObjectData = (void *)&collectedData;

#endif

	collectedData.imHandle = NULL;

	collectedData.imInfo = AcquireImageInfo();
	exInfo = AcquireExceptionInfo();
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
#ifndef IMAGE_WO_XEXEC
	operType = IMAGE_OPERATION_READ;
	performXExec(preExec);
#endif

	strcpy(collectedData.imInfo->filename, str.c_str());

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	collectedData.imHandle = ReadImage(collectedData.imInfo, exInfo);
	if (collectedData.imHandle == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	collectedData.imInfo->compression = collectedData.imHandle->compression;
	collectedData.imInfo->quality = collectedData.imHandle->quality;
	strcpy(collectedData.imInfo->magick, collectedData.imHandle->magick);

#ifndef IMAGE_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::read(const unsigned char * const data,
			unsigned long size)
{
#ifndef IMAGE_WO_XEXEC
	operType = IMAGE_OPERATION_READ;
	performXExec(preExec);
#endif

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	collectedData.imHandle = BlobToImage(collectedData.imInfo, data, size, exInfo);
	if (collectedData.imHandle == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	collectedData.imInfo->compression = collectedData.imHandle->compression;
	collectedData.imInfo->quality = collectedData.imHandle->quality;
	strcpy(collectedData.imInfo->magick, collectedData.imHandle->magick);

#ifndef IMAGE_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::read(const __imageInfo &info)
{
#ifndef IMAGE_WO_XEXEC
	operType = IMAGE_OPERATION_READ;
	performXExec(preExec);
#endif

	if (info.mapping < 0 || info.mapping >= IMAGE_MAPPINGS || info.pixelSize < 0 || info.pixelSize >= IMAGE_PIXELSIZES)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_LIBDODO, IMAGEEX_BADINFO, IMAGEEX_BADINFO_STR, __LINE__, __FILE__);

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	collectedData.imHandle = ConstituteImage(info.width, info.height, mappingStArr[info.mapping], pixelSizeStArr[info.pixelSize], info.data, exInfo);
	if (collectedData.imHandle == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	collectedData.imInfo->compression = collectedData.imHandle->compression;
	collectedData.imInfo->quality = collectedData.imHandle->quality;
	strcpy(collectedData.imInfo->magick, collectedData.imHandle->magick);

#ifndef IMAGE_WO_XEXEC
	performXExec(postExec);
#endif
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
#ifndef IMAGE_WO_XEXEC
	operType = IMAGE_OPERATION_WRITE;
	performXExec(preExec);
#endif

	if (collectedData.imHandle == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_WRITE, ERR_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, IMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	strcpy(collectedData.imHandle->filename, str.c_str());

	if (WriteImage(collectedData.imInfo, collectedData.imHandle) == MagickFalse)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_WRITE, ERR_IMAGEMAGICK, collectedData.imHandle->exception.error_number, exInfo->reason, __LINE__, __FILE__);

#ifndef IMAGE_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::write(unsigned char **data,
			 unsigned int &size)
{
#ifndef IMAGE_WO_XEXEC
	operType = IMAGE_OPERATION_WRITE;
	performXExec(preExec);
#endif

	if (collectedData.imHandle == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_WRITE, ERR_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, IMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	size = 0;
	*data = ImageToBlob(collectedData.imInfo, collectedData.imHandle, &size, exInfo);
	if (data == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_WRITE, ERR_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

#ifndef IMAGE_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
image::setCompression(short type)
{
	if (type < 0 || type >= IMAGE_COMPRESSIONS)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_SETENCODER, ERR_LIBDODO, IMAGEEX_BADINFO, IMAGEEX_BADINFO_STR, __LINE__, __FILE__);

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
	if (encoder < 0 || encoder >= IMAGE_ENCODERS)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_SETENCODER, ERR_LIBDODO, IMAGEEX_BADINFO, IMAGEEX_BADINFO_STR, __LINE__, __FILE__);

	strcpy(collectedData.imInfo->magick, encoderStArr[encoder]);
}

//-------------------------------------------------------------------

short
image::getCompression()
{
	for (int i = 0; i < IMAGE_COMPRESSIONS; ++i)
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
	for (int i = 0; i < IMAGE_ENCODERS; ++i)
		if (strcmp(collectedData.imInfo->magick, encoderStArr[i]) == 0)
			return i;
}

//-------------------------------------------------------------------

void
image::scale(unsigned long width,
			 unsigned long height)
{
	if (collectedData.imHandle == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_SCALE, ERR_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, IMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	Image *image = ScaleImage(collectedData.imHandle, width, height, exInfo);

	if (image == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_SCALE, ERR_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	collectedData.imHandle = image;
}

//-------------------------------------------------------------------

void
image::scale(unsigned long size)
{

	if (collectedData.imHandle == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_SCALE, ERR_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, IMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	float mult = (float)size / (float)((collectedData.imHandle->columns > collectedData.imHandle->rows) ? collectedData.imHandle->columns : collectedData.imHandle->rows);

	Image *image = ScaleImage(collectedData.imHandle, (unsigned long)floor(collectedData.imHandle->columns * mult), (unsigned long)floor(collectedData.imHandle->rows * mult), exInfo);

	if (image == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_SCALE, ERR_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	collectedData.imHandle = image;
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
image::rotate(double angle)
{
	if (collectedData.imHandle == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_ROTATE, ERR_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, IMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	Image *image = RotateImage(collectedData.imHandle, angle, exInfo);

	if (image == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_ROTATE, ERR_IMAGEMAGICK, exInfo->error_number, exInfo->reason, __LINE__, __FILE__);

	if (collectedData.imHandle != NULL)
		DestroyImage(collectedData.imHandle);

	collectedData.imHandle = image;
}

//-------------------------------------------------------------------

void
image::destroyImageData(unsigned char **data)
{
	free(*data);
}

#endif

//-------------------------------------------------------------------

