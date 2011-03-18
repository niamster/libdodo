/***************************************************************************
 *            ioNetworkHttpClient.cc
 *
 *  Wed Oct 8 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ioSsl.inline"

#include <libdodo/ioNetworkHttpClient.h>
#include <libdodo/toolsCode.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetworkClient.h>
#include <libdodo/ioNetworkSslClient.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/ioNetworkHttpClientEx.h>
#include <libdodo/ioNetworkClientEx.h>
#include <libdodo/ioNetworkSslClientEx.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioNetworkExchangeEx.h>
#include <libdodo/ioNetworkSslExchangeEx.h>
#include <libdodo/ioNetworkSslExchange.h>
#include <libdodo/types.h>
#include <libdodo/cgi.h>
#include <libdodo/toolsMisc.h>

using namespace dodo::io::network::http;

const dodo::string client::requestHeaderStatements[] = {
    "Accept",
    "Accept-Charset",
    "Accept-Encoding",
    "Accept-Language",
    "Accept-Ranges",
    "Authorization",
    "Proxy-Authorization",
    "Connection",
    "Date",
    "If-Modified-Since",
    "User-Agent",
    "Cookie",
};

//-------------------------------------------------------------------

const dodo::string client::responseHeaderStatements[] = {
    "Accept-Ranges",
    "Age",
    "Allow",
    "Cache-Control",
    "Content-Encoding",
    "Content-Language",
    "Content-Length",
    "Content-Location",
    "Content-Disposition",
    "Content-MD5",
    "Content-Range",
    "Content-Type",
    "Date",
    "Last-Modified",
    "Location",
    "Server",
    "Transfer-Encoding",
    "WWW-Authenticate",
    "Proxy-Authenticate",
    "X-Powered-By",
};

//-------------------------------------------------------------------

response::response() : code(0),
                       redirected(false)
{
}

//-------------------------------------------------------------------

file::file(const dodo::string path,
           const dodo::string mime) : path(path),
                                    mime(mime)
{
}

//-------------------------------------------------------------------

file::file()
{
}

//-------------------------------------------------------------------

client::client() : followRedirection(true),
                   cacheAuthentification(true),
                   authTries(0),
                   scheme(SCHEME_HTTP)
#ifdef OPENSSL_EXT
                   ,
                   certsSet(false)
#endif
{
    requestHeaders[REQUEST_HEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
    requestHeaders[REQUEST_HEADER_ACCEPT] = "*/*";
    requestHeaders[REQUEST_HEADER_CONNECTION] = "Close";
}

//-------------------------------------------------------------------

client::client(const dodo::string &url) : followRedirection(true),
                                        cacheAuthentification(true),
                                        authTries(0),
                                        scheme(SCHEME_HTTP)
#ifdef OPENSSL_EXT
                                        ,
                                        certsSet(false)
#endif
{
    requestHeaders[REQUEST_HEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
    requestHeaders[REQUEST_HEADER_ACCEPT] = "*/*";
    requestHeaders[REQUEST_HEADER_CONNECTION] = "Close";

    setUrl(url);
}

//-------------------------------------------------------------------

client::client(client &)
{
}

//-------------------------------------------------------------------

client::~client()
{
}

//-------------------------------------------------------------------

short
client::getStatusCode(const dodo::string &header) const
{
    if (header.size() < 12)
        return 0;

    short code = 0;

    code += ((short)header[9] - 0x30) * 100;
    code += ((short)header[10] - 0x30) * 10;
    code += (short)header[11] - 0x30;

    return code;
}

//-------------------------------------------------------------------

#ifdef OPENSSL_EXT
void
client::setSertificates(const io::ssl::__certificates__ &a_certs) const
{
    certs = a_certs;
    if ((certs.key.size() +
         certs.keyPassword.size() +
         certs.cert.size() +
         certs.ca.size() +
         certs.caPath.size() +
         certs.cipher.size()) > 0)
        certsSet = true;
    else
        certsSet = false;
}
#endif

//-------------------------------------------------------------------

void
client::setUrl(const dodo::string &a_url) const
{
    urlComponents = tools::code::parseUrl(a_url);

    if (urlComponents.protocol.size() == 0 || tools::string::iequal(urlComponents.protocol, "http")) {
        urlComponents.protocol = "http";
        scheme = SCHEME_HTTP;
    } else {
#ifdef OPENSSL_EXT
        if (tools::string::iequal(urlComponents.protocol, "https"))
            scheme = SCHEME_HTTPS;
        else
#endif

        dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_SETURL, exception::ERRNO_LIBDODO, CLIENTEX_UNSUPPORTEDSURICHEME, IONETWORKHTTPCLIENTEX_UNSUPPORTEDSURICHEME_STR, __LINE__, __FILE__);
    }

    unsigned long portSize = urlComponents.port.size();

    if (portSize == 0) {
        if (scheme == SCHEME_HTTP)
            urlComponents.port = "80";

#ifdef OPENSSL_EXT
        else if (scheme == SCHEME_HTTPS)
            urlComponents.port = "443";

#endif
    }

    urlBasePath.clear();
    urlQuery.clear();

    urlBasePath += urlComponents.protocol;
    urlBasePath += "://" ;
    urlBasePath += urlComponents.host;
    if (portSize > 0) {
        urlBasePath += ":";
        urlBasePath += urlComponents.port;
    }
    urlBasePath += "/";
    urlBasePath += urlComponents.path;

    if (urlComponents.request.size() > 0) {
        urlQuery += "?";
        urlQuery += tools::code::encodeUrl(urlComponents.request);
    }
}

