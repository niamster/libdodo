/***************************************************************************
 *            rpcResponse.cc
 *
 *  Sat Mar 22 2008
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

#include <libdodo/rpcResponse.h>

#include <libdodo/types.h>
#include <libdodo/rpcValue.h>
#include <libdodo/rpcResponseEx.h>

using namespace dodo::rpc;

response::response() : succ(false)
{
}

//-------------------------------------------------------------------

response::~response()
{
}

//-------------------------------------------------------------------

value
response::value(unsigned long position)
{
    if (position >= rValues.size())
        throw exception::basic(exception::MODULE_RPCRESPONSE, RESPONSEEX_GETVALUE, exception::ERRNO_LIBDODO, RESPONSEEX_ARRAYOUTOFBOUNDS, RPCRESPONSEEX_ARRAYOUTOFBOUNDS_STR, __LINE__, __FILE__);

    return rValues[position];
}

//-------------------------------------------------------------------

bool
response::isSuccessful()
{
    return succ;
}

//-------------------------------------------------------------------

void
response::addArgument(const dodo::rpc::value &argument)
{
    succ = true;

    rValues.push_back(argument);
}

//-------------------------------------------------------------------

void
response::fault(const dodo::rpc::value &argument)
{
    succ = false;

    rValues.assign(1, argument);
}

//-------------------------------------------------------------------

dodo::rpc::value
response::operator[](unsigned long position)
{
    if (position >= rValues.size())
        throw exception::basic(exception::MODULE_RPCRESPONSE, RESPONSEEX_BROPERATORUNSIGNEDLONG, exception::ERRNO_LIBDODO, RESPONSEEX_ARRAYOUTOFBOUNDS, RPCRESPONSEEX_ARRAYOUTOFBOUNDS_STR, __LINE__, __FILE__);

    return rValues[position];
}

//-------------------------------------------------------------------

unsigned long
response::valuesCount()
{
    return rValues.size();
}

//-------------------------------------------------------------------

dodoArray<dodo::rpc::value>
response::values()
{
    return rValues;
}

//-------------------------------------------------------------------

void
response::clear()
{
    rValues.clear();
}

//-------------------------------------------------------------------
