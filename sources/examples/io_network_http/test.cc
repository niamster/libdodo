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


int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        dodoStringMap cookies;
        cookies["test"] = "Dmytro Milinevskyy";

        dodoStringMap map;

        map["hidden"] = "hidden";
        map["t*et"] = "text";

        dodoMap<dodo::string, file> files;

        files["file"] = file("./test.cc", "t*et/plain");

        client HTTP;
        response res;

        HTTP.setCookies(cookies);

        //HTTP.cacheAuthentification = false;

        //HTTP.setProxy("127.0.0.1");
        //HTTP.setProxy(tools::network::hostPrimaryIp("localhost"));

        res =  HTTP.GET("http://google.com");
        //res =  HTTP.GET("http://libdodo:password@localhost/libdodo/cgi_test/test.cgi?status=digest_auth");
        //res =  HTTP.GET("http://niam:niam@localhost/auth/libdodo/cgi_test/test.cgi");
        //res =  HTTP.POST("http://localhost/libdodo/cgi_test/test.cgi", map);
        //res =  HTTP.POST("http://localhost/libdodo/cgi_test/test.cgi", map, files);

        tools::filesystem::writeToFile("test.dat", res.data);

        cout << res.code << "\n~~\n";
        cout << res.headers[RESPONSE_HEADER_CONTENTTYPE] << "\n~~\n";
        cout << res.headers[RESPONSE_HEADER_CONTENTLENGTH] << "\n~~\n";
        cout << res.data.size() << "\n~~\n";

        cout << "Cookies:\n";
        dodoArray<cgi::cookie>::iterator i(res.cookies.begin()), j(res.cookies.end());
        for (; i != j; ++i)
            cout << i->name << "=" << i->value << "\n~~\n";
#ifdef OPENSSL_EXT
        cout << "Fetching https://kernel.org/\n";
        res =  HTTP.GET("https://kernel.org/");

        tools::filesystem::writeToFile("test-https.dat", res.data);

        cout << res.code << "\n~~\n";
        cout << res.headers[RESPONSE_HEADER_CONTENTTYPE] << "\n~~\n";
        cout << res.headers[RESPONSE_HEADER_CONTENTLENGTH] << "\n~~\n";
        cout << res.data.size() << "\n~~\n";
#endif
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}
