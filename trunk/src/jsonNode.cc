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

using namespace dodo;

jsonNode::jsonNode() : valueDataType(JSON_DATATYPE_NULL)
{

}

//-------------------------------------------------------------------

jsonNode::jsonNode(const dodoString &value) : valueDataType(JSON_DATATYPE_STRING),
											stringValue(value)
{
	
}

//-------------------------------------------------------------------

jsonNode::jsonNode(long value) : valueDataType(JSON_DATATYPE_NUMERIC),
											numericValue(value)
{
	
}

//-------------------------------------------------------------------

jsonNode::jsonNode(bool value) : valueDataType(JSON_DATATYPE_BOOLEAN),
											booleanValue(value)
{
	
}

//-------------------------------------------------------------------

jsonNode::jsonNode(const dodoArray<jsonNode> &value) : valueDataType(JSON_DATATYPE_ARRAY),
											arrayValue(value)
{
	
}

//-------------------------------------------------------------------

jsonNode::jsonNode(const dodoMap<dodoString, jsonNode, stringTools::equal> &value) : valueDataType(JSON_DATATYPE_OBJECT),
											objectValue(value)
{
	
}

//-------------------------------------------------------------------

jsonNode::~jsonNode()
{

}

//-------------------------------------------------------------------

void
jsonNode::setType(short type)
{
	valueDataType = type;
}

//-------------------------------------------------------------------

void
jsonNode::setString(const dodoString &value)
{
	stringValue = value;
}

//-------------------------------------------------------------------

void
jsonNode::setBoolean(bool value)
{
	booleanValue = value;
}

//-------------------------------------------------------------------

void
jsonNode::setNumeric(long value)
{
	numericValue = value;
}

//-------------------------------------------------------------------

void
jsonNode::addArrayElement(const jsonNode &value)
{
	arrayValue.push_back(value);
}

//-------------------------------------------------------------------

void
jsonNode::addObjectMember(const dodoString &name, 
								const jsonNode &value)
{
	objectValue.insert(name, value);
}

//-------------------------------------------------------------------

void 
jsonNode::setArray(const dodoArray<jsonNode> &value)
{
	arrayValue = value;
}

//-------------------------------------------------------------------

void 
jsonNode::setObject(const dodoMap<dodoString, jsonNode, stringTools::equal> &value)
{
	objectValue = value;
}

//-------------------------------------------------------------------

jsonNode
jsonNode::operator[](const dodoString &key)
{
	if (valueDataType != JSON_DATATYPE_OBJECT)
		throw baseEx(ERRMODULE_JSONNODE, JSONNODEEX_BROPERATORSTRING, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return objectValue[key];
}

//-------------------------------------------------------------------

jsonNode
jsonNode::operator[](unsigned long key)
{
	if (valueDataType != JSON_DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_JSONNODE, JSONNODEEX_BROPERATORNUMERIC, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	if (key >= arrayValue.size())
		throw baseEx(ERRMODULE_JSONNODE, JSONNODEEX_BROPERATORNUMERIC, ERR_LIBDODO, JSONNODEEX_ARRAYOUTOFRANGE, JSONNODEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);

	return arrayValue[key];
}

//-------------------------------------------------------------------

short
jsonNode::getType()
{
	return valueDataType;
}

//-------------------------------------------------------------------

dodoString
jsonNode::getString()
{
	if (valueDataType != JSON_DATATYPE_STRING)
		throw baseEx(ERRMODULE_JSONNODE, JSONNODEEX_GETSTRING, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return stringValue;
}

//-------------------------------------------------------------------

bool
jsonNode::getBoolean()
{
	if (valueDataType != JSON_DATATYPE_BOOLEAN)
		throw baseEx(ERRMODULE_JSONNODE, JSONNODEEX_GETBOOLEAN, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return booleanValue;
}

//-------------------------------------------------------------------

long
jsonNode::getNumeric()
{
	if (valueDataType != JSON_DATATYPE_NUMERIC)
		throw baseEx(ERRMODULE_JSONNODE, JSONNODEEX_GETNUMERIC, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return numericValue;
}

//-------------------------------------------------------------------

dodoArray<jsonNode>
jsonNode::getArray()
{
	if (valueDataType != JSON_DATATYPE_ARRAY)
		throw baseEx(ERRMODULE_JSONNODE, JSONNODEEX_GETARRAY, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodoString, jsonNode, stringTools::equal>
jsonNode::getObject()
{
	if (valueDataType != JSON_DATATYPE_OBJECT)
		throw baseEx(ERRMODULE_JSONNODE, JSONNODEEX_GETOBJECT, ERR_LIBDODO, JSONNODEEX_WRONGTYPEREQUESTED, JSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

	return objectValue;
}

//-------------------------------------------------------------------

bool
jsonNode::isNull()
{
	return(valueDataType == JSON_DATATYPE_NULL);
}

//-------------------------------------------------------------------

void
jsonNode::clear()
{
	stringValue.clear();
	objectValue.clear();
	arrayValue.clear();

	valueDataType = JSON_DATATYPE_NULL;
}

//-------------------------------------------------------------------

