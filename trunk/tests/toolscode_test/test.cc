/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/toolsMisc.h>
#include <libdodo/toolsCode.h>
#include <libdodo/ioFile.h>
#include <libdodo/baseEx.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetwork.h>

#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		cout << "тест" << endl;
#ifdef ICONV_EXT
		cout << tools::code::codesetConversion(tools::code::codesetConversion("тест", "utf-8", "cp1251"), "cp1251", "utf-8") << endl;
#endif

		cout << "MD5 of \"\"" << endl;
		cout << tools::code::MD5Hex("") <<  endl;

#ifdef ICONV_EXT
		tools::code::codesetConversion(tools::filesystem::getFileContents("test.cc"), "cp1251", "utf-8");
#endif

#ifdef ZLIB_EXT
		cout << tools::code::zCompress(tools::filesystem::getFileContents("Makefile"), 9).size() << endl;
		cout << tools::code::zDecompress(tools::code::zCompress(tools::filesystem::getFileContents("test.cc")));
#endif

		cout << tools::code::decodeUrl(tools::code::encodeUrl("@!()HEY, that's working!")) << endl;
		cout << tools::code::encodeUrl("@!()HEY, that's working!") << endl;

		cout << tools::code::encodeASCII85("HEY, that's working!") << endl;
		cout << tools::code::decodeASCII85(tools::code::encodeASCII85("HEY, that's working!")) << endl;
		cout << "size of test ASCII85-encoded : " <<  tools::code::encodeASCII85(tools::filesystem::getFileContents("test")).size() << endl;

		cout << tools::code::encodeBase64("HEY, that's working!") << endl;
		cout << tools::code::decodeBase64(tools::code::encodeBase64("HEY, that's working!")) << endl;
		cout << "size of test base64-encoded : " <<  tools::code::encodeBase64(tools::filesystem::getFileContents("test")).size() << endl;

		dodoString tt = "ftp://user:pass@localhost.domain:32/path/init.cgi?net=true&work=true";
		cout << endl << tt << endl;
		cout << tools::code::parseUrl(tt).host << endl;
		cout << tools::code::parseUrl(tt).login << endl;
		cout << tools::code::parseUrl(tt).password << endl;
		cout << tools::code::parseUrl(tt).path << endl;
		cout << tools::code::parseUrl(tt).port << endl;
		cout << tools::code::parseUrl(tt).protocol << endl;
		cout << tools::code::parseUrl(tt).request << endl;

		cout << tools::code::makeUrl(tools::code::parseUrl(tt)) << endl;

		cout << tools::filesystem::getFileContents("Makefile").size() << endl;
#ifdef BZ2_EXT
		cout << tools::code::bzCompress(tools::filesystem::getFileContents("Makefile"), 9).size() << endl;
#endif
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

