#include <libdodo/tools.h>
#include <libdodo/image.h>
#include <libdodo/ioDisk.h>

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
		im.rotate(IMAGE_ROTATEDIRECTIONANGLE_180);
		im.setEncoder(IMAGE_ENC_PNG);
		im.write(&img, size);
		
		ioDisk io;
		io.open("my.png", IODISK_FILETYPE_REG_FILE, IODISK_OPENMODE_READ_WRITE_TRUNCATE);
		io.outSize = size;
		io.write((char *)img);
		im.destroyImageData(&img);
		
		cout << size << endl;
	}
	catch(baseEx ex)
	{
    		cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    
	return 0;
}

