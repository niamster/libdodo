/***************************************************************************
 *            toolsMisc.cc
 *
 *  Wed Aug 24 2005
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

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsMiscEx.h>

using namespace dodo::tools;

//-------------------------------------------------------------------

void
misc::randomBlob(void          *data,
             unsigned long size,
             short         strength)
{
    FILE *file;

    if (strength == RANDOM_STRENGTH_WEAK) {
        srand(time(NULL));

        unsigned long i = 0;
        for (;i<size;i+=sizeof(int)) {
            ((int *)data)[i] = rand();
        }
        unsigned long reminder = size%sizeof(int);
        if (reminder) {
            size = rand();
            memcpy((char *)data+(i-sizeof(int)), &size, reminder);
        }
    } else if (strength == RANDOM_STRENGTH_NORMAL) {
        file = fopen("/dev/urandom", "r");
        if (file == NULL)
            dodo_throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    } else if (strength == RANDOM_STRENGTH_STRONG) {
            file = fopen("/dev/random", "r");
            if (file == NULL)
                dodo_throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    } else {
        dodo_throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_LIBDODO, MISCEX_WRONGSTRENGTH, TOOLSMISCEX_WRONGSTRENGTH_STR, __LINE__, __FILE__);
    }

    while (true) {
        if (fread(data, size, 1, file) == 0) {
            if (feof(file) != 0 || errno == EAGAIN)
                break;

            if (errno == EINTR)
                continue;

            if (ferror(file) != 0)
                dodo_throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
        }

        break;
    }

    if (fclose(file) != 0)
        dodo_throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

dodo::string
misc::randomString(unsigned long size,
                   short         strength)
{
    dodo::string s('\0', size);
    char *data = (char *)s.data();

    randomBlob(data, size, strength);

    for (unsigned long i = 0; i < size; ++i)
        if (data[i] == '\0')
            data[i] = '*';

    return s;
}

//-------------------------------------------------------------------

bool
misc::isInArray(const dodoStringArray &arr,
                const dodo::string      &needle,
                bool                  icase)
{
    bool (*cmpFunc)(const dodo::string &,
                    const dodo::string &);

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
               const dodo::string     &needle,
               bool                 icase)
{
    bool (*cmpFunc)(const dodo::string &,
                    const dodo::string &);

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
misc::split(const dodo::string &fields,
            const dodo::string &separator,
            int              limit)
{
    unsigned long i(0), j(0), sep_size(separator.size());
    int k(1);
    dodoStringArray arr;

    while (true) {
        if (limit != -1) {
            if (k > limit) {
                arr.back() += dodo::string(fields.data() + j - sep_size);

                break;
            }
            ++k;
        }

        i = fields.find(separator, i);
        if (i == dodo::string::POSITION_END) {
            arr.push_back(dodo::string(fields.data() + j, fields.size() - j));

            break;
        } else
            arr.push_back(dodo::string(fields.data() + j, i - j));

        i += sep_size;
        j = i;
    }

    return arr;
}

//-------------------------------------------------------------------

dodo::string
misc::join(const dodoStringArray &fields,
           const dodo::string      &separator,
           int                   limit)
{
    if (fields.size() == 0)
        dodo_throw exception::basic(exception::MODULE_TOOLSMISC, MISCEX_JOIN, exception::ERRNO_LIBDODO, MISCEX_EMPTYARRAY, TOOLSMISCEX_EMPTYARRAY_STR, __LINE__, __FILE__);

    int k(0);

    dodo::string temp;
    dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
    if (i != j) {
        --j;
        for (; i != j; ++i) {
            if (limit != -1) {
                if (k > limit)
                    return temp;

                ++k;
            }
            temp += dodo::string(*i);
            temp += dodo::string(separator);
        }
        temp += dodo::string(*i);
    }

    return temp;
}

//-------------------------------------------------------------------
