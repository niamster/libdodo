#include <libdodo/tools.h>
#include <libdodo/image.h>
#include <libdodo/ioFile.h>

#include <iostream>

using namespace dodo;
using namespace std;

#ifdef IMAGEMAGICK_EXT

	void 
	hook(void *odata,
		short int type,
		void *udata)
	{
		__xexexImageCollectedData *imData = (__xexexImageCollectedData *)odata;
	
		if (imData->operType == IMAGE_OPERATION_WRITE)
		{
			try
			{
				image *img = (image *)imData->executor;
				img->disableAll();
				img->rotate(IMAGE_ROTATEDIRECTIONANGLE_180);
				img->enableAll();
			}
			catch(baseEx ex)
			{
    				cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
			}
		}
	}

#endif
	
int main(int argc, char **argv)
{	
	try
	{
#ifdef IMAGEMAGICK_EXT
		image im;
		im.addPreExec(hook, NULL);
		im.addPostExec(hook, NULL);///< revert

		im.read("test.png");
		cout << im.getCompression() << " " << im.getEncoder() << " " << im.getQuality() << endl;
	
		im.scale(1000, 1000);
		
		im.write("test.jpg");
		
		unsigned char *img; unsigned int size;
		im.setEncoder(IMAGE_ENCODER_PNG);
		im.setCompression(IMAGE_COMPRESSION_ZIP);
		im.setQuality(4);
		im.write(&img, size);
		
		ioFile io;
		io.open("my.png", IOFILE_FILETYPE_REG_FILE, IOFILE_OPENMODE_READ_WRITE_TRUNCATE);
		io.outSize = size;
		io.write((char *)img);
		im.destroyImageData(&img);
		
		cout << size << endl;
#endif
	}
	catch(baseEx ex)
	{
    		cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    	}
    
	return 0;
}

