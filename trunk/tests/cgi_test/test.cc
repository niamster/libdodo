#include <libdodo/baseEx.h>
#include <libdodo/cgi.h>
#include <libdodo/cgiProcessor.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{		
///first type
//	dodoStringMap head;
//	head["Content-type"] = "text/html";
//	cgi cgi(head, false);
	
///second type
	cgi cgit(true);
	cgit.HEADERS["Content-type"] = "text/html";
	cgit.setCookie("test","Ni@m");
	cgit.printHeaders();
	 
	
	cgit.printStream( "!" + cgit.GET["a"] + "!<br>" );
	cgit.printStream( "!" + cgit.POST["hidden"] + "!<br>" );
	cgit.printStream( "!" + cgit.POST["text"] + "!<br>" );
	cgit.printStream( "!" + cgit.ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING] + "<br>" );
	cgit.printStream( "!" + cgit.COOKIES["test"] + "<br>" );
	cgit.printStream( "!" + stringTools::ulToString(cgit.FILES["file"].size) + "<br>" );
	
	cgit.printStream( "\n\n\n" );
	
	try
	{
		cgiProcessor cgip(cgit);
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
		cgit.printStream( (string)ex + " " + stringTools::lToString(ex.line) + " " + ex.message );
	}	
		
	return 0;
}
