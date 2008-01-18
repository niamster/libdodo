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
	cgif(ioCGIFast *fcgi)
	{
		cgi cgit(fcgi, true);
		cgit.setCookie("test","Ni@m");
		cgit.printHeaders();
		 
		
		fcgi->writeStreamString("!" + cgit.GET["a"] + "!<br>");
		fcgi->writeStreamString("!" + cgit.POST["a"] + "!<br>");
		fcgi->writeStreamString("!" + cgit.POST["e"] + "!<br>");
		fcgi->writeStreamString("!" + cgit.ENVIRONMENT["QUERY_STRING"] + "<br>");
		fcgi->writeStreamString("!" + cgit.COOKIES["test"] + "<br>");
		
		fcgi->writeStreamString("!!!\n\n\n");
		
		try
		{
			cgiProcessor cgip(cgit);
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
			fcgi->writeStreamString(ex.baseErrstr + " " + stringTools::lToString(ex.line));
		}	
		
		fcgi->writeStreamString("\n\n\n!!!");
	}

#endif

int main(int argc, char **argv)
{	
#ifdef FCGI_EXT
	
	try
	{
		cgiFast cf;
		if (!cf.isFastCGI())
		{
			cout << "Not a fastCGI.";
			cout.flush();
		}
		
		cf.setCGIFunction(&cgif);
	
		cf.listen();
	}
	catch (baseEx &ex)
	{
		cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
	}
	
#else
	
		cout << "No fastCGI extension was compiled!";
		
#endif
		
	return 0;
}