//-------------------------------------------------------------------

void
client::setCookies(const dodoStringMap &cookies) const
{
    dodo::string data;

    dodoStringMap::const_iterator i = cookies.begin(), j = cookies.end();
    --j;

    for (; i != j; ++i) {
        data += i->first;
        data += "=";
        data += tools::code::encodeUrl(i->second);
        data += "; ";
    }
    data += i->first;
    data += "=";
    data += tools::code::encodeUrl(i->second);

    requestHeaders[REQUEST_HEADER_COOKIE] = data;
}

//-------------------------------------------------------------------

response
client::GET() const
{
    response response;

    exchange *ex = NULL;
    network::client *net = NULL;

    dodo::string data;

    if (scheme == SCHEME_HTTP) {
        net = new network::client(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
        ex = new exchange;
    }

#ifdef OPENSSL_EXT
    else {
        net = new ssl::client(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
        ex = new ssl::exchange;
    }
#endif

    if (proxyAuthInfo.host.size() > 0) {
        if (scheme == SCHEME_HTTP)
            net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);

#ifdef OPENSSL_EXT
        else {
            net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);
            data += "CONNECT ";
            data += urlComponents.host;
            data += ":";
            data += urlComponents.port;
            data += " HTTP/1.1\r\n";
            if (requestHeaders.find(REQUEST_HEADER_PROXYAUTHORIZATION) != requestHeaders.end()) {
                data += requestHeaderStatements[REQUEST_HEADER_PROXYAUTHORIZATION];
                data += ": ";
                data += requestHeaders[REQUEST_HEADER_PROXYAUTHORIZATION];
                data += "\r\n";
            }
            data += "\r\n";

            unsigned long bs = ex->bs;
            ex->bs = data.size();
            ex->exchange::_write(data.data());
            ex->bs = bs;

            dodo_try {
                switch (getProxyConnectResponse(ex, response)) {
                    case GETCONTENTSTATUS_NORMAL:

                        break;

                    case GETCONTENTSTATUS_PROXYBASICAUTH:

                        if (authTries > 2) {
                            authTries = 0;

                            dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                        }

                        makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

                        delete ex;
                        ex = NULL;
                        delete net;
                        net = NULL;

                        return GET();

                    case GETCONTENTSTATUS_PROXYDIGESTAUTH:

                        if (authTries > 2) {
                            authTries = 0;

                            dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                        }

                        makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password, response);

                        delete ex;
                        ex = NULL;
                        delete net;
                        net = NULL;

                        return GET();
                }
            } dodo_catch (exception::basic *e UNUSED) {
                delete ex;
                delete net;

                clear();

                dodo_rethrow;
            }

            if (certsSet)
                ((ssl::client *)net)->setSertificates(certs);
            else
                ((ssl::client *)net)->initSsl();

            net->socket = ex->socket;
            ((ssl::client *)net)->connectSsl();

            ((ssl::exchange *)ex)->handle->handle = ((ssl::client *)net)->handle->handle;

            net->socket = -1;
            ((ssl::client *)net)->handle->handle = NULL;
        }
#endif
    } else {
        tools::network::__host__ host = tools::network::host(urlComponents.host);

        dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
        for (; o != p; ++o) {
            dodo_try {
                if (scheme == SCHEME_HTTP) {
                    net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
                    ex->setInBufferSize(512);
                    ex->bs = 512;
                }

#ifdef OPENSSL_EXT
                else {
                    if (certsSet)
                        ((ssl::client *)net)->setSertificates(certs);

                    net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
                    ex->setInBufferSize(512);
                    ex->bs = 512;
                }
#endif

                break;
            } dodo_catch (exception::basic *e UNUSED) {
#ifdef OPENSSL_EXT
                if (e->function == CLIENTEX_CONNECT || e->function == ssl::CLIENTEX_CONNECT)

#else
                if (e->function == CLIENTEX_CONNECT)
#endif
                {
                    if ((o + 1) == p) {
                        delete net;
                        delete ex;

                        dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_CANNOTCONNECT, IONETWORKHTTPCLIENTEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
                    } else
                        continue;
                }

                delete net;
                delete ex;

                dodo_rethrow;
            }
        }
    }

    delete net;

    data.clear();

    data += "GET ";
    data += urlBasePath;
    data += urlQuery;
    data += " HTTP/1.1\r\n";

    if (cacheAuthentification) {
        dodoStringMap::iterator header = httpAuth.find(urlBasePath);
        if (header != httpAuth.end())
            requestHeaders[REQUEST_HEADER_AUTHORIZATION] = header->second;
    }

    dodoMap<short, dodo::string>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
    for (; i != j; ++i) {
#ifdef OPENSSL_EXT
        if (proxyAuthInfo.host.size() > 0 && scheme == SCHEME_HTTPS && i->first == REQUEST_HEADER_PROXYAUTHORIZATION)
            continue;

#endif

        data += requestHeaderStatements[i->first];
        data += ": ";
        data += i->second;
        data += "\r\n";
    }
    data += "Host: ";
    data += urlComponents.host;

    data += "\r\n\r\n";

    unsigned long bs = ex->bs;

    ex->bs = data.size();
    ex->write(data);

    ex->bs = bs;

    dodo_try {
        switch (getContent(ex, response)) {
            case GETCONTENTSTATUS_NORMAL:

                break;

            case GETCONTENTSTATUS_REDIRECT:

                setUrl(response.headers[RESPONSE_HEADER_LOCATION]);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_PROXYBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_WWWBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_PROXYDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password, response);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_WWWDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_WWWAUTHENTICATE, REQUEST_HEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password, response);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_WWWPROXYBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

                if (proxyAuthInfo.authType == PROXY_AUTH_BASIC)
                    makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
                else
                    makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password, response);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_WWWAUTHENTICATE, REQUEST_HEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password, response);

                if (proxyAuthInfo.authType == PROXY_AUTH_BASIC)
                    makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
                else
                    makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password, response);

                delete ex;
                ex = NULL;

                return GET();
        }
    } dodo_catch (exception::basic *e UNUSED) {
        delete ex;

        clear();

        dodo_rethrow;
    }

    delete ex;

    clear();

    return response;
}

