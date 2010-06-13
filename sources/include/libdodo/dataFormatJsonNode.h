/***************************************************************************
 *            dataFormatJsonNode.h
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

#ifndef _DATAFORMATJSONNODE_H_
#define _DATAFORMATJSONNODE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace rpc {
        namespace json {
            class method;
            class response;
            class value;
        };
    };

    namespace data {
        namespace format {
            namespace json {
                /**
                 * @class node
                 * @brief defines JSON object reprasentation
                 */
                class node {
                    friend class processor;
                    friend class rpc::json::method;
                    friend class rpc::json::response;
                    friend class rpc::json::value;

                  public:

                    /**
                     * @enum dataEnum defines JSON data types
                     */
                    enum dataEnum {
                        DATA_STRING,
                        DATA_OBJECT,
                        DATA_ARRAY,
                        DATA_BOOLEAN,
                        DATA_NUMERIC,
                        DATA_NULL,
                    };

                    /**
                     * copy constructor
                     */
                    node(const node &);

                    /**
                     * constructor
                     * @note constructs `null` object
                     */
                    node();

                    /**
                     * constructor
                     * @param value defines string value
                     */
                    node(const dodo::string &value);

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
                    node(const dodoMap<dodo::string, node, dodoMapStringCompare> &value);

                    /**
                     * destructor
                     */
                    ~node();

                    /**
                     * set string, date/time, base64 value
                     * @param value defines string value
                     */
                    void setString(const dodo::string &value);

                    /**
                     * set boolean value
                     * @param value defines boolean value
                     */
                    void setBoolean(bool value);

                    /**
                     * set integer value
                     * @param value defines integer value
                     */
                    void setNumeric(long value);

                    /**
                     * set null value
                     */
                    void setNull();

                    /**
                     * add array value
                     * @param value defines array member value
                     */
                    void addArrayElement(const node &value);

                    /**
                     * add struct value element
                     * @param name defines struct member name
                     * @param value defines struct member value
                     */
                    void addObjectMember(const dodo::string &name,
                                         const node       &value);

                    /**
                     * set array value member
                     * @param value defines array value
                     */
                    void setArray(const dodoArray<node> &value);

                    /**
                     * set struct value
                     * @param value defines struct value
                     */
                    void setObject(const dodoMap<dodo::string, node, dodoMapStringCompare> &value);

                    /**
                     * @return node by string key
                     * @param key defines key to search for node
                     * @note throws exception if data type is not DATA_OBJECT
                     */
                    node operator[](const dodo::string &key) const;

                    /**
                     * @return node by numeric key
                     * @param key defines key to search for node
                     * @note throws exception if data type is not DATA_ARRAY
                     */
                    node operator[](unsigned long key) const;

                    /**
                     * @return type of node, @see json::dataEnum
                     */
                    short type() const;

                    /**
                     * @return true if node is `null`
                     */
                    bool isNull() const;

                    /**
                     * @return string value
                     * @note throws exception if data type is not DATA_STRING
                     */
                    dodo::string string() const;

                    /**
                     * @return boolean value
                     * @note throws exception if data type is not DATA_BOOLEAN
                     */
                    bool boolean() const;

                    /**
                     * @return numeric value
                     * @note throws exception if data type is not DATA_NUMERIC
                     */
                    long numeric() const;

                    /**
                     * @return array value
                     * @note throws exception if data type is not DATA_ARRAY
                     */
                    dodoArray<node> array() const;

                    /**
                     * @return object value
                     * @note throws exception if data type is not DATA_OBJECT
                     */
                    dodoMap<dodo::string, node, dodoMapStringCompare> object() const;

                  private:

                    union {
                        dodo::string                                      *stringValue;                                               ///< string value of node
                        dodoMap<dodo::string, node, dodoMapStringCompare> *objectValue;                                               ///< object value of node
                        dodoArray<node>                                 *arrayValue;                                                ///< array value of node
                        bool                                            booleanValue;                                               ///< boolean value of node
                        long                                            numericValue;                                               ///< numeric value of node
                    };

                    mutable short valueDataType;                                                                                    ///< data type of value
                };
            };
        };
    };
};
#endif
