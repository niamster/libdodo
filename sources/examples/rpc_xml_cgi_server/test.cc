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

    virtual unsigned long
    _read(char * const data) const
    {
        return _readString(data);
    }

    virtual unsigned long
    _readString(char * const data) const
    {
        unsigned int size = provider.content.size();

        if (size != 0) {
            if (size > bs)
                size = bs;

            memcpy(data, provider.content.data(), size);
            if (size < bs)
                memset(data + size, 0x0, bs - size);
        }

        provider.content.clear();

        return size;
    }

    virtual unsigned long
    _write(const char * const data) const
    {
        return provider.print(data);
    }

    virtual unsigned long
    _writeString(const char * const data) const
    {
        return provider.printString(data);
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
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodoMap<short, dodoString> headers;
    headers[cgi::ENVIRONMENT_CONTENTTYPE] = "application/json";

    cgi::basic::exchange io;
    cgiIO provider(io, headers);

    xml::server srv(provider);

    try {
        srv.setHandler("callTest", ::handler);

        srv.serve();
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }

    return 0;
}
