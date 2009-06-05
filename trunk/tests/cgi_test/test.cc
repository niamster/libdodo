/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
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

	if (cgit.GET["status"] == "forbidden")
	{
		cgit.setResponseStatus(CGI_STATUSCODE_FORBIDDEN);

		cgit.printStream("FORBIDDEN");

		return ;
	}
	else if (cgit.GET["status"] == "notfound")
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
	else if (cgit.GET["status"] == "basic_auth")
	{
		if (user.size() == 0 || !cgit.isAuthenticated("libdodo", "password"))
		{
			cgit.requestAuthentication("libdodo", CGI_AUTHTYPE_BASIC);

			return ;
		}
	}
	else if (cgit.GET["status"] == "digest_auth")
	{
		if (user.size() == 0 || !cgit.isAuthenticated("libdodo", "password"))
		{
			cgit.requestAuthentication("libdodo", CGI_AUTHTYPE_DIGEST);

			return ;
		}
	}

	cgit.HEADERS[CGI_RESPONSEHEADER_CONTENTTYPE] = "text/html";

	cgit.setCookie(__cgiCookie__("test", "Ni@m"));

	exchange *io = cgit;
	io->writeStream("The headers thould be already printed successfully.<br>");

	cgit.printStream("User: " + user + "<br>");
	cgit.printStream("GET[\"argument\"]: " + cgit.GET["argument"] + "<br>");
	cgit.printStream("POST[\"hidden\"]: " + cgit.POST["hidden"] + "<br>");
	cgit.printStream("POST[\"text\"]: " + cgit.POST["text"] + "<br>");
	cgit.printStream("ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]: " + cgit.ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING] + "<br>");
	cgit.printStream("COOKIES[\"test\"]: " + cgit.COOKIES["test"] + "<br>");
	cgit.printStream("FILES[\"file\"].size: " + tools::string::iToString(cgit.FILES["file"].size) + "<br>");
	cgit.printStream("FILES[\"file\"].mime: " + cgit.FILES["file"].mime + "<br>");
	cgit.printStream("charset: " + cgit.getCharset() + "<br>");
	cgit.printStream("tpl::processor:<br>");

	try
	{
		processor cgip;

		cgip.assign("main", "index.tpl");
		cgip.assign("test", "test");
		cgip.assign("show", "show");
		cgip.assign("one", "one");

		dodoStringArray strarr;
		strarr.push_back("one");
		strarr.push_back("two");
		strarr.push_back("three");
		cgip.assign("strarr", strarr);

		dodoStringMap strmap;
		strmap["one"] = "one";
		strmap["two"] = "two";
		strmap["three"] = "three";
		cgip.assign("strmap", strmap);

		dodoArray<dodoStringMap> strmaparr;
		strmaparr.push_back(strmap);
		strmap["one"] = "three";
		strmaparr.push_back(strmap);
		cgip.assign("strmaparr", strmaparr);

		cgip.processFile("test.tpl", *io);
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
