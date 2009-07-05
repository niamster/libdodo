/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <string.h>

using namespace dodo;
using namespace rpc;

using namespace std;

class cgiIO : public io::stream::channel, public io::network::http
{
  public:

	cgiIO(cgi::exchange &cf,
		  dodoMap<short, dodoString> &headers) : io::stream::channel(io::CHANNEL_PROTECTION_NONE),
												 provider(cf, headers)
	{
	}

  protected:

	virtual void _read(char * const data) const
	{
		_readStream(data);
	}

	virtual unsigned long _readStream(char * const data) const
	{
		unsigned int size = provider.content.size();

		if (size != 0)
		{
			if (size > inSize)
				size = inSize;

			memcpy(data, provider.content.data(), size);
			if (size < inSize)
				memset(data+size, 0x0, inSize - size);
		}

		provider.content.clear();

		return size;
	}

	virtual void _write(const char * const data)
	{
		provider.print(data);
	}

	virtual void _writeStream(const char * const data)
	{
		provider.printStream(data);
	}

	int getOutDescriptor() const
	{
		return -1;
	}

	int getInDescriptor() const
	{
		return -1;
	}

	void flush()
	{
	}

	mutable cgi::dialogue provider;
};

response
handler(const dodoString &method, const dodoArray<value> &values, const void *idata, void *odata)
{
	response resp;

	resp.addArgument(dodoString("Got method: ") + method + "\n");
	resp.addArgument(dodoString("Amount of values: ") + tools::string::ulToString(values.size()) + "\n");

	return resp;
}

int main(int argc, char **argv)
{
	dodoMap<short, dodoString> headers;
	headers[cgi::CGI_ENVIRONMENT_CONTENTTYPE] = "application/json";

	cgi::basic::exchange cgiio;
	cgiIO provider(cgiio, headers);

	json::server srv(provider);

	try
	{
		srv.setHandler("callTest", ::handler);

		srv.serve();
	}
	catch (dodo::exception::basic ex)
	{
		cout << ex.message << "\t" << ex.baseErrstr << endl;
	}

	return 0;
}
