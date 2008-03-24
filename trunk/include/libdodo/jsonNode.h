/***************************************************************************
 *            jsonNode.h
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

#ifndef _JSONNODE_H_
#define _JSONNODE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/stringTools.h>
#include <libdodo/jsonNodeEx.h>

namespace dodo
{
	/**
	 * @enum jsonDataTypeEnum defines JSON data types
	 */
	enum jsonDataTypeEnum
	{
		JSON_DATATYPE_STRING,
		JSON_DATATYPE_OBJECT,
		JSON_DATATYPE_ARRAY,
		JSON_DATATYPE_BOOLEAN,
		JSON_DATATYPE_NUMERIC,
		JSON_DATATYPE_NULL,
	};

	/**
	 * @class jsonNode defines JSON object reprasentation
	 */
	class jsonNode
	{
			friend class json;

		public:

			/**
			 * constructor
			 * @note constructs `null` object
			 */
			jsonNode();

			/**
			 * constructor
			 * @param value defines string value
			 */
			jsonNode(const dodoString &value);

			/**
			 * constructor
			 * @param value defines numeric value
			 */
			jsonNode(long value);

			/**
			 * constructor
			 * @param value defines boolean value
			 */
			jsonNode(bool value);

			/**
			 * constructor
			 * @param value defines array value
			 */
			jsonNode(const dodoArray<jsonNode> &value);

			/**
			 * constructor
			 * @param value defines object value
			 */
			jsonNode(const dodoMap<dodoString, jsonNode, stringTools::equal> &value);

			/**
			 * destructor
			 */
			virtual ~jsonNode();
			
			/**
			 * set argument type 
			 * @param type defines argument type[see jsonDataTypeEnum]
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
			virtual void setNumeric(long value);
			
			/**
			 * add array value
			 * @param value defines array member value
			 */
			virtual void addArrayElement(const jsonNode &value);
			
			/**
			 * add struct value element
			 * @param name defines struct member name
			 * @param value defines struct member value
			 */
			virtual void addObjectMember(const dodoString &name, const jsonNode &value);
			
			/**
			 * set array value member
			 * @param value defines array value
			 */
			virtual void setArray(const dodoArray<jsonNode> &value);
			
			/**
			 * set struct value
			 * @param value defines struct value
			 */
			virtual void setObject(const dodoMap<dodoString, jsonNode, stringTools::equal> &value);

			/**
			 * @return node by string key
			 * @param key defines key to search for node
			 * @note throws exception if data type is not JSON_DATATYPE_OBJECT
			 */
			virtual jsonNode operator[](const dodoString &key);

			/**
			 * @return node by numeric key
			 * @param key defines key to search for node
			 * @note throws exception if data type is not JSON_DATATYPE_ARRAY
			 */
			virtual jsonNode operator[](unsigned long key);

			/**
			 * @return type of node[see jsonDataTypeEnum]
			 */
			virtual short getType();

			/**
			 * @return true if node is `null`
			 */
			virtual bool isNull();

			/**
			 * @return string value
			 * @note throws exception if data type is not JSON_DATATYPE_STRING
			 */
			virtual dodoString getString();

			/**
			 * @return boolean value
			 * @note throws exception if data type is not JSON_DATATYPE_BOOLEAN
			 */
			virtual bool getBoolean();

			/**
			 * @return numeric value
			 * @note throws exception if data type is not JSON_DATATYPE_NUMERIC
			 */
			virtual long getNumeric();

			/**
			 * @return array value
			 * @note throws exception if data type is not JSON_DATATYPE_ARRAY
			 */
			virtual dodoArray<jsonNode> getArray();

			/**
			 * @return object value
			 * @note throws exception if data type is not JSON_DATATYPE_OBJECT
			 */
			virtual dodoMap<dodoString, jsonNode, stringTools::equal> getObject();

			/**
			 * clear internal data
			 */
			virtual void clear();

		private:

			dodoString stringValue;                                         ///< string value of node
			dodoMap<dodoString, jsonNode, stringTools::equal> objectValue;  ///< object value of node
			dodoArray<jsonNode> arrayValue;                                 ///< array value of node
			bool booleanValue;                                              ///< boolean value of node
			long numericValue;                                              ///< numeric value of node

			short valueDataType;                                            ///< data type of value
	};
};

#endif