//-------------------------------------------------------------------

response
client::GET(const dodo::string &a_url) const
{
    setUrl(a_url);

    return GET();
}

//-------------------------------------------------------------------

response
client::POST(const dodo::string &a_url,
             const dodoStringMap &arguments,
             const dodoMap<dodo::string, file> &files) const
{
    setUrl(a_url);

    return POST(arguments, files);
}

//-------------------------------------------------------------------

response
client::POST(const dodoStringMap &arguments,
             const dodoMap<dodo::string, file> &files) const
{
    dodo::string boundary = "---------------------------" + tools::string::ulToString(tools::misc::random<unsigned long>()) + tools::string::ulToString(tools::misc::random<unsigned long>());
    dodo::string type = "multipart/form-data; boundary=" + boundary;
    boundary.insert(0, "--");

    dodo::string data;

    dodoMap<dodo::string, file>::const_iterator i = files.begin(), j = files.end();
    for (; i != j; ++i) {
        data += boundary;
        data += "\r\nContent-Disposition: form-data; name=\"";
        data += i->first;
        data += "\"; filename=\"";
        data += tools::filesystem::basename(i->second.path);
        data += "\"\r\n";

        data += "Content-Type: ";
        data += i->second.mime;
        data += "\r\n\r\n";

        data += tools::filesystem::fileContents(i->second.path);
        data += "\r\n";
    }

    dodoStringMap::const_iterator o = arguments.begin(), p = arguments.end();
    for (; o != p; ++o) {
        data += boundary;
        data += "\r\nContent-Disposition: form-data; name=\"";
        data += o->first;
        data += "\"\r\n\r\n";

        data += o->second;
        data += "\r\n";
    }
    data += boundary;
    data += "--";

    return POST(data, type);
}

//-------------------------------------------------------------------

response
client::POST(const dodo::string    &a_url,
             const dodoStringMap &arguments) const
{
    setUrl(a_url);

    return POST(arguments);
}

//-------------------------------------------------------------------

