#include <baseEx.h>
#include <cgiTools.h>
#include <cgiProcessor.h>
#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{		
	///first type
//	dodoStringMapContents head;
//	head["Content-type"] = "text/html";
//	cgiTools cgi(false,head);
	
	///second type
	cgiTools cgi(true);
	//cgi.HEADERS["Content-type"] = "image/jpg";
	cgi.HEADERS["Content-type"] = "text/html";
	cgi.setCookie("test","Ni@m");
	cgi.printHeaders();
	 
	
	cout << "!" << cgi.METHOD_GET["a"] << "!<br>";
	cout << "!" << cgi.METHOD_POST["a"] << "!<br>";
	cout << "!" << cgi.METHOD_POST["e"] << "!<br>";
	cout << "!" << cgi.ENVIRONMENT["QUERY_STRING"] << "<br>";
	cout << "!" << cgi.COOKIES["test"] << "<br>";
	cout << "!" << cgi.FILES["qsadasdaf"].error << "<br>";
	
	cout << "\n\n\n";
	
	try
	{
		cgiProcessor cgip;
		cgip.assign("main","index.tpl");
		cgip.assign("test","hoho");
		cgip.assign("show","That's works!");
		
		dodoStringArr arr;
		arr.push_back("one");
		arr.push_back("two");
		arr.push_back("three");
		cgip.assign("arr",arr);
		
		dodoStringMapContents arr1;
		arr1["one"] = "one";
		arr1["two"] = "two";
		arr1["three"] = "three";
		cgip.assign("arr1",arr1);
		
		dodoArray<dodoStringMapContents> arr2;
		arr2.push_back(arr1);
		arr1["one"] = "three";
		arr2.push_back(arr1);
		cgip.assign("arr2",arr2);
	
		cgip.assign("one","one");
	
		cgip.display("test.tpl");
	}
	catch(baseEx ex)
	{
		cout << ex << " " << ex.line << " " << ex.message;
	}	
		
	return 0;
}
