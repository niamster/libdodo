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
//	cgi cgi(false,head);
	
///second type
	cgi cgit(true);
	cgit.HEADERS["Content-type"] = "text/html";
	cgit.setCookie("test","Ni@m");
	cgit.printHeaders();
	 
	
	cout << "!" << cgit.METHOD_GET["a"] << "!<br>";
	cout << "!" << cgit.METHOD_POST["a"] << "!<br>";
	cout << "!" << cgit.METHOD_POST["e"] << "!<br>";
	cout << "!" << cgit.ENVIRONMENT["QUERY_STRING"] << "<br>";
	cout << "!" << cgit.COOKIES["test"] << "<br>";
	cout << "!" << cgit.FILES["qsadasdaf"].error << "<br>";
	
	cout << "\n\n\n";
	
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
		cout << (string)ex << " " << ex.line << " " << ex.message;
	}	
		
	return 0;
}
