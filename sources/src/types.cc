/***************************************************************************
 *            types.cc
 *
 *  Tue Jul  10 2007
 *  Copyright  2007  Ni@m
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

#include <vector>
#include <list>
#include <deque>
#include <map>
#include <string>
#include <string.h>

#include <libdodo/types.h>

namespace dodo {
    dodo::string __dodostring__;

    //-------------------------------------------------------------------

    dodoStringArray __dodostringarray__;

    //-------------------------------------------------------------------

    dodoStringMap __dodostringmap__;

    //-------------------------------------------------------------------

    dodoArray<dodoStringArray> __dodostringarrayarray__;

    //-------------------------------------------------------------------

    dodoStringMapArray __dodostringmaparray__;
};

//-------------------------------------------------------------------

using namespace dodo;

//-------------------------------------------------------------------

bool
dodoMapStringCompare::operator()(const dodo::string &first,
                                 const dodo::string &second)
{
    return strcmp(first.data(), second.data()) < 0 ? true : false;
}

//-------------------------------------------------------------------

bool
dodoMapICaseStringCompare::operator()(const dodo::string &first,
                                      const dodo::string &second)
{
    return strcasecmp(first.data(), second.data()) < 0 ? true : false;
}

//-------------------------------------------------------------------

bool
dodoMapStringCompare::operator()(const dodo::string &first,
                                 const dodo::string &second) const
{
    return strcmp(first.data(), second.data()) < 0 ? true : false;
}

//-------------------------------------------------------------------

bool
dodoMapICaseStringCompare::operator()(const dodo::string &first,
                                      const dodo::string &second) const
{
    return strcasecmp(first.data(), second.data()) < 0 ? true : false;
}

//-------------------------------------------------------------------

