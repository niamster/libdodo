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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <libdodo/rpcValue.h>
#include <libdodo/types.h>
#include <libdodo/rpcValueEx.h>

using namespace dodo::rpc;

value::value() : valueDataType(DATATYPE_BOOLEAN),
				 booleanValue(false)
{
}

//-------------------------------------------------------------------

value::value(const dodoString &value) : valueDataType(DATATYPE_STRING),
										stringValue(new dodoString(value))
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

value::value(const dodoArray<value> &nvalue) : valueDataType(DATATYPE_ARRAY),
											  arrayValue(new dodoArray<value>(nvalue))
{
}

//-------------------------------------------------------------------

value::value(const dodoMap<dodoString, value, dodoMapStringCompare> &nvalue) : valueDataType(DATATYPE_STRUCT),
																			  structValue(new dodoMap<dodoString, value, dodoMapStringCompare>(nvalue))
{
}

//-------------------------------------------------------------------

value::value(const value &on) : valueDataType(on.valueDataType)
{
	switch (valueDataType)
	{
		case DATATYPE_STRING:
			stringValue = new dodoString(*on.stringValue);
			break;

		case DATATYPE_STRUCT:
			structValue = new dodoMap<dodoString, value, dodoMapStringCompare>(*on.structValue);
			break;

		case DATATYPE_ARRAY:
			arrayValue = new dodoArray<value>(*on.arrayValue);
			break;

		case DATATYPE_INTEGER:
			integerValue = on.integerValue;
			break;

		case DATATYPE_DOUBLE:
			doubleValue = on.doubleValue;
			break;

		case DATATYPE_BOOLEAN:
			booleanValue = on.booleanValue;
			break;
	}
}

//-------------------------------------------------------------------

value::~value()
{
	switch (valueDataType)
	{
		case DATATYPE_STRING:
			delete stringValue;
			break;

		case DATATYPE_STRUCT:
			delete structValue;
			break;

		case DATATYPE_ARRAY:
			delete arrayValue;
			break;
	}
}

//-------------------------------------------------------------------

void
value::setString(const dodoString &nvalue)
{
	if (valueDataType != DATATYPE_STRING)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRUCT:
				delete structValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_STRING;
		stringValue = new dodoString(nvalue);
	}
	else
	{
		*stringValue = nvalue;
	}
}

//-------------------------------------------------------------------

void
value::setBoolean(bool value)
{
	if (valueDataType != DATATYPE_BOOLEAN)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_STRUCT:
				delete structValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_BOOLEAN;
	}

	booleanValue = value;
}

//-------------------------------------------------------------------

void
value::setInteger(long value)
{
	if (valueDataType != DATATYPE_INTEGER)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_STRUCT:
				delete structValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_INTEGER;
	}

	integerValue = value;
}

//-------------------------------------------------------------------

void
value::setDouble(double value)
{
	if (valueDataType != DATATYPE_DOUBLE)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_STRUCT:
				delete structValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_DOUBLE;
	}

	doubleValue = value;
}

//-------------------------------------------------------------------

void
value::addArrayElement(const value &nvalue)
{
	if (valueDataType != DATATYPE_ARRAY)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRUCT:
				delete structValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_ARRAY;
		arrayValue = new dodoArray<value>;
	}

	arrayValue->push_back(nvalue);
}

//-------------------------------------------------------------------

void
value::addStructMember(const dodoString &name,
					   const value      &nvalue)
{
	if (valueDataType != DATATYPE_STRUCT)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_STRUCT;
		structValue = new dodoMap<dodoString, value, dodoMapStringCompare>;
	}

	structValue->insert(make_pair(name,  nvalue));
}

//-------------------------------------------------------------------

void
value::setArray(const dodoArray<value> &nvalue)
{
	if (valueDataType != DATATYPE_ARRAY)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_STRUCT:
				delete structValue;
				break;
		}

		valueDataType = DATATYPE_ARRAY;
		arrayValue = new dodoArray<value>(nvalue);
	}
	else
	{
		*arrayValue = nvalue;
	}
}

//-------------------------------------------------------------------

void
value::setStruct(const dodoMap<dodoString, value, dodoMapStringCompare> &nvalue)
{
	if (valueDataType != DATATYPE_STRUCT)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_STRUCT;
		structValue = new dodoMap<dodoString, value, dodoMapStringCompare>(nvalue);
	}
	else
	{
		*structValue = nvalue;
	}
}

//-------------------------------------------------------------------

value
value::operator[](const dodoString &name)
{
	if (valueDataType != DATATYPE_STRUCT)
		throw exception::basic(exception::ERRMODULE_RPCVALUE, VALUEEX_BROPERATORSTRING, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return (*structValue)[name];
}

//-------------------------------------------------------------------

value
value::operator[](unsigned long key)
{
	if (valueDataType != DATATYPE_ARRAY)
		throw exception::basic(exception::ERRMODULE_RPCVALUE, VALUEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	if (key >= arrayValue->size())
		throw exception::basic(exception::ERRMODULE_RPCVALUE, VALUEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, VALUEEX_ARRAYOUTOFRANGE, RPCVALUEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);

	return (*arrayValue)[key];
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
		throw exception::basic(exception::ERRMODULE_RPCVALUE, VALUEEX_GETSTRING, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return *stringValue;
}

//-------------------------------------------------------------------

bool
value::getBoolean()
{
	if (valueDataType != DATATYPE_BOOLEAN)
		throw exception::basic(exception::ERRMODULE_RPCVALUE, VALUEEX_GETBOOLEAN, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return booleanValue;
}

//-------------------------------------------------------------------

long
value::getInteger()
{
	if (valueDataType != DATATYPE_INTEGER)
		throw exception::basic(exception::ERRMODULE_RPCVALUE, VALUEEX_GETINTEGER, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return integerValue;
}

//-------------------------------------------------------------------

double
value::getDouble()
{
	if (valueDataType != DATATYPE_DOUBLE)
		throw exception::basic(exception::ERRMODULE_RPCVALUE, VALUEEX_GETDOUBLE, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return doubleValue;
}

//-------------------------------------------------------------------

dodoArray<value>
value::getArray()
{
	if (valueDataType != DATATYPE_ARRAY)
		throw exception::basic(exception::ERRMODULE_RPCVALUE, VALUEEX_GETARRAY, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return *arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodoString, value, dodo::dodoMapStringCompare>
value::getStruct()
{
	if (valueDataType != DATATYPE_STRUCT)
		throw exception::basic(exception::ERRMODULE_RPCVALUE, VALUEEX_GETSTRUCT, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return *structValue;
}

//-------------------------------------------------------------------
