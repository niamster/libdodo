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

rpcValue::rpcValue() : valueDataType(RPC_DATATYPE_STRING)
{
	
}

//-------------------------------------------------------------------

rpcValue::rpcValue(const dodoString &value) : valueDataType(RPC_DATATYPE_STRING),
						stringValue(value)
{
	
}

//-------------------------------------------------------------------

rpcValue::rpcValue(long value) : valueDataType(RPC_DATATYPE_INTEGER),
						integerValue(value)
{
	
}

//-------------------------------------------------------------------

rpcValue::rpcValue(double value) : valueDataType(RPC_DATATYPE_DOUBLE),
						doubleValue(value)
{
	
}

//-------------------------------------------------------------------

rpcValue::rpcValue(bool value) : valueDataType(RPC_DATATYPE_BOOLEAN),
						booleanValue(value)
{
	
}

//-------------------------------------------------------------------

rpcValue::rpcValue(const dodoArray<rpcValue> &value) : valueDataType(RPC_DATATYPE_ARRAY),
						arrayValue(value)
{
	
}

//-------------------------------------------------------------------

rpcValue::rpcValue(const dodoMap<dodoString, rpcValue, stringTools::equal> &value) : valueDataType(RPC_DATATYPE_STRUCT),
						structValue(value)
{
	
}

//-------------------------------------------------------------------

rpcValue::~rpcValue()
{
	
}

//-------------------------------------------------------------------

void
rpcValue::clear()
{
	stringValue.clear();
	arrayValue.clear();
	structValue.clear();
}

//-------------------------------------------------------------------

void
rpcValue::setType(short type)
{
	valueDataType = type;
}

//-------------------------------------------------------------------

void
rpcValue::setString(const dodoString &value)
{
	stringValue = value;
}

//-------------------------------------------------------------------

void
rpcValue::setBoolean(bool value)
{
	booleanValue = value;
}

//-------------------------------------------------------------------

void
rpcValue::setInteger(long value)
{
	integerValue = value;
}

//-------------------------------------------------------------------

void
rpcValue::setDouble(double value)
{
	doubleValue = value;
}

//-------------------------------------------------------------------

void
rpcValue::addArrayElement(const rpcValue &value)
{
	arrayValue.push_back(value);
}

//-------------------------------------------------------------------

void
rpcValue::addStructMember(const dodoString &name, 
								const rpcValue &value)
{
	structValue.insert(name, value);
}

//-------------------------------------------------------------------

void 
rpcValue::setArray(const dodoArray<rpcValue> &value)
{
	arrayValue = value;
}

//-------------------------------------------------------------------

void 
rpcValue::setStruct(const dodoMap<dodoString, rpcValue, stringTools::equal> &value)
{
	structValue = value;
}

//-------------------------------------------------------------------

rpcValue 
rpcValue::operator[](const dodoString &name)
{
	if (valueDataType != RPC_DATATYPE_STRUCT)
		throw baseEx(ERRMODULE_RPC, RPCVALUEEX_BROPERATORSTRING, ERR_LIBDODO, RPCVALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return structValue[name];
}

//-------------------------------------------------------------------

rpcValue 
rpcValue::operator[](unsigned long key)
{
	if (valueDataType != RPC_DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_RPC, RPCVALUEEX_BROPERATORNUMERIC, ERR_LIBDODO, RPCVALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	if (key >= arrayValue.size())
		throw baseEx(ERRMODULE_RPC, RPCVALUEEX_BROPERATORNUMERIC, ERR_LIBDODO, RPCVALUEEX_ARRAYOUTOFRANGE, RPCVALUEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);

	return arrayValue[key];
}

//-------------------------------------------------------------------

short 
rpcValue::getType()
{
	return valueDataType;
}

//-------------------------------------------------------------------

dodoString 
rpcValue::getString()
{
	if (valueDataType != RPC_DATATYPE_STRING)
		throw baseEx(ERRMODULE_RPC, RPCVALUEEX_GETSTRING, ERR_LIBDODO, RPCVALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return stringValue;
}

//-------------------------------------------------------------------

bool 
rpcValue::getBoolean()
{
	if (valueDataType != RPC_DATATYPE_BOOLEAN)
		throw baseEx(ERRMODULE_RPC, RPCVALUEEX_GETBOOLEAN, ERR_LIBDODO, RPCVALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return booleanValue;
}

//-------------------------------------------------------------------

long 
rpcValue::getInteger()
{
	if (valueDataType != RPC_DATATYPE_INTEGER)
		throw baseEx(ERRMODULE_RPC, RPCVALUEEX_GETINTEGER, ERR_LIBDODO, RPCVALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return integerValue;
	
}

//-------------------------------------------------------------------

double 
rpcValue::getDouble()
{
	if (valueDataType != RPC_DATATYPE_DOUBLE)
		throw baseEx(ERRMODULE_RPC, RPCVALUEEX_GETDOUBLE, ERR_LIBDODO, RPCVALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return doubleValue;
	
}

//-------------------------------------------------------------------

dodoArray<rpcValue> 
rpcValue::getArray()
{
	if (valueDataType != RPC_DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_RPC, RPCVALUEEX_GETARRAY, ERR_LIBDODO, RPCVALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodoString, rpcValue, stringTools::equal> 
rpcValue::getStruct()
{
	if (valueDataType != RPC_DATATYPE_STRUCT)
		throw baseEx(ERRMODULE_RPC, RPCVALUEEX_GETSTRUCT, ERR_LIBDODO, RPCVALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return structValue;
	
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
rpcMethod::addArgument(const dodoString &name,
					const rpcValue &argument)
{
	arguments.insert(name, argument);
}

//-------------------------------------------------------------------
