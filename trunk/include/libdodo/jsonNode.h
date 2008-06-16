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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _JSONNODE_H_
#define _JSONNODE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/jsonNodeEx.h>

namespace dodo
{
	namespace json
	{
		/**
		 * @enum nodeDataTypeEnum defines JSON data types
		 */
		enum nodeDataTypeEnum
		{
			NODE_DATATYPE_STRING,
			NODE_DATATYPE_OBJECT,
			NODE_DATATYPE_ARRAY,
			NODE_DATATYPE_BOOLEAN,
			NODE_DATATYPE_NUMERIC,
			NODE_DATATYPE_NULL,
		};

		/**
		 * @class node defines JSON object reprasentation
		 */
		class node
		{
			friend class processor;

			public:

				/**
				 * constructor
				 * @note constructs `null` object
				 */
				node();

				/**
				 * constructor
				 * @param value defines string value
				 */
				node(const dodoString &value);

				/**
				 * constructor
				 * @param value defines numeric value
				 */
				node(long value);

				/**
				 * constructor
				 * @param value defines boolean value
				 */
				node(bool value);

				/**
				 * constructor
				 * @param value defines array value
				 */
				node(const dodoArray<node> &value);

				/**
				 * constructor
				 * @param value defines object value
				 */
				node(const dodoMap<dodoString, node, dodoMapStringCompare> &value);

				/**
				 * destructor
				 */
				virtual ~node();

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
				 * set null value
				 */
				virtual void setNull();

				/**
				 * add array value
				 * @param value defines array member value
				 */
				virtual void addArrayElement(const node &value);

				/**
				 * add struct value element
				 * @param name defines struct member name
				 * @param value defines struct member value
				 */
				virtual void addObjectMember(const dodoString &name, const node &value);

				/**
				 * set array value member
				 * @param value defines array value
				 */
				virtual void setArray(const dodoArray<node> &value);

				/**
				 * set struct value
				 * @param value defines struct value
				 */
				virtual void setObject(const dodoMap<dodoString, node, dodoMapStringCompare> &value);

				/**
				 * @return node by string key
				 * @param key defines key to search for node
				 * @note throws exception if data type is not NODE_DATATYPE_OBJECT
				 */
				virtual node operator[](const dodoString &key);

				/**
				 * @return node by numeric key
				 * @param key defines key to search for node
				 * @note throws exception if data type is not NODE_DATATYPE_ARRAY
				 */
				virtual node operator[](unsigned long key);

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
				 * @note throws exception if data type is not NODE_DATATYPE_STRING
				 */
				virtual dodoString getString();

				/**
				 * @return boolean value
				 * @note throws exception if data type is not NODE_DATATYPE_BOOLEAN
				 */
				virtual bool getBoolean();

				/**
				 * @return numeric value
				 * @note throws exception if data type is not NODE_DATATYPE_NUMERIC
				 */
				virtual long getNumeric();

				/**
				 * @return array value
				 * @note throws exception if data type is not NODE_DATATYPE_ARRAY
				 */
				virtual dodoArray<node> getArray();

				/**
				 * @return object value
				 * @note throws exception if data type is not NODE_DATATYPE_OBJECT
				 */
				virtual dodoMap<dodoString, node, dodoMapStringCompare> getObject();

				/**
				 * clear internal data
				 */
				virtual void clear();

			private:

				dodoString stringValue;                                                         ///< string value of node
				dodoMap<dodoString, node, dodoMapStringCompare> objectValue;                    ///< object value of node
				dodoArray<node> arrayValue;                                                     ///< array value of node
				bool booleanValue;                                                              ///< boolean value of node
				long numericValue;                                                              ///< numeric value of node

				short valueDataType;                                                            ///< data type of value
		};
	};
};

#endif
