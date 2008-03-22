/***************************************************************************
 *            rpc.cc
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

#include <libdodo/rpc.h>

using namespace dodo;

rpcMethodArgument::rpcMethodArgument()
{
	
}

//-------------------------------------------------------------------

rpcMethodArgument::~rpcMethodArgument()
{
	
}

//-------------------------------------------------------------------

void
rpcMethodArgument::setName(const dodoString &name)
{
	this->name = name;
}

//-------------------------------------------------------------------

void
rpcMethodArgument::clear()
{
	stringValue.clear();
	arrayValue.clear();
	structValue.clear();
}

//-------------------------------------------------------------------

void
rpcMethodArgument::setType(short type)
{
	this->type = type;
}

//-------------------------------------------------------------------

void
rpcMethodArgument::setStringValue(const dodoString &value)
{
	stringValue = value;
}

//-------------------------------------------------------------------

void
rpcMethodArgument::setBooleanValue(bool value)
{
	booleanValue = value;
}

//-------------------------------------------------------------------

void
rpcMethodArgument::setIntegerValue(long value)
{
	integerValue = value;
}

//-------------------------------------------------------------------

void
rpcMethodArgument::setDoubleValue(double value)
{
	doubleValue = value;
}

//-------------------------------------------------------------------

void
rpcMethodArgument::addArrayValue(const rpcMethodArgument &value)
{
	arrayValue.push_back(value);
}

//-------------------------------------------------------------------

void
rpcMethodArgument::addStructValue(const dodoString &name, 
								const rpcMethodArgument &value)
{
	structValue.insert(name, value);
}

//-------------------------------------------------------------------

rpcMethod::rpcMethod()
{
	
}

//-------------------------------------------------------------------

rpcMethod::~rpcMethod()
{
	
}

//-------------------------------------------------------------------

void
rpcMethod::setName(const dodoString &name)
{
	this->name = name;
}

//-------------------------------------------------------------------

void
rpcMethod::clear()
{
	arguments.clear();
}

//-------------------------------------------------------------------

void
rpcMethod::addArgument(const rpcMethodArgument &argument)
{
	arguments.push_back(argument);
}

//-------------------------------------------------------------------