response
client::POST(const dodoStringMap &arguments) const
{
    dodo::string data;

    dodoStringMap::const_iterator i(arguments.begin()), j(arguments.end());
    --j;

    for (; i != j; ++i) {
        data += tools::code::encodeUrl(i->first);
        data += "=";
        data += tools::code::encodeUrl(i->second);
        data += "&";
    }
    data += tools::code::encodeUrl(i->first);
    data += "=";
    data += tools::code::encodeUrl(i->second);

    return POST(data, "application/x-www-form-urlencoded");
}

//-------------------------------------------------------------------

response
client::POST(const dodo::string &a_url,
             const dodo::string &data,
             const dodo::string &type) const
{
    setUrl(a_url);

    return POST(data, type);
}

//-------------------------------------------------------------------

response
client::POST(const dodo::string &rdata,
             const dodo::string &type) const
{
    response response;

    exchange *ex = NULL;
    network::client *net = NULL;

    dodo::string data;

    if (scheme == SCHEME_HTTP) {
        net = new network::client(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
        ex = new exchange;
    }

#ifdef OPENSSL_EXT
    else {
        net = new ssl::client(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
        ex = new ssl::exchange;
    }
#endif

    if (proxyAuthInfo.host.size() > 0) {
        if (scheme == SCHEME_HTTP)
            net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);

#ifdef OPENSSL_EXT
        else {
            net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *(exchange *)ex);
            data += "CONNECT ";
            data += urlComponents.host;
            data += ":";
            data += urlComponents.port;
            data += " HTTP/1.1\r\n";
            if (requestHeaders.find(REQUEST_HEADER_PROXYAUTHORIZATION) != requestHeaders.end()) {
                data += requestHeaderStatements[REQUEST_HEADER_PROXYAUTHORIZATION];
                data += ": ";
                data += requestHeaders[REQUEST_HEADER_PROXYAUTHORIZATION];
                data += "\r\n";
            }
            data += "\r\n";

            unsigned long bs = ex->bs;
            ex->bs = data.size();
            ex->exchange::_write(data.data());
            ex->bs = bs;

            dodo_try {
                switch (getProxyConnectResponse(ex, response)) {
                    case GETCONTENTSTATUS_NORMAL:

                        break;

                    case GETCONTENTSTATUS_PROXYBASICAUTH:

                        if (authTries > 2) {
                            authTries = 0;

                            dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                        }

                        makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

                        delete ex;
                        ex = NULL;
                        delete net;
                        net = NULL;

                        return POST(data, type);

                    case GETCONTENTSTATUS_PROXYDIGESTAUTH:

                        if (authTries > 2) {
                            authTries = 0;

                            dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                        }

                        makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password, response);

                        delete ex;
                        ex = NULL;
                        delete net;
                        net = NULL;

                        return POST(data, type);
                }
            } dodo_catch (exception::basic *e UNUSED) {
                delete ex;
                delete net;

                clear();

                dodo_rethrow;
            }

            if (certsSet)
                ((ssl::client *)net)->setSertificates(certs);
            else
                ((ssl::client *)net)->initSsl();

            net->socket = ex->socket;
            ((ssl::client *)net)->connectSsl();

            ((ssl::exchange *)ex)->handle->handle = ((ssl::client *)net)->handle->handle;

            net->socket = -1;
            ((ssl::client *)net)->handle->handle = NULL;
        }
#endif
    } else {
        tools::network::__host__ host = tools::network::host(urlComponents.host);

        dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
        for (; o != p; ++o) {
            dodo_try {
                if (scheme == SCHEME_HTTP) {
                    net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
                    ex->setInBufferSize(512);
                    ex->bs = 512;
                }

#ifdef OPENSSL_EXT
                else {
                    if (certsSet)
                        ((ssl::client *)net)->setSertificates(certs);

                    net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
                    ex->setInBufferSize(512);
                    ex->bs = 512;
                }
#endif

                break;
            } dodo_catch (exception::basic *e UNUSED) {
#ifdef OPENSSL_EXT
                if (e->function == CLIENTEX_CONNECT || e->function == ssl::CLIENTEX_CONNECT)
#else
                if (e->function == CLIENTEX_CONNECT)
#endif
                {
                    if ((o + 1) == p) {
                        delete net;
                        delete ex;

                        dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_CANNOTCONNECT, IONETWORKHTTPCLIENTEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
                    } else
                        continue;
                }

                delete net;
                delete ex;

                dodo_rethrow;
            }
        }
    }

    delete net;

    data += "POST ";
    data += urlBasePath;
    data += urlQuery;
    data += " HTTP/1.1\r\n";

    if (cacheAuthentification) {
        dodoStringMap::iterator header = httpAuth.find(urlBasePath);
        if (header != httpAuth.end())
            requestHeaders[REQUEST_HEADER_AUTHORIZATION] = header->second;
    }

    dodoMap<short, dodo::string>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
    for (; i != j; ++i) {
#ifdef OPENSSL_EXT
        if (proxyAuthInfo.host.size() > 0 && scheme == SCHEME_HTTPS && i->first == REQUEST_HEADER_PROXYAUTHORIZATION)
            continue;

#endif

        data += requestHeaderStatements[i->first];
        data += ": ";
        data += i->second;
        data += "\r\n";
    }
    data += "Host: ";
    data += urlComponents.host;
    data += "\r\n";

    data += "Content-length: ";
    data += tools::string::ulToString(rdata.size());
    data += "\r\n";

    data += "Content-type: ";
    data += type;
    data += "\r\n\r\n";

    unsigned long bs = ex->bs;

    ex->bs = data.size();
    ex->write(data);

    ex->bs = rdata.size();
    ex->write(rdata);

    ex->bs = bs;

    dodo_try {
        switch (getContent(ex, response)) {
            case GETCONTENTSTATUS_NORMAL:

                break;

            case GETCONTENTSTATUS_REDIRECT:

                setUrl(response.headers[RESPONSE_HEADER_LOCATION]);

                return POST(rdata, type);

            case GETCONTENTSTATUS_PROXYBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

                return POST(rdata, type);

            case GETCONTENTSTATUS_WWWBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

                return POST(rdata, type);

            case GETCONTENTSTATUS_PROXYDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password, response);

                return POST(rdata, type);

            case GETCONTENTSTATUS_WWWDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_WWWAUTHENTICATE, REQUEST_HEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password, response);

                return POST(rdata, type);

            case GETCONTENTSTATUS_WWWPROXYBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

                if (proxyAuthInfo.authType == PROXY_AUTH_BASIC)
                    makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
                else
                    makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password, response);

                return POST(rdata, type);

            case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_WWWAUTHENTICATE, REQUEST_HEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password, response);

                if (proxyAuthInfo.authType == PROXY_AUTH_BASIC)
                    makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
                else
                    makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password, response);

                return POST(rdata, type);
        }
    } dodo_catch (exception::basic *e UNUSED) {
        delete ex;

        clear();

        dodo_rethrow;
    }

    delete ex;

    clear();

    return response;
}

