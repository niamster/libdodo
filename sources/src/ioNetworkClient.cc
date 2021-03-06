/***************************************************************************
 *            ioNetworkClient.cc
 *
 *  Thu Sep 20 2005
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

#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <libdodo/ioNetworkClient.h>
#include <libdodo/ioNetworkClientEx.h>
#include <libdodo/types.h>
#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/xexec.h>

using namespace dodo::io::network;

#ifndef IO_WO_XEXEC
client::__collected_data__::__collected_data__(xexec *executor,
                                               short execObject) : xexec::__collected_data__(executor, execObject)
{
}
#endif

//-------------------------------------------------------------------

#ifndef IO_WO_XEXEC
client::client(client &c) : xexec(c),
                            collectedData(this, xexec::OBJECT_IONETWORKCLIENT)
#else
client::client(client &)
#endif
{
}

//-------------------------------------------------------------------

client::client(short a_family,
               short a_type) : blockInherited(false),
                               family(a_family),
                               type(a_type)
#ifndef IO_WO_XEXEC
                               ,
                               collectedData(this, xexec::OBJECT_IONETWORKCLIENT)
#endif
{
}

//-------------------------------------------------------------------

client::~client()
{
    if (socket != -1) {
        ::shutdown(socket, SHUT_RDWR);

        ::close(socket);
    }
}

//-------------------------------------------------------------------

void
client::restoreOptions()
{
    setInBufferSize(inSocketBufferSize);
    setOutBufferSize(outSocketBufferSize);

    setInTimeout(inSocketTimeout);
    setOutTimeout(outSocketTimeout);

    setLingerOption(lingerOpts, lingerSeconds);

    block(blocked);
}

//-------------------------------------------------------------------

void
client::makeSocket()
{
    if (socket != -1) {
        ::shutdown(socket, SHUT_RDWR);

        if (::close(socket) == -1)
            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_MAKESOCKET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        socket = -1;
    }

    int real_domain(PF_INET), real_type(SOCK_STREAM);

    switch (family) {
        case PROTOCOL_FAMILY_IPV4:

            real_domain = PF_INET;

            break;

        case PROTOCOL_FAMILY_IPV6:

            real_domain = PF_INET6;

            break;

        case PROTOCOL_FAMILY_UNIX_SOCKET:

            real_domain = PF_UNIX;

            break;

        default:

            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_MAKESOCKET, exception::ERRNO_LIBDODO, CLIENTEX_WRONGPARAMETER, IONETWORKCLIENTEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
    }

    switch (type) {
        case TRANSFER_STREAM:

            real_type = SOCK_STREAM;

            break;

        case TRANSFER_DATAGRAM:

            real_type = SOCK_DGRAM;

            break;

        default:

            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_MAKESOCKET, exception::ERRNO_LIBDODO, CLIENTEX_WRONGPARAMETER, IONETWORKCLIENTEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
    }

    socket = ::socket(real_domain, real_type, 0);
    if (socket == -1)
        dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_MAKESOCKET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    restoreOptions();
}

//-------------------------------------------------------------------

void
client::connect(const dodo::string &host,
                int              port,
                exchange         &exchange)
{
#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_CONNECT);
#endif

    makeSocket();

    if (family == PROTOCOL_FAMILY_IPV6) {
        struct sockaddr_in6 sa;
        sa.sin6_family = AF_INET6;
        sa.sin6_port = htons(port);
        sa.sin6_flowinfo = 0;
        sa.sin6_scope_id = 0;
        inet_pton(AF_INET6, host.data(), &sa.sin6_addr);

        if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
            if (::close(socket) == -1)
                dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

            socket = -1;

            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }
    } else {
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        inet_aton(host.data(), &sa.sin_addr);

        if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
            if (::close(socket) == -1)
                dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

            socket = -1;

            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }
    }

    exchange.init(socket, blocked, blockInherited);

    socket = -1;

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_CONNECT);
#endif
}

//-------------------------------------------------------------------

void
client::connectFrom(const dodo::string &local,
                    const dodo::string &host,
                    int              port,
                    exchange         &exchange)
{
#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_CONNECTFROM);
#endif

    makeSocket();

    int sockFlag(1);
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
        dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    addFlag(socketOpts, 1 << OPTION_REUSE_ADDRESS);

    if (family == PROTOCOL_FAMILY_IPV6) {
        struct sockaddr_in6 sa;
        sa.sin6_family = AF_INET6;
        sa.sin6_flowinfo = 0;
        sa.sin6_scope_id = 0;
        sa.sin6_port = htons(0);
        inet_pton(AF_INET6, local.data(), &sa.sin6_addr);

        if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        sa.sin6_port = htons(port);
        inet_pton(AF_INET6, host.data(), &sa.sin6_addr);

        if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
            if (::close(socket) == -1)
                dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

            socket = -1;

            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }
    } else {
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(0);
        inet_aton(local.data(), &sa.sin_addr);

        if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        sa.sin_port = htons(port);
        inet_aton(host.data(), &sa.sin_addr);

        if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
            if (::close(socket) == -1)
                dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

            socket = -1;

            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }
    }

    exchange.init(socket, blocked, blockInherited);

    socket = -1;

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_CONNECTFROM);
#endif
}

//-------------------------------------------------------------------

void
client::connect(const dodo::string &path,
                exchange         &exchange)
{
#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_CONNECT);
#endif

    makeSocket();

    struct sockaddr_un sa;

    unsigned long size = path.size();

    if (size >= 108)
        dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, exception::ERRNO_LIBDODO, CLIENTEX_LONGPATH, IONETWORKCLIENTEX_LONGPATH_STR, __LINE__, __FILE__);

    strncpy(sa.sun_path, path.data(), size);
    sa.sun_family = AF_UNIX;

    if (::connect(socket, (struct sockaddr *)&sa, path.size() + sizeof(sa.sun_family)) == -1) {
        if (::close(socket) == -1)
            dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        socket = -1;

        dodo_throw exception::basic(exception::MODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    exchange.init(socket, blocked, blockInherited);

    socket = -1;

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_CONNECT);
#endif
}

//-------------------------------------------------------------------

