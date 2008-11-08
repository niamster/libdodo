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

		dr.circle(300, 300, 100, graphics::color::red, graphics::color::green, 5);
		dr.circle(300, 300, 50, graphics::color::blue, graphics::color::white, 5);

		im.write("test.jpg");

#ifndef GRAPHICS_WO_XEXEC

		im.delPreExec(prewrite);
		im.delPostExec(postwrite);

#endif

		unsigned char *img; unsigned int size;
		im.setEncoder(IMAGE_ENCODER_PNG);
		im.setCompression(IMAGE_COMPRESSION_ZIP);
		im.setQuality(4);
		im.write(&img, size);

		file::regular io;
		io.open("my.png", file::REGULAR_OPENMODE_READ_WRITE_TRUNCATE);
		io.outSize = size;
		io.write((char *)img);
		im.destroyImageData(&img);

		cout << size << endl;

		im.create(400, 400);
		dr.circle(200, 200, 100, graphics::color::red, graphics::color::green, 5);
		dr.circle(200, 200, 50, graphics::color::blue, graphics::color::white, 5);
		im.write("new.png");
#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << endl << ex.baseErrstr << endl << ex.line << endl << ex.baseErrno << endl;
	}

	return 0;
}

