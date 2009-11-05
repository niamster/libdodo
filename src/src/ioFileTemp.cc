/***************************************************************************
 *            ioFileTemp.cc
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

#include <libdodo/ioFileTemp.h>
#include <libdodo/ioFileTempEx.h>
#include <libdodo/ioFileRegular.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioBlockChannel.h>
#include <libdodo/pcSyncStack.h>

using namespace dodo::io::file;

temp::temp(bool  open,
           short protection) : block::channel(protection),
                               regular(protection)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IOFILETEMP);
#endif

    if (open) {
        handle->file = tmpfile();
        if (handle->file == NULL) {
            delete handle;

            throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_TEMP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }
    }
}

//-------------------------------------------------------------------

temp::temp(const temp &fd) : block::channel(protection),
                             regular(protection)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IOFILETEMP);
#endif

    handle = new io::__file__;

    append = fd.append;
    pos = fd.pos;
    bs = fd.bs;

    if (fd.handle->file != NULL) {
        int oldDesc, newDesc;

        oldDesc = fileno(fd.handle->file);
        if (oldDesc == -1) {
            delete handle;

            throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_TEMP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        newDesc = dup(oldDesc);
        if (newDesc == -1) {
            delete handle;

            throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_TEMP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        handle->file = fdopen(newDesc, "r+");

        if (handle->file == NULL) {
            delete handle;

            throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_TEMP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }
    }
}

//-------------------------------------------------------------------

temp::~temp()
{
    if (handle->file != NULL)
        fclose(handle->file);

    delete handle;
}

//-------------------------------------------------------------------

void
temp::clone(const temp &fd)
{
    pc::sync::stack pg(keeper);

    if (handle->file != NULL) {
        if (fclose(handle->file) != 0)
            throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        handle->file = NULL;
    }

    pos = fd.pos;
    append = fd.append;
    bs = fd.bs;

    if (fd.handle->file != NULL) {
        int oldDesc, newDesc;

        oldDesc = fileno(fd.handle->file);
        if (oldDesc == -1)
            throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        newDesc = dup(oldDesc);
        if (newDesc == -1)
            throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        handle->file = fdopen(newDesc, "r+");

        if (handle->file == NULL)
            throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }
}

//-------------------------------------------------------------------

void
temp::open()
{
    pc::sync::stack pg(keeper);

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_OPEN);
#endif

    if (handle->file != NULL) {
        if (fclose(handle->file) != 0)
            throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

        handle->file = NULL;
    }

    handle->file = tmpfile();
    if (handle->file == NULL)
        throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_OPEN);
#endif
}

//-------------------------------------------------------------------

