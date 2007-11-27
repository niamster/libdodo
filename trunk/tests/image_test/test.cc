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
		im.write("test.jpg");
	}
	catch(baseEx ex)
	{
    		cout << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    
	return 0;
}

