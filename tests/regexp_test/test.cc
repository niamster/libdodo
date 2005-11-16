#include <regexp.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{	
	regexp expr;
	stringArr pock;

	cout << "<hr>pattern: (.*)a([0-9]+)(f*)(a*)(b*)<br> test string: dsdasa4565aaa<br> pockets: <br>";
	
	if (expr.match("(.*)a([0-9]+)(f*)(a*)(b*)","dsdasa",pock)/*4565aaa",pock)*/)
	{
		cout << pock[0] << "<br>";
		cout << pock[1] << "<br>";
		cout << pock[2] << "<br>";
		cout << pock[3] << "<br>";
		cout << pock[4] << "<br>";
	}
	else
		cout << "not matched";
	
	cout << "<hr>Replacemets: a-> '!!!', '@@'=> result:<br>";
	
	stringArr a;
	a.push_back("!!!");
	a.push_back("@@");
	cout << expr.reReplace("dsdasa4565aaa",a) << "<br>";
	return 0;
}