//-------------------------------------------------------------------

void
client::setProxy(const dodo::string &host,
                 unsigned int     port,
                 const dodo::string &user,
                 const dodo::string &password) const
{
    proxyAuthInfo.host = host;
    proxyAuthInfo.port = port;
    proxyAuthInfo.user = user;
    proxyAuthInfo.password = password;
    proxyAuthInfo.authType = PROXY_AUTH_NONE;

    requestHeaders.erase(REQUEST_HEADER_PROXYAUTHORIZATION);
}

//-------------------------------------------------------------------

void
client::getHeaders(const dodo::string &headers,
                   response         &response) const
{
    unsigned long i(0), j(0);
    unsigned long size = headers.size();

    dodoStringArray arr;
    dodo::string piece;

    short o;

    bool statusCode = false;

    while (i < size) {
        i = headers.find("\n", i);
        if (i == dodo::string::POSITION_END)
            return;

        piece = tools::string::trim(dodo::string(headers.data() + j, i - j), '\r');

        arr = tools::misc::split(piece, ":", 2);
        if (arr.size() != 2) {
            if (!statusCode) {
                statusCode = true;

                response.code = getStatusCode(piece);
            }
        } else {
            for (o = 0; o < RESPONSE_HEADER_ENUMSIZE; ++o)
                if (tools::string::equal(responseHeaderStatements[o], arr[0]))
                    response.headers[o] = tools::string::lTrim(arr[1], ' ');

            if (tools::string::equal("Set-Cookie", arr[0]))
                response.cookies.push_back(parseCookie(tools::string::lTrim(arr[1], ' ')));
        }

        ++i;
        j = i;
    }
}

//-------------------------------------------------------------------

unsigned int
client::extractHeaders(const dodo::string &data,
                       dodo::string       &headers) const
{
    headers += data;

    unsigned long i = headers.find("\r\n\r\n");
    if (i == dodo::string::POSITION_END) {
        i = headers.find("\n\n");
        if (i == dodo::string::POSITION_END)
            return 0;
        else {
            headers.erase(i + 1);

            return i + 2;
        }
    } else {
        headers.erase(i + 2);

        return i + 4;
    }

    return 0;
}

//-------------------------------------------------------------------

