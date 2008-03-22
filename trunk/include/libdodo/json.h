/***************************************************************************
 *            json.h
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

#ifndef _JSON_H_
#define _JSON_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/stringTools.h>
#include <libdodo/jsonEx.h>

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
	 * @struct __jsonNodeDef defines JSON object data
	 */
	struct __jsonNodeDef
	{
		dodoString stringValue;                                             ///< string value of node
		dodoMap<dodoString, __jsonNodeDef, stringTools::equal> objectValue; ///< object value of node
		dodoArray<__jsonNodeDef> arrayValue;                                ///< array value of node
		bool booleanValue;                                                  ///< boolean value of node
		long numericValue;                                                  ///< numeric value of node

		short valueDataType;                                                ///< data type of value
	};

	/**
	 * @class jsonNode defines JSON object reprasentation
	 */
	class jsonNode
	{
			friend class json;

		public:

			/**
			 * copy constructor
			 */
			jsonNode(const jsonNode &node);

			/**
			 * constructor
			 */
			jsonNode();

			/**
			 * destructor
			 */
			virtual ~jsonNode();

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

	/**
	 * @class json provides JSON operations
	 */
	class json
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			json(json &js);

		public:

			/**
			 * contructor
			 */
			json();

			/**
			 * destructor
			 */
			virtual ~json();

			/**
			 * @return string that contains serialized JSON object
			 * @param root defines root of unserialized JSON object
			 */
			virtual dodoString makeJSON(const __jsonNodeDef &root);

			/**
			 * @return root of unserialized JSON object
			 * @param root defines string that contains serialized JSON object
			 */
			virtual jsonNode processJSON(const dodoString &root);

			/**
			 * @return string that contains serialized JSON object
			 * @param root defines root of unserialized JSON object
			 */
			virtual dodoString mapToJSON(const dodoStringMap &root);

			/**
			 * @return root of unserialized JSON object
			 * @param root defines string that contains serialized JSON object
			 */
			virtual dodoStringMap JSONToMap(const dodoString &root);

		protected:

			/**
			 * process string value
			 * @return position of last processed symbol of serialized JSON string object
			 * @param node defines buffer for unserialized JSON string
			 * @param root defines serialized JSON string object
			 * @param pos defines start position for parsing in serialized JSON string object
			 */
			virtual unsigned long processString(dodoString &node, const dodoString &root, unsigned long pos);

			/**
			 * process array value
			 * @return position of last processed symbol of serialized JSON string object
			 * @param node defines buffer for unserialized JSON array
			 * @param root defines serialized JSON string object
			 * @param pos defines start position for parsing in serialized JSON string object
			 */
			virtual unsigned long processArray(dodoArray<jsonNode> &node, const dodoString &root, unsigned long pos);

			/**
			 * process object value
			 * @return position of last processed symbol of serialized JSON string object
			 * @param node defines buffer for unserialized JSON object
			 * @param root defines serialized JSON string object
			 * @param pos defines start position for parsing in serialized JSON string object
			 */
			virtual unsigned long processObject(dodoMap<dodoString, jsonNode, stringTools::equal> &node, const dodoString &root, unsigned long pos);

			/**
			 * process json value
			 * @return position of last processed symbol of serialized JSON string object
			 * @param node defines buffer for unserialized JSON object
			 * @param root defines serialized JSON string object
			 * @param pos defines start position for parsing in serialized JSON string object
			 */
			virtual unsigned long processValue(jsonNode &node, const dodoString &root, unsigned long pos);

			/**
			 * process json boolean value
			 * @return position of last processed symbol of serialized JSON string object
			 * @param node defines buffer for unserialized JSON boolean
			 * @param root defines serialized JSON string object
			 * @param pos defines start position for parsing in serialized JSON string object
			 */
			virtual unsigned long processBoolean(bool &node, const dodoString &root, unsigned long pos);

			/**
			 * processes json numeric value
			 * @return position of last processed symbol of serialized JSON string object
			 * @param node defines buffer for unserialized JSON numeric
			 * @param root defines serialized JSON string object
			 * @param pos defines start position for parsing in serialized JSON string object
			 */
			virtual unsigned long processNumeric(long &node, const dodoString &root, unsigned long pos);

			/**
			 * process json null value
			 * @return position of last processed symbol of serialized JSON string object
			 * @param root defines serialized JSON string object
			 * @param pos defines start position for parsing in serialized JSON string object
			 */
			virtual unsigned long processNull(const dodoString &root, unsigned long pos);

			/**
			 * @enum jsonStateEnum describes states for json processor
			 */
			enum jsonStateEnum
			{
				JSON_STATE_OBJECT_INITIAL,
				JSON_STATE_OBJECT_OBJECTNAME,
				JSON_STATE_OBJECT_OBJECTVALUE,
			};
	};
};

#endif /*_JSON_H_*/
