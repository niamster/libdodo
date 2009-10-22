/***************************************************************************
 *            ioFileRegular.cc
 *
 *  Wed Oct 8 2005
 *  Copyright  2005  Ni@m
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "ioFile.inline"

#include <libdodo/ioFileRegular.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioFileRegularEx.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncStack.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioBlockChannel.h>

using namespace dodo::io::file;

regular::regular(short protection) : block::channel(protection),
                                     handle(new io::__file__)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IOFILEREGULAR);
#endif
}

//-------------------------------------------------------------------

regular::regular(const dodoString &a_path,
                 short            a_mode,
                 short            protection) : block::channel(protection),
                                                path(a_path),
                                                mode(a_mode),
                                                handle(new io::__file__)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IOFILEREGULAR);
#endif

    bool exists(false);

    if (path.size() == 0) {
        delete handle;

        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_LIBDODO, REGULAREX_WRONGFILENAME, IOFILEREGULAREX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
    } else {
        struct stat st;

        if (::lstat(path.data(), &st) == -1) {
            if (errno != ENOENT) {
                delete handle;

                throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
            }
        } else
            exists = true;

        if (exists && !S_ISREG(st.st_mode) && !S_ISBLK(st.st_mode)) {
            delete handle;

            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_LIBDODO, REGULAREX_WRONGFILENAME, IOFILEREGULAREX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
        }

        switch (mode) {
            case OPEN_MODE_READ_WRITE:

                handle->file = fopen(path.data(), "r+");
                if (handle->file == NULL)
                    handle->file = fopen(path.data(), "w+");

                break;

            case OPEN_MODE_READ_WRITE_TRUNCATE:

                handle->file = fopen(path.data(), "w+");

                break;

            case OPEN_MODE_READ_ONLY:
            default:

                handle->file = fopen(path.data(), "r");
        }
    }

    if (handle->file == NULL) {
        delete handle;

        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
    }

    if (!exists) {
        try {
            tools::filesystem::chmod(path, DEFAULT_FILE_PERM);
        } catch (...) {
            delete handle;

            throw;
        }
    }
}

//-------------------------------------------------------------------

regular::regular(const regular &fd) : block::channel(fd.protection),
                                      path(fd.path),
                                      mode(fd.mode),
                                      handle(new io::__file__)

{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IOFILEREGULAR);
#endif

    block = fd.block;
    append = fd.append;
    pos = fd.pos;
    blockSize = fd.blockSize;

    if (fd.handle->file !=  NULL) {
        int oldDesc, newDesc;

        oldDesc = fileno(fd.handle->file);
        if (oldDesc == -1) {
            delete handle;

            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        newDesc = dup(oldDesc);
        if (newDesc == -1) {
            delete handle;

            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        switch (mode) {
            case OPEN_MODE_READ_WRITE:
            case OPEN_MODE_READ_WRITE_TRUNCATE:

                handle->file = fdopen(newDesc, "r+");

                break;

            case OPEN_MODE_READ_ONLY:
            default:

                handle->file = fdopen(newDesc, "r");
        }

        if (handle->file == NULL) {
            delete handle;

            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }
    }
}

//-------------------------------------------------------------------

regular::~regular()
{
    if (handle->file !=  NULL)
        fclose(handle->file);

    delete handle;
}

//-------------------------------------------------------------------

int
regular::inDescriptor() const
{
    pc::sync::stack pg(keeper);

    if (handle->file == NULL)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_INDESCRIPTOR, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

    return fileno(handle->file);
}

//-------------------------------------------------------------------

int
regular::outDescriptor() const
{
    pc::sync::stack pg(keeper);

    if (handle->file == NULL)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_OUTDESCRIPTOR, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

    return fileno(handle->file);
}

//-------------------------------------------------------------------

void
regular::clone(const regular &fd)
{
    pc::sync::stack pg(keeper);

    if (handle->file !=  NULL) {
        if (fclose(handle->file) != 0)
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        handle->file = NULL;
    }

    path = fd.path;
    mode = fd.mode;
    pos = fd.pos;
    block = fd.block;
    append = fd.append;
    blockSize = fd.blockSize;

    if (fd.handle->file !=  NULL) {
        int oldDesc, newDesc;

        oldDesc = fileno(fd.handle->file);
        if (oldDesc == -1)
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        newDesc = dup(oldDesc);
        if (newDesc == -1)
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        switch (mode) {
            case OPEN_MODE_READ_WRITE:
            case OPEN_MODE_READ_WRITE_TRUNCATE:

                handle->file = fdopen(newDesc, "r+");

                break;

            case OPEN_MODE_READ_ONLY:
            default:

                handle->file = fdopen(newDesc, "r");
        }

        if (handle->file == NULL)
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }
}

//-------------------------------------------------------------------

void
regular::close()
{
    pc::sync::stack pg(keeper);

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_CLOSE);
#endif

    if (handle->file !=  NULL) {
        if (fclose(handle->file) != 0)
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

        handle->file = NULL;
    }

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_CLOSE);
#endif
}

//-------------------------------------------------------------------

void
regular::open(const dodoString &a_path,
              short            a_mode)
{
    pc::sync::stack pg(keeper);

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_OPEN);
#endif

    path = a_path;
    mode = a_mode;

    if (handle->file !=  NULL) {
        if (fclose(handle->file) != 0)
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

        handle->file = NULL;
    }

    bool exists(false);

    if (path.size() == 0)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_LIBDODO, REGULAREX_WRONGFILENAME, IOFILEREGULAREX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
    else {
        struct stat st;

        if (::lstat(path.data(), &st) == -1) {
            if (errno != ENOENT)
                throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
        } else
            exists = true;

        if (exists && !S_ISREG(st.st_mode) && !S_ISBLK(st.st_mode))
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_LIBDODO, REGULAREX_WRONGFILENAME, IOFILEREGULAREX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

        switch (mode) {
            case OPEN_MODE_READ_WRITE:

                handle->file = fopen(path.data(), "r+");
                if (handle->file == NULL)
                    handle->file = fopen(path.data(), "w+");

                break;

            case OPEN_MODE_READ_WRITE_TRUNCATE:

                handle->file = fopen(path.data(), "w+");

                break;

            case OPEN_MODE_READ_ONLY:
            default:

                handle->file = fopen(path.data(), "r");
        }
    }

    if (handle->file == NULL)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    if (!exists)
        tools::filesystem::chmod(path, DEFAULT_FILE_PERM);

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_OPEN);
#endif
}

//-------------------------------------------------------------------

void
regular::_read(char * const a_data) const
{
    if (handle->file == NULL)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__READ, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

    unsigned long pos = block ? this->pos * blockSize : this->pos;

    if (fseek(handle->file, pos, SEEK_SET) == -1)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    while (true) {
        if (fread(a_data, blockSize, 1, handle->file) == 0) {
            if (feof(handle->file) != 0 || errno == EAGAIN)
                break;

            if (errno == EINTR)
                continue;

            if (ferror(handle->file) != 0)
                throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        break;
    }
}

//-------------------------------------------------------------------

void
regular::_write(const char *const a_data) const
{
    if (handle->file == NULL)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

    if (append) {
        if (fseek(handle->file, 0, SEEK_END) == -1)
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
    } else {
        unsigned long pos = block ? this->pos * blockSize : this->pos;

        if (fseek(handle->file, pos, SEEK_SET) == -1)
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
    }

    while (true) {
        if (fwrite(a_data, blockSize, 1, handle->file) == 0) {
            if (errno == EINTR)
                continue;

            if (errno == EAGAIN)
                break;

            if (ferror(handle->file) != 0)
                throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        break;
    }
}

//-------------------------------------------------------------------

void
regular::erase()
{
    pc::sync::stack pg(keeper);

    char *empty = new char[blockSize];

    memset(empty, 0, blockSize);
    try {
        _write(empty);
    } catch (...) {
        delete [] empty;

        throw;
    }

    delete [] empty;
}

//-------------------------------------------------------------------

void
regular::flush() const
{
    pc::sync::stack pg(keeper);

    if (handle->file == NULL)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_FLUSH, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

    if (fflush(handle->file) != 0)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX_FLUSH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

unsigned long
regular::_readString(char * const a_data) const
{
    if (handle->file == NULL)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

    unsigned long readSize = blockSize + 1;

    if (block) {
        if (fseek(handle->file, 0, SEEK_SET) == -1)
            throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

        for (unsigned long i = 0; i < pos; ++i) {
            if (fgets(a_data, readSize, handle->file) == NULL) {
                switch (errno) {
                    case EIO:
                    case EINTR:
                    case EBADF:
                    case EOVERFLOW:
                    case ENOMEM:
                    case ENXIO:

                        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
                }

                throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_LIBDODO, REGULAREX_FILEISSHORTERTHANGIVENPOSITION, IOFILEREGULAREX_FILEISSHORTERTHANGIVENPOSITION_STR, __LINE__, __FILE__, path);
            }
        }
    } else if (fseek(handle->file, pos, SEEK_SET) == -1)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    memset(a_data, '\0', readSize);

    while (true) {
        if (fgets(a_data, readSize, handle->file) == NULL) {
            if (errno == EINTR)
                continue;

            if (errno == EAGAIN)
                break;

            if (ferror(handle->file) != 0)
                throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        break;
    }

    return strlen(a_data);
}

//-------------------------------------------------------------------

void
regular::_writeString(const char *const a_data) const
{
    if (handle->file == NULL)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__WRITESTREAM, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

    if (fseek(handle->file, 0, SEEK_END) == -1)
        throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    unsigned long _blockSize = strnlen(a_data, blockSize);

    while (true) {
        if (fwrite(a_data, _blockSize, 1, handle->file) == 0) {
            if (errno == EINTR)
                continue;

            if (ferror(handle->file) != 0)
                throw exception::basic(exception::MODULE_IOFILEREGULAR, REGULAREX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        break;
    }
}

//-------------------------------------------------------------------

