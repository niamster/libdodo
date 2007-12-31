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
	 * @enum jsonDataTypeEnum
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
	 * @class jsonObject represents json object data
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
	 * @class jsonObject represents json object
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
			 * @param key describes string key to search for node
			 * @note throws exception if data type is not JSON_DATATYPE_OBJECT
			 */
			virtual jsonNode operator[](const dodoString &key);

			/**
			 * @return node by numeric key
			 * @param key describes numeric key to search for node
			 * @note throws exception if data type is not JSON_DATATYPE_ARRAY
			 */
			virtual jsonNode operator[](unsigned long key);

			/**
			 * @return type of node[see jsonDataTypeEnum]
			 */
			virtual short getType();

			/**
			 * @return true if node is NULL
			 */
			virtual bool isNull();

			/**
			 * @return value as string
			 * @note throws exception if data type is not JSON_DATATYPE_STRING
			 */
			virtual dodoString getString();

			/**
			 * @return value as bool
			 * @note throws exception if data type is not JSON_DATATYPE_BOOLEAN
			 */
			virtual bool getBoolean();

			/**
			 * @return value as numeric
			 * @note throws exception if data type is not JSON_DATATYPE_NUMERIC
			 */
			virtual long getNumeric();

			/**
			 * @return value as array
			 * @note throws exception if data type is not JSON_DATATYPE_ARRAY
			 */
			virtual dodoArray<jsonNode> getArray();

			/**
			 * @return value as object
			 * @note throws exception if data type is not JSON_DATATYPE_OBJECT
			 */
			virtual dodoMap<dodoString, jsonNode, stringTools::equal> getObject();

			/**
			 * clears internal data
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
	 * @class json is to perform json operations
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
			 * @return string that contain json object
			 * @param root describes root of json object
			 */
			virtual dodoString makeJSON(const __jsonNodeDef &root);

			/**
			 * processes json object
			 * @param node describes root of json object
			 * @param root describes string that contain json object
			 */
			virtual void processJSON(jsonNode &node, const dodoString &root);

			/**
			 * @return string that contain json object
			 * @param root describes map that contain json object
			 */
			virtual dodoString mapToJSON(const dodoStringMap &root);

			/**
			 * @return string to string map that contain json object
			 * @param root describes map that contain json object
			 */
			virtual dodoStringMap JSONToMap(const dodoString &node);

		protected:

			/**
			 * processes string value
			 * @return last symbol of processed string
			 * @param root describes json object that is forming
			 * @param symbol describes string to process
			 * @param pos describes start position in string
			 */
			virtual unsigned long processString(dodoString &node, const dodoString &root, unsigned long pos);

			/**
			 * processes string value
			 * @return last symbol of processed string
			 * @param root describes json object that is forming
			 * @param symbol describes string to process
			 * @param pos describes start position in string
			 */
			virtual unsigned long processArray(dodoArray<jsonNode> &node, const dodoString &root, unsigned long pos);

			/**
			 * processes json object
			 * @return last symbol of processed object
			 * @param node describes root of json object
			 * @param root describes string that contain json object
			 * @param pos describes start position in string
			 */
			virtual unsigned long processObject(dodoMap<dodoString, jsonNode, stringTools::equal> &node, const dodoString &root, unsigned long pos);

			/**
			 * processes json object value
			 * @return last symbol of processed object value
			 * @param node describes value of json object
			 * @param root describes string that contain json object
			 * @param pos describes start position in string
			 */
			virtual unsigned long processValue(jsonNode &node, const dodoString &root, unsigned long pos);

			/**
			 * processes json boolean value
			 * @return last symbol of processed object value
			 * @param node describes value of json object
			 * @param root describes string that contain json object
			 * @param pos describes start position in string
			 */
			virtual unsigned long processBoolean(bool &node, const dodoString &root, unsigned long pos);

			/**
			 * processes json numeric value
			 * @return last symbol of processed object value
			 * @param node describes value of json object
			 * @param root describes string that contain json object
			 * @param pos describes start position in string
			 */
			virtual unsigned long processNumeric(long &node, const dodoString &root, unsigned long pos);

			/**
			 * processes json null value
			 * @return last symbol of processed object value
			 * @param root describes string that contain json object
			 * @param pos describes start position in string
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
