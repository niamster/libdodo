/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/cgiDialogue.h>
#include <libdodo/cgiBasicServer.h>
#include <libdodo/cgiBasicExchange.h>
#include <libdodo/dataTplProcessor.h>

#include <iostream>

using namespace dodo;
using namespace data::tpl;
using cgi::exchange;

using namespace std;

void
cgif(exchange &cgiio)
{
	using namespace cgi;

	///first type: pass headers and print them immediately
	//	dodoStringMap head;
	//	head[CGI_RESPONSEHEADER_CONTENTTYPE] = "text/html";
	//	server cgit(&cgiio, head, false);

	///second type: use default headers and do not print them immediately
	dialogue cgit(cgiio, true);

	dodoString user = cgit.getAuthenticationInfo().user;

	if (cgit.GET["a"] == "forbidden")
	{
		cgit.setResponseStatus(CGI_STATUSCODE_FORBIDDEN);

		cgit.printStream("FORBIDDEN");

		return ;
	}
	else if (cgit.GET["a"] == "notfound")
	{
		cgit.setResponseStatus(CGI_STATUSCODE_NOTFOUND);

		cgit.printStream("NOT FOUND");

		return ;
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
			cgit.requestAuthentication("libdodo", CGI_AUTHTYPE_BASIC);

			return ;
		}
	}
	else if (cgit.GET["a"] == "digest_auth")
	{
		if (user.size() == 0 || !cgit.isAuthenticated("libdodo", "password"))
		{
			cgit.requestAuthentication("libdodo", CGI_AUTHTYPE_DIGEST);

			return ;
		}
	}

	cgit.HEADERS[CGI_RESPONSEHEADER_CONTENTTYPE] = "text/html";

	cgit.setCookie("test", "Ni@m");

	exchange *io = cgit;
	io->writeStreamString("The headers thould be already printed successfully.<br>");

	cgit.printStream("User: " + user + "<br>");
	cgit.printStream(cgit.GET["a"] + "<br>");
	cgit.printStream(cgit.POST["hidden"] + "<br>");
	cgit.printStream(cgit.POST["text"] + "<br>");
	cgit.printStream(cgit.ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING] + "<br>");
	cgit.printStream(cgit.COOKIES["test"] + "<br>");
	cgit.printStream(tools::string::ulToString(cgit.FILES["file"].size) + "<br>");
	cgit.printStream(cgit.FILES["file"].mime + "<br>");
	cgit.printStream(cgit.getCharset() + "<br>");
	cgit.printStream( "<br>" );

	try
	{
		processor cgip;

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

		cgit.printStream(cgip.processFile("test.tpl"));
	}
	catch (dodo::exception::basic ex)
	{
		cgit.printStream((dodoString)ex + " " + tools::string::lToString(ex.line) + " " + ex.file + " " + ex.message );
	}
}

int main(int argc, char **argv)
{
	using namespace cgi::basic;

	server c;

	c.serve(&cgif);

	return 0;
}
