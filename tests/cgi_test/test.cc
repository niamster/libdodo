#include <baseEx.h>
#include <cgiTools.h>
#include <cgiProcessor.h>

using namespace dodo;

using namespace std;

void 
cgi()
{
	///first type
//	assocArr head;
//	head["Content-type"] = "text/html";
//	cgiTools cgi(false,head);
	
	///second type
	cgiTools cgi(true);
	//cgi.HEADERS["Content-type"] = "image/jpg";
	cgi.setCookie("test","Ni@m");
	cgi.printHeaders();
	 
	
/*	cout << "!" << cgi.METHOD_GET["a"] << "!<br>";
	cout << "!" << cgi.METHOD_POST["a"] << "!<br>";
	cout << "!" << cgi.METHOD_POST["e"] << "!<br>";
	cout << "!" << cgi.ENVIRONMENT["QUERY_STRING"] << "<br>";
	cout << "!" << cgi.COOKIES["test"] << "<br>";*/
	//cout << "!" << cgi.getFile("qsadasdaf").error << "<br>";
	
	cout << "\n\n\n";
	
	try
	{
		cgiProcessor cgip;
		cgip.assign("main","index.tpl");
		cgip.assign("test","hoho");
		cgip.assign("show","That's works!");
		
		stringArr arr;
		arr.push_back("one");
		arr.push_back("two");
		arr.push_back("three");
		cgip.assign("arr",arr);
		
		assocArr arr1;
		arr1["one"] = "one";
		arr1["two"] = "two";
		arr1["three"] = "three";
		cgip.assign("arr1",arr1);
		
		std::vector<assocArr> arr2;
		arr2.push_back(arr1);
		arr1["one"] = "three";
		arr2.push_back(arr1);
		cgip.assign("arr2",arr2);
	
		cgip.assign("one","one");
	
		cgip.display("test.tpl");
	}
	catch(baseEx ex)
	{
		cout << ex << " " << ex.line;
	}	
}

int main(int argc, char **argv)
{	
	
	cgi();
		
	return 0;
}
