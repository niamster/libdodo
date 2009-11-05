/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>

#include <math.h>
#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

#ifdef IMAGEMAGICK_EXT
using namespace graphics;

#ifndef GRAPHICS_WO_XEXEC
void
hook(xexec::__collected_data__ *odata,
     short                     type,
     short                     operation,
     void                      *udata)
{
    image::__collected_data__ *imData = (image::__collected_data__ *)odata;

    if (operation == image::OPERATION_WRITE) {
        try {
            cout << "Rotating" << endl;

            image *img = dynamic_cast<image *>(imData->executor);
            transform::rotate(*img, transform::ROTATE_DIRECTION_ANGLE_180);
        } catch (dodo::exception::basic &ex)   {
            cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
        }
    }
}
#endif
#endif

int
main(int  argc,
     char **argv)
{
    try {
#ifdef IMAGEMAGICK_EXT
        image im;

#ifndef GRAPHICS_WO_XEXEC
        int prewrite = im.addXExec(xexec::ACTION_PREEXEC, ::hook, NULL);
        int postwrite = im.addXExec(xexec::ACTION_POSTEXEC, ::hook, NULL); ///another one to revert
#endif

        im.read(file::regular("test.png", file::regular::OPEN_MODE_READ_ONLY));
        cout << im.compression() << " " << im.encoder() << " " << im.quality() << endl;

        transform::scale(im, 1000, 1000);

        draw::circle(im, point(300, 300), 100, color::red, color::green, 5);
        draw::circle(im, point(300, 300), 50, color::blue, color::white, 5);

        im.write(file::regular("test-0.jpg", file::regular::OPEN_MODE_READ_WRITE_TRUNCATE));

#ifndef GRAPHICS_WO_XEXEC
        im.removeXExec(prewrite);
        im.removeXExec(postwrite);
#endif

        dodoString img;
        im.setEncoder(image::ENCODER_PNG);
        im.setCompression(image::COMPRESSION_ZIP);
        im.setQuality(4);
        im.setColorSpecification(image::COLOR_SPECIFICATION_GRAYSCALE);
        im.write(file::regular("test-1.jpg", file::regular::OPEN_MODE_READ_WRITE_TRUNCATE));

        im.create(400, 400);
        draw::circle(im, point(200, 200), 100, color::red, color::green, 5);
        draw::circle(im, point(200, 200), 50, color::blue, color::white, 5);
        draw::rectangle(im, point(200, 200), point(300, 300), color::green, color::red, 15);
        im.write(file::regular("new-0.png", file::regular::OPEN_MODE_READ_WRITE_TRUNCATE));

        im.read(file::regular("new-0.png", file::regular::OPEN_MODE_READ_ONLY));
        im.removeAlpha();
        im.write(file::regular("new-1.png", file::regular::OPEN_MODE_READ_WRITE_TRUNCATE));

        im.read(file::regular("new-1.png", file::regular::OPEN_MODE_READ_ONLY));
        im.setBackgroundColor(color::transparent);
        draw::circle(im, point(200, 200), 100, color::red, color::green, 5);
        im.setColorSpecification(image::COLOR_SPECIFICATION_GRAYSCALE);
        draw::circle(im, point(200, 200), 50, color::blue, color::white, 5);
        im.setOpacity(65535 / 2);
        __color__ green = color::green;
        green.opacity = 65535 / 2;
        draw::circle(im, point(250, 250), 50, green, color::white, 5);
        im.write(file::regular("new-2.png", file::regular::OPEN_MODE_READ_WRITE_TRUNCATE));

        im.create(400, 400);
        draw::circle(im, point(200, 200), 100, color::red, color::green, 5);
        dodoArray<point> points;
        for (int i = 0; i < 10; ++i)
            points.push_back(point(i * 20, (unsigned long)(390 - pow(i, 2) * 5)));
        draw::line(im, points, color::black, 1);
        for (int i = 0; i < 10; ++i)
            draw::point(im, point(i * 20 + 100, (unsigned long)(390 - pow(i, 2) * 5)), color::blue, 5);
        for (int i = 0; i < 360; ++i)
            draw::point(im, point((unsigned long)(cos(i) * 100 + 150), (unsigned long)(200 - sin(i) * 100)), color::black);
        im.write(file::regular("new-3.png", file::regular::OPEN_MODE_READ_WRITE_TRUNCATE));

        im.create(400, 400);
        draw::circle(im, point(200, 200), 100, color::red, color::green, 5);
        draw::text(im, point(100, 200), "libdodo", "Arial", 70, color::blue, color::green);
        draw::text(im, point(100, 100), "libdodo", "Arial", 30);
        draw::text(im, point(150, 150), "libdodo", "Arial", 50, color::blue, color::green, 2, 180);
        draw::text(im, point(150, 200), "libdodo", "Arial", 50, color::blue, color::green, 2, 90);
        im.write(file::regular("new-4.png", file::regular::OPEN_MODE_READ_WRITE_TRUNCATE));

        image wm;
        wm.read(file::regular("new-4.png", file::regular::OPEN_MODE_READ_ONLY));

        im.create(600, 600);
        draw::image(im, point(100, 100), wm, 45);
        im.setOpacity(65535 / 2);
        im.write(file::regular("new-5.png", file::regular::OPEN_MODE_READ_WRITE_TRUNCATE));
#endif
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}

