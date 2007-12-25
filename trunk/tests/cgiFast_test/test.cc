#include <libdodo/baseEx.h>
#include <libdodo/cgi.h>
#include <libdodo/cgiProcessor.h>
#include <libdodo/tools.h>
#include <libdodo/cgiFast.h>

#include <iostream>

using namespace dodo;

using namespace std;

#ifdef FCGI_EXT

	void 
	cgif(cgiFastIO *fcgi)
	{
		cgi cgit(fcgi, true);
		cgit.setCookie("test","Ni@m");
		cgit.printHeaders();
		 
		
		fcgi->print("!" + cgit.METHOD_GET["a"] + "!<br>");
		fcgi->print("!" + cgit.METHOD_POST["a"] + "!<br>");
		fcgi->print("!" + cgit.METHOD_POST["e"] + "!<br>");
		fcgi->print("!" + cgit.ENVIRONMENT["QUERY_STRING"] + "<br>");
		fcgi->print("!" + cgit.COOKIES["test"] + "<br>");
		
		fcgi->print("!!!\n\n\n");
		
		try
		{
			cgiProcessor cgip(fcgi);
			cgip.assign("test","hoho");
			cgip.assign("show","That's works!");
			
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
			fcgi->print(ex.baseErrstr + " " + stringTools::lToString(ex.line));
		}	
		
		fcgi->print("\n\n\n!!!");
	}

#endif

int main(int argc, char **argv)
{	
	#ifdef FCGI_EXT
	
		cgiFast cf;
		
		cf.setCGIFunction(&cgif);
	
		cf.listen();
	
	#else
	
		cout << "No fastCGI extension was compiled!";
		
	#endif
		
	return 0;
}
