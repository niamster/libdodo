/***************************************************************************
 *            dataFormatJsonNode.cc
 *
 *  Mon Oct 20 2007
 *  Copyright  2007  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

node::node() : valueDataType(DATA_NULL)
{
}

//-------------------------------------------------------------------

node::node(const dodo::string &value) : stringValue(new dodo::string(value)),
                                      valueDataType(DATA_STRING)
{
}

//-------------------------------------------------------------------

node::node(long value) : numericValue(value),
                         valueDataType(DATA_NUMERIC)
{
}

//-------------------------------------------------------------------

node::node(bool value) : booleanValue(value),
                         valueDataType(DATA_BOOLEAN)
{
}

//-------------------------------------------------------------------

node::node(const dodoArray<node> &value) : arrayValue(new dodoArray<node>(value)),
                                           valueDataType(DATA_ARRAY)
{
}

//-------------------------------------------------------------------

node::node(const dodoMap<dodo::string, node, dodoMapStringCompare> &value) : objectValue(new dodoMap<dodo::string, node, dodoMapStringCompare>(value)),
                                                                           valueDataType(DATA_OBJECT)
{
}

//-------------------------------------------------------------------

node::node(const node &on) : valueDataType(on.valueDataType)
{
    switch (valueDataType) {
        case DATA_STRING:
            stringValue = new dodo::string(*on.stringValue);
            break;

        case DATA_OBJECT:
            objectValue = new dodoMap<dodo::string, node, dodoMapStringCompare>(*on.objectValue);
            break;

        case DATA_ARRAY:
            arrayValue = new dodoArray<node>(*on.arrayValue);
            break;

        case DATA_NUMERIC:
            numericValue = on.numericValue;
            break;

        case DATA_BOOLEAN:
            booleanValue = on.booleanValue;
            break;
    }
}

//-------------------------------------------------------------------

node::~node()
{
    switch (valueDataType) {
        case DATA_STRING:
            delete stringValue;
            break;

        case DATA_OBJECT:
            delete objectValue;
            break;

        case DATA_ARRAY:
            delete arrayValue;
            break;
    }
}

//-------------------------------------------------------------------

void
node::setString(const dodo::string &value)
{
    if (valueDataType != DATA_STRING) {
        switch (valueDataType) {
            case DATA_OBJECT:
                delete objectValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_STRING;
        stringValue = new dodo::string(value);
    } else
        *stringValue = value;
}

//-------------------------------------------------------------------

void
node::setBoolean(bool value)
{
    if (valueDataType != DATA_BOOLEAN) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_OBJECT:
                delete objectValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_BOOLEAN;
    }

    booleanValue = value;
}

//-------------------------------------------------------------------

void
node::setNumeric(long value)
{
    if (valueDataType != DATA_NUMERIC) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_OBJECT:
                delete objectValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_NUMERIC;
    }

    numericValue = value;
}

//-------------------------------------------------------------------

void
node::setNull()
{
    if (valueDataType != DATA_NULL) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_OBJECT:
                delete objectValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_NULL;
    }
}

//-------------------------------------------------------------------

void
node::addArrayElement(const node &value)
{
    if (valueDataType != DATA_ARRAY) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_OBJECT:
                delete objectValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_ARRAY;
        arrayValue = new dodoArray<node>;
    }

    arrayValue->push_back(value);
}

//-------------------------------------------------------------------

void
node::addObjectMember(const dodo::string &name,
                      const node       &value)
{
    if (valueDataType != DATA_OBJECT) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_OBJECT;
        objectValue = new dodoMap<dodo::string, node, dodoMapStringCompare>;
    }

    objectValue->insert(std::make_pair(name, value));
}

//-------------------------------------------------------------------

void
node::setArray(const dodoArray<node> &value)
{
    if (valueDataType != DATA_ARRAY) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_OBJECT:
                delete objectValue;
                break;
        }

        valueDataType = DATA_ARRAY;
        arrayValue = new dodoArray<node>(value);
    } else
        *arrayValue = value;
}

//-------------------------------------------------------------------

void
node::setObject(const dodoMap<dodo::string, node, dodoMapStringCompare> &value)
{
    if (valueDataType != DATA_OBJECT) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_OBJECT;
        objectValue = new dodoMap<dodo::string, node, dodoMapStringCompare>(value);
    } else
        *objectValue = value;
}

//-------------------------------------------------------------------

node
node::operator[](const dodo::string &key) const
{
    if (valueDataType != DATA_OBJECT)
        throw exception::basic(exception::MODULE_DATAFORMATJSONNODE, NODEEX_BROPERATORSTRING, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return (*objectValue)[key];
}

//-------------------------------------------------------------------

node
node::operator[](unsigned long key) const
{
    if (valueDataType != DATA_ARRAY)
        throw exception::basic(exception::MODULE_DATAFORMATJSONNODE, NODEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    if (key >= (arrayValue)->size())
        throw exception::basic(exception::MODULE_DATAFORMATJSONNODE, NODEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, NODEEX_ARRAYOUTOFRANGE, DATAFORMATJSONNODEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);

    return (*arrayValue)[key];
}

//-------------------------------------------------------------------

short
node::type() const
{
    return valueDataType;
}

//-------------------------------------------------------------------

dodo::string
node::string() const
{
    if (valueDataType != DATA_STRING)
        throw exception::basic(exception::MODULE_DATAFORMATJSONNODE, NODEEX_GETSTRING, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return *stringValue;
}

//-------------------------------------------------------------------

bool
node::boolean() const
{
    if (valueDataType != DATA_BOOLEAN)
        throw exception::basic(exception::MODULE_DATAFORMATJSONNODE, NODEEX_GETBOOLEAN, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return booleanValue;
}

//-------------------------------------------------------------------

long
node::numeric() const
{
    if (valueDataType != DATA_NUMERIC)
        throw exception::basic(exception::MODULE_DATAFORMATJSONNODE, NODEEX_GETNUMERIC, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return numericValue;
}

//-------------------------------------------------------------------

dodoArray<node>
node::array() const
{
    if (valueDataType != DATA_ARRAY)
        throw exception::basic(exception::MODULE_DATAFORMATJSONNODE, NODEEX_GETARRAY, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return *arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodo::string, node, dodo::dodoMapStringCompare>
node::object() const
{
    if (valueDataType != DATA_OBJECT)
        throw exception::basic(exception::MODULE_DATAFORMATJSONNODE, NODEEX_GETOBJECT, exception::ERRNO_LIBDODO, NODEEX_WRONGTYPEREQUESTED, DATAFORMATJSONNODEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return *objectValue;
}

//-------------------------------------------------------------------

bool
node::isNull() const
{
    return (valueDataType == DATA_NULL);
}

//-------------------------------------------------------------------

