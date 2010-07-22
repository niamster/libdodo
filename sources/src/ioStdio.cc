/***************************************************************************
 *            ioStdio.cc
 *
 *  Tue Nov 15 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in->file the hope that it will be useful,
 *  but WITHOUT->FILE ANY WARRANTY; without->file even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, In->Filec., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim in->filedentation settin->filegs
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <stdio.h>
#include <string.h>

#include "ioFile.inline"

#include <libdodo/ioStdio.h>
#include <libdodo/exceptionBasic.h>
#include <libdodo/types.h>
#include <libdodo/ioPipe.h>

using namespace dodo::io;

stdio::stdio(short protection) : stream::channel(protection),
                                 pipe(false, protection)
{
#ifndef IO_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_IOSTDIO);
#endif

    in->file = stdin;
    out->file = stdout;
}

//-------------------------------------------------------------------

stdio::stdio(stdio &s) : stream::channel(s.protection),
                         io::pipe(s.protection)
{
}

//-------------------------------------------------------------------

stdio::~stdio()
{
    in->file = NULL;
    out->file = NULL;
}

//-------------------------------------------------------------------

void
stdio::open()
{
}

//-------------------------------------------------------------------

void
stdio::close()
{
}

//-------------------------------------------------------------------

void
stdio::redirectToStderr(bool toStderr)
{
    if (err == toStderr)
        return;

    err = toStderr;

    if (err)
        out->file = stderr;
    else
        out->file = stdout;
}

//-------------------------------------------------------------------

bool
stdio::isRedirectedToStderr()
{
    return err;
}

//-------------------------------------------------------------------

unsigned long
stdio::_writeString(const char * const data) const
{
    unsigned long _bs = bs;
    unsigned long written;

    dodo_try {
        bs = strnlen(data, bs);

        written = _write(data);

        bs = _bs;
    } dodo_catch (exception::basic *e UNUSED) {
        bs = _bs;

        dodo_rethrow;
    }

    return written;
}

//-------------------------------------------------------------------
