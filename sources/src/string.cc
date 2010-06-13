/***************************************************************************
 *            string.cc
 *
 *  Thu Jun  10 2010
 *  Copyright  2010  Dmytro Milinevskyy
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

#include <libdodo/string.h>

#include <stdio.h>
#include <string.h>

using namespace dodo;

string::string() : buf(NULL),
                   bufSize(0),
                   strLen(0)
{
}

//-------------------------------------------------------------------

string::string(const string &s) : buf(NULL),
                                  bufSize(s.bufSize),
                                  strLen(s.strLen)
{
    if (bufSize) {
        buf = new char[bufSize];
        memcpy(buf, s.buf, bufSize);
    }
}

//-------------------------------------------------------------------

string::string(const char *data) : buf(NULL),
                                   bufSize(0),
                                   strLen(0)
{
    if (data) {
        strLen = strlen(data);
        if (strLen) {
            bufSize = strLen + 1;
            buf = new char[bufSize];
            memcpy(buf, data, bufSize);
        }
    }
}

//-------------------------------------------------------------------

string::string(const char *str,
               unsigned long length) : buf(NULL),
                                       bufSize(0),
                                       strLen(length)
{
    if (str && strLen) {
        bufSize = strLen + 1;
        buf = new char[bufSize];
        memcpy(buf, str, strLen);
        buf[strLen] = 0x0;
    }
}

//-------------------------------------------------------------------

string::string(char symbol,
               unsigned long count) : buf(NULL),
                                      bufSize(0),
                                      strLen(count)
{
    if (strLen) {
        bufSize = strLen + 1;
        buf = new char[bufSize];
        memset(buf, symbol, strLen);
        buf[strLen] = 0x0;
    }
}

//-------------------------------------------------------------------

string::~string()
{
    if (buf)
        delete [] buf;
}

//-------------------------------------------------------------------

const char *
string::data() const
{
    return buf;
}

//-------------------------------------------------------------------

unsigned long
string::size() const
{
    return strLen;
}

//-------------------------------------------------------------------

bool
string::empty() const
{
    return !(buf && strLen);
}

//-------------------------------------------------------------------

void
string::erase(unsigned long index,
              unsigned long count)
{
    if (!buf)
        return;

    unsigned long end = index + count;

    if (end >= strLen) {
        buf[strLen = index] = 0x0;
    } else {
        memmove(buf+index, buf+end, count);
        strLen -= count;
    }
}

//-------------------------------------------------------------------

void
string::clear()
{
    strLen = 0;
    if (buf)
        buf[strLen] = 0x0;
}

//-------------------------------------------------------------------

void
string::resize(unsigned long len)
{
    if (len == strLen)
        return;

    if (!(len < strLen || bufSize > len)) {
        unsigned long newBufSize = len + 1;

        char *newBuf = new char[newBufSize];

        if (buf) {
            memcpy(newBuf, buf, strLen+1);
            delete [] buf;
        }
        /* memset(newBuf+strLen, 0x0, newBufSize - bufSize); */

        buf = newBuf;
        bufSize = newBufSize;
    }

    strLen = len;
    buf[strLen] = 0x0;
}

//-------------------------------------------------------------------

unsigned long
string::find(const string &str,
             unsigned long index) const
{
    if (!buf || !str.buf || index > strLen)
        return POSITION_END;

    return strstr(buf+index, str.buf) - buf;
}

//-------------------------------------------------------------------

string &
string::replace(unsigned long index,
                unsigned long num,
                const string &str)
{
    // TODO
}

//-------------------------------------------------------------------

string &
string::insert(unsigned long index,
               const string &str)
{
    // TODO
}

//-------------------------------------------------------------------

string
string::substr(unsigned long index,
               unsigned long length) const
{
    if (!buf || index > strLen)
        return string(); // FIXME: throw exception

    return string(buf+index, length+index>strLen?strLen-index:length);
}

//-------------------------------------------------------------------

void
string::reserve(unsigned long count)
{
    // TODO
}

//-------------------------------------------------------------------

string &
string::operator=(const string &data)
{
    strLen = data.strLen;

    if (buf)
        delete [] buf;

    if (strLen) {
        bufSize = strLen + 1;
        buf = new char[bufSize];
        memcpy(buf, data.buf, bufSize);
    }
}

//-------------------------------------------------------------------

string &
string::operator+=(const string &str)
{
    // TODO
}

//-------------------------------------------------------------------

char
string::operator[](unsigned long index) const
{
    if (!buf || index > strLen)
        return 0x0; // FIXME: throw exception

    return buf[index];
}

//-------------------------------------------------------------------

string::operator const char *() const
{
    return buf;
}


//-------------------------------------------------------------------

bool
operator<(const string &s1,
          const string &s2)
{
    // TODO
}

//-------------------------------------------------------------------

bool
operator>(const string &s1,
          const string &s2)
{
    // TODO
}

//-------------------------------------------------------------------

bool
operator==(const string &s1,
           const string &s2)
{
    // TODO
}

//-------------------------------------------------------------------

bool
operator==(const string &s1,
           const char *s2)
{
    // TODO
}

//-------------------------------------------------------------------

string &
operator+(const string &s1,
          const string &s2)
{
    // TODO
}

//-------------------------------------------------------------------
