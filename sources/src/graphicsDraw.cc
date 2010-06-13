/***************************************************************************
 *            graphicsDraw.cc
 *
 *  Thu Nov 23 2007
 *  Copyright  2007  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#include <libdodo/graphicsDraw.h>
#include <libdodo/types.h>
#include <libdodo/graphicsImage.h>
#include <libdodo/graphicsColor.h>
#include <libdodo/graphicsDrawEx.h>

using namespace dodo::graphics;

point::point(unsigned long x, unsigned long y) : x(x),
                                                 y(y)
{
}

//-------------------------------------------------------------------

void
draw::primitive(graphics::image &image,
                char            *description,
                const __color__ &fillColor,
                const __color__ &borderColor,
                unsigned short  borderWidth)
{
    if (image.collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSDRAW, DRAWEX_PRIMITIVE, exception::ERRNO_IMAGEMAGICK, DRAWEX_EMPTYIMAGE, GRAPHICSDRAWEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

#ifndef IMAGEMAGICK_PRE_63
    DrawInfo *di = AcquireDrawInfo();
#else
    DrawInfo *di = CloneDrawInfo(image.collectedData.handle->imInfo, NULL);
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

    if (DrawImage(image.collectedData.handle->im, di) == MagickFalse) {
        di->primitive = NULL;
        DestroyDrawInfo(di);

        throw exception::basic(exception::MODULE_GRAPHICSDRAW, DRAWEX_PRIMITIVE, exception::ERRNO_IMAGEMAGICK, DRAWEX_CANNOTDRAWPRIMITIVE, GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR, __LINE__, __FILE__);
    }

    di->primitive = NULL;
    DestroyDrawInfo(di);
}

//-------------------------------------------------------------------

void
draw::circle(graphics::image       &image,
             const graphics::point &center,
             unsigned long         radius,
             const __color__       &fillColor,
             const __color__       &borderColor,
             unsigned short        borderWidth)
{
    char description[128];
    snprintf(description, 128, "circle %ld,%ld %ld,%ld", center.x, center.y, center.x + radius, center.y);

    primitive(image, description, fillColor, borderColor, borderWidth);
}

//-------------------------------------------------------------------

void
draw::line(graphics::image                  &image,
           const dodoArray<graphics::point> &points,
           const __color__                  &lineColor,
           unsigned short                   lineWidth)
{
    char pointDesc[128];

    dodo::string description = "polyline";

    dodoArray<graphics::point>::const_iterator i = points.begin(), j = points.end();
    for (; i != j; ++i) {
        snprintf(pointDesc, 128, " %ld,%ld", i->x, i->y);

        description += dodo::string(pointDesc);
    }

    primitive(image, (char *)description.data(), color::transparent, lineColor, lineWidth);
}

//-------------------------------------------------------------------

void
draw::rectangle(graphics::image       &image,
                const graphics::point &tl,
                const graphics::point &br,
                const __color__       &fillColor,
                const __color__       &borderColor,
                unsigned short        borderWidth)
{
    char description[128];
    snprintf(description, 128, "rectangle %ld,%ld %ld,%ld", tl.x, tl.y, br.x, br.y);

    primitive(image, description, fillColor, borderColor, borderWidth);
}

//-------------------------------------------------------------------

void
draw::text(graphics::image       &image,
           const graphics::point &position,
           const dodo::string      &text,
           const dodo::string      &font,
           unsigned short        fontWidth,
           const __color__       &fillColor,
           const __color__       &borderColor,
           unsigned short        borderWidth,
           double                angle)
{
    if (image.collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSDRAW, DRAWEX_TEXT, exception::ERRNO_IMAGEMAGICK, DRAWEX_EMPTYIMAGE, GRAPHICSDRAWEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

    dodo::string txt = "text 0,0 \"";
    txt += dodo::string(text);
    txt += dodo::string("\"");

#ifndef IMAGEMAGICK_PRE_63
    DrawInfo *di = AcquireDrawInfo();
#else
    DrawInfo *di = CloneDrawInfo(image.collectedData.handle->imInfo, NULL);
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

    di->primitive = (char *)txt.data();

    di->font = (char *)font.data();
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

    if (DrawImage(image.collectedData.handle->im, di) == MagickFalse) {
        di->primitive = NULL;
        di->font = NULL;
        DestroyDrawInfo(di);

        throw exception::basic(exception::MODULE_GRAPHICSDRAW, DRAWEX_TEXT, exception::ERRNO_IMAGEMAGICK, DRAWEX_CANNOTDRAWPRIMITIVE, GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR, __LINE__, __FILE__);
    }

    di->primitive = NULL;
    di->font = NULL;
    DestroyDrawInfo(di);
}

//-------------------------------------------------------------------

void
draw::image(graphics::image       &image,
            const graphics::point &position,
            const graphics::image &a_im,
            double                angle)
{
    if (image.collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSDRAW, DRAWEX_IMAGE, exception::ERRNO_IMAGEMAGICK, DRAWEX_EMPTYIMAGE, GRAPHICSDRAWEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

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

    if (DrawAffineImage(image.collectedData.handle->im, a_im.collectedData.handle->im, &current) == MagickFalse)
        throw exception::basic(exception::MODULE_GRAPHICSDRAW, DRAWEX_IMAGE, exception::ERRNO_IMAGEMAGICK, DRAWEX_CANNOTDRAWPRIMITIVE, GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
draw::point(graphics::image       &image,
            const graphics::point &position,
            const __color__       &pointColor,
            unsigned short        pointWidth)
{
    if (image.collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSDRAW, DRAWEX_POINT, exception::ERRNO_IMAGEMAGICK, DRAWEX_EMPTYIMAGE, GRAPHICSDRAWEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

    char description[128];

    if (pointWidth == 1)
        snprintf(description, 128, "point %ld,%ld", position.x, position.y);
    else
        snprintf(description, 128, "circle %ld,%ld %ld,%ld", position.x, position.y, position.x + pointWidth, position.y);

#ifndef IMAGEMAGICK_PRE_63
    DrawInfo *di = AcquireDrawInfo();
#else
    DrawInfo *di = CloneDrawInfo(image.collectedData.handle->imInfo, NULL);
#endif

    di->primitive = description;

    di->fill.red = pointColor.red;
    di->fill.green = pointColor.green;
    di->fill.blue = pointColor.blue;
    di->fill.opacity = pointColor.opacity;

    if (DrawImage(image.collectedData.handle->im, di) == MagickFalse) {
        di->primitive = NULL;
        DestroyDrawInfo(di);

        throw exception::basic(exception::MODULE_GRAPHICSDRAW, DRAWEX_POINT, exception::ERRNO_IMAGEMAGICK, DRAWEX_CANNOTDRAWPRIMITIVE, GRAPHICSDRAWEX_CANNOTDRAWPRIMITIVE_STR, __LINE__, __FILE__);
    }

    di->primitive = NULL;
    DestroyDrawInfo(di);
}
#endif

//-------------------------------------------------------------------

