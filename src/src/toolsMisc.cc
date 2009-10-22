/***************************************************************************
 *            toolsMisc.cc
 *
 *  Wed Aug 24 2005
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

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsMiscEx.h>

using namespace dodo::tools;

//-------------------------------------------------------------------

void
misc::random(void          *data,
             unsigned long size,
             short         strength)
{
    FILE *file;

    if (strength == RANDOM_STRENGTH_DEFAULT) {
        file = fopen("/dev/urandom", "r");
        if (file == NULL)
            throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    } else {
        if (strength == RANDOM_STRENGTH_STRONG) {
            file = fopen("/dev/random", "r");
            if (file == NULL)
                throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        } else
            throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_LIBDODO, MISCEX_WRONGSTRENGTH, TOOLSMISCEX_WRONGSTRENGTH_STR, __LINE__, __FILE__);
    }

    while (true) {
        if (fread(data, size, 1, file) == 0) {
            if (feof(file) != 0 || errno == EAGAIN)
                break;

            if (errno == EINTR)
                continue;

            if (ferror(file) != 0)
                throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        break;
    }

    if (fclose(file) != 0)
        throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

dodoString
misc::stringRandom(unsigned long size,
                   short         strength)
{
    char *data = new char[size + 1];
    try {
        random(data, size, strength);
    } catch (...) {
        delete [] data;

        throw;
    }
    for (unsigned long i = 0; i < size; ++i)
        if (data[i] == '\0')
            data[i] = '*';

    dodoString res(data, size);
    delete [] data;

    return res;
}

//-------------------------------------------------------------------

unsigned long
misc::ulRandom(short strength)
{
    unsigned long rnd;

    random(&rnd, sizeof(unsigned long), strength);

    return rnd;
}

//-------------------------------------------------------------------

long
misc::lRandom(short strength)
{
    long rnd;

    random(&rnd, sizeof(long), strength);

    return rnd;
}

//-------------------------------------------------------------------

unsigned int
misc::uiRandom(short strength)
{
    unsigned int rnd;

    random(&rnd, sizeof(unsigned int), strength);

    return rnd;
}

//-------------------------------------------------------------------

int
misc::iRandom(short strength)
{
    int rnd;

    random(&rnd, sizeof(int), strength);

    return rnd;
}

//-------------------------------------------------------------------

unsigned short
misc::usRandom(short strength)
{
    unsigned short rnd;

    random(&rnd, sizeof(unsigned short), strength);

    return rnd;
}

//-------------------------------------------------------------------

short
misc::sRandom(short strength)
{
    short rnd;

    random(&rnd, sizeof(short), strength);

    return rnd;
}

//-------------------------------------------------------------------

unsigned char
misc::ucRandom(short strength)
{
    unsigned char rnd;

    random(&rnd, sizeof(unsigned char), strength);

    return rnd;
}

//-------------------------------------------------------------------

char
misc::cRandom(short strength)
{
    char rnd;

    random(&rnd, sizeof(char), strength);

    return rnd;
}

//-------------------------------------------------------------------

double
misc::dRandom(short strength)
{
    double rnd;

    random(&rnd, sizeof(double), strength);

    return rnd;
}

//-------------------------------------------------------------------

bool
misc::isInArray(const dodoStringArray &arr,
                const dodoString      &needle,
                bool                  icase)
{
    bool (*cmpFunc)(const dodoString &,
                    const dodoString &);

    if (icase)
        cmpFunc = string::iequal;
    else
        cmpFunc = string::equal;

    dodoStringArray::const_iterator i(arr.begin()), j(arr.end());
    for (; i != j; ++i)
        if (cmpFunc(*i, needle))
            return true;

    return false;
}

//-------------------------------------------------------------------

bool
misc::isInList(const dodoStringList &arr,
               const dodoString     &needle,
               bool                 icase)
{
    bool (*cmpFunc)(const dodoString &,
                    const dodoString &);

    if (icase)
        cmpFunc = string::iequal;
    else
        cmpFunc = string::equal;

    dodoStringList::const_iterator i(arr.begin()), j(arr.end());
    for (; i != j; ++i)
        if (cmpFunc(*i, needle))
            return true;

    return false;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
misc::split(const dodoString &fields,
            const dodoString &separator,
            int              limit)
{
    unsigned long i(0), j(0), sep_size(separator.size());
    int k(1);
    dodoStringArray arr;

    while (true) {
        if (limit != -1) {
            if (k > limit) {
                arr.back().append(fields.data() + j - sep_size);

                break;
            }
            ++k;
        }

        i = fields.find(separator, i);
        if (i == dodoString::npos) {
            arr.push_back(dodoString(fields.data() + j, fields.size() - j));

            break;
        } else
            arr.push_back(dodoString(fields.data() + j, i - j));

        i += sep_size;
        j = i;
    }

    return arr;
}

//-------------------------------------------------------------------

dodoString
misc::join(const dodoStringArray &fields,
           const dodoString      &separator,
           int                   limit)
{
    if (fields.size() == 0)
        throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_JOIN, exception::ERRNO_LIBDODO, MISCEX_EMPTYARRAY, TOOLSMISCEX_EMPTYARRAY_STR, __LINE__, __FILE__);

    int k(0);

    dodoString temp;
    dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
    if (i != j) {
        --j;
        for (; i != j; ++i) {
            if (limit != -1) {
                if (k > limit)
                    return temp;

                ++k;
            }
            temp.append(*i);
            temp.append(separator);
        }
        temp.append(*i);
    }

    return temp;
}

//-------------------------------------------------------------------
