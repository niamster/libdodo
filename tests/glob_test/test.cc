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
		//tools tl;
		
		/*baseEx::setErrorHandler(ERRMODULE_FLUSHDISK,&baseHandler,NULL);
		if(!baseEx::setErrorHandler(ERRMODULE_FLUSHDISK,"./module",NULL))
			cout << "WTF";*/
		
		//cout << flushDiskTools::getFileContent("test.cc");
		
		//cout << "Что происходит?" << endl;
		
		/*for (register int i(0);i<10000;++i)
			tl.codesetConversion(flushDiskTools::getFileContent("test.cc"),"cp1251","utf-8");*/
		
		/*tl.codesetConversion(flushDiskTools::getFileContent("test.cc"),"cp1251","utf-8");
		for (register int i(0);i<9999;++i)
			tl.reCodesetConversion(flushDiskTools::getFileContent("test.cc"));*/
	
		//cout << flushDiskTools::getFileContent("Makefile").size() << endl;
		//cout << tl.zCompress(flushDisk::getFileContent("Makefile"),9).size() << endl;
	
		//cout << tl.zDecompress(tl.zCompress(flushDiskTools::getFileContent("test.cc")));
		
		cout << tools::decodeURL(tools::encodeURL("@!()HEY, that's works!")) << endl;
		cout << tools::encodeURL("@!()HEY, that's works!") << endl;
		
		cout << tools::encodeASCII85("HEY, that's works!") << endl;
		cout << tools::decodeASCII85(tools::encodeASCII85("HEY, that's works!")) << endl;
		cout << "size of test.cc ASCII85-encoded : " <<  tools::encodeASCII85(flushDiskTools::getFileContent("test.cc")).size() << endl;
		
	
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

