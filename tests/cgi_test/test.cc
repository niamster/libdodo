#include <cgiTools.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	
	///first type
	/*assocArr head;
	head["Content-type"] = "image/png";
	cgipp cgi(false,head);*/
	
	///second type
	cgiTools cgi(true,head); /// or cgipp cgi(true);
	//cgi.HEADERS["Content-type"] = "image/jpg";
	cgi.setCookie("test","Ni@m");
	cgi.printHeaders();
	 
	
	cout << "!" << cgi.METHOD_GET["a"] << "!<br>";
	cout << "!" << cgi.METHOD_POST["a"] << "!<br>";
	cout << "!" << cgi.METHOD_POST["e"] << "!<br>";
	cout << "!" << cgi.ENVIRONMENT["QUERY_STRING"] << "<br>";
	cout << "!" << cgi.COOKIES["test"] << "<br>";
	cout << "!" << cgi.getFile("qsadasdaf").error << "<br>";
	
	return 0;
}
