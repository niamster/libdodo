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

#include <libdodo/graphicsTransform.h>

#ifdef IMAGEMAGICK_EXT

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
	if (im == NULL || im->collectedData.imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, TRANSFORMEX_EMPTYIMAGE, GRAPHICSTRANSFORMEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	GetExceptionInfo(&(im->exInfo));

	Image *image = ScaleImage(im->collectedData.imHandle, width, height, &(im->exInfo));

	if (image == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, im->exInfo.error_number, im->exInfo.reason, __LINE__, __FILE__, im->exInfo.description);
	}

	DestroyImage(im->collectedData.imHandle);

	im->collectedData.imHandle = image;
}

//-------------------------------------------------------------------

void
transform::scale(unsigned long size)
{
	if (im == NULL || im->collectedData.imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, TRANSFORMEX_EMPTYIMAGE, GRAPHICSTRANSFORMEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	float mult = (float)size / (float)((im->collectedData.imHandle->columns > im->collectedData.imHandle->rows) ? im->collectedData.imHandle->columns : im->collectedData.imHandle->rows);

	GetExceptionInfo(&(im->exInfo));

	Image *image = ScaleImage(im->collectedData.imHandle, (unsigned long)floor(im->collectedData.imHandle->columns * mult), (unsigned long)floor(im->collectedData.imHandle->rows * mult), &(im->exInfo));

	if (image == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_SCALE, exception::ERRNO_IMAGEMAGICK, im->exInfo.error_number, im->exInfo.reason, __LINE__, __FILE__, im->exInfo.description);
	}

	DestroyImage(im->collectedData.imHandle);

	im->collectedData.imHandle = image;
}

//-------------------------------------------------------------------

void
transform::rotate(double angle)
{
	if (im == NULL || im->collectedData.imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_ROTATE, exception::ERRNO_IMAGEMAGICK, TRANSFORMEX_EMPTYIMAGE, GRAPHICSTRANSFORMEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	GetExceptionInfo(&(im->exInfo));

	Image *image = RotateImage(im->collectedData.imHandle, angle, &(im->exInfo));

	if (image == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSTRANSFORM, TRANSFORMEX_ROTATE, exception::ERRNO_IMAGEMAGICK, im->exInfo.error_number, im->exInfo.reason, __LINE__, __FILE__, im->exInfo.description);
	}

	DestroyImage(im->collectedData.imHandle);

	im->collectedData.imHandle = image;
}

#endif

//-------------------------------------------------------------------

