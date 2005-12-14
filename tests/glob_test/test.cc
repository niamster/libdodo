#include <tools.h>
#include <flushDisk.h>

using namespace dodo;
using namespace std;

void
baseHandler(errorModuleEnum module, baseEx *ex, void *data)
{
	cout << "baseHandler: " << ex->baseErrstr << endl;
}

int main(int argc, char **argv)
{	

	try
	{
		tools tl;
		
		baseEx::setErrorHandler(ERRMODULE_FLUSHDISK,&baseHandler,NULL);
		
		//cout << flushDisk::getFileContent("test.cc");
		
		//cout << "Что происходит?" << endl;
		
		/*for (register int i(0);i<10000;++i)
			tl.codesetConversion(flushDisk::getFileContent("test.cc"),"cp1251","utf-8");*/
		
		/*tl.codesetConversion(flushDisk::getFileContent("test.cc"),"cp1251","utf-8");
		for (register int i(0);i<9999;++i)
			tl.reCodesetConversion(flushDisk::getFileContent("test.cc"));*/
	
		cout << flushDisk::getFileContent("Nakefile").size() << endl;
		//cout << tl.zCompress(flushDisk::getFileContent("Makefile"),9).size() << endl;
	
		//cout << tl.zDecompress(tl.zCompress(flushDisk::getFileContent("test.cc")));
	}
    catch(baseEx ex)
    {
    }
    catch(std::exception ex)
    {
    	cout << endl << ex.what() << endl;
    }
	return 0;
}

