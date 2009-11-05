/***************************************************************************
 *            graphicsImage.cc
 *
 *  Thu Nov 23 2007
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
#include <string.h>

#include "graphicsImage.inline"

#include <libdodo/graphicsImage.h>
#include <libdodo/types.h>
#include <libdodo/graphicsColor.h>
#include <libdodo/graphicsImageEx.h>
#include <libdodo/xexec.h>
#include <libdodo/ioChannel.h>

namespace dodo {
    namespace graphics {
        /**
         * @class __image_init__
         * @brief initializes image evironment
         */
        class __image_init__ {
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

using namespace dodo::graphics;

#ifndef GRAPHICS_WO_XEXEC
image::__collected_data__::__collected_data__(xexec *executor,
                                              short execObject) : xexec::__collected_data__(executor, execObject),
                                                                  handle(new __image__)
{
}
#else
image::__collected_data__::__collected_data__() : handle(new __image__)
{
}
#endif

//-------------------------------------------------------------------

image::__collected_data__::~__collected_data__()
{
    delete handle;
}

//-------------------------------------------------------------------

__image_init__::__image_init__()
{
    if (IsMagickInstantiated() == MagickFalse) {
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
    if (IsMagickInstantiated() == MagickTrue) {
#ifndef IMAGEMAGICK_PRE_63
        MagickCoreTerminus();
#else
        DestroyMagick();
#endif
    }
}

//-------------------------------------------------------------------

void
__image_init__::imWarningHandler(const ExceptionType,
                                 const char *,
                                 const char *)
{
}

//-------------------------------------------------------------------

void
__image_init__::imErrorHandler(const ExceptionType,
                               const char          *reason,
                               const char          *description)
{
    throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_IMERRORHANDLER, exception::ERRNO_LIBDODO, IMAGEEX_IMERROR, reason ? reason : __dodostring__, __LINE__, __FILE__, description ? description : __dodostring__);
}

//-------------------------------------------------------------------

const char *image::mappingStatements[] = {
    "RGB",
    "RGBA",
};

//-------------------------------------------------------------------

const int image::typeStatements[] = {
    TrueColorMatteType,
    GrayscaleMatteType,
};

//-------------------------------------------------------------------

const int image::pixelSizeStatements[] = {
    CharPixel,
    ShortPixel,
    IntegerPixel,
    LongPixel,
    FloatPixel,
    DoublePixel
};

//-------------------------------------------------------------------

const char *image::encoderStatements[] = {
    "PNG",
    "JPEG",
    "GIF",
    "BMP",
    "XPM",
    "ICO"
};

//-------------------------------------------------------------------

const int image::compressionStatements[] = {
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

image::image(image &i)
#ifndef GRAPHICS_WO_XEXEC
    : xexec(i),
      collectedData(this, OBJECT_GRAPHICSIMAGE)
#endif
{
}

//-------------------------------------------------------------------

image::image() : exInfo(new ExceptionInfo)
#ifndef GRAPHICS_WO_XEXEC
                 ,
                 collectedData(this, OBJECT_GRAPHICSIMAGE)
#endif
{
#ifndef IMAGEMAGICK_PRE_63
    collectedData.handle->imInfo = AcquireImageInfo();
#else
    collectedData.handle->imInfo = CloneImageInfo(NULL);
#endif
}

//-------------------------------------------------------------------

image::~image()
{
    if (collectedData.handle->im != NULL)
        DestroyImage(collectedData.handle->im);

    DestroyImageInfo(collectedData.handle->imInfo);
    DestroyExceptionInfo((ExceptionInfo *)exInfo);

    delete (ExceptionInfo *)exInfo;
}

//-------------------------------------------------------------------

void
image::read(const io::channel &img)
{
#ifndef GRAPHICS_WO_XEXEC
    performPreExec(OPERATION_READ);
#endif

    GetExceptionInfo((ExceptionInfo *)exInfo);
    GetImageInfo(collectedData.handle->imInfo);

    if (collectedData.handle->im != NULL)
        DestroyImage(collectedData.handle->im);

    dodoString data, dataPart;

    while (true) {
        dataPart = img.read();
        if (dataPart.size() == 0)
            break;

        data.append(dataPart);
    }
    dataPart.clear();

    collectedData.handle->im = BlobToImage(collectedData.handle->imInfo, data.data(), data.size(), (ExceptionInfo *)exInfo);
    if (collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_READ, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)exInfo)->error_number, ((ExceptionInfo *)exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)exInfo)->description ? ((ExceptionInfo *)exInfo)->description : __dodostring__);

    collectedData.handle->imInfo->compression = collectedData.handle->im->compression;
    collectedData.handle->imInfo->quality = collectedData.handle->im->quality;

    strcpy(collectedData.handle->imInfo->magick, collectedData.handle->im->magick);

#ifndef GRAPHICS_WO_XEXEC
    performPostExec(OPERATION_READ);
#endif
}

//-------------------------------------------------------------------

