#include <libdodo/toolsMisc.h>
#include <libdodo/ioFile.h>
#include <libdodo/baseEx.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetwork.h>

#include <iostream>

using namespace dodo;
using namespace io;

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
		cout << misc::codesetConversion(misc::codesetConversion("тест", "utf-8", "cp1251"), "cp1251", "utf-8") << endl;
#endif
		
		cout << "MD5 of \"\"" << endl;
		cout << misc::MD5Hex("") <<  endl;
		
		baseEx::setErrorHandler(ERRMODULE_IOFILE,&baseHandler,NULL);
#ifdef DL_EXT
		if(!baseEx::setErrorHandler("./module",NULL))
			cout << "WTF";
#endif
		
#ifdef ICONV_EXT
		misc::codesetConversion(toolsFilesystem::getFileContents("test.cc"),"cp1251","utf-8");
#endif

#ifdef ZLIB_EXT
		cout << misc::zCompress(toolsFilesystem::getFileContents("Makefile"),9).size() << endl;
		cout << misc::zDecompress(misc::zCompress(toolsFilesystem::getFileContents("test.cc")));
#endif
		
		cout << misc::decodeUrl(misc::encodeUrl("@!()HEY, that's working!")) << endl;
		cout << misc::encodeUrl("@!()HEY, that's working!") << endl;
		
		cout << misc::encodeASCII85("HEY, that's working!") << endl;
		cout << misc::decodeASCII85(misc::encodeASCII85("HEY, that's working!")) << endl;
		cout << "size of test ASCII85-encoded : " <<  misc::encodeASCII85(toolsFilesystem::getFileContents("test")).size() << endl;
		
		cout << misc::encodeBase64("HEY, that's working!") << endl;
		cout << misc::decodeBase64(misc::encodeBase64("HEY, that's working!")) << endl;
		cout << "size of test base64-encoded : " <<  misc::encodeBase64(toolsFilesystem::getFileContents("test")).size() << endl;
		
		dodoString tt = "ftp://user:pass@localhost.domain:32/init.cgi?net=true&work=true";
		cout << endl << tt << endl;
		cout << misc::parseUrl(tt).host << endl;
		cout << misc::parseUrl(tt).login << endl;
		cout << misc::parseUrl(tt).password << endl;
		cout << misc::parseUrl(tt).path << endl;
		cout << misc::parseUrl(tt).port << endl;
		cout << misc::parseUrl(tt).protocol << endl;
		cout << misc::parseUrl(tt).request << endl;

		cout << misc::makeUrl(misc::parseUrl(tt)) << endl;

		cout << toolsFilesystem::getFileContents("Makefile").size() << endl;
#ifdef BZ2_EXT
		cout << misc::bzCompress(toolsFilesystem::getFileContents("Makefile"),9).size() << endl;
#endif
			
		dodoString t = "abcddF";
		toolsString::replace("cd","WW",t);
		cout << t << endl;	
		
		dodoString rnd = misc::stringRandom(12);
		cout << misc::MD5Hex(rnd) << endl;	

		//misc::mail("niam", "test", "test");
		//misc::mail("127.0.0.1", 25, "niam@niam.mu", "niam@niam.mu", "test", "test", "niam", "niam");
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

