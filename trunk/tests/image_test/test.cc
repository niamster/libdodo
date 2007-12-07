#include <libdodo/tools.h>
#include <libdodo/image.h>
#include <libdodo/flushDisk.h>

#include <iostream>

using namespace dodo;
using namespace std;

int main(int argc, char **argv)
{	
	try
	{
		image im;

		im.read("test.png");
		im.scale(1000, 1000);
		
		im.write("test.jpg");
		
		unsigned char *img; unsigned int size;
		im.rotate(IMAGE_RDA_180);
		im.setEncoder(IMAGE_ENC_PNG);
		im.write(&img, size);
		
		flushDisk flush;
		flush.open("my.png", FLUSHDISK_FILETYPE_REG_FILE, FLUSHDISK_OPENMODE_READ_WRITE_TRUNCATE);
		flush.outSize = size;
		flush.write((char *)img);
		im.destroyImageData(&img);
		
		cout << size << endl;
	}
	catch(baseEx ex)
	{
    		cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    
	return 0;
}

