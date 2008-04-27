
#include <libdodo/baseEx.h>
#include <libdodo/ioNetworkHttp.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;

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

		ioNetworkHttp http;
		__httpResponse res;

		http.setCookies(cookies);
		
		//http.setProxyInformation("127.0.0.1");

		res =  http.GET("http://google.com");
		//res =  http.GET("http://libdodo:password@temp.niam.mu/libdodo/cgi_test/test.cgi?a=digest_auth");
		//res =  http.GET("http://niam:niam@localhost/auth/libdodo/cgi_test/test.cgi");
		//res =  http.POST("http://localhost/libdodo/cgi_test/test.cgi", map);
		//res =  http.POST("http://localhost/libdodo/cgi_test/test.cgi", map, files);
		
		toolsFilesystem::writeToFile("test.dat", res.data);
		

		cout << res.code << "\n~~\n";
		cout << res.headers[IONETWORKHTTP_RESPONSEHEADER_CONTENTTYPE] << "\n~~\n";
		//cout << res.data << "\n~~\n";
		cout << res.headers[IONETWORKHTTP_RESPONSEHEADER_CONTENTLENGTH] << "\n~~\n";
		cout << res.data.size() << "\n~~\n";

		cout << "Cookies:\n";
		dodoArray<cgi::__clientCookie>::iterator i(res.cookies.begin()), j(res.cookies.end());
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
