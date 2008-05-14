#include <libdodo/baseEx.h>
#include <libdodo/cgiServer.h>
#include <libdodo/cgiProcessor.h>

#include <iostream>

using namespace dodo;
using namespace cgi;

using namespace std;

int main(int argc, char **argv)
{		
///first type
//	dodoStringMap head;
//	head[SERVER_RESPONSEHEADER_CONTENTTYPE] = "text/html";
//	server cgi(head, false);
	
///second type									}

	server cgit(true);

	dodoString user = cgit.getAuthentificationInfo().user;

	if (cgit.GET["a"] == "forbidden")
	{
		cgit.setResponseStatus(SERVER_STATUSCODE_FORBIDDEN);

		cgit.printStream( "<b>FORBIDDEN</b><br>" );

		return 0;
	}
	else if (cgit.GET["a"] == "notfound")
	{
		cgit.setResponseStatus(SERVER_STATUSCODE_NOTFOUND);

		cgit.printStream( "<b>NOT FOUND</b><br>" );

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
		if (user.size() == 0 || !cgit.checkAuthentification("libdodo", "password"))
		{
			cgit.requestAuthentification("libdodo", SERVER_AUTHTYPE_BASIC);

			return 0;
		}
	}
	else if (cgit.GET["a"] == "digest_auth")
	{
		if (user.size() == 0 || !cgit.checkAuthentification("libdodo", "password"))
		{
			cgit.requestAuthentification("libdodo", SERVER_AUTHTYPE_DIGEST);

			return 0;
		}
	}

	cgit.HEADERS[SERVER_RESPONSEHEADER_CONTENTTYPE] = "text/html";
	cgit.setCookie("test","Ni@m");
	cgit.printHeaders();
	
	cgit.printStream( "User: " + user + "<br>" );
	
	cgit.printStream( "!" + cgit.GET["a"] + "!<br>" );
	cgit.printStream( "!" + cgit.POST["hidden"] + "!<br>" );
	cgit.printStream( "!" + cgit.POST["text"] + "!<br>" );
	cgit.printStream( "!" + cgit.ENVIRONMENT[SERVER_ENVIRONMENT_QUERYSTRING] + "<br>" );
	cgit.printStream( "!" + cgit.COOKIES["test"] + "<br>" );
	cgit.printStream( "!" + tools::string::ulToString(cgit.FILES["file"].size) + "<br>" );
	
	cgit.printStream( "\n\n\n" );
	
	try
	{
		processor cgip(cgit);
		cgip.assign("main","index.tpl");
		cgip.assign("test","test");
		cgip.assign("show","show");
		
		dodoStringArray arr;
		arr.push_back("one");
		arr.push_back("two");
		arr.push_back("three");
		cgip.assign("arr",arr);
		
		dodoStringMap arr1;
		arr1["one"] = "one";
		arr1["two"] = "two";
		arr1["three"] = "three";
		cgip.assign("arr1",arr1);
		
		dodoArray<dodoStringMap> arr2;
		arr2.push_back(arr1);
		arr1["one"] = "three";
		arr2.push_back(arr1);
		cgip.assign("arr2",arr2);
	
		cgip.assign("one","one");
	
		cgip.display("test.tpl");
	}
	catch(baseEx ex)
	{
		cgit.printStream( (string)ex + " " + tools::string::lToString(ex.line) + " " + ex.message );
	}	
		
	return 0;
}