short
client::getProxyConnectResponse(exchange *ex,
                                response &response) const
{
    unsigned long size = 0, i;
    bool endOfHeaders = false;

    dodo::string headers;

    char data[512];
    ex->setInBufferSize(512);
    ex->bs = 512;

    while (true) {
        dodo_try {
            size = ex->exchange::_readString(data);

            if (size == 0)
                break;

            headers += data;

            i = headers.find("\r\n\r\n");
            if (i == dodo::string::POSITION_END) {
                i = headers.find("\n\n");
                if (i != dodo::string::POSITION_END) {
                    headers.erase(i + 1);

                    endOfHeaders = true;
                }
            } else {
                headers.erase(i + 2);

                endOfHeaders = true;
            }

            if (endOfHeaders) {
                getHeaders(headers, response);

                if (response.code == 407) {
                    ++authTries;

                    if (tools::string::contains(response.headers[RESPONSE_HEADER_PROXYAUTHENTICATE], "Basic")) {
                        proxyAuthInfo.authType = PROXY_AUTH_BASIC;

                        return GETCONTENTSTATUS_PROXYBASICAUTH;
                    } else {
                        if (tools::string::contains(response.headers[RESPONSE_HEADER_PROXYAUTHENTICATE], "Digest")) {
                            proxyAuthInfo.authType = PROXY_AUTH_DIGEST;

                            return GETCONTENTSTATUS_PROXYDIGESTAUTH;
                        } else
                            dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GETPROXYCONNECTRESPONSE, exception::ERRNO_LIBDODO, CLIENTEX_UNKNOWNPROXYAUTHTYPE, IONETWORKHTTPCLIENTEX_UNKNOWNPROXYAUTH_STR, __LINE__, __FILE__);
                    }
                }

                break;
            }
        } dodo_catch (exception::basic *e UNUSED) {
            if (e->function == EXCHANGEEX__READSTRING)
                break;
            else
                dodo_rethrow;
        }
    }

    authTries = 0;

    return GETCONTENTSTATUS_NORMAL;
}

//-------------------------------------------------------------------

