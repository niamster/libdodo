/***************************************************************************
 *            ioBlockChannel.cc
 *
 *  Sat Jun 13 2009
 *  Copyright  2009  Ni@m
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

#include <string.h>

#include <libdodo/ioBlockChannel.h>
#include <libdodo/ioChannel.h>
#include <libdodo/xexec.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncStack.h>

using namespace dodo::io::block;

channel::channel(short protection) : io::channel(protection),
                                     pos(0),
                                     append(false)
{
}

//-------------------------------------------------------------------

channel::~channel()
{
}

//-------------------------------------------------------------------

dodoString
channel::read() const
{
    pc::sync::stack pg(keeper);

    dodoString data;
    unsigned long n;

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_READ);
#endif

    data.assign(bs, '\0');

    try {
        n = _read((char *)data.data());
        data.resize(n);
    } catch (...) {
        data.clear();

#ifndef IO_WO_XEXEC
        collectedData.buffer.clear();
#endif

        throw;
    }

#ifndef IO_WO_XEXEC
    collectedData.buffer = data;

    performPostExec(OPERATION_READ);

    data = collectedData.buffer;

    collectedData.buffer.clear();
#else
#endif

    pos += n;

    return data;
}

//-------------------------------------------------------------------

dodoString
channel::readString() const
{
    pc::sync::stack pg(keeper);

    dodoString data;
    unsigned long n;

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_READSTRING);
#endif

    data.assign(bs, '\0');

    try {
        n = _readString((char *)data.data());
        data.resize(n);
    } catch (...) {
        data.clear();

        throw;
    }

#ifndef IO_WO_XEXEC
    collectedData.buffer = data;

    performPostExec(OPERATION_READSTRING);

    data = collectedData.buffer;

    collectedData.buffer.clear();
#endif

    pos += n;

    return data;
}

//-------------------------------------------------------------------

unsigned long
channel::write(const dodoString &data) const
{
    pc::sync::stack pg(keeper);

    unsigned long n;

#ifndef IO_WO_XEXEC
    collectedData.buffer.assign(data, 0, bs);

    performPreExec(OPERATION_WRITE);

    try {
        n = _write(collectedData.buffer.data());
    } catch (...) {
        collectedData.buffer.clear();

        throw;
    }
#else
    n = _write(data.data());
#endif

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_WRITE);

    collectedData.buffer.clear();
#endif

    pos += n;

    return n;
}

//-------------------------------------------------------------------

unsigned long
channel::writeString(const dodoString &data) const
{
    pc::sync::stack pg(keeper);

    unsigned long n;

#ifndef IO_WO_XEXEC
    collectedData.buffer = data;

    performPreExec(OPERATION_WRITESTRING);

    try {
        n = _writeString(collectedData.buffer.data());
    } catch (...) {
        collectedData.buffer.clear();

        throw;
    }

    pos += n;
#else
    n = _writeString(data.data());

    pos += n;
#endif

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_WRITESTRING);

    collectedData.buffer.clear();
#endif

    return n;
}

//-------------------------------------------------------------------

