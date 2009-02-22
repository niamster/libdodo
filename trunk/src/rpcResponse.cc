/***************************************************************************
 *            rpcResponse.cc
 *
 *  Sat Mar 22 00:06:55 2008
 *  Copyright  2008  Ni@m
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

#include <libdodo/rpcResponse.h>

using namespace dodo::rpc;

response::response() : succ(false)
{
}

//-------------------------------------------------------------------

response::~response()
{
}

//-------------------------------------------------------------------

value response::getValue(unsigned long position)
{
	if (position >= values.size())
	{
		throw exception::basic(exception::ERRMODULE_RPCRESPONSE, RESPONSEEX_GETVALUE, exception::ERRNO_LIBDODO, RESPONSEEX_ARRAYOUTOFBOUNDS, RPCRESPONSEEX_ARRAYOUTOFBOUNDS_STR, __LINE__, __FILE__);
	}

	return values[position];
}

//-------------------------------------------------------------------

bool response::isSuccessful()
{
	return succ;
}

//-------------------------------------------------------------------

void response::addArgument(const value &argument)
{
	succ = true;

	values.push_back(argument);
}

//-------------------------------------------------------------------

void response::fault(const value &argument)
{
	succ = false;

	values.assign(1, argument);
}

//-------------------------------------------------------------------

value
response::operator[](unsigned long position)
{
	if (position >= values.size())
	{
		throw exception::basic(exception::ERRMODULE_RPCRESPONSE, RESPONSEEX_BROPERATORUNSIGNEDLONG, exception::ERRNO_LIBDODO, RESPONSEEX_ARRAYOUTOFBOUNDS, RPCRESPONSEEX_ARRAYOUTOFBOUNDS_STR, __LINE__, __FILE__);
	}

	return values[position];
}

//-------------------------------------------------------------------

unsigned long response::getValuesCount()
{
	return values.size();
}

//-------------------------------------------------------------------

dodoArray<value>response::getValues()
{
	return values;
}

//-------------------------------------------------------------------

void response::clear()
{
	values.clear();
}

//-------------------------------------------------------------------
