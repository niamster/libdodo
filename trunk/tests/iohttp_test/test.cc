
#include <libdodo/baseEx.h>
#include <libdodo/ioHTTP.h>
#include <libdodo/types.h>

#include <iostream>

using namespace dodo;

using namespace std;


int main(int argc, char **argv)
{
	try
	{	
		ioHTTP http;
		//__httpResponse res =  http.GET("http://google.com");
		__httpResponse res =  http.GET("http://62.80.178.137/file/5050058/4156786/12.jpg");

		cout << res.code << "\n~~\n";
		cout << res.headers[IOHTTP_RESPONSEHEADER_CONTENTTYPE] << "\n~~\n";
		cout << res.data << "\n~~\n";
		cout << res.headers[IOHTTP_RESPONSEHEADER_CONTENTLENGTH] << "\n~~\n";
		cout << res.data.size() << "\n~~\n";
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << "\t" << ex.file << endl;
		cout.flush();
	}
	
	return 0;
}
