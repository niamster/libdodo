/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/toolsMisc.h>
#include <libdodo/toolsCode.h>
#include <libdodo/ioFile.h>
#include <libdodo/exceptionBasic.h>
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

		cout << "MD5 of \"\", should be d41d8cd98f00b204e9800998ecf8427e" << endl;
		cout << "MD5 of \"\", should is " << tools::code::MD5Hex("") <<  endl;
		cout << "MD5 of \"abc\", should be 900150983cd24fb0d6963f7d28e17f72" << endl;
		cout << "MD5 of \"abc\",        is " << tools::code::MD5Hex("abc") <<  endl;

		cout << "SHA-1 of \"\", should be da39a3ee5e6b4b0d3255bfef95601890afd80709" << endl;
		cout << "SHA-1 of \"\",        is " << tools::code::SHA1Hex("") <<  endl;
		cout << "SHA-1 of \"abc\", should be a9993e364706816aba3e25717850c26c9cd0d89d" << endl;
		cout << "SHA-1 of \"abc\",        is " << tools::code::SHA1Hex("abc") <<  endl;
		
		cout << "SHA-256 of \"\", should be e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" << endl;
		cout << "SHA-256 of \"\",        is " << tools::code::SHA256Hex("") <<  endl;
		cout << "SHA-256 of \"abc\", should be ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" << endl;
		cout << "SHA-256 of \"abc\",        is " << tools::code::SHA256Hex("abc") <<  endl;
		
		cout << "SHA-512 of \"\", should be cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e" << endl;
		cout << "SHA-512 of \"\",        is " << tools::code::SHA512Hex("") <<  endl;
		cout << "SHA-512 of \"abc\", should be ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f" << endl;
		cout << "SHA-512 of \"abc\",        is " << tools::code::SHA512Hex("abc") <<  endl;
		
		cout << "tools::code::hexToLong(tools::code::longToHex(356456942))" << endl;
		cout << tools::code::hexToLong(tools::code::longToHex(356456942)) << endl;

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
	catch (dodo::exception::basic ex)
	{
		cout << endl << ex.baseErrno << endl << ex.baseErrstr  << ex.line << endl << ex.file << endl;
	}
	catch (std::exception ex)
	{
		cout << endl << ex.what() << endl;
	}

	return 0;
}

