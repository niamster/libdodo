/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/cgiServer.h>
#include <libdodo/cgiProcessor.h>

#include <iostream>

using namespace dodo;
using namespace cgi;

using namespace std;

int main(int argc, char **argv)
{
	///first type: pass headers and print them immediately
	//	dodoStringMap head;
	//	head[SERVER_RESPONSEHEADER_CONTENTTYPE] = "text/html";
	//	server cgit(head, false);

	///second type: use default headers and do not print them immediately
	server cgit(true);

	dodoString user = cgit.getAuthenticationInfo().user;

	if (cgit.GET["a"] == "forbidden")
	{
		cgit.setResponseStatus(SERVER_STATUSCODE_FORBIDDEN);

		cgit.printStream("FORBIDDEN");

		return 0;
	}
	else if (cgit.GET["a"] == "notfound")
	{
		cgit.setResponseStatus(SERVER_STATUSCODE_NOTFOUND);

		cgit.printStream("NOT FOUND");

		return 0;
	}
	/**
	 * A workaround for apache web server to get auth headers:
	 *
	 * RewriteEngine on
	 * RewriteBase /
	 * RewriteCond %{HTTP:Authorization}  ^(.*)
	 * RewriteRule ^(.*)$ $1 [e=HTTP_AUTHORIZATION:%1
	 */
	else if (cgit.GET["a"] == "basic_auth")
	{
		if (user.size() == 0 || !cgit.isAuthenticated("libdodo", "password"))
		{
			cgit.requestAuthentication("libdodo", SERVER_AUTHTYPE_BASIC);

			return 0;
		}
	}
	else if (cgit.GET["a"] == "digest_auth")
	{
		if (user.size() == 0 || !cgit.isAuthenticated("libdodo", "password"))
		{
			cgit.requestAuthentication("libdodo", SERVER_AUTHTYPE_DIGEST);

			return 0;
		}
	}
	
	cgit.HEADERS[SERVER_RESPONSEHEADER_CONTENTTYPE] = "text/html";

	cgit.setCookie("test", "Ni@m");

	///print HTTP headers
	/// will print headers if they haven't been printed before
	/// headers are printed also if print was called but headers hadn't been called before
	cgit.printHeaders();

	cgit.printStream("User: " + user + "<br>");
	cgit.printStream(cgit.GET["a"] + "<br>");
	cgit.printStream(cgit.POST["hidden"] + "<br>");
	cgit.printStream(cgit.POST["text"] + "<br>");
	cgit.printStream(cgit.ENVIRONMENT[SERVER_ENVIRONMENT_QUERYSTRING] + "<br>");
	cgit.printStream(cgit.COOKIES["test"] + "<br>");
	cgit.printStream(tools::string::ulToString(cgit.FILES["file"].size) + "<br>");
	cgit.printStream(cgit.FILES["file"].mime + "<br>");
	cgit.printStream(cgit.getCharset() + "<br>");
	cgit.printStream( "<br>" );

	try
	{
		processor cgip(cgit);

		cgip.assign("main", "index.tpl");
		cgip.assign("test", "test");
		cgip.assign("show", "show");
		cgip.assign("one", "one");

		dodoStringArray arr;
		arr.push_back("one");
		arr.push_back("two");
		arr.push_back("three");
		cgip.assign("arr", arr);

		dodoStringMap arr1;
		arr1["one"] = "one";
		arr1["two"] = "two";
		arr1["three"] = "three";
		cgip.assign("arr1", arr1);

		dodoArray<dodoStringMap> arr2;
		arr2.push_back(arr1);
		arr1["one"] = "three";
		arr2.push_back(arr1);
		cgip.assign("arr2", arr2);

		cgip.display("test.tpl");
	}
	catch (dodo::exception::basic ex)
	{
		cgit.printStream((string)ex + " " + tools::string::lToString(ex.line) + " " + ex.file + " " + ex.message );
	}

	return 0;
}
