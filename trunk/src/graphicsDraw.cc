/***************************************************************************
 *            graphicsDraw.cc
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

#include <libdodo/graphicsDraw.h>

#ifdef IMAGEMAGICK_EXT

using namespace dodo::graphics;

point::point(unsigned long x, unsigned long y) : x(x),
												 y(y)
{
}

//-------------------------------------------------------------------

draw::draw(draw &a_draw)
{
}

//-------------------------------------------------------------------

draw::draw() : im(NULL)
{
}

//-------------------------------------------------------------------

draw::draw(graphics::image *im) : im(im)
{
}

//-------------------------------------------------------------------

draw::~draw()
{
}

//-------------------------------------------------------------------

void draw::setImage(graphics::image *a_im)
{
	im = a_im;
}


//-------------------------------------------------------------------

void draw::primitive(char           *description,
					 const __color  &fillColor,
					 const __color  &borderColor,
					 unsigned short borderWidth)
{
	if (im == NULL || im->collectedData.imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_PRIMITIVE, exception::ERRNO_IMAGEMAGICK, DRAWEX_EMPTYIMAGE, GRAPHICSDRAWEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

#ifndef IMAGEMAGICK_PRE_63

	DrawInfo *di = AcquireDrawInfo();

#else

	DrawInfo *di = CloneDrawInfo(im->collectedData.imInfo, NULL);

#endif

	di->primitive = description;

	di->stroke.red = borderColor.red;
	di->stroke.green = borderColor.green;
	di->stroke.blue = borderColor.blue;
	di->stroke.opacity = borderColor.opacity;

	di->stroke_width = borderWidth;

	di->fill.red = fillColor.red;
	di->fill.green = fillColor.green;
	di->fill.blue = fillColor.blue;
	di->fill.opacity = fillColor.opacity;

	if (DrawImage(im->collectedData.imHandle, di) == MagickFalse)
	{
		di->primitive = NULL;
		DestroyDrawInfo(di);

		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_PRIMITIVE, exception::ERRNO_IMAGEMAGICK, DRAWEX_CANNOTDRAWPRIMITIVE, GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR, __LINE__, __FILE__);
	}

	di->primitive = NULL;
	DestroyDrawInfo(di);
}

//-------------------------------------------------------------------

void draw::circle(const graphics::point &center,
				  unsigned long         radius,
				  const __color         &fillColor,
				  const __color         &borderColor,
				  unsigned short        borderWidth)
{
	char description[128];
	snprintf(description, 128, "circle %d,%d %d,%d", center.x, center.y, center.x + radius, center.y);

	primitive(description, fillColor, borderColor, borderWidth);
}


//-------------------------------------------------------------------

void draw::line(const dodoArray<graphics::point> &points,
				const __color                    &lineColor,
				unsigned short                   lineWidth)
{
	char pointDesc[128];

	dodoString description = "polyline";

	dodoArray<graphics::point>::const_iterator i = points.begin(), j = points.end();
	for (; i != j; ++i)
	{
		snprintf(pointDesc, 128, " %d,%d", i->x, i->y);

		description.append(pointDesc);
	}

	primitive((char *)description.c_str(), color::transparent, lineColor, lineWidth);
}

//-------------------------------------------------------------------

void draw::rectangle(const graphics::point &tl,
					 const graphics::point &br,
					 const __color         &fillColor,
					 const __color         &borderColor,
					 unsigned short        borderWidth)
{
	char description[128];
	snprintf(description, 128, "rectangle %d,%d %d,%d", tl.x, tl.y, br.x, br.y);

	primitive(description, fillColor, borderColor, borderWidth);
}

//-------------------------------------------------------------------

void draw::text(const graphics::point &position,
				const dodoString      &text,
				const dodoString      &font,
				unsigned short        fontWidth,
				const __color         &fillColor,
				const __color         &borderColor,
				unsigned short        borderWidth,
				double                angle)
{
	if (im == NULL || im->collectedData.imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_TEXT, exception::ERRNO_IMAGEMAGICK, DRAWEX_EMPTYIMAGE, GRAPHICSDRAWEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	dodoString txt = "text 0,0 \"";
	txt.append(text);
	txt.append("\"");

#ifndef IMAGEMAGICK_PRE_63

	DrawInfo *di = AcquireDrawInfo();

#else

	DrawInfo *di = CloneDrawInfo(im->collectedData.imInfo, NULL);

#endif

	double radians = angle * M_PI / 180;
	double _cos = cos(radians);
	double _sin = sin(radians);

	AffineMatrix affine;

	affine.sx = _cos;
	affine.rx = _sin;
	affine.ry = -1 * _sin;
	affine.sy = _cos;
	affine.tx = position.x;
	affine.ty = position.y;

	AffineMatrix current = di->affine;

	di->affine.sx = current.sx * affine.sx + current.ry * affine.rx;
	di->affine.rx = current.rx * affine.sx + current.sy * affine.rx;

	di->affine.ry = current.sx * affine.ry + current.ry * affine.sy;
	di->affine.sy = current.rx * affine.ry + current.sy * affine.sy;

	di->affine.tx = current.sx * affine.tx + current.ry * affine.ty + current.tx;
	di->affine.ty = current.rx * affine.tx + current.sy * affine.ty + current.ty;

	di->primitive = (char *)txt.c_str();

	di->font = (char *)font.c_str();
	di->pointsize = fontWidth;

	di->stroke.red = borderColor.red;
	di->stroke.green = borderColor.green;
	di->stroke.blue = borderColor.blue;
	di->stroke.opacity = borderColor.opacity;

	di->stroke_width = borderWidth;

	di->fill.red = fillColor.red;
	di->fill.green = fillColor.green;
	di->fill.blue = fillColor.blue;
	di->fill.opacity = fillColor.opacity;

	if (DrawImage(im->collectedData.imHandle, di) == MagickFalse)
	{
		di->primitive = NULL;
		di->font = NULL;
		DestroyDrawInfo(di);

		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_TEXT, exception::ERRNO_IMAGEMAGICK, DRAWEX_CANNOTDRAWPRIMITIVE, GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR, __LINE__, __FILE__);
	}

	di->primitive = NULL;
	di->font = NULL;
	DestroyDrawInfo(di);
}

//-------------------------------------------------------------------

void draw::image(const graphics::point &position,
				 const graphics::image &a_im,
				 double                angle)
{
	if (im == NULL || im->collectedData.imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_IMAGE, exception::ERRNO_IMAGEMAGICK, DRAWEX_EMPTYIMAGE, GRAPHICSDRAWEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	AffineMatrix current;
	GetAffineMatrix(&current);

	double radians = angle * M_PI / 180;
	double _cos = cos(radians);
	double _sin = sin(radians);

	AffineMatrix affine;

	affine.sx = _cos;
	affine.rx = _sin;
	affine.ry = -1 * _sin;
	affine.sy = _cos;
	affine.tx = position.x;
	affine.ty = position.y;

	AffineMatrix _current = current;

	current.sx = _current.sx * affine.sx + _current.ry * affine.rx;
	current.rx = _current.rx * affine.sx + _current.sy * affine.rx;

	current.ry = _current.sx * affine.ry + _current.ry * affine.sy;
	current.sy = _current.rx * affine.ry + _current.sy * affine.sy;

	current.tx = _current.sx * affine.tx + _current.ry * affine.ty + _current.tx;
	current.ty = _current.rx * affine.tx + _current.sy * affine.ty + _current.ty;

	if (DrawAffineImage(im->collectedData.imHandle, a_im.collectedData.imHandle, &current) == MagickFalse)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_IMAGE, exception::ERRNO_IMAGEMAGICK, DRAWEX_CANNOTDRAWPRIMITIVE, GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void draw::point(const graphics::point &position,
				 const __color         &pointColor,
				 unsigned short        pointWidth)
{
	if (im == NULL || im->collectedData.imHandle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_POINT, exception::ERRNO_IMAGEMAGICK, DRAWEX_EMPTYIMAGE, GRAPHICSDRAWEX_EMPTYIMAGE_STR, __LINE__, __FILE__);
	}

	char description[128];

	if (pointWidth == 1)
	{
		snprintf(description, 128, "point %d,%d", position.x, position.y);
	}
	else
	{
		snprintf(description, 128, "circle %d,%d %d,%d", position.x, position.y, position.x + pointWidth, position.y);
	}

#ifndef IMAGEMAGICK_PRE_63

	DrawInfo *di = AcquireDrawInfo();

#else

	DrawInfo *di = CloneDrawInfo(im->collectedData.imInfo, NULL);

#endif

	di->primitive = description;

	di->fill.red = pointColor.red;
	di->fill.green = pointColor.green;
	di->fill.blue = pointColor.blue;
	di->fill.opacity = pointColor.opacity;

	if (DrawImage(im->collectedData.imHandle, di) == MagickFalse)
	{
		di->primitive = NULL;
		DestroyDrawInfo(di);

		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_POINT, exception::ERRNO_IMAGEMAGICK, DRAWEX_CANNOTDRAWPRIMITIVE, GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR, __LINE__, __FILE__);
	}

	di->primitive = NULL;
	DestroyDrawInfo(di);
}

#endif

//-------------------------------------------------------------------

