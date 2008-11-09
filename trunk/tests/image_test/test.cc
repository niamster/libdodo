/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/toolsMisc.h>
#include <libdodo/graphicsImage.h>
#include <libdodo/graphicsTransform.h>
#include <libdodo/graphicsDraw.h>
#include <libdodo/ioFile.h>

#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

#ifdef IMAGEMAGICK_EXT

using namespace graphics;

#ifndef GRAPHICS_WO_XEXEC

void
hook(__xexecCollectedData *odata,
	 short int type,
	 void *udata)
{
	__xexecImageCollectedData *imData = (__xexecImageCollectedData *)odata;

	if (imData->operType == IMAGE_OPERATION_WRITE)
	{
		try
		{
			cout << "Rotating" << endl;

			image *img = dynamic_cast<image *>(imData->executor);
			graphics::transform tr(img);
			tr.rotate(TRANSFORM_ROTATEDIRECTIONANGLE_180);
		}
		catch (dodo::exception::basic ex)
		{
			cout << endl << ex.baseErrstr << endl << ex.line << endl << ex.baseErrno << endl;
		}
	}
}

#endif

#endif

int main(int argc, char **argv)
{
	try
	{
#ifdef IMAGEMAGICK_EXT
		image im;
		graphics::transform tr(&im);
		graphics::draw dr(&im);

#ifndef GRAPHICS_WO_XEXEC

		int prewrite = im.addPreExec(hook, NULL);

		///another one to revert
		int postwrite = im.addPostExec(hook, NULL);

#endif

		im.read("test.png");
		cout << im.getCompression() << " " << im.getEncoder() << " " << im.getQuality() << endl;

		tr.scale(1000, 1000);

		dr.circle(point(300, 300), 100, graphics::color::red, graphics::color::green, 5);
		dr.circle(point(300, 300), 50, graphics::color::blue, graphics::color::white, 5);

		im.write("test.jpg");

#ifndef GRAPHICS_WO_XEXEC

		im.delPreExec(prewrite);
		im.delPostExec(postwrite);

#endif

		unsigned char *img; unsigned int size;
		im.setEncoder(IMAGE_ENCODER_PNG);
		im.setCompression(IMAGE_COMPRESSION_ZIP);
		im.setQuality(4);
		im.setType(IMAGE_TYPE_GRAYSCALE);
		im.write(&img, size);

		file::regular io;
		io.open("my.png", file::REGULAR_OPENMODE_READ_WRITE_TRUNCATE);
		io.outSize = size;
		io.write((char *)img);
		im.destroyImageData(&img);

		cout << size << endl;

		using graphics::point;

		im.create(400, 400);
		dr.circle(point(200, 200), 100, graphics::color::red, graphics::color::green, 5);
		dr.circle(point(200, 200), 50, graphics::color::blue, graphics::color::white, 5);
		dr.rectangle(point(200, 200), point(300, 300), graphics::color::green, graphics::color::red, 15);
		im.write("new.png");

		im.read("new.png");
		im.removeAlpha();
		im.write("new-1.png");

		im.read("new-1.png");
		im.setBackgroundColor(graphics::color::transparent);
		dr.circle(point(200, 200), 100, graphics::color::red, graphics::color::green, 5);
		im.setType(IMAGE_TYPE_GRAYSCALE);
		dr.circle(point(200, 200), 50, graphics::color::blue, graphics::color::white, 5);
		im.setOpacity(65535/2);
		graphics::__color mygreen = graphics::color::green;
		mygreen.opacity = 65535/2;
		dr.circle(point(250, 250), 50, mygreen, graphics::color::white, 5);
		im.write("new-2.png");
		
		im.create(400, 400);

		dodoArray<point> points;
		for (int i=0;i<10;++i)
			points.push_back(point(i*20, 400 - pow(i, 2)*5));

		dr.circle(point(200, 200), 100, graphics::color::red, graphics::color::green, 5);
		dr.line(points, graphics::color::black, 1);
		im.write("new-3.png");
#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << endl << ex.baseErrstr << endl << ex.line << endl << ex.baseErrno << endl;
	}

	return 0;
}

