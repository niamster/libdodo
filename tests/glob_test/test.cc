#include <tools.h>
#include <flushDisk.h>
#include <baseEx.h>
#include <flushDiskTools.h>
#include <flushSocket.h>

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
		cout << "MD5 of \"\"" << endl;
		string md5 = tools::MD5("");

		for (int i = 0; i < 16; i++)
 			printf ("%02x", (unsigned char)md5[i]);
 			
		cout << endl;


		tools tl;
		
		//tools::mail("64.46.38.133",PROTO_FAMILY_IPV4,25,"niam.niam@gmail.com","root@winerrorfixer.com","test","test","scanner","ae$%re!H@");
		
		baseEx::setErrorHandler(ERRMODULE_FLUSHDISK,&baseHandler,NULL);
		if(!baseEx::setErrorHandler(ERRMODULE_FLUSHDISK,"./module",NULL))
			cout << "WTF";
		
		cout << flushDiskTools::getFileContent("test.cc");
		
		//cout << "Что происходит?" << endl;
		
		for (register int i(0);i<10000;++i)
			tl.codesetConversion(flushDiskTools::getFileContent("test.cc"),"cp1251","utf-8");
		
		tl.codesetConversion(flushDiskTools::getFileContent("test.cc"),"cp1251","utf-8");
		for (register int i(0);i<9999;++i)
			tl.reCodesetConversion(flushDiskTools::getFileContent("test.cc"));
	
		cout << flushDiskTools::getFileContent("Makefile").size() << endl;
		cout << tl.zCompress(flushDiskTools::getFileContent("Makefile"),9).size() << endl;
	
		cout << tl.zDecompress(tl.zCompress(flushDiskTools::getFileContent("test.cc")));
		
		cout << tools::decodeURL(tools::encodeURL("@!()HEY, that's working!")) << endl;
		cout << tools::encodeURL("@!()HEY, that's working!") << endl;
		
		cout << "real size of test is : " << flushDiskTools::getSize("test") << endl;
		
		cout << tools::encodeASCII85("HEY, that's working!") << endl;
		cout << tools::decodeASCII85(tools::encodeASCII85("HEY, that's working!")) << endl;
		cout << "size of test ASCII85-encoded : " <<  tools::encodeASCII85(flushDiskTools::getFileContent("test")).size() << endl;
		
		cout << tools::encodeBase64("HEY, that's working!") << endl;
		cout << tools::decodeBase64(tools::encodeBase64("HEY, that's working!")) << endl;
		cout << tools::decodeBase64("PDQxMjg3NjA3ODEuMTMzODQ4NjJAd2luZXJyb3JmaXhlci5jb20+") << endl;
		cout << "size of test base64-encoded : " <<  tools::encodeBase64(flushDiskTools::getFileContent("test")).size() << endl;
		
		string tt = "ftp://hihi:hoho@niam.mu:32/init.cgi?gonivo=true&work=true";

		cout << tools::parseURL(tt).host << endl;
		cout << tools::parseURL(tt).login << endl;
		cout << tools::parseURL(tt).password << endl;
		cout << tools::parseURL(tt).path << endl;
		cout << tools::parseURL(tt).port << endl;
		cout << tools::parseURL(tt).protocol << endl;
		cout << tools::parseURL(tt).request << endl;

		cout << flushDiskTools::getFileContent("Makefile").size() << endl;
		cout << tl.bzCompress(flushDiskTools::getFileContent("Makefile"),9).size() << endl;
		for (int i(0);i<10000;++i)
			tl.bzDecompress(tl.bzCompress(flushDiskTools::getFileContent("Makefile"),9));
			
		std::string t = "abcddF";
		tools::replace("cd","WW",t);
		cout << t << endl;	
		
		t = "abcddF";
		dodoStringArr rep, nee;
		nee.push_back("cd");
		nee.push_back("ab");
		rep.push_back("WW");
		rep.push_back("EE");
		tools::replace(nee,rep,t);
		cout << t << endl;
				
	}
    catch(baseEx ex)
    {
    	cout << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    catch(std::exception ex)
    {
    	cout << endl << ex.what() << endl;
    }
    
	return 0;
}

