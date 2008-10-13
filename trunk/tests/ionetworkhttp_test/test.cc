/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/exceptionBasic.h>
#include <libdodo/ioNetworkHttp.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;
using namespace io::network;

using namespace std;


int main(int argc, char **argv)
{
	try
	{
		dodoStringMap cookies;
		cookies["test"] = "Ni@m";

		dodoStringMap map;

		map["hidden"] = "hidden";
		map["text"] = "text";

		dodoMap<dodoString, __httpPostFile> files;

		files["file"] = __httpPostFile("./test.cc", "text/plain");

		http HTTP;
		__httpResponse res;

		HTTP.setCookies(cookies);

		//HTTP.cacheAuthentification = false;

		//HTTP.setProxy("127.0.0.1");

		res =  HTTP.GET("http://google.com");
		//res =  HTTP.GET("https://sourceforge.net");
		//res =  HTTP.GET("http://libdodo:password@localhost/libdodo/cgi_test/test.cgi?a=digest_auth");
		//res =  HTTP.GET("http://niam:niam@localhost/auth/libdodo/cgi_test/test.cgi");
		//res =  HTTP.POST("http://localhost/libdodo/cgi_test/test.cgi", map);
		//res =  HTTP.POST("http://localhost/libdodo/cgi_test/test.cgi", map, files);

		tools::filesystem::writeToFile("test.dat", res.data);

		cout << res.code << "\n~~\n";
		cout << res.headers[HTTP_RESPONSEHEADER_CONTENTTYPE] << "\n~~\n";
		//cout << res.data << "\n~~\n";
		cout << res.headers[HTTP_RESPONSEHEADER_CONTENTLENGTH] << "\n~~\n";
		cout << res.data.size() << "\n~~\n";

		cout << "Cookies:\n";
		dodoArray<cgi::__cgiCookie>::iterator i(res.cookies.begin()), j(res.cookies.end());
		for (; i != j; ++i)
			cout << i->name << "=" << i->value << "\n~~\n";
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
