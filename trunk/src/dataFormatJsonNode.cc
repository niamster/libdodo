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
									  stringValue(value)
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
										   arrayValue(value)
{
}

//-------------------------------------------------------------------

node::node(const dodoMap<dodoString, node, dodoMapStringCompare> &value) : valueDataType(DATATYPE_OBJECT),
																		   objectValue(value)
{
}

//-------------------------------------------------------------------

node::~node()
{
}

//-------------------------------------------------------------------

void
node::setString(const dodoString &value)
{
	valueDataType = DATATYPE_STRING;

	stringValue = value;
}

//-------------------------------------------------------------------

void
node::setBoolean(bool value)
{
	valueDataType = DATATYPE_BOOLEAN;

	booleanValue = value;
}

//-------------------------------------------------------------------

void
node::setNumeric(long value)
{
	valueDataType = DATATYPE_NUMERIC;

	numericValue = value;
}

//-------------------------------------------------------------------

void
node::setNull()
{
	valueDataType = DATATYPE_NULL;
}

//-------------------------------------------------------------------

void
node::addArrayElement(const node &value)
{
	valueDataType = DATATYPE_ARRAY;

	arrayValue.push_back(value);
}

//-------------------------------------------------------------------

void
node::addObjectMember(const dodoString &name,
					  const node       &value)
{
	valueDataType = DATATYPE_OBJECT;

	objectValue.insert(make_pair(name, value));
}

//-------------------------------------------------------------------

void
node::setArray(const dodoArray<node> &value)
{
	valueDataType = DATATYPE_ARRAY;

	arrayValue = value;
}

//-------------------------------------------------------------------

void
node::setObject(const dodoMap<dodoString, node, dodoMapStringCompare> &value)
{
	valueDataType = DATATYPE_OBJECT;

	objectValue = value;
}

//-------------------------------------------------------------------

node
node::operator[](const dodoString &key)
{
	if (valueDataType != DATATYPE_OBJECT)
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_BROPERATORSTRING, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	}

	return objectValue[key];
}

//-------------------------------------------------------------------

node
node::operator[](unsigned long key)
{
	if (valueDataType != DATATYPE_ARRAY)
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	}

	if (key >= arrayValue.size())
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, NODEEX_ARRAYOUTOFRANGE, DATAFORMATJSONNODEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);
	}

	return arrayValue[key];
}

//-------------------------------------------------------------------

short
node::getType()
{
	return valueDataType;
}

//-------------------------------------------------------------------

dodoString
node::getString()
{
	if (valueDataType != DATATYPE_STRING)
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETSTRING, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	}

	return stringValue;
}

//-------------------------------------------------------------------

bool
node::getBoolean()
{
	if (valueDataType != DATATYPE_BOOLEAN)
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETBOOLEAN, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	}

	return booleanValue;
}

//-------------------------------------------------------------------

long
node::getNumeric()
{
	if (valueDataType != DATATYPE_NUMERIC)
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETNUMERIC, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	}

	return numericValue;
}

//-------------------------------------------------------------------

dodoArray<node>
node::getArray()
{
	if (valueDataType != DATATYPE_ARRAY)
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETARRAY, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	}

	return arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodoString, node, dodo::dodoMapStringCompare>
node::getObject()
{
	if (valueDataType != DATATYPE_OBJECT)
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATJSONNODE, NODEEX_GETOBJECT, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);
	}

	return objectValue;
}

//-------------------------------------------------------------------

bool
node::isNull()
{
	return (valueDataType == DATATYPE_NULL);
}

//-------------------------------------------------------------------

void
node::clear()
{
	stringValue.clear();
	objectValue.clear();
	arrayValue.clear();

	valueDataType = DATATYPE_NULL;
}

//-------------------------------------------------------------------

