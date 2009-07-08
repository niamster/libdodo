/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io::network::http;

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

		dodoMap<dodoString, file> files;

		files["file"] = file("./test.cc", "text/plain");

		client HTTP;
		response res;

		HTTP.setCookies(cookies);

		//HTTP.cacheAuthentification = false;

		//HTTP.setProxy("127.0.0.1");
		//HTTP.setProxy(tools::network::getHostPrimaryIp("localhost"));

		res =  HTTP.GET("http://google.com");
		//res =  HTTP.GET("https://kernel.org/");
		//res =  HTTP.GET("http://libdodo:password@localhost/libdodo/cgi_test/test.cgi?status=digest_auth");
		//res =  HTTP.GET("http://niam:niam@localhost/auth/libdodo/cgi_test/test.cgi");
		//res =  HTTP.POST("http://localhost/libdodo/cgi_test/test.cgi", map);
		//res =  HTTP.POST("http://localhost/libdodo/cgi_test/test.cgi", map, files);

		tools::filesystem::writeToFile("test.dat", res.data);

		cout << res.code << "\n~~\n";
		cout << res.headers[RESPONSEHEADER_CONTENTTYPE] << "\n~~\n";
		//cout << res.data << "\n~~\n";
		cout << res.headers[RESPONSEHEADER_CONTENTLENGTH] << "\n~~\n";
		cout << res.data.size() << "\n~~\n";

		cout << "Cookies:\n";
		dodoArray<cgi::cookie>::iterator i(res.cookies.begin()), j(res.cookies.end());
		for (; i != j; ++i)
			cout << i->name << "=" << i->value << "\n~~\n";
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
