
#include <libdodo/baseEx.h>
#include <libdodo/ioNetworkHTTP.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;

using namespace std;


int main(int argc, char **argv)
{
	try
	{
		dodoStringMap map;

		map["hidden"] = "hidden";
		map["text"] = "text";

		dodoStringMap files;

		files["file"] = "./test.cc";

		ioNetworkHTTP http;
		__httpResponse res;
		//res =  http.GET("http://temp.niam.mu/libdodo/cgi_test/test.cgi");
		//res =  http.GET("http://google.com");
		
		//res =  http.POST("http://temp.niam.mu/libdodo/cgi_test/test.cgi", map);
		res =  http.POST("http://temp.niam.mu/libdodo/cgi_test/test.cgi", map, files);
		

		cout << res.code << "\n~~\n";
		cout << res.headers[IONETWORKHTTP_RESPONSEHEADER_CONTENTTYPE] << "\n~~\n";
		cout << res.data << "\n~~\n";
		cout << res.headers[IONETWORKHTTP_RESPONSEHEADER_CONTENTLENGTH] << "\n~~\n";
		cout << res.data.size() << "\n~~\n";
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}
	
	return 0;
}