short
client::getContent(exchange *ex,
                   response &response) const
{
    dodo::string data;

    unsigned long contentSize = 0;

    unsigned long chunkSize = 0;
    dodo::string chunkSizeHex;

    unsigned long eoc;

    unsigned int endOfHeaders = 0;
    bool chunked = false;

    dodo::string headers;

    while (true) {
        dodo_try {
            if (chunked) {
                if (chunkSize > 0) {
                    if (chunkSize > data.size()) {
                        ex->bs = chunkSize - data.size();
                        data += ex->read();
                    }

                    response.data += data.substr(0, chunkSize);

                    data.erase(0, chunkSize);
                    chunkSize = 0;
                }

                if (data.size() == 0) {
                    ex->bs = 512;
                    data = ex->readString();

                    if (data.size() == 0)
                        break;
                }

                eoc = data.find("\r\n");
                if (eoc == dodo::string::POSITION_END) {
                    eoc = data.find('\n');
                    if (eoc != dodo::string::POSITION_END)
                        ++eoc;
                } else {
                    eoc += 2;
                }

                if (eoc != dodo::string::POSITION_END) {
                    chunkSizeHex.clear();

                    for (unsigned long i = 0; i < eoc; ++i) {
                        if (data[i] == '\r')
                            continue;
                        if (data[i] == '\n' || data[i] == ';')
                            break;

                        chunkSizeHex += data[i];
                    }
                    data.erase(0, eoc);

                    if (chunkSizeHex.size() == 0) {
                        chunkSize = 0;

                        continue;
                    }

                    chunkSize = tools::code::hexToLong(chunkSizeHex);
                    if (chunkSize == 0)
                        break;
                } else {
                    ex->bs = 512;
                    data += ex->readString();
                }
            } else {
                data = ex->readString();

                if (data.size() == 0 && contentSize <= 0) {
                    if (endOfHeaders == 0 && headers.size() > 0)
                        response.data = headers;

                    break;
                }

                if (endOfHeaders != 0) {
                    response.data += data;
                } else {
                    endOfHeaders = extractHeaders(data, headers);

                    if (endOfHeaders > 0) {
                        getHeaders(headers, response);
                        headers.clear();

                        if (followRedirection && (response.code / 100) == 3 && response.code != 304) {
                            response.redirected = true;

                            return GETCONTENTSTATUS_REDIRECT;
                        }

                        if (response.code == 401) {
                            ++authTries;

                            if (proxyAuthInfo.authType != PROXY_AUTH_NONE) {
                                if (tools::string::contains(response.headers[RESPONSE_HEADER_WWWAUTHENTICATE], "Basic")) {
                                    return GETCONTENTSTATUS_WWWPROXYBASICAUTH;
                                } else {
                                    if (tools::string::contains(response.headers[RESPONSE_HEADER_WWWAUTHENTICATE], "Digest"))
                                        return GETCONTENTSTATUS_WWWPROXYDIGESTAUTH;
                                    else
                                        dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GETCONTENT, exception::ERRNO_LIBDODO, CLIENTEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPCLIENTEX_UNKNOWNWWWAUTH_STR, __LINE__, __FILE__);
                                }
                            } else {
                                if (tools::string::contains(response.headers[RESPONSE_HEADER_WWWAUTHENTICATE], "Basic")) {
                                    return GETCONTENTSTATUS_WWWBASICAUTH;
                                } else {
                                    if (tools::string::contains(response.headers[RESPONSE_HEADER_WWWAUTHENTICATE], "Digest"))
                                        return GETCONTENTSTATUS_WWWDIGESTAUTH;
                                    else
                                        dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GETCONTENT, exception::ERRNO_LIBDODO, CLIENTEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPCLIENTEX_UNKNOWNWWWAUTH_STR, __LINE__, __FILE__);
                                }
                            }
                        }

                        if (response.code == 407) {
                            ++authTries;

                            if (tools::string::contains(response.headers[RESPONSE_HEADER_PROXYAUTHENTICATE], "Basic")) {
                                proxyAuthInfo.authType = PROXY_AUTH_BASIC;

                                return GETCONTENTSTATUS_PROXYBASICAUTH;
                            } else {
                                if (tools::string::contains(response.headers[RESPONSE_HEADER_PROXYAUTHENTICATE], "Digest")) {
                                    proxyAuthInfo.authType = PROXY_AUTH_DIGEST;

                                    return GETCONTENTSTATUS_PROXYDIGESTAUTH;
                                } else {
                                    dodo_throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GETCONTENT, exception::ERRNO_LIBDODO, CLIENTEX_UNKNOWNPROXYAUTHTYPE, IONETWORKHTTPCLIENTEX_UNKNOWNPROXYAUTH_STR, __LINE__, __FILE__);
                                }
                            }
                        }

                        chunked = tools::string::equal(response.headers[RESPONSE_HEADER_TRANSFERENCODING], "chunked");

                        if (chunked) {
                            data.erase(0, endOfHeaders);
                        } else {
                            response.data = data.substr(0, endOfHeaders);

                            contentSize = tools::string::stringToUL(response.headers[RESPONSE_HEADER_CONTENTLENGTH]);

                            ex->bs = 16384;
                        }

                        ex->setInBufferSize(16384);
                    }
                }

                if (contentSize > 0 && response.data.size() == contentSize)
                    break;
            }
        } dodo_catch (exception::basic *e UNUSED) {
#ifdef OPENSSL_EXT
            if (e->function == EXCHANGEEX__READSTRING || e->function == ssl::EXCHANGEEX__READSTRING)
#else
            if (e->function == EXCHANGEEX__READSTRING)
#endif
            {
                if (!endOfHeaders && headers.size() > 0)
                    response.data = headers;

                break;
            } else {
                dodo_rethrow;
            }
        }
    }

    authTries = 0;

    return GETCONTENTSTATUS_NORMAL;
}

//-------------------------------------------------------------------

