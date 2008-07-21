/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

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
		cout << tools::misc::codesetConversion(tools::misc::codesetConversion("тест", "utf-8", "cp1251"), "cp1251", "utf-8") << endl;
#endif

		cout << "MD5 of \"\"" << endl;
		cout << tools::misc::MD5Hex("") <<  endl;

		baseEx::setErrorHandler(ERRMODULE_IOFILE, &baseHandler, NULL);
#ifdef DL_EXT
		if (!baseEx::setErrorHandler("./module", NULL))
			cout << "WTF";
#endif

		try
		{
			tools::filesystem::unlink("some.file", false);
		}
		catch(...)
		{
		}
		
		try
		{
			tools::filesystem::unlink("some.file", false);
		}
		catch(...)
		{
		}

#ifdef ICONV_EXT
		tools::misc::codesetConversion(tools::filesystem::getFileContents("test.cc"), "cp1251", "utf-8");
#endif

#ifdef ZLIB_EXT
		cout << tools::misc::zCompress(tools::filesystem::getFileContents("Makefile"), 9).size() << endl;
		cout << tools::misc::zDecompress(tools::misc::zCompress(tools::filesystem::getFileContents("test.cc")));
#endif

		cout << tools::misc::decodeUrl(tools::misc::encodeUrl("@!()HEY, that's working!")) << endl;
		cout << tools::misc::encodeUrl("@!()HEY, that's working!") << endl;

		cout << tools::misc::encodeASCII85("HEY, that's working!") << endl;
		cout << tools::misc::decodeASCII85(tools::misc::encodeASCII85("HEY, that's working!")) << endl;
		cout << "size of test ASCII85-encoded : " <<  tools::misc::encodeASCII85(tools::filesystem::getFileContents("test")).size() << endl;

		cout << tools::misc::encodeBase64("HEY, that's working!") << endl;
		cout << tools::misc::decodeBase64(tools::misc::encodeBase64("HEY, that's working!")) << endl;
		cout << "size of test base64-encoded : " <<  tools::misc::encodeBase64(tools::filesystem::getFileContents("test")).size() << endl;

		dodoString tt = "ftp://user:pass@localhost.domain:32/path/init.cgi?net=true&work=true";
		cout << endl << tt << endl;
		cout << tools::misc::parseUrl(tt).host << endl;
		cout << tools::misc::parseUrl(tt).login << endl;
		cout << tools::misc::parseUrl(tt).password << endl;
		cout << tools::misc::parseUrl(tt).path << endl;
		cout << tools::misc::parseUrl(tt).port << endl;
		cout << tools::misc::parseUrl(tt).protocol << endl;
		cout << tools::misc::parseUrl(tt).request << endl;

		cout << tools::misc::makeUrl(tools::misc::parseUrl(tt)) << endl;

		cout << tools::filesystem::getFileContents("Makefile").size() << endl;
#ifdef BZ2_EXT
		cout << tools::misc::bzCompress(tools::filesystem::getFileContents("Makefile"), 9).size() << endl;
#endif

		dodoString t = "abcddF";
		tools::string::replace("cd", "WW", t);
		cout << t << endl;

		dodoString rnd = tools::misc::stringRandom(12);
		cout << tools::misc::MD5Hex(rnd) << endl;

		//tools::misc::mail("niam", "test", "test");
		//tools::misc::mail("127.0.0.1", 25, "niam@niam.mu", "niam@niam.mu", "test", "test", "niam", "niam");
	}
	catch (baseEx ex)
	{
		cout << endl << ex.baseErrno << endl << ex.baseErrstr  << ex.line << endl << ex.file << endl;
	}
	catch (exception ex)
	{
		cout << endl << ex.what() << endl;
	}

	return 0;
}

