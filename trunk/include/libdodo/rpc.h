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
	 * @class rpcMethodArgument defines RPC argumet for method reprasentation
	 */
	class rpcMethodArgument
	{
		public:
			
			/**
			 * constructor
			 */
			rpcMethodArgument();

			/**
			 * destructor
			 */
			virtual ~rpcMethodArgument();

			/**
			 * set argument name
			 * @param name defines method name
			 */
			virtual void setName(const dodoString &name);

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
			virtual void setStringValue(const dodoString &value);
			
			/**
			 * set boolean value
			 * @param value defines boolean value
			 */
			virtual void setBooleanValue(bool value);
			
			/**
			 * set integer value
			 * @param value defines integer value
			 */
			virtual void setIntegerValue(long value);
			
			/**
			 * set double value
			 * @param value defines double value
			 */
			virtual void setDoubleValue(double value);
			
			/**
			 * add array value
			 * @param value defines array member value
			 */
			virtual void addArrayValue(const rpcMethodArgument &value);
			
			/**
			 * add struct value
			 * @param name defines struct member name
			 * @param value defines struct member value
			 */
			virtual void addStructValue(const dodoString &name, const rpcMethodArgument &value);
			
		protected:
			
			short type;///< argument type
			dodoString name;///< argument name
			
			dodoString stringValue;///< string, datetime, base64 value
			long integerValue;///< integer value
			bool booleanValue;///< boolean value
			double doubleValue;///< double value
			dodoArray<rpcMethodArgument> arrayValue;///< array value
			dodoMap<dodoString, rpcMethodArgument, stringTools::equal> structValue;///< struct value
	};

	/**
	 * @class rpcMethod defines RPC method reprasentation
	 */
	class rpcMethod
	{
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
			virtual void addArgument(const rpcMethodArgument &argument);
			
		private:
			
			dodoArray<rpcMethodArgument> arguments;///< method arguments
			dodoString name;///< method name
	};
};

#endif /*_RPC_H_*/
