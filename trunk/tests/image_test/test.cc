/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/toolsMisc.h>
#include <libdodo/image.h>
#include <libdodo/ioFile.h>

#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

#ifdef IMAGEMAGICK_EXT

#ifndef IMAGE_WO_XEXEC

void
hook(void *odata,
	 short int type,
	 void *udata)
{
	__xexecImageCollectedData *imData = (__xexecImageCollectedData *)odata;

	if (imData->operType == IMAGE_OPERATION_WRITE)
	{
		try
		{
			image *img = (image *)imData->executor;
			img->disableAll();
			img->rotate(IMAGE_ROTATEDIRECTIONANGLE_180);
			img->enableAll();
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

#ifndef IMAGE_WO_XEXEC

		im.addPreExec(hook, NULL);

		///another one to revert
		im.addPostExec(hook, NULL);

#endif

		im.read("test.png");
		cout << im.getCompression() << " " << im.getEncoder() << " " << im.getQuality() << endl;

		im.scale(1000, 1000);

		im.write("test.jpg");

		unsigned char *img; unsigned int size;
		im.setEncoder(IMAGE_ENCODER_PNG);
		im.setCompression(IMAGE_COMPRESSION_ZIP);
		im.setQuality(4);
		im.write(&img, size);

		file io;
		io.open("my.png", FILE_FILETYPE_REG_FILE, FILE_OPENMODE_READ_WRITE_TRUNCATE);
		io.outSize = size;
		io.write((char *)img);
		im.destroyImageData(&img);

		cout << size << endl;
#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << endl << ex.baseErrstr << endl << ex.line << endl << ex.baseErrno << endl;
	}

	return 0;
}

