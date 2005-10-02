#include <cgi.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	
	///first type
	assocArr head;
	//head["Content-type"] = "image/png";
	cgipp cgi(false,head);
	
	///second type
	/*cgipp cgi(true,head); /// or cgipp cgi(true);
	 cgi.HEADERS["Content-type"] = "image/jpg";
	 cgi.printHeaders();
	 */
	
	//cout << "\n\n" << cgi.METHOD_GET["sdsd"] << "\n\n";
	cout << "!" << cgi.METHOD_POST["a"] << "!<br>";
	cout << "!" << cgi.METHOD_POST["e"] << "!<br>";
	cout << "!" << cgi.ENVIRONMENT["CONTENT_TYPE"] << "<br>";
	regexp expr;
	stringArr pock;
	expr.match("(.*)a([0-9]+)(f*)(a*)(b*)","dsdasa4565aaa",pock);
	cout << pock[0] << "\n";
	cout << pock[1] << "\n";
	cout << pock[2] << "\n";
	cout << pock[3] << "\n";
	cout << pock[4] << "\n";
	
	stringArr a;
	a.push_back("!!!");
	a.push_back("@@");
	cout << expr.reReplace("dsdasa4565aaa",a) << "\n";
	return 0;
}
