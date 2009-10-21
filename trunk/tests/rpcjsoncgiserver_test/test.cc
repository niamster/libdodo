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

class cgiIO : public io::stream::channel {
  public:

    cgiIO(cgi::exchange &cf,
          dodoMap<short, dodoString> &headers) : io::stream::channel(io::channel::PROTECTION_NONE),
                                                 provider(cf, headers)
    {
    }

  protected:

    virtual void
    _read(char * const data) const
    {
        _readString(data);
    }

    virtual unsigned long
    _readString(char * const data) const
    {
        unsigned int size = provider.content.size();

        if (size != 0) {
            if (size > blockSize)
                size = blockSize;

            memcpy(data, provider.content.data(), size);
            if (size < blockSize)
                memset(data + size, 0x0, blockSize - size);
        }

        provider.content.clear();

        return size;
    }

    virtual void
    _write(const char * const data) const
    {
        provider.print(data);
    }

    virtual void
    _writeString(const char * const data) const
    {
        provider.printString(data);
    }

    int
    outDescriptor() const
    {
        return -1;
    }

    int
    inDescriptor() const
    {
        return -1;
    }

    void
    flush() const
    {
    }

    mutable cgi::dialogue provider;
};

response
handler(const dodoString       &method,
        const dodoArray<value> &values,
        const void             *idata,
        void                   *odata)
{
    response resp;

    resp.addArgument(dodoString("Got method: ") + method + "\n");
    resp.addArgument(dodoString("Amount of values: ") + tools::string::ulToString(values.size()) + "\n");

    return resp;
}

int
main(int  argc,
     char **argv)
{
    dodoMap<short, dodoString> headers;
    headers[cgi::ENVIRONMENT_CONTENTTYPE] = "application/json";

    cgi::basic::exchange io;
    cgiIO provider(io, headers);

    json::server srv(provider);

    try {
        srv.setHandler("callTest", ::handler);

        srv.serve();
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}
