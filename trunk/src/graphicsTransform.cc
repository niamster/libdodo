/***************************************************************************
 *            graphicsTransform.cc
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

#include <libdodo/graphicsTransform.h>
#include <libdodo/graphicsImage.h>
#include <libdodo/graphicsTransformEx.h>

using namespace dodo::graphics;

//-------------------------------------------------------------------

void
transform::scale(image &image,
				 unsigned long width,
				 unsigned long height)
{
	if (image.collectedData.handle->im == NULL)
		throw exception::basic(exception::MODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, TRANSFORMEX_EMPTYIMAGE, GRAPHICSTRANSFORMEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	GetExceptionInfo((ExceptionInfo *)image.exInfo);

	Image *im = ScaleImage(image.collectedData.handle->im, width, height, (ExceptionInfo *)image.exInfo);
	if (im == NULL)
		throw exception::basic(exception::MODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)image.exInfo)->error_number, ((ExceptionInfo *)image.exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)image.exInfo)->description?((ExceptionInfo *)image.exInfo)->description:__dodostring__);

	DestroyImage(image.collectedData.handle->im);

	image.collectedData.handle->im = im;
}

//-------------------------------------------------------------------

void
transform::scale(image &image,
							  unsigned long size)
{
	if (image.collectedData.handle->im == NULL)
		throw exception::basic(exception::MODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, TRANSFORMEX_EMPTYIMAGE, GRAPHICSTRANSFORMEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	float mult = (float)size / (float)((image.collectedData.handle->im->columns > image.collectedData.handle->im->rows) ? image.collectedData.handle->im->columns : image.collectedData.handle->im->rows);

	GetExceptionInfo((ExceptionInfo *)image.exInfo);

	Image *im = ScaleImage(image.collectedData.handle->im, (unsigned long)floor(image.collectedData.handle->im->columns * mult), (unsigned long)floor(image.collectedData.handle->im->rows * mult), (ExceptionInfo *)image.exInfo);
	if (im == NULL)
		throw exception::basic(exception::MODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)image.exInfo)->error_number, ((ExceptionInfo *)image.exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)image.exInfo)->description?((ExceptionInfo *)image.exInfo)->description:__dodostring__);

	DestroyImage(image.collectedData.handle->im);

	image.collectedData.handle->im = im;
}

//-------------------------------------------------------------------

void
transform::rotate(image &image,
							  double angle)
{
	if (image.collectedData.handle->im == NULL)
		throw exception::basic(exception::MODULE_GRAPHICSTRANSFORM, TRANSFORMEX_ROTATE, exception::ERRNO_IMAGEMAGICK, TRANSFORMEX_EMPTYIMAGE, GRAPHICSTRANSFORMEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	GetExceptionInfo((ExceptionInfo *)image.exInfo);

	Image *im = RotateImage(image.collectedData.handle->im, angle, (ExceptionInfo *)image.exInfo);
	if (im == NULL)
		throw exception::basic(exception::MODULE_GRAPHICSTRANSFORM, TRANSFORMEX_ROTATE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)image.exInfo)->error_number, ((ExceptionInfo *)image.exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)image.exInfo)->description?((ExceptionInfo *)image.exInfo)->description:__dodostring__);

	DestroyImage(image.collectedData.handle->im);

	image.collectedData.handle->im = im;
}
#endif

//-------------------------------------------------------------------

