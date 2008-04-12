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

#include <libdodo/rpcResponse.h>

using namespace dodo;

rpcResponse::rpcResponse() : succ(false)
{
	
}

//-------------------------------------------------------------------

rpcResponse::~rpcResponse()
{
	
}

//-------------------------------------------------------------------

rpcValue
rpcResponse::getValue(unsigned long position)
{
	if (position > values.size())
		throw baseEx(ERRMODULE_RPCRESPONSE, RPCRESPONSEEX_GETVALUE, ERR_LIBDODO, RPCRESPONSEEX_ARRAYOUTOFBOUNDS, RPCRESPONSEEX_ARRAYOUTOFBOUNDS_STR, __LINE__, __FILE__);
	
	return values[position];
}

//-------------------------------------------------------------------

bool
rpcResponse::isSuccessful()
{
	return succ;
}

//-------------------------------------------------------------------

void
rpcResponse::addArgument(const rpcValue &argument)
{
	succ = true;
	
	values.push_back(argument);
}

//-------------------------------------------------------------------

void
rpcResponse::fault(const rpcValue &argument)
{
	succ = false;
	
	values.assign(1, argument);
}

//-------------------------------------------------------------------

rpcValue
rpcResponse::operator[](unsigned long position)
{
	if (position > values.size())
		throw baseEx(ERRMODULE_RPCRESPONSE, RPCRESPONSEEX_BROPERATORUNSIGNEDLONG, ERR_LIBDODO, RPCRESPONSEEX_ARRAYOUTOFBOUNDS, RPCRESPONSEEX_ARRAYOUTOFBOUNDS_STR, __LINE__, __FILE__);
	
	return values[position];
}

//-------------------------------------------------------------------

unsigned long 
rpcResponse::getValuesCount()
{
	return values.size();
}

//-------------------------------------------------------------------

dodoArray<rpcValue> 
rpcResponse::getValues()
{
	return values;
}

//-------------------------------------------------------------------

void
rpcResponse::clear()
{
	values.clear();
}

//-------------------------------------------------------------------
