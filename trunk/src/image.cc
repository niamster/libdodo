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

#include <libdodo/image.h>

#ifdef IMAGEMAGICK_EXT

using namespace dodo;

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

__image_init__ __image_init_object__;

//-------------------------------------------------------------------

const __statements image::mappingStArr[] = 
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

const __statements image::encoderStArr[] = 
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

image::image(image &im)
{
	
}

//-------------------------------------------------------------------

image::image() : im(NULL)
{
	imInfo = AcquireImageInfo();
	exInfo = AcquireExceptionInfo();
}

//-------------------------------------------------------------------

image::~image()
{
	if (im != NULL)
		DestroyImage(im);
	DestroyImageInfo(imInfo);
	DestroyExceptionInfo(exInfo);
}

//-------------------------------------------------------------------

void
image::read(const dodoString &str)
{	
	strcpy(imInfo->filename, str.c_str());
	
	im = ReadImage(imInfo, exInfo);
	if (im == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_IMAGEMAGICK, exInfo->error_number, GetExceptionMessage(exInfo->error_number), __LINE__, __FILE__);
	
	imInfo->compression = im->compression;
	imInfo->quality = im->quality;
	strcpy(imInfo->magick, im->magick);
}

//-------------------------------------------------------------------

void 
image::read(const unsigned char * const data, 
		unsigned long size)
{
	im = BlobToImage(imInfo, data, size, exInfo);
	if (im == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_IMAGEMAGICK, exInfo->error_number, GetExceptionMessage(exInfo->error_number), __LINE__, __FILE__);	

	imInfo->compression = im->compression;
	imInfo->quality = im->quality;
	strcpy(imInfo->magick, im->magick);
}

//-------------------------------------------------------------------

void
image::read(const __imageInfo &info)
{
	if (info.mapping < 0 || info.mapping >= sizeof(mappingStArr)/sizeof(__statements) || info.pixelSize < 0 || info.pixelSize >= sizeof(pixelSizeStArr)/sizeof(StorageType))
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_LIBDODO, IMAGEEX_BADINFO, IMAGEEX_BADINFO_STR, __LINE__, __FILE__);
	
	im = ConstituteImage(info.width, info.height, mappingStArr[info.mapping].str, pixelSizeStArr[info.pixelSize], info.data, exInfo);
	if (im == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_IMAGEMAGICK, exInfo->error_number, GetExceptionMessage(exInfo->error_number), __LINE__, __FILE__);

	imInfo->compression = im->compression;
	imInfo->quality = im->quality;
	strcpy(imInfo->magick, im->magick);
}

//-------------------------------------------------------------------

void
image::write(const dodoString &str)
{	
	strcpy(im->filename, str.c_str());
	
	if (WriteImage(imInfo, im) == MagickFalse)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_IMAGEMAGICK, im->exception.error_number, GetExceptionMessage(im->exception.error_number), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
image::write(unsigned char **data, 
			unsigned int &size)
{	
	size = 0;
	*data = ImageToBlob(imInfo, im, &size, exInfo);
	if (data == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_READ, ERR_IMAGEMAGICK, exInfo->error_number, GetExceptionMessage(exInfo->error_number), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void 
image::setCompression(short type)
{
	if (type < 0 || type >= sizeof(compressionStArr)/sizeof(CompressionType))
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_SETENCODER, ERR_LIBDODO, IMAGEEX_BADINFO, IMAGEEX_BADINFO_STR, __LINE__, __FILE__);
	
	imInfo->compression = compressionStArr[type];
}

//-------------------------------------------------------------------

void 
image::setQuality(short quality)
{
	imInfo->quality = quality;
}

//-------------------------------------------------------------------

void 
image::setEncoder(short encoder)
{
	if (encoder < 0 || encoder >= sizeof(encoderStArr)/sizeof(__statements))
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_SETENCODER, ERR_LIBDODO, IMAGEEX_BADINFO, IMAGEEX_BADINFO_STR, __LINE__, __FILE__);
	
	strcpy(imInfo->magick, encoderStArr[encoder].str);
}

//-------------------------------------------------------------------

short 
image::getCompression()
{	
	int i = 0, j = sizeof(compressionStArr)/sizeof(CompressionType);
	for (;i<j;++i)
		if (imInfo->compression == compressionStArr[i])
			return i;
}

//-------------------------------------------------------------------

short 
image::getQuality()
{
	return imInfo->quality;
}

//-------------------------------------------------------------------

short 
image::getEncoder()
{
	int i = 0, j = sizeof(encoderStArr)/sizeof(__statements);
	for (;i<j;++i)
		if (strcmp(imInfo->magick, encoderStArr[i].str) == 0)
			return i;
}

//-------------------------------------------------------------------

void 
image::scale(unsigned long width, 
		unsigned long height)
{
	Image *image = ScaleImage(im, width, height, exInfo);
	
	if (image == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_SCALE, ERR_IMAGEMAGICK, exInfo->error_number, GetExceptionMessage(exInfo->error_number), __LINE__, __FILE__);
	
	if (im != NULL)
		DestroyImage(im);
	im = image;
}
//-------------------------------------------------------------------

void 
image::rotate(double angle)
{	
	Image *image = RotateImage(im, angle, exInfo);
	
	if (image == NULL)
		throw baseEx(ERRMODULE_IMAGE, IMAGEEX_ROTATE, ERR_IMAGEMAGICK, exInfo->error_number, GetExceptionMessage(exInfo->error_number), __LINE__, __FILE__);
	
	if (im != NULL)
		DestroyImage(im);
	im = image;
}

//-------------------------------------------------------------------

void 
image::destroyImageData(unsigned char **data)
{
	free(*data);
}

//-------------------------------------------------------------------

#endif
