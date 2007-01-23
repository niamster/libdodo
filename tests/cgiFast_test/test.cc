#include <baseEx.h>
#include <cgiTools.h>
#include <cgiProcessor.h>
#include <tools.h>
#include <cgiFast.h>

using namespace dodo;

using namespace std;

#ifdef FCGI_EXT

	void 
	cgi(cgiFastSTD *fcgi)
	{
		cgiTools cgi(fcgi, true);
		//cgi.HEADERS["Content-type"] = "image/jpg";
		cgi.setCookie("test","Ni@m");
		cgi.printHeaders();
		 
		
	/*	cout << "!" << cgi.METHOD_GET["a"] << "!<br>";
		cout << "!" << cgi.METHOD_POST["a"] << "!<br>";
		cout << "!" << cgi.METHOD_POST["e"] << "!<br>";
		cout << "!" << cgi.ENVIRONMENT["QUERY_STRING"] << "<br>";
		cout << "!" << cgi.COOKIES["test"] << "<br>";*/
		//cout << "!" << cgi.getFile("qsadasdaf").error << "<br>";
		
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
			
			dodoAssocArr arr1;
			arr1["one"] = "one";
			arr1["two"] = "two";
			arr1["three"] = "three";
			cgip.assign("arr1",arr1);
			
			dodoArray<dodoAssocArr> arr2;
			arr2.push_back(arr1);
			arr1["one"] = "three";
			arr2.push_back(arr1);
			cgip.assign("arr2",arr2);
		
			cgip.assign("one","one");
		
			cgip.display("test.tpl");
		}
		catch(baseEx ex)
		{
			fcgi->print(ex.baseErrstr + " " + tools::lToString(ex.line));
		}	
		
		fcgi->print("\n\n\n!!!");
	}

#endif

int main(int argc, char **argv)
{	
	#ifdef FCGI_EXT
	
		cgiFast cf;
		
		cf.setCGIFunction(&cgi);
	
		cf.listen();
	
	#else
	
		cout << "No fastCGI extension was compiled!";
		
	#endif
		
	return 0;
}
