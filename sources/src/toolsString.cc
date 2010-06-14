/***************************************************************************
 *            toolsString.cc
 *
 *  Sun Oct 30 2007
 *  Copyright  2007  Dmytro Milinevskyy
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

#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <libdodo/toolsString.h>
#include <libdodo/types.h>

using namespace dodo::tools;

bool
string::equal(const dodo::string &first,
              const dodo::string &second)
{
    unsigned long firstSize = first.size();

    if (firstSize != second.size())
        return false;

    if (firstSize == 0)
        return true;

    for (unsigned long i = 0; i < firstSize; ++i)
        if (first[i] != second[i])
            return false;

    return true;
}

//-------------------------------------------------------------------

bool
string::iequal(const dodo::string &first,
               const dodo::string &second)
{
    unsigned long firstSize = first.size();

    if (firstSize != second.size())
        return false;

    if (firstSize == 0)
        return true;

    for (unsigned long i = 0; i < firstSize; ++i)
        if (tolower(first[i]) != tolower(second[i]))
            return false;

    return true;
}

//-------------------------------------------------------------------

bool
string::contains(const dodo::string &str,
                 const dodo::string &needle,
                 bool             icase)
{
    if (icase) {
        if (strcasestr(str.data(), needle.data()) != NULL)
            return true;
    } else if (strstr(str.data(), needle.data()) != NULL)
        return true;


    return false;
}

//-------------------------------------------------------------------

unsigned long
string::find(const dodo::string &str,
             const dodo::string &needle,
             unsigned long    position,
             bool             icase)
{
    if (position > str.size())
        return dodo::string::POSITION_END;

    char *pos = NULL;

    if (icase) {
        if ((pos = strcasestr((char *)(str.data() + position), needle.data())) == NULL)
            return dodo::string::POSITION_END;
        else
            return (pos - str.data());
    } else {
        if ((pos = strstr((char *)(str.data() + position), needle.data())) == NULL)
            return dodo::string::POSITION_END;
        else
            return (pos - str.data());
    }

    return dodo::string::POSITION_END;
}

//-------------------------------------------------------------------

unsigned long
string::find(const dodo::string &str,
             const dodo::string &needle,
             bool             icase)
{
    return find(str, needle, 0, icase);
}

//-------------------------------------------------------------------

dodo::string
string::format(const dodo::string &format,
               ...)
{
    unsigned long length = format.size() * 3;
    char *str = new char[length + 1];

    va_list ap;

    va_start(ap, format);
    vsnprintf(str, length, format.data(), ap);
    va_end(ap);

    dodo::string res = str;

    delete [] str;

    return res;
}

//-------------------------------------------------------------------

dodo::string
string::lToString(long number)
{
    char temp[NUMERIC_STRING_SIZE];
    sprintf(temp, "%ld", number);

    return temp;
}

//-------------------------------------------------------------------

dodo::string
string::ulToString(unsigned long number)
{
    char temp[NUMERIC_STRING_SIZE];
    sprintf(temp, "%ld", number);

    return temp;
}

//-------------------------------------------------------------------

dodo::string
string::iToString(int number)
{
    char temp[NUMERIC_STRING_SIZE];
    sprintf(temp, "%d", number);

    return temp;
}

//-------------------------------------------------------------------

dodo::string
string::uiToString(unsigned int number)
{
    char temp[NUMERIC_STRING_SIZE];
    sprintf(temp, "%u", number);

    return temp;
}

//-------------------------------------------------------------------

dodo::string
string::sToString(short number)
{
    char temp[NUMERIC_STRING_SIZE];
    sprintf(temp, "%hd", number);

    return temp;
}

//-------------------------------------------------------------------

dodo::string
string::usToString(unsigned short number)
{
    char temp[NUMERIC_STRING_SIZE];
    sprintf(temp, "%hd", number);

    return temp;
}

//-------------------------------------------------------------------

dodo::string
string::fToString(float number)
{
    char temp[NUMERIC_STRING_SIZE];
    sprintf(temp, "%f", number);

    return temp;
}

//-------------------------------------------------------------------

dodo::string
string::dToString(double number)
{
    char temp[NUMERIC_STRING_SIZE];
    sprintf(temp, "%f", number);

    return temp;
}

//-------------------------------------------------------------------

dodo::string
string::lTrim(const dodo::string &data,
              char             symbol)
{
    int size = data.size(), i(0);

    for (; i < size; ++i)
        if (data[i] != symbol)
            break;

    return dodo::string(data.data() + i, size - i);
}

//-------------------------------------------------------------------

dodo::string
string::rTrim(const dodo::string &data,
              char             symbol)
{
    int i(data.size() - 1);

    for (; i >= 0; --i)
        if (data[i] != symbol)
            break;

    return dodo::string(data.data(), i + 1);
}

//-------------------------------------------------------------------

dodo::string
string::rTrim(const dodo::string &data,
              const char       symbols[],
              int              symCount)
{
    int i(data.size() - 1), j, empty;

    for (; i >= 0; --i) {
        for (j = 0, empty = 0; j < symCount; ++j)
            if (data[i] != symbols[j])
                ++empty;
        if (empty == symCount)
            break;
    }

    return dodo::string(data.data(), i + 1);
}

//-------------------------------------------------------------------

dodo::string
string::lTrim(const dodo::string &data,
              const char       symbols[],
              int              symCount)
{
    int size = data.size(), i(0), empty, j;

    for (; i < size; ++i) {
        for (j = 0, empty = 0; j < symCount; ++j)
            if (data[i] != symbols[j])
                ++empty;
        if (empty == symCount)
            break;
    }

    return dodo::string(data.data() + i, size - i);
}

//-------------------------------------------------------------------

dodo::string
string::trim(const dodo::string &data,
             const char       symbols[],
             int              symCount)
{
    return rTrim(lTrim(data, symbols, symCount), symbols, symCount);
}

//-------------------------------------------------------------------

dodo::string
string::trim(const dodo::string &data,
             char             symbol)
{
    return rTrim(lTrim(data, symbol), symbol);
}

//-------------------------------------------------------------------

long
string::stringToL(const dodo::string &data)
{
    return atol(data.data());
}

//-------------------------------------------------------------------

unsigned long
string::stringToUL(const dodo::string &data)
{
    return strtoul(data.data(), NULL, 10);
}

//-------------------------------------------------------------------

int
string::stringToI(const dodo::string &data)
{
    return atoi(data.data());
}

//-------------------------------------------------------------------

unsigned int
string::stringToUI(const dodo::string &data)
{
    return (unsigned int)atol(data.data());
}

//-------------------------------------------------------------------

short
string::stringToS(const dodo::string &data)
{
    return (short)atoi(data.data());
}

//-------------------------------------------------------------------

unsigned short
string::stringToUS(const dodo::string &data)
{
    return (unsigned short)atoi(data.data());
}

//-------------------------------------------------------------------

float
string::stringToF(const dodo::string &data)
{
    return (float)atof(data.data());
}

//-------------------------------------------------------------------

double
string::stringToD(const dodo::string &data)
{
    return atof(data.data());
}

//-------------------------------------------------------------------

void
string::replace(const dodoStringArray &needle,
                const dodoStringArray &replacement,
                dodo::string            &data)
{
    dodoStringArray::const_iterator i(needle.begin()), j(needle.end());
    dodoStringArray::const_iterator o(replacement.begin()), p(replacement.end());
    for (; i != j && o != p; ++i, ++o)
        replace(*i, *o, data);
}

//-------------------------------------------------------------------

void
string::replace(const dodo::string &needle,
                const dodo::string &replacement,
                dodo::string       &data)
{
    unsigned long i(0), j(needle.size()), k(replacement.size());

    while (true) {
        i = data.find(needle, i);
        if (i == dodo::string::POSITION_END)
            break;

        data.replace(i, j, replacement);
        i += k;
    }
}

//-------------------------------------------------------------------

