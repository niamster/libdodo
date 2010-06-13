/***************************************************************************
 *            dataBaseSqlConstructor.h
 *
 *  Mon Jul 18 2005
 *  Copyright  2005  Dmytro Milinevskyy
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

#ifndef _DATABASESQLCONSTRUCTOR_H_
#define _DATABASESQLCONSTRUCTOR_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/dataBaseConnector.h>

namespace dodo {
    namespace data {
        namespace base {
            namespace sql {
                /**
                 * @class condition
                 * @brief defines condition for data base operations
                 */
                class condition : public data::base::condition {
                  public:

                    /*
                     * @enum orderDirectionEnum defines order condition direction
                     */
                    enum orderDirectionEnum {
                        ORDER_DIRECTION_ASC,
                        ORDER_DIRECTION_DESC,
                    };

                    /**
                     * constructor
                     */
                    condition();

                    /**
                     * constructor
                     * @param table defines query table
                     * @param statement defines query condition
                     */
                    condition(const dodo::string &table,
                              const dodo::string &statement = __dodostring__);

                    /**
                     * constructor
                     * @param table defines query table
                     * @param fields defines fields in table that correspond to data in rows
                     * @param statement defines query condition
                     */
                    condition(const dodo::string &table,
                              const dodoStringArray &fields,
                              const dodo::string &statement = __dodostring__);

                    /**
                     * destructor
                     */
                    virtual ~condition();

                    /**
                     * @return condition
                     * @param limit defines limit condition
                     */
                    const condition &limit(long limit) const;

                    /**
                     * @return condition
                     * @param offset defines offset condition
                     */
                    const condition &offset(long offset) const;

                    /**
                     * @return condition
                     * @param field defines field name for order condition
                     * @param direction defines direction for order condition[@see enum orderDirectionEnum]
                     */
                    const condition &order(const dodo::string &field,
                                           short direction = ORDER_DIRECTION_ASC) const;

                    /**
                     * @enum joinEnum defines JOIN types
                     */
                    enum joinEnum {
                        JOIN_INNER,
                        JOIN_OUTER,
                        JOIN_LEFT,
                        JOIN_RIGHT,
                    };

                    /**
                     * @return condition
                     * @param table defines table to join with
                     * @param condition defines JOIN condition
                     * @param type defines JOIN type
                     */
                    const condition &join(const dodo::string &table,
                                          const dodo::string &condition,
                                          short type = JOIN_INNER) const;

                    /**
                     * @struct __join__
                     * @brief defines JOIN statement
                     */
                    struct __join__ {
                        short type; ///< JOIN type [@see condition::joinEnum]
                        dodo::string table; ///< table to join with
                        dodo::string condition; ///< JOIN condition
                    };

                    mutable dodoArray<__join__> _join;
                    mutable dodo::string _orderby; ///< order condition
                    mutable long _limit; ///< limit condition
                    mutable long _offset; ///< offset condition
                    dodo::string _table; ///< table in data base
                    dodo::string _statement; ///< SQL request condition
                    dodoStringArray _fields; ///< names of fiels
                };

                /**
                 * @class row
                 * @brief defines rows of data base
                 */
                class rows : public data::base::rows {
                  public:

                    /**
                     * constructor
                     */
                    rows();

                    /**
                     * constructor
                     * @param values defines data in rows
                     * @param fields defines fields in table that correspond to data in rows
                     */
                    rows(const dodoArray<dodoStringArray> &values,
                         const dodoStringArray &fields = __dodostringarray__);

                    /**
                     * constructor
                     * @param values defines data in rows
                     * @param fields defines fields in table that correspond to data in rows
                     */
                    rows(const dodoStringArray &values,
                         const dodoStringArray &fields = __dodostringarray__);

                    /**
                     * constructor
                     * @param map defines array of mapping of fields to data in row
                     */
                    rows(const dodoArray<dodoStringMap> &map);

                    /**
                     * constructor
                     * @param map defines mapping of fields to data in row
                     */
                    rows(const dodoStringMap &map);

                    /**
                     * destructor
                     */
                    virtual ~rows();

                    dodoStringArray fields; ///< names of fiels
                    dodoArray<dodoStringArray> values; ///< data in rows
                };

                /**
                 * @class query
                 * @brief defines data base query
                 */
                class query: public data::base::query {
                  public:

                    /**
                     * constructor
                     */
                    query();

                    /**
                     * constructor
                     * @param sql defines data base request in SQL form
                     */
                    query(const dodo::string &sql);

                    /**
                     * destructor
                     */
                    virtual ~query();

                    dodo::string sql;
                };

                /**
                 * @enum fieldEnum defines field type
                 */
                enum fieldEnum {
                    FIELD_TEXT,
                    FIELD_BINARY,
                    FIELD_NUMERIC,
                };

                /**
                 * @class constructor
                 * @brief provide construction of SQL request
                 * @note all data become escaped and framed with '. Names of fields are not framed with ` to prevent cases as `count(*)`, etc.
                 * If you want to prevent data framing define preventFraming sqlConstructor class propertie as true but remember
                 */
                class constructor : public connector {
                  public:

                    /**
                     * @enum requestEnum defines type of request
                     */
                    enum requestEnum {
                        REQUEST_NONE,
                        REQUEST_SELECT,
                        REQUEST_INSERT,
                        REQUEST_UPDATE,
                        REQUEST_REMOVE,
                    };

                    /**
                     * @class __collected_data__
                     * @brief defines data that could be retrieved from the db object
                     */
                    class __collected_data__
#ifndef DATABASE_WO_XEXEC
                        : public xexec::__collected_data__
#endif
                    {
                      public:

#ifndef DATABASE_WO_XEXEC
                        /**
                         * constructor
                         * @param executor defines class that executed hook
                         * @param execObject defines type of object that executed a hook, @see xexec::objectEnum
                         */
                        __collected_data__(xexec *executor,
                                           short execObject);
#else
                        /**
                         * constructor
                         */
                        __collected_data__();
#endif
                        /**
                         * clear collected data
                         */
                        void clear();

                        int type;                                   ///< type of request @see constructor::requestEnum

                        sql::condition condition; ///< request condition
                        sql::rows rows; ///< request data

                        const sql::query *query; ///< ready for execution query
                    };

                    /*
                     * constructor
                     */
                    constructor();

                    /**
                     * destructor
                     */
                    virtual ~constructor();

                    /**
                     * @param fields defines fields to select
                     * @param condition defines row selection condition
                     * @note if param fields is empty all fields are fetched
                     */
                    virtual void select(const data::base::condition &condition);

                    /**
                     * @param rows defines rows for insertion into data base
                     * @param condition defines row insertion condition
                     */
                    virtual void insert(const data::base::rows      &rows,
                                        const data::base::condition &condition);

                    /**
                     * @param rows defines values of row for update
                     * @param condition defines row update condition
                     */
                    virtual void update(const data::base::rows      &rows,
                                        const data::base::condition &condition);

                    /**
                     * @param condition defines row remove condition
                     */
                    virtual void remove(const data::base::condition &condition);

                    /**
                     * @return constructed query from collected data
                     * @note it doesn't clean collected data
                     */
                    virtual dodo::string construct();

                    /**
                     * automaticaly detect fields types
                     * @param table defines table for which rules will be applied
                     */
                    virtual void requestFieldsTypes(const dodo::string &table) = 0;

                    /**
                     * set field type
                     * @param table defines table for which rules will be applied
                     * @param field defines field name
                     * @param type defines field type, @see data::base::sql::fieldEnum
                     */
                    virtual void setFieldType(const dodo::string &table,
                                              const dodo::string &field,
                                              short            type);

                    /**
                     * @return escaped string
                     * @param data defines a string to escape
                     */
                    static dodo::string escapeFields(const dodo::string &data);

                  protected:

                    dodoMap<dodo::string, dodoMap<dodo::string, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare> fieldTypes;   ///< hash of 'db:table' => 'field => 'type''

                    /**
                     * @return select SQL statement
                     */
                    virtual dodo::string select();

                    /**
                     * @return insert SQL statement
                     */
                    virtual dodo::string insert();

                    /**
                     * @return update SQL statement
                     */
                    virtual dodo::string update();

                    /**
                     * @return delete SQL statement
                     */
                    virtual dodo::string remove();

                    /**
                     * @return string of substrings, separated with separator, framed with frame and applied escapeFields to each
                     * @param fields defines substrings to join
                     * @param separator defines separator string
                     * @param frame defines frame string[frame='; substring => 'substring']
                     * @param limit defines maximum number of parts to divide string; -1 for unlimit
                     */
                    static dodo::string joinFields(const dodoStringArray &fields,
                                                 const dodo::string      &separator,
                                                 const dodo::string      &frame,
                                                 int                   limit = -1);

                    /**
                     * @enum statementEnum defines constructor statements
                     * @note defines positions of string representation in 'statements' class property
                     */
                    enum statementEnum {
                        STATEMENT_EQUAL,
                        STATEMENT_EQUALAPOSTROPHE,
                        STATEMENT_APOSTROPHE,
                        STATEMENT_COMA,
                        STATEMENT_LEFTBRACKET,
                        STATEMENT_RIGHTBRACKET,
                        STATEMENT_SELECT,
                        STATEMENT_FROM,
                        STATEMENT_STAR,
                        STATEMENT_COLON,
                        STATEMENT_APOSTROPHECOMA,
                        STATEMENT_RIGHTBRACKETCOMA,
                        STATEMENT_INSERT,
                        STATEMENT_INTO,
                        STATEMENT_VALUES,
                        STATEMENT_RIGHTBRACKETSELECT,
                        STATEMENT_UPDATE,
                        STATEMENT_SET,
                        STATEMENT_DELETE,
                        STATEMENT_WHERE,
                        STATEMENT_LIMIT,
                        STATEMENT_OFFSET,
                        STATEMENT_ORDERBY,
                        STATEMENT_JOININNER,
                        STATEMENT_JOINOUTER,
                        STATEMENT_JOINLEFT,
                        STATEMENT_JOINRIGHT,
                        STATEMENT_ON,
                        STATEMENT_NULL,

                        STATEMENT_ENUMSIZE
                    };

                    static const dodo::string statements[STATEMENT_ENUMSIZE]; ///< constructor statements

                    bool result; ///< if true try to get result from the request

                    __collected_data__ collectedData; ///< data collected
                };
            };
        };
    };
};
#endif
