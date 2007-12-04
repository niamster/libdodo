#include <libdodo/tools.h>
#include <libdodo/image.h>

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
		im.setEncoder(IMAGE_ENC_RGB);
		im.write(img, size);
		
		im.write("test.bmp");
		
		cout << size << endl;
	}
	catch(baseEx ex)
	{
    		cout << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    
	return 0;
}

