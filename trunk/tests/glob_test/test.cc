#include <libdodo/tools.h>
#include <libdodo/ioDisk.h>
#include <libdodo/baseEx.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetwork.h>

#include <iostream>

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
		cout << "тест" << endl;
#ifdef ICONV_EXT
		cout << tools::codesetConversion(tools::codesetConversion("тест", "utf-8", "cp1251"), "cp1251", "utf-8") << endl;
#endif
		
		cout << "MD5 of \"\"" << endl;
		cout << tools::MD5Hex("") <<  endl;
		
		baseEx::setErrorHandler(ERRMODULE_IODISK,&baseHandler,NULL);
#ifdef DL_EXT
		if(!baseEx::setErrorHandler("./module",NULL))
			cout << "WTF";
#endif
		
#ifdef ICONV_EXT
		tools::codesetConversion(toolsFilesystem::getFileContents("test.cc"),"cp1251","utf-8");
#endif

#ifdef ZLIB_EXT
		cout << tools::zCompress(toolsFilesystem::getFileContents("Makefile"),9).size() << endl;
		cout << tools::zDecompress(tools::zCompress(toolsFilesystem::getFileContents("test.cc")));
#endif
		
		cout << tools::decodeUrl(tools::encodeUrl("@!()HEY, that's working!")) << endl;
		cout << tools::encodeUrl("@!()HEY, that's working!") << endl;
		
		cout << tools::encodeASCII85("HEY, that's working!") << endl;
		cout << tools::decodeASCII85(tools::encodeASCII85("HEY, that's working!")) << endl;
		cout << "size of test ASCII85-encoded : " <<  tools::encodeASCII85(toolsFilesystem::getFileContents("test")).size() << endl;
		
		cout << tools::encodeBase64("HEY, that's working!") << endl;
		cout << tools::decodeBase64(tools::encodeBase64("HEY, that's working!")) << endl;
		cout << "size of test base64-encoded : " <<  tools::encodeBase64(toolsFilesystem::getFileContents("test")).size() << endl;
		
		dodoString tt = "ftp://user:pass@localhost.domain:32/init.cgi?net=true&work=true";
		cout << endl << tt << endl;
		cout << tools::parseUrl(tt).host << endl;
		cout << tools::parseUrl(tt).login << endl;
		cout << tools::parseUrl(tt).password << endl;
		cout << tools::parseUrl(tt).path << endl;
		cout << tools::parseUrl(tt).port << endl;
		cout << tools::parseUrl(tt).protocol << endl;
		cout << tools::parseUrl(tt).request << endl;

		cout << tools::makeUrl(tools::parseUrl(tt)) << endl;

		cout << toolsFilesystem::getFileContents("Makefile").size() << endl;
#ifdef BZ2_EXT
		cout << tools::bzCompress(toolsFilesystem::getFileContents("Makefile"),9).size() << endl;
#endif
			
		dodoString t = "abcddF";
		toolsString::replace("cd","WW",t);
		cout << t << endl;	
		
		dodoString rnd = tools::stringRandom(12);
		cout << tools::MD5Hex(rnd) << endl;	

		//tools::mail("niam", "test", "test");
		//tools::mail("127.0.0.1", 25, "niam@niam.mu", "niam@niam.mu", "test", "test", "niam", "niam");
	}
    catch(baseEx ex)
    {
    	cout << endl << ex.baseErrno << "!!" << ex.baseErrstr  << ex.line << "!!" << "!!" << ex.file << endl;
    }
    catch(exception ex)
    {
    	cout << endl << ex.what() << endl;
    }
    
	return 0;
}

