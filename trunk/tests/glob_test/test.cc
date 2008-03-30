#include <libdodo/tools.h>
#include <libdodo/ioDisk.h>
#include <libdodo/baseEx.h>
#include <libdodo/ioDiskTools.h>
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
		dodoString md5 = tools::MD5("");

		for (int i = 0; i < 16; i++)
 			printf ("%02x", (unsigned char)md5[i]);
		cout << endl;
		
		baseEx::setErrorHandler(ERRMODULE_IODISK,&baseHandler,NULL);
#ifdef DL_EXT
		if(!baseEx::setErrorHandler("./module",NULL))
			cout << "WTF";
#endif
		
#ifdef ICONV_EXT
		tools::codesetConversion(ioDiskTools::getFileContents("test.cc"),"cp1251","utf-8");
#endif

#ifdef ZLIB_EXT
		cout << tools::zCompress(ioDiskTools::getFileContents("Makefile"),9).size() << endl;
		cout << tools::zDecompress(tools::zCompress(ioDiskTools::getFileContents("test.cc")));
#endif
		
		cout << tools::decodeURL(tools::encodeURL("@!()HEY, that's working!")) << endl;
		cout << tools::encodeURL("@!()HEY, that's working!") << endl;
		
		cout << tools::encodeASCII85("HEY, that's working!") << endl;
		cout << tools::decodeASCII85(tools::encodeASCII85("HEY, that's working!")) << endl;
		cout << "size of test ASCII85-encoded : " <<  tools::encodeASCII85(ioDiskTools::getFileContents("test")).size() << endl;
		
		cout << tools::encodeBase64("HEY, that's working!") << endl;
		cout << tools::decodeBase64(tools::encodeBase64("HEY, that's working!")) << endl;
		cout << "size of test base64-encoded : " <<  tools::encodeBase64(ioDiskTools::getFileContents("test")).size() << endl;
		
		dodoString tt = "ftp://user:pass@localhost.domain:32/init.cgi?net=true&work=true";
		cout << endl << tt << endl;
		cout << tools::parseURL(tt).host << endl;
		cout << tools::parseURL(tt).login << endl;
		cout << tools::parseURL(tt).password << endl;
		cout << tools::parseURL(tt).path << endl;
		cout << tools::parseURL(tt).port << endl;
		cout << tools::parseURL(tt).protocol << endl;
		cout << tools::parseURL(tt).request << endl;

		cout << ioDiskTools::getFileContents("Makefile").size() << endl;
#ifdef BZ2_EXT
		cout << tools::bzCompress(ioDiskTools::getFileContents("Makefile"),9).size() << endl;
#endif
			
		dodoString t = "abcddF";
		stringTools::replace("cd","WW",t);
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
    catch(std::exception ex)
    {
    	cout << endl << ex.what() << endl;
    }
    
	return 0;
}

