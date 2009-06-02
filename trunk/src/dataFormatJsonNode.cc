/***************************************************************************
 *            dataFormatJsonNode.cc
 *
 *  Mon Oct 20 02:15:55 2007
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

#include <libdodo/dataFormatJsonNode.h>
#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/dataFormatJsonNodeEx.h>

using namespace dodo::data::format::json;

node::node() : valueDataType(DATATYPE_NULL)
{
}

//-------------------------------------------------------------------

node::node(const dodoString &value) : valueDataType(DATATYPE_STRING),
									  stringValue(new dodoString(value))
{
}

//-------------------------------------------------------------------

node::node(long value) : valueDataType(DATATYPE_NUMERIC),
						 numericValue(value)
{
}

//-------------------------------------------------------------------

node::node(bool value) : valueDataType(DATATYPE_BOOLEAN),
						 booleanValue(value)
{
}

//-------------------------------------------------------------------

node::node(const dodoArray<node> &value) : valueDataType(DATATYPE_ARRAY),
										   arrayValue(new dodoArray<node>(value))
{
}

//-------------------------------------------------------------------

node::node(const dodoMap<dodoString, node, dodoMapStringCompare> &value) : valueDataType(DATATYPE_OBJECT),
																		   objectValue(new dodoMap<dodoString, node, dodoMapStringCompare>(value))
{
}

//-------------------------------------------------------------------

node::node(const node &on) : valueDataType(on.valueDataType)
{
	switch (valueDataType)
	{
		case DATATYPE_STRING:
			stringValue = new dodoString(*on.stringValue);
			break;

		case DATATYPE_OBJECT:
			objectValue = new dodoMap<dodoString, node, dodoMapStringCompare>(*on.objectValue);
			break;

		case DATATYPE_ARRAY:
			arrayValue = new dodoArray<node>(*on.arrayValue);
			break;

		case DATATYPE_NUMERIC:
			numericValue = on.numericValue;
			break;

		case DATATYPE_BOOLEAN:
			booleanValue = on.booleanValue;
			break;
	}
}

//-------------------------------------------------------------------

node::~node()
{
	switch (valueDataType)
	{
		case DATATYPE_STRING:
			delete stringValue;
			break;

		case DATATYPE_OBJECT:
			delete objectValue;
			break;

		case DATATYPE_ARRAY:
			delete arrayValue;
			break;
	}
}

//-------------------------------------------------------------------

void
node::setString(const dodoString &value)
{
	if (valueDataType != DATATYPE_STRING)
	{
		switch (valueDataType)
		{
			case DATATYPE_OBJECT:
				delete objectValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_STRING;
		stringValue = new dodoString(value);
	}
	else
	{
		*stringValue = value;
	}

}

//-------------------------------------------------------------------

void
node::setBoolean(bool value)
{
	if (valueDataType != DATATYPE_BOOLEAN)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_OBJECT:
				delete objectValue;
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
node::setNumeric(long value)
{
	if (valueDataType != DATATYPE_NUMERIC)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_OBJECT:
				delete objectValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_NUMERIC;
	}

	numericValue = value;
}

//-------------------------------------------------------------------

void
node::setNull()
{
	if (valueDataType != DATATYPE_NULL)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_OBJECT:
				delete objectValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_NULL;
	}
}

//-------------------------------------------------------------------

void
node::addArrayElement(const node &value)
{
	if (valueDataType != DATATYPE_ARRAY)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_OBJECT:
				delete objectValue;
				break;

			case DATATYPE_ARRAY:
				delete arrayValue;
				break;
		}

		valueDataType = DATATYPE_ARRAY;
		arrayValue = new dodoArray<node>;
	}

	arrayValue->push_back(value);
}

//-------------------------------------------------------------------

void
node::addObjectMember(const dodoString &name,
					  const node       &value)
{
	if (valueDataType != DATATYPE_OBJECT)
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

		valueDataType = DATATYPE_OBJECT;
		objectValue = new dodoMap<dodoString, node, dodoMapStringCompare>;
	}

	objectValue->insert(make_pair(name, value));
}

//-------------------------------------------------------------------

void
node::setArray(const dodoArray<node> &value)
{
	if (valueDataType != DATATYPE_ARRAY)
	{
		switch (valueDataType)
		{
			case DATATYPE_STRING:
				delete stringValue;
				break;

			case DATATYPE_OBJECT:
				delete objectValue;
				break;
		}

		valueDataType = DATATYPE_ARRAY;
		arrayValue = new dodoArray<node>(value);
	}
	else
	{
		*arrayValue = value;
	}
}

//-------------------------------------------------------------------

void
node::setObject(const dodoMap<dodoString, node, dodoMapStringCompare> &value)
{
	if (valueDataType != DATATYPE_OBJECT)
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

		valueDataType = DATATYPE_OBJECT;
		objectValue = new dodoMap<dodoString, node, dodoMapStringCompare>(value);
	}
	else
	{
		*objectValue = value;
	}
}

//-------------------------------------------------------------------

node
node::operator[](const dodoString &key) const
{
	if (valueDataType != DATATYPE_OBJECT)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_BROPERATORSTRING, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return (*objectValue)[key];
}

//-------------------------------------------------------------------

node
node::operator[](unsigned long key) const
{
	if (valueDataType != DATATYPE_ARRAY)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	if (key >= (arrayValue)->size())
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, NODEEX_ARRAYOUTOFRANGE, DATAFORMATJSONNODEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);

	return (*arrayValue)[key];
}

//-------------------------------------------------------------------

short
node::getType() const
{
	return valueDataType;
}

//-------------------------------------------------------------------

dodoString
node::getString() const
{
	if (valueDataType != DATATYPE_STRING)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETSTRING, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return *stringValue;
}

//-------------------------------------------------------------------

bool
node::getBoolean() const
{
	if (valueDataType != DATATYPE_BOOLEAN)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETBOOLEAN, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return booleanValue;
}

//-------------------------------------------------------------------

long
node::getNumeric() const
{
	if (valueDataType != DATATYPE_NUMERIC)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETNUMERIC, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return numericValue;
}

//-------------------------------------------------------------------

dodoArray<node>
node::getArray() const
{
	if (valueDataType != DATATYPE_ARRAY)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETARRAY, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return *arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodoString, node, dodo::dodoMapStringCompare>
node::getObject() const
{
	if (valueDataType != DATATYPE_OBJECT)
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETOBJECT, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return *objectValue;
}

//-------------------------------------------------------------------

bool
node::isNull() const
{
	return (valueDataType == DATATYPE_NULL);
}

//-------------------------------------------------------------------

