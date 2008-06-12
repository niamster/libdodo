/***************************************************************************
 *            jsonNode.cc
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

#include <libdodo/jsonNode.h>

using namespace dodo::json;

node::node() : valueDataType(NODE_DATATYPE_NULL)
{

}

//-------------------------------------------------------------------

node::node(const dodoString &value) : valueDataType(NODE_DATATYPE_STRING),
				      stringValue(value)
{

}

//-------------------------------------------------------------------

node::node(long value) : valueDataType(NODE_DATATYPE_NUMERIC),
			 numericValue(value)
{

}

//-------------------------------------------------------------------

node::node(bool value) : valueDataType(NODE_DATATYPE_BOOLEAN),
			 booleanValue(value)
{

}

//-------------------------------------------------------------------

node::node(const dodoArray<node> &value) : valueDataType(NODE_DATATYPE_ARRAY),
					   arrayValue(value)
{

}

//-------------------------------------------------------------------

node::node(const dodoMap<dodoString, node, dodoMapStringCompare> &value) : valueDataType(NODE_DATATYPE_OBJECT),
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
	valueDataType = NODE_DATATYPE_STRING;

	stringValue = value;
}

//-------------------------------------------------------------------

void
node::setBoolean(bool value)
{
	valueDataType = NODE_DATATYPE_BOOLEAN;

	booleanValue = value;
}

//-------------------------------------------------------------------

void
node::setNumeric(long value)
{
	valueDataType = NODE_DATATYPE_NUMERIC;

	numericValue = value;
}

//-------------------------------------------------------------------

void
node::setNull()
{
	valueDataType = NODE_DATATYPE_NULL;
}

//-------------------------------------------------------------------

void
node::addArrayElement(const node &value)
{
	valueDataType = NODE_DATATYPE_ARRAY;

	arrayValue.push_back(value);
}

//-------------------------------------------------------------------

void
node::addObjectMember(const dodoString &name,
		      const node &value)
{
	valueDataType = NODE_DATATYPE_OBJECT;

	objectValue.insert(make_pair(name, value));
}

//-------------------------------------------------------------------

void
node::setArray(const dodoArray<node> &value)
{
	valueDataType = NODE_DATATYPE_ARRAY;

	arrayValue = value;
}

//-------------------------------------------------------------------

void
node::setObject(const dodoMap<dodoString, node, dodoMapStringCompare> &value)
{
	valueDataType = NODE_DATATYPE_OBJECT;

	objectValue = value;
}

//-------------------------------------------------------------------

node
node::operator[](const dodoString &key)
{
	if (valueDataType != NODE_DATATYPE_OBJECT)
		throw baseEx(ERRMODULE_JSONNODE, NODEEX_BROPERATORSTRING, ERR_LIBDODO, NODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return objectValue[key];
}

//-------------------------------------------------------------------

node
node::operator[](unsigned long key)
{
	if (valueDataType != NODE_DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_JSONNODE, NODEEX_BROPERATORNUMERIC, ERR_LIBDODO, NODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	if (key >= arrayValue.size())
		throw baseEx(ERRMODULE_JSONNODE, NODEEX_BROPERATORNUMERIC, ERR_LIBDODO, NODEEX_ARRAYOUTOFRANGE, JSONNODEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);

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
	if (valueDataType != NODE_DATATYPE_STRING)
		throw baseEx(ERRMODULE_JSONNODE, NODEEX_GETSTRING, ERR_LIBDODO, NODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return stringValue;
}

//-------------------------------------------------------------------

bool
node::getBoolean()
{
	if (valueDataType != NODE_DATATYPE_BOOLEAN)
		throw baseEx(ERRMODULE_JSONNODE, NODEEX_GETBOOLEAN, ERR_LIBDODO, NODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return booleanValue;
}

//-------------------------------------------------------------------

long
node::getNumeric()
{
	if (valueDataType != NODE_DATATYPE_NUMERIC)
		throw baseEx(ERRMODULE_JSONNODE, NODEEX_GETNUMERIC, ERR_LIBDODO, NODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return numericValue;
}

//-------------------------------------------------------------------

dodoArray<node>
node::getArray()
{
	if (valueDataType != NODE_DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_JSONNODE, NODEEX_GETARRAY, ERR_LIBDODO, NODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodoString, node, dodo::dodoMapStringCompare>
node::getObject()
{
	if (valueDataType != NODE_DATATYPE_OBJECT)
		throw baseEx(ERRMODULE_JSONNODE, NODEEX_GETOBJECT, ERR_LIBDODO, NODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return objectValue;
}

//-------------------------------------------------------------------

bool
node::isNull()
{
	return(valueDataType == NODE_DATATYPE_NULL);
}

//-------------------------------------------------------------------

void
node::clear()
{
	stringValue.clear();
	objectValue.clear();
	arrayValue.clear();

	valueDataType = NODE_DATATYPE_NULL;
}

//-------------------------------------------------------------------

