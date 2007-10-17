#include <regexpTools.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{	
	regexpTools expr;
	stringArr pock;

	cout << "\npattern: (.*)a([0-9]+)(f*)(a*)(b*)<br> test string: dsdasa4565aaa<br> pockets: \n";
	
	if (expr.match("(.*)a([0-9]+)(f*)(a*)(b*)",/*"dsdasa",pock)*/"ssa4565aaa",pock))
	{
		cout << pock[0] << "\n";
		cout << pock[1] << "\n";
		cout << pock[2] << "\n";
		cout << pock[3] << "\n";
		cout << pock[4] << "\n";
	}
	else
		cout << "not matched";
	
	cout << "\nReplacemets: a-> '!!!', '@@'=> result:\n";
	
	stringArr a;
	a.push_back("!!!");
	a.push_back("@@");
	cout << expr.reReplace("dsdasa4565aaa",a) << "\n";
	return 0;
}
