/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace tools;

using namespace std;

int main(int argc, char **argv)
{
	regexp expr;
	dodoStringArray pock;

	expr.multiline = true;
	expr.greedy = false;

	dodoString sample = "string1234\n";

	dodoString pattern = "^(.*)([0-9]+)([^0-9]+)";

	cout << "\nMatch: pattern: " << pattern << "\n\ntest string: " << sample << "\n\npockets:\n";

	if (expr.match(pattern, sample, pock))
	{
		cout << "size: " << pock.size() << "\n";

		cout << "1:" << pock[0] << "\n";
		cout << "2:" << pock[1] << "\n";
		cout << "3:" << pock[2] << "\n";
	}
	else
		cout << "not matched";

	sample = "string12345string";
	cout << "\nReplacement: \npattern: " << pattern << "\n\ntest string: " << sample << "\n\nreplace with '<>', '@@'\nResult:\n";

	dodoStringArray replacements;
	replacements.push_back("<>");
	replacements.push_back("@@");
	cout << expr.replace(pattern, sample, replacements) << "\n";

	return 0;
}
