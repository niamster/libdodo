/***************************************************************************
 *            rpcValue.cc
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

#include <libdodo/rpcValue.h>

using namespace dodo::rpc;

value::value() : valueDataType(DATATYPE_STRING)
{
	
}

//-------------------------------------------------------------------

value::value(const dodoString &value) : valueDataType(DATATYPE_STRING),
						stringValue(value)
{
	
}

//-------------------------------------------------------------------

value::value(long value) : valueDataType(DATATYPE_INTEGER),
						integerValue(value)
{
	
}

//-------------------------------------------------------------------

value::value(double value) : valueDataType(DATATYPE_DOUBLE),
						doubleValue(value)
{
	
}

//-------------------------------------------------------------------

value::value(bool value) : valueDataType(DATATYPE_BOOLEAN),
						booleanValue(value)
{
	
}

//-------------------------------------------------------------------

value::value(const dodoArray<value> &value) : valueDataType(DATATYPE_ARRAY),
						arrayValue(value)
{
	
}

//-------------------------------------------------------------------

value::value(const dodoMap<dodoString, value, dodoMapStringCompare> &value) : valueDataType(DATATYPE_STRUCT),
						structValue(value)
{
	
}

//-------------------------------------------------------------------

value::~value()
{
	
}

//-------------------------------------------------------------------

void
value::clear()
{
	stringValue.clear();
	arrayValue.clear();
	structValue.clear();
}

//-------------------------------------------------------------------

void
value::setString(const dodoString &value)
{
	valueDataType = DATATYPE_STRING;
	
	stringValue = value;
}

//-------------------------------------------------------------------

void
value::setBoolean(bool value)
{
	valueDataType = DATATYPE_BOOLEAN;

	booleanValue = value;
}

//-------------------------------------------------------------------

void
value::setInteger(long value)
{
	valueDataType = DATATYPE_INTEGER;

	integerValue = value;
}

//-------------------------------------------------------------------

void
value::setDouble(double value)
{
	valueDataType = DATATYPE_DOUBLE;

	doubleValue = value;
}

//-------------------------------------------------------------------

void
value::addArrayElement(const value &value)
{
	valueDataType = DATATYPE_ARRAY;

	arrayValue.push_back(value);
}

//-------------------------------------------------------------------

void
value::addStructMember(const dodoString &name, 
								const value &value)
{
	valueDataType = DATATYPE_STRUCT;

	structValue.insert(make_pair(name, value));
}

//-------------------------------------------------------------------

void 
value::setArray(const dodoArray<value> &value)
{
	valueDataType = DATATYPE_ARRAY;

	arrayValue = value;
}

//-------------------------------------------------------------------

void 
value::setStruct(const dodoMap<dodoString, value, dodoMapStringCompare> &value)
{
	valueDataType = DATATYPE_STRUCT;

	structValue = value;
}

//-------------------------------------------------------------------

value 
value::operator[](const dodoString &name)
{
	if (valueDataType != DATATYPE_STRUCT)
		throw baseEx(ERRMODULE_RPCVALUE, VALUEEX_BROPERATORSTRING, ERR_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return structValue[name];
}

//-------------------------------------------------------------------

value 
value::operator[](unsigned long key)
{
	if (valueDataType != DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_RPCVALUE, VALUEEX_BROPERATORNUMERIC, ERR_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	if (key >= arrayValue.size())
		throw baseEx(ERRMODULE_RPCVALUE, VALUEEX_BROPERATORNUMERIC, ERR_LIBDODO, VALUEEX_ARRAYOUTOFRANGE, RPCVALUEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);

	return arrayValue[key];
}

//-------------------------------------------------------------------

short 
value::getType()
{
	return valueDataType;
}

//-------------------------------------------------------------------

dodoString 
value::getString()
{
	if (valueDataType != DATATYPE_STRING)
		throw baseEx(ERRMODULE_RPCVALUE, VALUEEX_GETSTRING, ERR_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return stringValue;
}

//-------------------------------------------------------------------

bool 
value::getBoolean()
{
	if (valueDataType != DATATYPE_BOOLEAN)
		throw baseEx(ERRMODULE_RPCVALUE, VALUEEX_GETBOOLEAN, ERR_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return booleanValue;
}

//-------------------------------------------------------------------

long 
value::getInteger()
{
	if (valueDataType != DATATYPE_INTEGER)
		throw baseEx(ERRMODULE_RPCVALUE, VALUEEX_GETINTEGER, ERR_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return integerValue;
	
}

//-------------------------------------------------------------------

double 
value::getDouble()
{
	if (valueDataType != DATATYPE_DOUBLE)
		throw baseEx(ERRMODULE_RPCVALUE, VALUEEX_GETDOUBLE, ERR_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return doubleValue;
	
}

//-------------------------------------------------------------------

dodoArray<value> 
value::getArray()
{
	if (valueDataType != DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_RPCVALUE, VALUEEX_GETARRAY, ERR_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodoString, value, dodo::dodoMapStringCompare> 
value::getStruct()
{
	if (valueDataType != DATATYPE_STRUCT)
		throw baseEx(ERRMODULE_RPCVALUE, VALUEEX_GETSTRUCT, ERR_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return structValue;
	
}

//-------------------------------------------------------------------