void
image::create(unsigned long   width,
              unsigned long   height,
              const __color__ &background,
              unsigned short  backgroundDepth)
{
#ifndef GRAPHICS_WO_XEXEC
    performPreExec(OPERATION_CREATE);
#endif

    GetExceptionInfo((ExceptionInfo *)exInfo);
    GetImageInfo(collectedData.handle->imInfo);

    if (collectedData.handle->im != NULL)
        DestroyImage(collectedData.handle->im);

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

    collectedData.handle->im = NewMagickImage(collectedData.handle->imInfo, width, height, &bg);
    if (collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_CREATE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)exInfo)->error_number, ((ExceptionInfo *)exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)exInfo)->description ? ((ExceptionInfo *)exInfo)->description : __dodostring__);

    collectedData.handle->imInfo->compression = collectedData.handle->im->compression;
    collectedData.handle->imInfo->quality = collectedData.handle->im->quality;

    strcpy(collectedData.handle->imInfo->magick, collectedData.handle->im->magick);

#ifndef GRAPHICS_WO_XEXEC
    performPostExec(OPERATION_CREATE);
#endif
}

//-------------------------------------------------------------------

void
image::setColorSpecification(short type)
{
    if (type < 0 || type >= COLOR_SPECIFICATION_ENUMSIZE)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_SETTYPE, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);

    if (collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_SETTYPE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

    if (SetImageType(collectedData.handle->im, (ImageType)typeStatements[type]) == MagickFalse)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_SETTYPE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_CANNOTSETCOLORSPECIFICATION, GRAPHICSIMAGEEX_CANNOTSETCOLORSPECIFICATION_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
image::setAlpha()
{
    if (collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_SETALPHA, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

    collectedData.handle->im->matte = MagickTrue;
}

//-------------------------------------------------------------------

void
image::removeAlpha()
{
    if (collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_REMOVEALPHA, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

    collectedData.handle->im->matte = MagickFalse;
}

//-------------------------------------------------------------------

void
image::setOpacity(unsigned short opacity)
{
    if (collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_SETOPACITY, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

    SetImageOpacity(collectedData.handle->im, opacity);
}

//-------------------------------------------------------------------

void
image::setBackgroundColor(__color__ background)
{
    if (collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_SETBACKGROUNDCOLOR, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

    collectedData.handle->im->background_color.red = background.red;
    collectedData.handle->im->background_color.green = background.green;
    collectedData.handle->im->background_color.blue = background.blue;
    collectedData.handle->im->background_color.opacity = background.opacity;

    SetImageBackgroundColor(collectedData.handle->im);
}

//-------------------------------------------------------------------

void
image::close()
{
    if (collectedData.handle->im != NULL) {
        DestroyImage(collectedData.handle->im);

        collectedData.handle->im = NULL;
    }
}

//-------------------------------------------------------------------

void
image::write(const io::channel &img,
             short encoder)
{
#ifndef GRAPHICS_WO_XEXEC
    performPreExec(OPERATION_WRITE);
#endif

    if (collectedData.handle->im == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, IMAGEEX_EMPTYIMAGE, GRAPHICSIMAGEEX_EMPTYIMAGE_STR, __LINE__, __FILE__);

    GetExceptionInfo((ExceptionInfo *)exInfo);

    if (encoder < 0 || encoder >= ENCODER_ENUMSIZE)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);

    strcpy(collectedData.handle->imInfo->magick, encoderStatements[encoder]);

    size_t size = 0;
    unsigned char *data = ImageToBlob(collectedData.handle->imInfo, collectedData.handle->im, (size_t *)&size, (ExceptionInfo *)exInfo);
    if (data == NULL)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_WRITE, exception::ERRNO_IMAGEMAGICK, ((ExceptionInfo *)exInfo)->error_number, ((ExceptionInfo *)exInfo)->reason, __LINE__, __FILE__, ((ExceptionInfo *)exInfo)->description ? ((ExceptionInfo *)exInfo)->description : __dodostring__);

    img.bs = size;
    img.write(dodoString((char *)data, size));

#ifndef GRAPHICS_WO_XEXEC
    performPostExec(OPERATION_WRITE);
#endif
}

//-------------------------------------------------------------------

void
image::setCompression(short type)
{
    if (type < 0 || type >= COMPRESSION_ENUMSIZE)
        throw exception::basic(exception::MODULE_GRAPHICSIMAGE, IMAGEEX_SETCOMPRESSION, exception::ERRNO_LIBDODO, IMAGEEX_BADINFO, GRAPHICSIMAGEEX_BADINFO_STR, __LINE__, __FILE__);

    collectedData.handle->imInfo->compression = (CompressionType)compressionStatements[type];
}

//-------------------------------------------------------------------

void
image::setQuality(short quality)
{
    collectedData.handle->imInfo->quality = quality;
}

//-------------------------------------------------------------------

short
image::compression()
{
    for (int i = 0; i < COMPRESSION_ENUMSIZE; ++i)
        if (collectedData.handle->imInfo->compression == compressionStatements[i])
            return i;

    return -1;
}

//-------------------------------------------------------------------

short
image::quality()
{
    return collectedData.handle->imInfo->quality;
}

//-------------------------------------------------------------------

short
image::encoder()
{
    for (int i = 0; i < ENCODER_ENUMSIZE; ++i)
        if (strcmp(collectedData.handle->imInfo->magick, encoderStatements[i]) == 0)
            return i;

    return -1;
}

//-------------------------------------------------------------------

image::__size__
image::imageSize()
{
    if (collectedData.handle->im == NULL)
        return __size__();

    __size__ info;

    info.height = collectedData.handle->im->rows;
    info.width = collectedData.handle->im->columns;

    return info;
}
#endif

//-------------------------------------------------------------------

