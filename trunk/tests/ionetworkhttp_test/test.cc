
#include <libdodo/baseEx.h>
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

		dodoStringMap files;

		files["file"] = "./test.cc";

		http HTTP;
		__httpResponse res;

		HTTP.setCookies(cookies);
		
		//HTTP.setProxyInformation("127.0.0.1");

		res =  HTTP.GET("http://google.com");
		//res =  HTTP.GET("http://libdodo:password@temp.niam.mu/libdodo/cgi_test/test.cgi?a=digest_auth");
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
		dodoArray<cgi::__serverCookie>::iterator i(res.cookies.begin()), j(res.cookies.end());
		for (;i!=j;++i)
			cout << i->name << "=" << i->value << "\n~~\n";
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}
	
	return 0;
}
