#include <baseEx.h>
#include <cgi.h>
#include <cgiProcessor.h>
#include <tools.h>
#include <cgiFast.h>
#include <iostream>

using namespace dodo;

using namespace std;

#ifdef FCGI_EXT

	void 
	cgif(cgiFastSTD *fcgi)
	{
		cgi cgit(fcgi, true);
		//cgit.HEADERS["Content-type"] = "image/jpg";
		cgit.setCookie("test","Ni@m");
		cgit.printHeaders();
		 
		
	/*	cout << "!" << cgit.METHOD_GET["a"] << "!<br>";
		cout << "!" << cgit.METHOD_POST["a"] << "!<br>";
		cout << "!" << cgit.METHOD_POST["e"] << "!<br>";
		cout << "!" << cgit.ENVIRONMENT["QUERY_STRING"] << "<br>";
		cout << "!" << cgit.COOKIES["test"] << "<br>";*/
		//cout << "!" << cgit.getFile("qsadasdaf").error << "<br>";
		
		fcgi->print("!!!\n\n\n");
		
		try
		{
			cgiProcessor cgip(fcgi);
			cgip.assign("test","hoho");
			cgip.assign("show","That's works!");
			
			dodoStringArr arr;
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
