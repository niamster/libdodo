#include <cgi.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	
	///first type
	assocArr head;
	/*head["Content-type"] = "image/png";
	cgipp cgi(false,head);*/
	
	///second type
	cgipp cgi(true,head); /// or cgipp cgi(true);
	//cgi.HEADERS["Content-type"] = "image/jpg";
	cgi.setCookie("test","Ni@m");
	cgi.printHeaders();
	 
	
	cout << "!" << cgi.METHOD_GET["a"] << "!<br>";
	cout << "!" << cgi.METHOD_POST["a"] << "!<br>";
	cout << "!" << cgi.METHOD_POST["e"] << "!<br>";
	cout << "!" << cgi.ENVIRONMENT["QUERY_STRING"] << "<br>";
	cout << "!" << cgi.COOKIES["test"] << "<br>";
	
	regexp expr;
	stringArr pock;
	
	cout << "<hr>pattern: (.*)a([0-9]+)(f*)(a*)(b*)<br> test string: dsdasa4565aaa<br> pockets: <br>";
	
	expr.match("(.*)a([0-9]+)(f*)(a*)(b*)","dsdasa4565aaa",pock);
	cout << pock[0] << "<br>";
	cout << pock[1] << "<br>";
	cout << pock[2] << "<br>";
	cout << pock[3] << "<br>";
	cout << pock[4] << "<br>";
	
	cout << "<hr>Replacemets: a-> '!!!', '@@'=> result:<br>";
	
	stringArr a;
	a.push_back("!!!");
	a.push_back("@@");
	cout << expr.reReplace("dsdasa4565aaa",a) << "<br>";
	return 0;
}
