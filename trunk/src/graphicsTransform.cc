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

transform::transform(transform &a_transform)
{
}

//-------------------------------------------------------------------

transform::transform() : im(NULL)
{
}

//-------------------------------------------------------------------

transform::transform(image *im) : im(im)
{
}

//-------------------------------------------------------------------

transform::~transform()
{
}

//-------------------------------------------------------------------

void
transform::setImage(image *a_im)
{
	im = a_im;
}

//-------------------------------------------------------------------

void
transform::scale(unsigned long width,
				 unsigned long height)
{
	if (im == NULL || im->collectedData.handle->im == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, TRANSFORMEX_EMPTYIMAGE, GRAPHICSTRANSFORMEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	GetExceptionInfo((ExceptionInfo *)im->exInfo);

	Image *image = ScaleImage(im->collectedData.handle->im, width, height, (ExceptionInfo *)im->exInfo);

	if (image == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)im->exInfo)->error_number, ((ExceptionInfo *)im->exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)im->exInfo)->description?((ExceptionInfo *)im->exInfo)->description:__dodostring__);

	DestroyImage(im->collectedData.handle->im);

	im->collectedData.handle->im = image;
}

//-------------------------------------------------------------------

void
transform::scale(unsigned long size)
{
	if (im == NULL || im->collectedData.handle->im == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, TRANSFORMEX_EMPTYIMAGE, GRAPHICSTRANSFORMEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	float mult = (float)size / (float)((im->collectedData.handle->im->columns > im->collectedData.handle->im->rows) ? im->collectedData.handle->im->columns : im->collectedData.handle->im->rows);

	GetExceptionInfo((ExceptionInfo *)im->exInfo);

	Image *image = ScaleImage(im->collectedData.handle->im, (unsigned long)floor(im->collectedData.handle->im->columns * mult), (unsigned long)floor(im->collectedData.handle->im->rows * mult), (ExceptionInfo *)im->exInfo);

	if (image == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)im->exInfo)->error_number, ((ExceptionInfo *)im->exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)im->exInfo)->description?((ExceptionInfo *)im->exInfo)->description:__dodostring__);

	DestroyImage(im->collectedData.handle->im);

	im->collectedData.handle->im = image;
}

//-------------------------------------------------------------------

void
transform::rotate(double angle)
{
	if (im == NULL || im->collectedData.handle->im == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_ROTATE, exception::ERRNO_IMAGEMAGICK, TRANSFORMEX_EMPTYIMAGE, GRAPHICSTRANSFORMEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	GetExceptionInfo((ExceptionInfo *)im->exInfo);

	Image *image = RotateImage(im->collectedData.handle->im, angle, (ExceptionInfo *)im->exInfo);

	if (image == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_ROTATE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)im->exInfo)->error_number, ((ExceptionInfo *)im->exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)im->exInfo)->description?((ExceptionInfo *)im->exInfo)->description:__dodostring__);

	DestroyImage(im->collectedData.handle->im);

	im->collectedData.handle->im = image;
}
#endif

//-------------------------------------------------------------------

