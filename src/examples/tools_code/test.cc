/**
 * vim indentation seurlings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

int
main(int  argc,
     char **argv)
{
    try {
        cout << "тест" << endl;
#ifdef ICONV_EXT
        cout << tools::code::codesetConversion(tools::code::codesetConversion("тест", "utf-8", "cp1251"), "cp1251", "utf-8") << endl;
#endif

        dodoString longString = dodoString(1000000, 'l') + dodoString(1000000, 'i') + dodoString(1000000, 'b') + dodoString(1000000, 'd') + dodoString(1000000, 'o') + dodoString(1000000, 'd') + dodoString(1000000, 'o');

        cout << "longString = dodoString(1000000, 'l') + dodoString(1000000, 'i') + dodoString(1000000, 'b') + dodoString(1000000, 'd') + dodoString(1000000, 'o') + dodoString(1000000, 'd') + dodoString(1000000, 'o');" << endl;

        cout << "MD5 of \"\", should be d41d8cd98f00b204e9800998ecf8427e" << endl;
        cout << "MD5 of \"\",        is " << tools::code::MD5Hex("") <<  endl;
        cout << "MD5 of \"abc\", should be 900150983cd24fb0d6963f7d28e17f72" << endl;
        cout << "MD5 of \"abc\",        is " << tools::code::MD5Hex("abc") <<  endl;
        cout << "MD5 of longString, should be 67314880276cfa6c63c3276f7c3820ce" << endl;
        cout << "MD5 of longString,        is " << tools::code::MD5Hex(longString) <<  endl;


        cout << "SHA-1 of \"\", should be da39a3ee5e6b4b0d3255bfef95601890afd80709" << endl;
        cout << "SHA-1 of \"\",        is " << tools::code::SHA1Hex("") <<  endl;
        cout << "SHA-1 of \"abc\", should be a9993e364706816aba3e25717850c26c9cd0d89d" << endl;
        cout << "SHA-1 of \"abc\",        is " << tools::code::SHA1Hex("abc") <<  endl;
        cout << "SHA-1 of longString, should be 759643681255032e13ddc81d839518f21804d854" << endl;
        cout << "SHA-1 of longString,        is " << tools::code::SHA1Hex(longString) <<  endl;

        cout << "SHA-256 of \"\", should be e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" << endl;
        cout << "SHA-256 of \"\",        is " << tools::code::SHA256Hex("") <<  endl;
        cout << "SHA-256 of \"abc\", should be ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" << endl;
        cout << "SHA-256 of \"abc\",        is " << tools::code::SHA256Hex("abc") <<  endl;
        cout << "SHA-256 of longString, should be a900f98ea455478f5b4ba820250ea4964982f64d56f1fc0b132d3c19204634ee" << endl;
        cout << "SHA-256 of longString,        is " << tools::code::SHA256Hex(longString) <<  endl;

        cout << "SHA-512 of \"\", should be cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e" << endl;
        cout << "SHA-512 of \"\",        is " << tools::code::SHA512Hex("") <<  endl;
        cout << "SHA-512 of \"abc\", should be ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f" << endl;
        cout << "SHA-512 of \"abc\",        is " << tools::code::SHA512Hex("abc") <<  endl;
        cout << "SHA-512 of longString, should be d64cdf8f171e1474423a002c0627be1c10bfb0d6c84ffcb555d9eca7c033504c38fb0f41e1ef0e1c86d6c61b1c09100a7192566f18dc5ecc9aa8121a50b521e7" << endl;
        cout << "SHA-512 of longString,        is " << tools::code::SHA512Hex(longString) <<  endl;

        cout << "tools::code::hexToLong(tools::code::longToHex(356456942))" << endl;
        cout << tools::code::hexToLong(tools::code::longToHex(356456942)) << endl;

#ifdef ICONV_EXT
        tools::code::codesetConversion(tools::filesystem::fileContents("test.cc"), "cp1251", "utf-8");
#endif

#ifdef ZLIB_EXT
        cout << tools::code::zCompress(tools::filesystem::fileContents("Makefile"), 9).size() << endl;
        cout << tools::code::zDecompress(tools::code::zCompress(tools::filesystem::fileContents("test.cc")));
#endif

        cout << tools::code::decodeUrl(tools::code::encodeUrl("@!()HEY, that's working!")) << endl;
        cout << tools::code::encodeUrl("@!()HEY, that's working!") << endl;

        cout << tools::code::decodeASCII85(tools::code::encodeASCII85("HEY, that's working!")) << endl;
        cout << tools::code::encodeASCII85("HEY, that's working!") << endl;
        cout << "size of test ASCII85-encoded : " <<  tools::code::encodeASCII85(tools::filesystem::fileContents("test")).size() << endl;

        cout << tools::code::decodeBase64(tools::code::encodeBase64("HEY, that's working!")) << endl;
        cout << tools::code::encodeBase64("HEY, that's working!") << endl;
        cout << "size of test base64-encoded : " <<  tools::code::encodeBase64(tools::filesystem::fileContents("test")).size() << endl;

        dodoString url = "ftp://user:pass@localhost.domain:32/path/init.cgi?net=true&work=true";
        cout << endl << url << endl;
        cout << tools::code::parseUrl(url).host << endl;
        cout << tools::code::parseUrl(url).login << endl;
        cout << tools::code::parseUrl(url).password << endl;
        cout << tools::code::parseUrl(url).path << endl;
        cout << tools::code::parseUrl(url).port << endl;
        cout << tools::code::parseUrl(url).protocol << endl;
        cout << tools::code::parseUrl(url).request << endl;

        cout << tools::code::makeUrl(tools::code::parseUrl(url)) << endl;

        cout << "Size of Makefile" << tools::filesystem::fileContents("Makefile").size() << endl;
#ifdef BZ2_EXT
        cout << "Size of bziped Makefile" << tools::code::bzCompress(tools::filesystem::fileContents("Makefile"), 9).size() << endl;
#endif
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    } catch (std::exception ex)   {
        cout << endl << ex.what() << endl;
    }

    return 0;
}