void
client::makeDigestAuth(short            requestHeader,
                       short            responseHeader,
                       const dodo::string &method,
                       const dodo::string &user,
                       const dodo::string &password,
                       response         &response) const
{
    dodo::string nonce, opaque, realm;

    dodo::string &rh = response.headers[requestHeader];
    dodoStringArray parts = tools::misc::split(dodo::string(rh.data() + 7, rh.size() - 7), ",");

    dodo::string HA1;

    unsigned char HA[16];
    tools::code::__MD5Context__ context;

    dodoStringArray tuple;

    dodoStringArray::iterator i = parts.begin(), j = parts.end();
    for (; i != j; ++i) {
        tuple = tools::misc::split(tools::string::trim(*i, ' '), "=");
        if (tuple.size() != 2)
            continue;

        if (tools::string::iequal(tuple[0], "realm")) {
            realm = tools::string::trim(tuple[1], '"');

            tools::code::MD5Init(&context);
            tools::code::MD5Update(&context, (unsigned char *)user.data(), user.size());
            tools::code::MD5Update(&context, (unsigned char *)":", 1);
            tools::code::MD5Update(&context, (unsigned char *)realm.data(), realm.size());
            tools::code::MD5Update(&context, (unsigned char *)":", 1);
            tools::code::MD5Update(&context, (unsigned char *)password.data(), password.size());
            tools::code::MD5Final(HA, &context);

            HA1 = tools::code::binToHex(dodo::string((char *)&HA, 16));
        } else {
            if (tools::string::iequal(tuple[0], "nonce"))
                nonce = tools::string::trim(tuple[1], '"');
            else if (tools::string::iequal(tuple[0], "opaque"))
                opaque = tools::string::trim(tuple[1], '"');
        }
    }

    dodo::string cnonce = tools::code::MD5Hex(tools::misc::randomString(5));

    dodo::string methodForAuth = method + ":";

    dodo::string url = urlBasePath;
    url += urlQuery;

    tools::code::MD5Init(&context);
    tools::code::MD5Update(&context, (unsigned char *)methodForAuth.data(), methodForAuth.size());
    tools::code::MD5Update(&context, (unsigned char *)url.data(), url.size());
    tools::code::MD5Final(HA, &context);

    dodo::string HA2 = tools::code::binToHex(dodo::string((char *)&HA, 16));

    tools::code::MD5Init(&context);
    tools::code::MD5Update(&context, (unsigned char *)HA1.data(), HA1.size());
    tools::code::MD5Update(&context, (unsigned char *)":", 1);
    tools::code::MD5Update(&context, (unsigned char *)nonce.data(), nonce.size());
    tools::code::MD5Update(&context, (unsigned char *)":00000001:", 10);
    tools::code::MD5Update(&context, (unsigned char *)cnonce.data(), cnonce.size());
    tools::code::MD5Update(&context, (unsigned char *)":auth:", 6);
    tools::code::MD5Update(&context, (unsigned char *)HA2.data(), HA2.size());
    tools::code::MD5Final(HA, &context);

    {
        dodo::string response = tools::code::binToHex(dodo::string((char *)&HA, 16));

        requestHeaders[responseHeader] = "Digest username=\"" + user +
                                         "\", realm=\"" + realm +
                                         + "\", nonce=\"" + nonce +
                                         + "\", uri=\"" + url +
                                         + "\", qop=\"auth\", nc=00000001" +
                                         + ", cnonce=\"" + cnonce +
                                         + "\", response=\"" + response +
                                         + "\", opaque=\"" + opaque + "\"";
    }
}

//-------------------------------------------------------------------

void
client::makeBasicAuth(short            responseHeader,
                      const dodo::string &user,
                      const dodo::string &password) const
{
    requestHeaders[responseHeader] = "Basic " + tools::code::encodeBase64(user + ":" + password);
}

//-------------------------------------------------------------------

void
client::clear() const
{
    requestHeaders.erase(REQUEST_HEADER_COOKIE);

    if (!cacheAuthentification) {
        requestHeaders.erase(REQUEST_HEADER_PROXYAUTHORIZATION);
        proxyAuthInfo.authType = PROXY_AUTH_NONE;
    } else {
        dodoMap<short, dodo::string>::iterator header = requestHeaders.find(REQUEST_HEADER_AUTHORIZATION);
        if (header != requestHeaders.end())
            httpAuth[urlBasePath] = header->second;
    }

    requestHeaders.erase(REQUEST_HEADER_AUTHORIZATION);
}

//-------------------------------------------------------------------

void
client::setAuth(const dodo::string &user,
                const dodo::string &password) const
{
    urlComponents.login = user;
    urlComponents.password = password;
}

//-------------------------------------------------------------------

dodo::cgi::cookie
client::parseCookie(const dodo::string &header) const
{
    dodoStringArray parts = tools::misc::split(header, ";");

    dodoStringArray tuple;

    dodoStringArray::iterator i = parts.begin(), j = parts.end();

    tuple = tools::misc::split(*i, "=", 2);
    if (tuple.size() != 2)
        return cgi::cookie();

    cgi::cookie cookie;
    cookie.name = tuple[0];
    cookie.value = tools::code::decodeUrl(tuple[1]);

    ++i;

    for (; i != j; ++i) {
        tuple = tools::misc::split(tools::string::trim(*i, ' '), "=");

        if (tools::string::iequal(tuple[0], "path"))
            cookie.path = tuple[1];
        else {
            if (tools::string::iequal(tuple[0], "expires"))
                cookie.expires = tuple[1];
            else {
                if (tools::string::iequal(tuple[0], "domain"))
                    cookie.domain = tuple[1];
                else if (tools::string::iequal(tuple[0], "secure"))
                    cookie.secure = true;
            }
        }
    }

    return cookie;
}

//-------------------------------------------------------------------
