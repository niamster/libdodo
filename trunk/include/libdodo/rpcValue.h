/***************************************************************************
 *            rpcValue.h
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

#ifndef _RPCVALUE_H_
#define _RPCVALUE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcValueEx.h>

namespace dodo
{
	namespace rpc
	{
		namespace xml
		{
			class value;
		};
	
		/**
		 * @enum dataTypeEnum defines RPC data types
		 */
		enum dataTypeEnum
		{
			DATATYPE_STRING,
			DATATYPE_BOOLEAN,
			DATATYPE_INTEGER,
			DATATYPE_DOUBLE,
			DATATYPE_ARRAY,
			DATATYPE_STRUCT,
		};
	
		/**
		 * @class value defines RPC value
		 */
		class value
		{
			friend class client;
			friend class xml::value;
			
			public:
				
				/**
				 * constructor
				 * @note constructs empty string value
				 */
				value();
	
				/**
				 * constructor
				 * @param value defines string, date/time, base64 value
				 */
				value(const dodoString &value);
	
				/**
				 * constructor
				 * @param value defines integer value
				 */
				value(long value);
	
				/**
				 * constructor
				 * @param value defines double value
				 */
				value(double value);
	
				/**
				 * constructor
				 * @param value defines boolean value
				 */
				value(bool value);
	
				/**
				 * constructor
				 * @param value defines array value
				 */
				value(const dodoArray<value> &value);
	
				/**
				 * constructor
				 * @param value defines object value
				 */
				value(const dodoMap<dodoString, value, dodoMapStringCompare> &value);
	
				/**
				 * destructor
				 */
				virtual ~value();
	
				/**
				 * clear arguments information
				 */
				virtual void clear();
				
				/**
				 * set string, date/time, base64 value
				 * @param value defines string value
				 */
				virtual void setString(const dodoString &value);
				
				/**
				 * set boolean value
				 * @param value defines boolean value
				 */
				virtual void setBoolean(bool value);
				
				/**
				 * set integer value
				 * @param value defines integer value
				 */
				virtual void setInteger(long value);
				
				/**
				 * set double value
				 * @param value defines double value
				 */
				virtual void setDouble(double value);
				
				/**
				 * add array value
				 * @param value defines array member value
				 */
				virtual void addArrayElement(const value &value);
				
				/**
				 * add struct value element
				 * @param name defines struct member name
				 * @param value defines struct member value
				 */
				virtual void addStructMember(const dodoString &name, const value &value);
				
				/**
				 * set array value member
				 * @param value defines array value
				 */
				virtual void setArray(const dodoArray<value> &value);
				
				/**
				 * set struct value
				 * @param value defines struct value
				 */
				virtual void setStruct(const dodoMap<dodoString, value, dodoMapStringCompare> &value);
				
				/**
				 * @return structure member
				 * @param name defines structure member name
				 * @note throws exception if data type is not DATATYPE_STRUCT
				 */
				virtual value operator[](const dodoString &name);
	
				/**
				 * @return array element
				 * @param key defines array index
				 * @note throws exception if data type is not DATATYPE_ARRAY
				 */
				virtual value operator[](unsigned long key);
				
				/**
				 * get argument type 
				 */
				virtual short getType();
				
				/**
				 * get string, date/time, base64 value
				 * @note throws exception if data type is not DATATYPE_STRING, DATATYPE_DATETIME, DATATYPE_BASE64
				 */
				virtual dodoString getString();
				
				/**
				 * get boolean value
				 * @note throws exception if data type is not DATATYPE_BOOLEAN
				 */
				virtual bool getBoolean();
				
				/**
				 * get integer value
				 * @note throws exception if data type is not DATATYPE_INTEGER
				 */
				virtual long getInteger();
				
				/**
				 * get double value
				 * @note throws exception if data type is not DATATYPE_DOUBLE
				 */
				virtual double getDouble();
				
				/**
				 * get array value
				 * @note throws exception if data type is not DATATYPE_ARRAY
				 */
				virtual dodoArray<value> getArray();
				
				/**
				 * get struct value
				 * @note throws exception if data type is not DATATYPE_STRUCT
				 */
				virtual dodoMap<dodoString, value, dodoMapStringCompare> getStruct();
				
			protected:
				
				short valueDataType;///< argument type[see dataTypeEnum]
				
				dodoString stringValue;///< string, datetime, base64 value
				long integerValue;///< integer value
				bool booleanValue;///< boolean value
				double doubleValue;///< double value
				dodoArray<value> arrayValue;///< array value
				dodoMap<dodoString, value, dodoMapStringCompare> structValue;///< struct value
		};
	};
};

#endif
