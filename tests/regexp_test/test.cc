
#include <regexpTools.h>
#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{	
	regexpTools expr;
	dodoStringArr pock;
	
	expr.multiline = true;

	std::string sample = "dsdasa4565a\naa";

	std::string pattern = "^(.*)a([0-9]+)(f*)(a*)(.*)$";

	cout << "\npattern: "<<pattern<<"\n\ntest string: "<<sample<<"\n\npockets:\n";
	
	if (expr.match(pattern,sample,pock))
	{
		cout << pock.size() << " ::size\n";
		
		cout << pock[0] << " >>0\n";
		cout << pock[1] << " >>1\n";
		cout << pock[2] << " >>2\n";
		cout << pock[3] << " >>3\n";
		cout << pock[4] << " >>4\n";
	}
	else
		cout << "not matched";
	
	cout << "\nReplacemets: a-> '!!!', '@@'=> result:\n";
	
	dodoStringArr a;
	a.push_back("!!!");
	a.push_back("@@");
	cout << expr.reReplace("dsdasa4565aaa",a) << "\n";
	return 0;
}
