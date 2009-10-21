/***************************************************************************
 *            ioNetworkSslServer.cc
 *  Tue Jun 10 2008
 *  Copyright  2008  Ni@m

 *  niam.niam@gmail.com
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

#ifdef OPENSSL_EXT
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/stat.h>

#include "ioSsl.inline"

#include <libdodo/ioNetworkSslServer.h>
#include <libdodo/ioNetworkSslClient.h>
#include <libdodo/pcSyncProtector.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetworkSslServerEx.h>
#include <libdodo/ioNetworkServer.h>
#include <libdodo/ioSsl.h>
#include <libdodo/types.h>
#include <libdodo/ioNetworkSslExchange.h>
#include <libdodo/xexec.h>

using namespace dodo::io::network::ssl;

server::server(server &fs) : network::server(fs)
{
}

//-------------------------------------------------------------------

server::server(short a_family,
               short a_type) : network::server(a_family,
                                               a_type),
                               ctx(new io::ssl::__context__)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IONETWORKSSLSERVER);
#endif
}

//-------------------------------------------------------------------

server::~server()
{
    if (ctx->ctx != NULL)
        SSL_CTX_free(ctx->ctx);

    delete ctx;
}

//-------------------------------------------------------------------

void
server::removeSertificates()
{
    if (ctx->ctx != NULL) {
        SSL_CTX_free(ctx->ctx);

        ctx->ctx = NULL;
    }

    ctx->ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx->ctx == NULL)
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_REMOVESERTIFICATES, exception::ERRNO_LIBDODO, SERVEREX_UNABLETOINITCONTEXT, IONETWORKSSLSERVEREX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
server::setSertificates(const io::ssl::__certificates__ &certs)
{
    if (ctx->ctx != NULL)
        SSL_CTX_free(ctx->ctx);

    ctx->ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx->ctx == NULL)
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_LIBDODO, SERVEREX_UNABLETOINITCONTEXT, IONETWORKSSLSERVEREX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);

    if (certs.cipher.size() > 0 && SSL_CTX_set_cipher_list(ctx->ctx, certs.cipher.data()) != 1) {
        unsigned long nerr = ERR_get_error();
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
    }

    if (certs.ca.size() > 0 && SSL_CTX_use_certificate_chain_file(ctx->ctx, certs.ca.data()) != 1) {
        unsigned long nerr = ERR_get_error();
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
    }

    if (certs.cert.size() > 0 && SSL_CTX_use_certificate_file(ctx->ctx, certs.cert.data(), SSL_FILETYPE_PEM) != 1) {
        unsigned long nerr = ERR_get_error();
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
    }

    if (certs.keyPassword.size() > 0)
        SSL_CTX_set_default_passwd_cb_userdata(ctx->ctx, (void *)certs.keyPassword.data());

    bool keySet = false;

    if (certs.key.size() > 0) {
        switch (certs.keyType) {
            case io::ssl::KEY_PKEY:

                if (SSL_CTX_use_PrivateKey_file(ctx->ctx, certs.key.data(), SSL_FILETYPE_PEM) != 1) {
                    unsigned long nerr = ERR_get_error();
                    throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
                }

                keySet = true;

                break;

            case io::ssl::KEY_RSA:

                if (SSL_CTX_use_RSAPrivateKey_file(ctx->ctx, certs.key.data(), SSL_FILETYPE_PEM) != 1) {
                    unsigned long nerr = ERR_get_error();
                    throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
                }

                keySet = true;

                break;

            default:

                throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_LIBDODO, SERVEREX_UNKNOWNKEYTYPE, IONETWORKSSLSERVEREX_UNKNOWNKEY_STR, __LINE__, __FILE__);
        }
    } else {
        if (certs.ca.size() > 0) {
            if (SSL_CTX_use_PrivateKey_file(ctx->ctx, certs.ca.data(), SSL_FILETYPE_PEM) != 1) {
                unsigned long nerr = ERR_get_error();
                throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
            }

            keySet = true;
        }
    }

    if (certs.caPath.size() > 0) {
        if (tools::filesystem::file(certs.caPath).type == tools::filesystem::FILE_DIRECTORY) {
            if (SSL_CTX_load_verify_locations(ctx->ctx, NULL, certs.caPath.data()) != 1) {
                unsigned long nerr = ERR_get_error();
                throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
            }
        } else {
            if (SSL_CTX_load_verify_locations(ctx->ctx, certs.caPath.data(), NULL) != 1) {
                unsigned long nerr = ERR_get_error();
                throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
            }
        }
    }

    if (keySet && SSL_CTX_check_private_key(ctx->ctx) != 1) {
        unsigned long nerr = ERR_get_error();
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
    }
}

//-------------------------------------------------------------------

void
server::initSsl()
{
    if (ctx->ctx == NULL) {
        ctx->ctx = SSL_CTX_new(SSLv23_server_method());
        if (ctx->ctx == NULL)
            throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_INITSSL, exception::ERRNO_LIBDODO, SERVEREX_UNABLETOINITCONTEXT, IONETWORKSSLSERVEREX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);
    }
}

//-------------------------------------------------------------------

void
server::acceptSsl(exchange::__init__ &init)
{
    io::ssl::__openssl___init_object__.addEntropy();

    init.handle->handle = SSL_new(ctx->ctx);
    if (init.handle->handle == NULL)
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_INITSSL, exception::ERRNO_LIBDODO, SERVEREX_UNABLETOINITSSL, IONETWORKSSLSERVEREX_UNABLETOINITSSL_STR, __LINE__, __FILE__);

    if (SSL_set_fd(init.handle->handle, init.socket) == 0) {
        unsigned long nerr = ERR_get_error();
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
    }

    int res = SSL_accept(init.handle->handle);
    switch (res) {
        case 1:
            break;

        case 0:
        {
            unsigned long nerr = ERR_get_error();
            throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
        }

        case - 1:
        {
            int nerr = SSL_get_error(init.handle->handle, res);
            if (nerr == SSL_ERROR_WANT_READ || nerr == SSL_ERROR_WANT_WRITE || nerr == SSL_ERROR_WANT_X509_LOOKUP)
                break;
        }

        default:
        {
            unsigned long nerr = ERR_get_error();

            int err = SSL_shutdown(init.handle->handle);
            if (err < 0) {
                nerr = ERR_get_error();
                throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
            }
            if (err == 0) {
                err = SSL_shutdown(init.handle->handle);
                if (err < 0) {
                    nerr = ERR_get_error();
                    throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
                }
            }

            throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
        }
    }
}

//-------------------------------------------------------------------

void
server::serve(const dodoString &host,
              int              port,
              int              numberOfConnections)
{
#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_BINDNLISTEN);
#endif

    initSsl();
    makeSocket();

    int sockFlag(1);
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == 1)
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    addFlag(socketOpts, 1 << OPTION_REUSE_ADDRESS);

    setLingerOption(IONETWORKCONNECTION_SOCKET_LINGER_OPTION, IONETWORKCONNECTION_SOCKET_LINGER_PERIOD);

    if (family == PROTOCOL_FAMILY_IPV6) {
        struct sockaddr_in6 sa;
        sa.sin6_family = AF_INET6;
        sa.sin6_port = htons(port);
        sa.sin6_flowinfo = 0;
        sa.sin6_scope_id = 0;
        if (host == "*")
            sa.sin6_addr = in6addr_any;
        else
            inet_pton(AF_INET6, host.data(), &sa.sin6_addr);

        if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
            throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    } else {
        struct sockaddr_in sa;

        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        if (host == "*")
            sa.sin_addr.s_addr = htonl(INADDR_ANY);
        else
            inet_pton(AF_INET, host.data(), &sa.sin_addr);

        if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
            throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    if (type == TRANSFER_STREAM)
        if (::listen(socket, numberOfConnections) == -1)
            throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_BINDNLISTEN);
#endif
}

//-------------------------------------------------------------------

void
server::serve(const dodoString &path,
              int              numberOfConnections,
              bool             force)
{
#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_SERVE);
#endif

    initSsl();
    makeSocket();

    if (force) {
        struct stat st;
        if (::lstat(path.data(), &st) != -1) {
            if (S_ISSOCK(st.st_mode))
                ::unlink(path.data());
            else
                throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_LIBDODO, SERVEREX_WRONGFILENAME, IONETWORKSSLSERVEREX_WRONGFILENAME_STR, __LINE__, __FILE__);
        }
    }

    int sockFlag(1);
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    addFlag(socketOpts, 1 << OPTION_REUSE_ADDRESS);

    setLingerOption(IONETWORKCONNECTION_SOCKET_LINGER_OPTION, IONETWORKCONNECTION_SOCKET_LINGER_PERIOD);

    struct sockaddr_un sa;

    unsigned long size = path.size();

    if (size >= 108)
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_LIBDODO, SERVEREX_LONGPATH, IONETWORKSSLSERVEREX_LONGPATH_STR, __LINE__, __FILE__);

    strncpy(sa.sun_path, path.data(), size);
    sa.sun_family = AF_UNIX;

    if (::bind(socket, (struct sockaddr *)&sa, path.size() + sizeof(sa.sun_family)) == -1)
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    if (::listen(socket, numberOfConnections) == -1)
        throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    unixSock = path;

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_SERVE);
#endif
}

//-------------------------------------------------------------------

bool
server::accept(network::exchange::__init__ &init,
               __peer__                    &info)
{
#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_ACCEPT);
#endif

    if (type != TRANSFER_STREAM) {
        init.socket = socket;
        init.blocked = blocked;
        init.blockInherited = blockInherited;

        return true;
    }

    int sock(-1);
    info.host.clear();

    switch (family) {
        case PROTOCOL_FAMILY_IPV4:
        {
            struct sockaddr_in sa;
            socklen_t len = sizeof(sockaddr_in);
            sock = ::accept(socket, (sockaddr *)&sa, &len);

            if (sock == -1) {
                if (errno == EAGAIN)
                    return false;
                else
                    throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            }

            char temp[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &(sa.sin_addr), temp, INET_ADDRSTRLEN) != NULL)
                info.host.assign(temp);
            info.port = ntohs(sa.sin_port);

            break;
        }

        case PROTOCOL_FAMILY_IPV6:
        {
            struct sockaddr_in6 sa;
            socklen_t len = sizeof(sockaddr_in6);

            sock = ::accept(socket, (sockaddr *)&sa, &len);

            if (sock == -1) {
                if (errno == EAGAIN)
                    return false;
                else
                    throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            }

            char temp[INET6_ADDRSTRLEN];
            if (inet_ntop(AF_INET6, &(sa.sin6_addr), temp, INET6_ADDRSTRLEN) != NULL)
                info.host.assign(temp);
            info.port = ntohs(sa.sin6_port);

            break;
        }

        case PROTOCOL_FAMILY_UNIX_SOCKET:

            sock = ::accept(socket, NULL, NULL);
            if (sock == -1) {
                if (errno == EAGAIN)
                    return false;
                else
                    throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            }

            break;

        default:

            throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_LIBDODO, SERVEREX_WRONGPARAMETER, IONETWORKSSLSERVEREX_WRONGPARAMETER_STR, __LINE__, __FILE__);
    }

    init.socket = sock;
    init.blocked = blocked;
    init.blockInherited = blockInherited;

    acceptSsl(dynamic_cast<exchange::__init__ &>(init));

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_ACCEPT);
#endif

    return true;
}

//-------------------------------------------------------------------

bool
server::accept(network::exchange::__init__ &init)
{
#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_ACCEPT);
#endif

    if (type != TRANSFER_STREAM) {
        init.socket = socket;
        init.blocked = blocked;
        init.blockInherited = blockInherited;

        return true;
    }

    int sock = ::accept(socket, NULL, NULL);
    if (sock == -1) {
        if (errno == EAGAIN)
            return false;
        else
            throw exception::basic(exception::MODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    init.socket = sock;
    init.blocked = blocked;
    init.blockInherited = blockInherited;

    acceptSsl(dynamic_cast<exchange::__init__ &>(init));

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_ACCEPT);
#endif

    return true;
}

//-------------------------------------------------------------------
#endif

