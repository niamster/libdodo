/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#
#include <iostream>

using namespace dodo;
using namespace io::network;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		try
		{
			client st(CONNECTION_PROTO_FAMILY_IPV4, CONNECTION_TRANSFER_TYPE_STREAM);
			exchange exch;
			dodoString str;

			try
			{
				st.connectFrom("127.0.0.1", "127.0.0.1", 21, exch);

				str = exch.readStream();
				cout << "\ngot:\n" << str << "\n";
			}
			catch (dodo::exception::basic ex)
			{
				cout << (dodoString)ex << "\t" << ex.line << endl;
			}
		}
		catch (dodo::exception::basic ex)
		{
			cout << (dodoString)ex << "\t" << ex.line << endl;
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
