/***************************************************************************
 *            rpc.h
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

#ifndef _RPC_H_
#define _RPC_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/stringTools.h>
#include <libdodo/rpcEx.h>

namespace dodo
{
	/**
	 * @enum rpcDataTypeEnum defines RPC data types
	 */
	enum rpcDataTypeEnum
	{
		RPC_DATATYPE_STRING,
		RPC_DATATYPE_DATETIME,
		RPC_DATATYPE_BASE64,
		RPC_DATATYPE_BOOLEAN,
		RPC_DATATYPE_INTEGER,
		RPC_DATATYPE_DOUBLE,
		RPC_DATATYPE_ARRAY,
		RPC_DATATYPE_STRUCT,
	};

	/**
	 * @class rpcValue defines RPC value
	 */
	class rpcValue
	{
		friend class rpcClient;
		
		public:
			
			/**
			 * constructor
			 * @note constructs empty string value
			 */
			rpcValue();

			/**
			 * constructor
			 * @param value defines string, date/time, base64 value
			 */
			rpcValue(const dodoString &value);

			/**
			 * constructor
			 * @param value defines integer value
			 */
			rpcValue(long value);

			/**
			 * constructor
			 * @param value defines double value
			 */
			rpcValue(double value);

			/**
			 * constructor
			 * @param value defines boolean value
			 */
			rpcValue(bool value);

			/**
			 * constructor
			 * @param value defines array value
			 */
			rpcValue(const dodoArray<rpcValue> &value);

			/**
			 * constructor
			 * @param value defines object value
			 */
			rpcValue(const dodoMap<dodoString, rpcValue, stringTools::equal> &value);

			/**
			 * destructor
			 */
			virtual ~rpcValue();

			/**
			 * clear arguments information
			 */
			virtual void clear();
			
			/**
			 * set argument type 
			 * @param type defines argument type[see rpcDataTypeEnum]
			 */
			virtual void setType(short type);
			
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
			virtual void addArrayElement(const rpcValue &value);
			
			/**
			 * add struct value element
			 * @param name defines struct member name
			 * @param value defines struct member value
			 */
			virtual void addStructMember(const dodoString &name, const rpcValue &value);
			
			/**
			 * set array value member
			 * @param value defines array value
			 */
			virtual void setArray(const dodoArray<rpcValue> &value);
			
			/**
			 * set struct value
			 * @param value defines struct value
			 */
			virtual void setStruct(const dodoMap<dodoString, rpcValue, stringTools::equal> &value);
			
			/**
			 * @return structure member
			 * @param name defines structure member name
			 * @note throws exception if data type is not RPC_DATATYPE_STRUCT
			 */
			virtual rpcValue operator[](const dodoString &name);

			/**
			 * @return array element
			 * @param index defines array index
			 * @note throws exception if data type is not RPC_DATATYPE_ARRAY
			 */
			virtual rpcValue operator[](unsigned long key);
			
			/**
			 * get argument type 
			 */
			virtual short getType();
			
			/**
			 * get string, date/time, base64 value
			 * @note throws exception if data type is not RPC_DATATYPE_STRING, RPC_DATATYPE_DATETIME, RPC_DATATYPE_BASE64
			 */
			virtual dodoString getString();
			
			/**
			 * get boolean value
			 * @note throws exception if data type is not RPC_DATATYPE_BOOLEAN
			 */
			virtual bool getBoolean();
			
			/**
			 * get integer value
			 * @note throws exception if data type is not RPC_DATATYPE_INTEGER
			 */
			virtual long getInteger();
			
			/**
			 * get double value
			 * @note throws exception if data type is not RPC_DATATYPE_DOUBLE
			 */
			virtual double getDouble();
			
			/**
			 * get array value
			 * @note throws exception if data type is not RPC_DATATYPE_ARRAY
			 */
			virtual dodoArray<rpcValue> getArray();
			
			/**
			 * get struct value
			 * @note throws exception if data type is not RPC_DATATYPE_STRUCT
			 */
			virtual dodoMap<dodoString, rpcValue, stringTools::equal> getStruct();
			
		protected:
			
			short valueDataType;///< argument type[see rpcDataTypeEnum]
			
			dodoString stringValue;///< string, datetime, base64 value
			long integerValue;///< integer value
			bool booleanValue;///< boolean value
			double doubleValue;///< double value
			dodoArray<rpcValue> arrayValue;///< array value
			dodoMap<dodoString, rpcValue, stringTools::equal> structValue;///< struct value
	};
	
	/**
	 * @class rpcMethod defines RPC method reprasentation
	 */
	class rpcMethod
	{
		friend class rpcClient;
		
		public:

			/**
			 * constructor
			 */
			rpcMethod();

			/**
			 * destructor
			 */
			virtual ~rpcMethod();

			/**
			 * set method name
			 * @param name defines method name
			 */
			virtual void setName(const dodoString &name);

			/**
			 * clear arguments information
			 */
			virtual void clear();
			
			/**
			 * add argument
			 * @param argument defines method argument 
			 */
			virtual void addArgument(const dodoString &name, const rpcValue &argument);
			
		private:
			
			dodoMap<dodoString, rpcValue, stringTools::equal> arguments;///< method arguments
			dodoString name;///< method name
	};
	
	/**
	 * @class rpcResponse defines RPC response reprasentation
	 */
	class rpcResponse
	{
		friend class rpcClient;
		
		public:

			/**
			 * constructor
			 */
			rpcResponse();

			/**
			 * destructor
			 */
			virtual ~rpcResponse();

			/**
			 * @return response value
			 */
			virtual rpcValue getValue();
			
			/**
			 * @return true if request has  
			 */
			virtual bool successful();
			
			/**
			 * @return rpcValue for type casting  
			 */
			virtual operator rpcValue(); 
			
		private:
			
			rpcValue value;///< response value
			bool succ;///< true if server returned non-fail response
	};
	
	class ioHTTP;
	
	/**
	 * @class rpcClient defines 
	 */
	class rpcClient
	{
		public:

			/**
			 * constructor
			 */
			rpcClient();

			/**
			 * destructor
			 */
			virtual ~rpcClient() = 0;
		
			/**
			 * @param method defines rpc method call
			 */
			virtual void sendRequest(const rpcMethod &method) = 0;
			
			/**
			 * @return rpc response result 
			 */
			virtual rpcResponse receiveResponse() = 0;
			
			/**
			 * set transport layer provider
			 * @param provider defines trasport layer provider
			 */
			virtual void setIOProvider(ioHTTP *provider);
		
		protected:
			
			ioHTTP *ioProvider;
	};
};

#endif
