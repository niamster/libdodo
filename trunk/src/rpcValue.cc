/***************************************************************************
 *            rpcValue.cc
 *
 *  Sat Mar 22 2008
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

value::value() : valueDataType(DATA_BOOLEAN),
                 booleanValue(false)
{
}

//-------------------------------------------------------------------

value::value(const dodoString &value) : valueDataType(DATA_STRING),
                                        stringValue(new dodoString(value))
{
}

//-------------------------------------------------------------------

value::value(long value) : valueDataType(DATA_NUMERIC),
                           numericValue(value)
{
}

//-------------------------------------------------------------------

value::value(double value) : valueDataType(DATA_NUMERIC),
                             numericValue(value)
{
}

//-------------------------------------------------------------------

value::value(bool value) : valueDataType(DATA_BOOLEAN),
                           booleanValue(value)
{
}

//-------------------------------------------------------------------

value::value(const dodoArray<value> &nvalue) : valueDataType(DATA_ARRAY),
                                               arrayValue(new dodoArray<value>(nvalue))
{
}

//-------------------------------------------------------------------

value::value(const dodoMap<dodoString, value, dodoMapStringCompare> &nvalue) : valueDataType(DATA_STRUCT),
                                                                               structValue(new dodoMap<dodoString, value, dodoMapStringCompare>(nvalue))
{
}

//-------------------------------------------------------------------

value::value(const value &on) : valueDataType(on.valueDataType)
{
    switch (valueDataType) {
        case DATA_STRING:
            stringValue = new dodoString(*on.stringValue);
            break;

        case DATA_STRUCT:
            structValue = new dodoMap<dodoString, value, dodoMapStringCompare>(*on.structValue);
            break;

        case DATA_ARRAY:
            arrayValue = new dodoArray<value>(*on.arrayValue);
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

value::~value()
{
    switch (valueDataType) {
        case DATA_STRING:
            delete stringValue;
            break;

        case DATA_STRUCT:
            delete structValue;
            break;

        case DATA_ARRAY:
            delete arrayValue;
            break;
    }
}

//-------------------------------------------------------------------

void
value::setString(const dodoString &nvalue)
{
    if (valueDataType != DATA_STRING) {
        switch (valueDataType) {
            case DATA_STRUCT:
                delete structValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_STRING;
        stringValue = new dodoString(nvalue);
    } else
        *stringValue = nvalue;
}

//-------------------------------------------------------------------

void
value::setBoolean(bool value)
{
    if (valueDataType != DATA_BOOLEAN) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_STRUCT:
                delete structValue;
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
value::setNumeric(double value)
{
    if (valueDataType != DATA_NUMERIC) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_STRUCT:
                delete structValue;
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
value::addArrayElement(const value &nvalue)
{
    if (valueDataType != DATA_ARRAY) {
        switch (valueDataType) {
            case DATA_STRUCT:
                delete structValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_ARRAY;
        arrayValue = new dodoArray<value>;
    }

    arrayValue->push_back(nvalue);
}

//-------------------------------------------------------------------

void
value::addStructureMember(const dodoString &name,
                          const value      &nvalue)
{
    if (valueDataType != DATA_STRUCT) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_STRUCT;
        structValue = new dodoMap<dodoString, value, dodoMapStringCompare>;
    }

    structValue->insert(make_pair(name,  nvalue));
}

//-------------------------------------------------------------------

void
value::setArray(const dodoArray<value> &nvalue)
{
    if (valueDataType != DATA_ARRAY) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_STRUCT:
                delete structValue;
                break;
        }

        valueDataType = DATA_ARRAY;
        arrayValue = new dodoArray<value>(nvalue);
    } else
        *arrayValue = nvalue;
}

//-------------------------------------------------------------------

void
value::setStruct(const dodoMap<dodoString, value, dodoMapStringCompare> &nvalue)
{
    if (valueDataType != DATA_STRUCT) {
        switch (valueDataType) {
            case DATA_STRING:
                delete stringValue;
                break;

            case DATA_ARRAY:
                delete arrayValue;
                break;
        }

        valueDataType = DATA_STRUCT;
        structValue = new dodoMap<dodoString, value, dodoMapStringCompare>(nvalue);
    } else
        *structValue = nvalue;
}

//-------------------------------------------------------------------

value
value::operator[](const dodoString &name)
{
    if (valueDataType != DATA_STRUCT)
        throw exception::basic(exception::MODULE_RPCVALUE, VALUEEX_BROPERATORSTRING, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return (*structValue)[name];
}

//-------------------------------------------------------------------

value
value::operator[](unsigned long key)
{
    if (valueDataType != DATA_ARRAY)
        throw exception::basic(exception::MODULE_RPCVALUE, VALUEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    if (key >= arrayValue->size())
        throw exception::basic(exception::MODULE_RPCVALUE, VALUEEX_BROPERATORNUMERIC, exception::ERRNO_LIBDODO, VALUEEX_ARRAYOUTOFRANGE, RPCVALUEEX_ARRAYOUTOFRANGE_STR, __LINE__, __FILE__);

    return (*arrayValue)[key];
}

//-------------------------------------------------------------------

short
value::type()
{
    return valueDataType;
}

//-------------------------------------------------------------------

dodoString
value::string()
{
    if (valueDataType != DATA_STRING)
        throw exception::basic(exception::MODULE_RPCVALUE, VALUEEX_STRING, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return *stringValue;
}

//-------------------------------------------------------------------

bool
value::boolean()
{
    if (valueDataType != DATA_BOOLEAN)
        throw exception::basic(exception::MODULE_RPCVALUE, VALUEEX_BOOLEAN, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return booleanValue;
}

//-------------------------------------------------------------------

long
value::numeric()
{
    if (valueDataType != DATA_NUMERIC)
        throw exception::basic(exception::MODULE_RPCVALUE, VALUEEX_NUMERIC, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return (long)numericValue;
}

//-------------------------------------------------------------------

dodoArray<value>
value::array()
{
    if (valueDataType != DATA_ARRAY)
        throw exception::basic(exception::MODULE_RPCVALUE, VALUEEX_ARRAY, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return *arrayValue;
}

//-------------------------------------------------------------------

dodoMap<dodoString, value, dodo::dodoMapStringCompare>
value::structure()
{
    if (valueDataType != DATA_STRUCT)
        throw exception::basic(exception::MODULE_RPCVALUE, VALUEEX_STRUCT, exception::ERRNO_LIBDODO, VALUEEX_WRONGTYPEREQUESTED, RPCVALUEEX_WRONGTYPEREQUESTED_STR, __LINE__, __FILE__);

    return *structValue;
}

//-------------------------------------------------------------------
