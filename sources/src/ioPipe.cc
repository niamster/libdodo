/***************************************************************************
 *            ioPipe.cc
 *
 *  Tue Jul 1 2008
 *  Copyright  2008  Dmytro Milinevskyy
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

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "ioFile.inline"

#include <libdodo/ioPipe.h>
#include <libdodo/ioPipeEx.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioStreamChannel.h>
#include <libdodo/ioNetworkConnection.h>
#include <libdodo/pcSyncStack.h>

using namespace dodo;

io::pipe::pipe(bool  open,
               short protection) : stream::channel(protection),
                                   in(new io::__file__),
                                   out(new io::__file__),
                                   blocked(true)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IOPIPE);
#endif

    if (open) {
        int pipefd[2];

        if (::pipe(pipefd) != 0)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PIPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        in->file = fdopen(pipefd[0], "r");
        if (in->file == NULL)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PIPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        out->file = fdopen(pipefd[1], "w");
        if (out->file == NULL)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PIPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }
}

//-------------------------------------------------------------------

io::pipe::pipe(const pipe &fd) : stream::channel(protection),
                                 in(new io::__file__),
                                 out(new io::__file__),
                                 blocked(fd.blocked)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IOPIPE);
#endif

    bs = fd.bs;

    if (fd.in->file != NULL && fd.out->file != NULL) {
        int oldDesc, newDesc;

        oldDesc = fileno(fd.in->file);
        if (oldDesc == -1)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PIPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        newDesc = dup(oldDesc);
        if (newDesc == -1)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PIPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        in->file = fdopen(newDesc, "r");
        if (in->file == NULL)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PIPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        oldDesc = fileno(fd.out->file);
        if (oldDesc == -1)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PIPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        newDesc = dup(oldDesc);
        if (newDesc == -1)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PIPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        out->file = fdopen(newDesc, "w");
        if (out->file == NULL)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PIPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }
}

//-------------------------------------------------------------------

io::pipe::~pipe()
{
    if (in->file != NULL)
        fclose(in->file);

    if (out->file != NULL)
        fclose(out->file);

    delete in;
    delete out;
}

//-------------------------------------------------------------------

void
io::pipe::clone(const pipe &fd)
{
    pc::sync::stack pg(keeper);

    if (in->file != NULL) {
        if (fclose(in->file) != 0)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        in->file = NULL;
    }

    if (out->file != NULL) {
        if (fclose(out->file) != 0)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        out->file = NULL;
    }

    blocked = fd.blocked;
    bs = fd.bs;

    if (fd.in->file != NULL && fd.out->file != NULL) {
        int oldDesc, newDesc;

        oldDesc = fileno(fd.in->file);
        if (oldDesc == -1)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        newDesc = dup(oldDesc);
        if (newDesc == -1)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        in->file = fdopen(newDesc, "r");
        if (in->file == NULL)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        oldDesc = fileno(fd.out->file);
        if (oldDesc == -1)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        newDesc = dup(oldDesc);
        if (newDesc == -1)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        out->file = fdopen(newDesc, "w");
        if (out->file == NULL)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }
}

//-------------------------------------------------------------------

int
io::pipe::inDescriptor() const
{
    pc::sync::stack pg(keeper);

    if (in->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_INDESCRIPTOR, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_NOTOPENED_STR, __LINE__, __FILE__);

    return fileno(in->file);
}

//-------------------------------------------------------------------

int
io::pipe::outDescriptor() const
{
    pc::sync::stack pg(keeper);

    if (out->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_OUTDESCRIPTOR, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_NOTOPENED_STR, __LINE__, __FILE__);

    return fileno(out->file);
}

//-------------------------------------------------------------------

void
io::pipe::close()
{
    pc::sync::stack pg(keeper);

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_CLOSE);
#endif

    if (in->file != NULL) {
        if (fclose(in->file) != 0)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        in->file = NULL;
    }

    if (out->file != NULL) {
        if (fclose(out->file) != 0)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        out->file = NULL;
    }

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_CLOSE);
#endif
}

//-------------------------------------------------------------------

void
io::pipe::open()
{
    pc::sync::stack pg(keeper);

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_OPEN);
#endif

    if (in->file != NULL) {
        if (fclose(in->file) != 0)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        in->file = NULL;
    }

    if (out->file != NULL) {
        if (fclose(out->file) != 0)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        out->file = NULL;
    }

    int pipefd[2];

    if (::pipe(pipefd) != 0)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    in->file = fdopen(pipefd[0], "r");
    if (in->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    out->file = fdopen(pipefd[1], "w");
    if (out->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_OPEN);
#endif
}

//-------------------------------------------------------------------

unsigned long
io::pipe::_read(char * const data) const
{
    if (in->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX__READ, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_NOTOPENED_STR, __LINE__, __FILE__);

    char *s = data;

    unsigned long batch = bs, n;

    while (batch > 0) {
        while (true) {
            if ((n = fread(s, 1, batch, in->file)) == 0) {
                if (feof(in->file) != 0 || errno == EAGAIN)
                    return bs - batch;

                if (errno == EINTR)
                    continue;

                if (ferror(in->file) != 0)
                    throw exception::basic(exception::MODULE_IOPIPE, PIPEEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            }

            break;
        }

        batch -= n;
        s += n;
    }

    return bs;
}

//-------------------------------------------------------------------

unsigned long
io::pipe::_write(const char *const data) const
{
    if (out->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX__WRITE, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_NOTOPENED_STR, __LINE__, __FILE__);

    const char *s = data;

    unsigned long batch = bs, n;

    while (batch != 0) {
        while (true) {
            if ((n = fwrite(s, 1, batch, out->file)) == 0) {
                if (errno == EINTR)
                    continue;

                if (errno == EAGAIN)
                    return bs - batch;

                if (ferror(out->file) != 0)
                    throw exception::basic(exception::MODULE_IOPIPE, PIPEEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            }

            break;
        }

        batch -= n;
        s += n;
    }

    return bs;
}

//-------------------------------------------------------------------

void
io::pipe::flush() const
{
    pc::sync::stack pg(keeper);

    if (out->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_FLUSH, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_NOTOPENED_STR, __LINE__, __FILE__);

    if (fflush(out->file) != 0)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_FLUSH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

io::network::exchange::__peer__
io::pipe::peer()
{
    pc::sync::stack pg(keeper);

    if (in->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PEERINFO, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_NOTOPENED_STR, __LINE__, __FILE__);

    network::exchange::__peer__ info;

    struct sockaddr sa;

    socklen_t len = sizeof(sockaddr_in6);

    int desc = fileno(in->file);
    if (desc == -1)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PEERINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    if (::getpeername(desc, &sa, &len) == 1) {
        if (errno != ENOTSOCK)
            throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_PEERINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        else
            return info;
    }

    switch (len) {
        case sizeof(sockaddr_in):

        {
            char temp[15];
            sockaddr_in *sa4;
            sa4 = (sockaddr_in *)&sa;
            if (inet_ntop(AF_INET, &(sa4->sin_addr), temp, 15) != NULL)
                info.host = dodo::string(temp);
            info.port = ntohs(sa4->sin_port);

            return info;
        }

        case sizeof(sockaddr_in6):

        {
            char temp[INET6_ADDRSTRLEN];
            sockaddr_in6 *sa6;
            sa6 = (sockaddr_in6 *)&sa6;
            if (inet_ntop(AF_INET6, &(sa6->sin6_addr), temp, INET6_ADDRSTRLEN) != NULL)
                info.host = dodo::string(temp);
            info.port = ntohs(sa6->sin6_port);

            return info;
        }

        default:

            return info;
    }
}

//-------------------------------------------------------------------

bool
io::pipe::isBlocked()
{
    pc::sync::stack pg(keeper);

    return blocked;
}

//-------------------------------------------------------------------

void
io::pipe::block(bool flag)
{
    pc::sync::stack pg(keeper);

    if (in->file == NULL && out->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_NOTOPENED_STR, __LINE__, __FILE__);

    if (blocked == flag)
        return;

    int blockFlag;
    int desc;

    desc = fileno(in->file);
    if (desc == -1)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    blockFlag = fcntl(desc, F_GETFL);
    if (blockFlag == -1)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    if (flag)
        blockFlag &= ~O_NONBLOCK;
    else
        blockFlag |= O_NONBLOCK;

    if (fcntl(desc, F_SETFL, blockFlag) == -1)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    desc = fileno(out->file);
    if (desc == -1)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    blockFlag = fcntl(desc, F_GETFL);
    if (blockFlag == -1)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    if (flag)
        blockFlag &= ~O_NONBLOCK;
    else
        blockFlag |= O_NONBLOCK;

    if (fcntl(desc, F_SETFL, blockFlag) == -1)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    blocked = flag;
}

//-------------------------------------------------------------------

unsigned long
io::pipe::_readString(char * const data) const
{
    if (in->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX__READSTRING, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_NOTOPENED_STR, __LINE__, __FILE__);

    unsigned long readSize = bs + 1;

    while (true) {
        if (fgets(data, readSize, in->file) == NULL) {
            if (errno == EINTR)
                continue;

            if (errno == EAGAIN)
                return 0;

            if (ferror(in->file) != 0)
                throw exception::basic(exception::MODULE_IOPIPE, PIPEEX__READSTRING, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        break;
    }

    return strlen(data);
}

//-------------------------------------------------------------------

unsigned long
io::pipe::_writeString(const char * const data) const
{
    if (out->file == NULL)
        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX__READSTRING, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_NOTOPENED_STR, __LINE__, __FILE__);

    unsigned long _bs = bs;
    unsigned long written;

    try {
        bs = strnlen(data, bs);

        written = _write(data);

        if (data[bs - 1] != '\n') {
            while (true) {
                if (fputc('\n', out->file) == EOF) {
                    if (errno == EINTR)
                        continue;

                    if (errno == EAGAIN)
                        break;

                    if (ferror(out->file) != 0)
                        throw exception::basic(exception::MODULE_IOPIPE, PIPEEX__WRITESTRING, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
                }

                break;
            }
        }

        bs = _bs;
    } catch (...) {
        bs = _bs;

        throw;
    }

    return written;
}

//-------------------------------------------------------------------

