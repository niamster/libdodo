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

draw::draw(image *im) : im(im)
{
}

//-------------------------------------------------------------------

draw::~draw()
{
}

//-------------------------------------------------------------------

void
draw::setImage(image *a_im)
{
	im = a_im;
}


//-------------------------------------------------------------------

void
draw::primitive(char *description,
                const __color &fillColor,
                const __color &borderColor,
                double borderWidth)
{
	if (im == NULL || im->collectedData.imHandle == NULL)
		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_PRIMITIVE, exception::ERRNO_IMAGEMAGICK, DRAWEX_EMPTYIMAGE, GRAPHICSDRAWEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

	DrawInfo *di = AcquireDrawInfo();

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
		di->primitive =  NULL;
		DestroyDrawInfo(di);

		throw exception::basic(exception::ERRMODULE_GRAPHICSDRAW, DRAWEX_PRIMITIVE, exception::ERRNO_IMAGEMAGICK, DRAWEX_CANNOTDRAWPRIMITIVE, GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR, __LINE__, __FILE__);
	}

	di->primitive =  NULL;
	DestroyDrawInfo(di);

}

//-------------------------------------------------------------------

void
draw::circle(const point &center,
			unsigned long radius,
			const __color &fillColor,
			const __color &borderColor,
			double borderWidth)
{
	char description[128];
	snprintf(description, 128,"circle %d,%d %d,%d", center.x, center.y, center.x + radius, center.y);

	primitive(description, fillColor, borderColor, borderWidth);
}


//-------------------------------------------------------------------

void
draw::line(const dodoArray<point> &points,
           const __color &lineColor,
           double lineWidth)
{
	char pointDesc[128];

	dodoString description = "polyline";

	dodoArray<point>::const_iterator i = points.begin(), j = points.end();
	for (;i!=j;++i)
	{
		snprintf(pointDesc, 128," %d,%d", i->x, i->y);

		description.append(pointDesc);
	}

	primitive((char *)description.c_str(), color::transparent, lineColor, lineWidth);
}

//-------------------------------------------------------------------

void
draw::rectangle(const point &tl,
                const point &br,
				const __color &fillColor,
				const __color &borderColor,
				double borderWidth)
{
	char description[128];
	snprintf(description, 128,"rectangle %d,%d %d,%d", tl.x, tl.y, br.x, br.y);

	primitive(description, fillColor, borderColor, borderWidth);
}

#endif

//-------------------------------------------------------------------

