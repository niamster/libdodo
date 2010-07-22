/***************************************************************************
 *            ioNetworkExchange.cc
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

#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioNetworkExchangeEx.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncStack.h>

using namespace dodo::io::network;

exchange::__init__::__init__() : socket(-1)
{
}

//-------------------------------------------------------------------

exchange::__init__::__init__(__init__ &init) : socket(init.socket)
{
    init.socket = -1;
}

//-------------------------------------------------------------------

exchange::__init__::~__init__()
{
}

//-------------------------------------------------------------------

exchange::exchange(exchange &fse) : stream::channel(fse.protection)
{
}

//-------------------------------------------------------------------

exchange::exchange(short protection) : stream::channel(protection)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IONETWORKEXCHANGE);
#endif
}

//-------------------------------------------------------------------

exchange::exchange(__init__ &a_init,
                   short    protection) : stream::channel(protection)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IONETWORKEXCHANGE);
#endif

    init(a_init.socket, a_init.blocked, a_init.blockInherited);

    a_init.socket = -1;
}

//-------------------------------------------------------------------

exchange::~exchange()
{
    if (socket != -1) {
        ::shutdown(socket, SHUT_RDWR);

        ::close(socket);
    }
}

//-------------------------------------------------------------------

void
exchange::close()
{
    pc::sync::stack pg(keeper);

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_CLOSE);
#endif

    if (socket != -1) {
        _close(socket);

        socket = -1;
    }

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_CLOSE);
#endif
}

//-------------------------------------------------------------------

void
exchange::init(int  dataocket,
               bool a_blocked,
               bool blockInherited)
{
    pc::sync::stack pg(keeper);

    if (socket != -1) {
        _close(socket);

        socket = -1;
    }

    blocked = a_blocked;
    socket = dataocket;

    setInBufferSize(inSocketBufferSize);
    setOutBufferSize(outSocketBufferSize);

    setInTimeout(inSocketTimeout);
    setOutTimeout(outSocketTimeout);

    setLingerOption(lingerOpts, lingerSeconds);

    if (!blocked) {
        if (blockInherited)
            block(false);
        else
            block(true);
    } else
        block(true);
}

//-------------------------------------------------------------------

bool
exchange::isAlive()
{
    pc::sync::stack pg(keeper);

    if (socket == -1)
        return false;

    pollfd fd;
    fd.fd = socket;
    fd.events = POLLOUT;

    if (poll(&fd, 1, -1) > 0)
        if (isSetFlag(fd.revents, POLLOUT))
            return true;

    _close(socket);

    socket = -1;

    return false;
}

//-------------------------------------------------------------------

unsigned long
exchange::_write(const char * const data) const
{
    if (socket == -1)
        dodo_throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

    unsigned long iter = bs / outSocketBufferSize;
    unsigned long rest = bs % outSocketBufferSize;

    const char *s = data;

    long batch, n;

    for (unsigned long i = 0; i < iter; ++i) {
        batch = outSocketBufferSize;
        while (batch > 0) {
            while (true) {
                if ((n = ::send(socket, s, batch, 0)) == -1) {
                    if (errno == EINTR)
                        continue;

                    if (errno == EAGAIN)
                        return s - data;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
                }

                break;
            }

            batch -= n;
            s += n;
        }
    }

    if (rest > 0) {
        batch = rest;
        while (batch > 0) {
            while (true) {
                if ((n = ::send(socket, s, batch, 0)) == -1) {
                    if (errno == EINTR)
                        continue;

                    if (errno == EAGAIN)
                        return s - data;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
                }

                break;
            }

            batch -= n;
            s += n;
        }
    }

    return bs;
}

//-------------------------------------------------------------------

unsigned long
exchange::_read(char * const data) const
{
    if (socket == -1)
        dodo_throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

    unsigned long iter = bs / inSocketBufferSize;
    unsigned long rest = bs % inSocketBufferSize;

    char *s = data;
    char *_s = s;

    long batch, n;

    for (unsigned long i = 0; i < iter; ++i) {
        batch = inSocketBufferSize;
        while (batch > 0) {
            while (true) {
                if ((n = ::recv(socket, s, batch, 0)) == -1) {
                    if (errno == EINTR)
                        continue;

                    if (errno == EAGAIN)
                        return s - _s;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
                }

                break;
            }

            if (n == 0)
                return s - _s;

            batch -= n;
            s += n;
        }
    }

    if (rest > 0) {
        batch = rest;
        while (batch > 0) {
            while (true) {
                if ((n = ::recv(socket, s, batch, 0)) == -1) {
                    if (errno == EINTR)
                        continue;

                    if (errno == EAGAIN)
                        return s - _s;

                    dodo_throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
                }

                break;
            }

            if (n == 0)
                return s - _s;

            batch -= n;
            s += n;
        }
    }

    return bs;
}

//-------------------------------------------------------------------

unsigned long
exchange::_writeString(const char * const s) const
{
    unsigned long _bs = bs;
    unsigned long written;

    dodo_try {
        bs = strnlen(s, bs) + 1;

        written = _write(s);

        bs = _bs;
    } dodo_catch (exception::basic *e UNUSED) {
        bs = _bs;

        dodo_rethrow;
    }

    return written;
}

//-------------------------------------------------------------------

unsigned long
exchange::_readString(char * const s) const
{
    if (socket == -1)
        dodo_throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READSTRING, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

    long n = 0;

    while (true) {
        if ((n = ::recv(socket, s, bs, 0)) == -1) {
            if (errno == EINTR)
                continue;

            if (errno == EAGAIN)
                return 0;

            dodo_throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READSTRING, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        break;
    }

    return n;
}

//-------------------------------------------------------------------

void
exchange::flush() const
{
}

//-------------------------------------------------------------------

int
exchange::inDescriptor() const
{
    pc::sync::stack pg(keeper);

    return socket;
}

//-------------------------------------------------------------------

int
exchange::outDescriptor() const
{
    pc::sync::stack pg(keeper);

    return socket;
}

//-------------------------------------------------------------------

