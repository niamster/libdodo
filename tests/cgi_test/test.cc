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
	
	cout << "\n\n" << cgi.METHOD_GET["sdsd"] << "\n\n";

	return 0;
}
