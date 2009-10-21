/***************************************************************************
 *            dataBaseSqlConstructor.h
 *
 *  Mon Jul 18 2005
 *  Copyright  2005  Ni@m
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

#ifndef _DATABASESQLCONSTRUCTOR_H_
#define _DATABASESQLCONSTRUCTOR_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/dataBaseAccumulator.h>

namespace dodo {
    namespace data {
        namespace base {
            namespace sql {
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
                class constructor : public accumulator {
                  public:

                    /*
                     * constructor
                     */
                    constructor();

                    /**
                     * destructor
                     */
                    virtual ~constructor();

                    /**
                     * @return constructed query from collected data
                     * @note it doesn't clean collected data
                     */
                    virtual dodoString construct();

                    /**
                     * automaticaly detect fields types
                     * @param table defines table for which rules will be applied
                     */
                    virtual void requestFieldsTypes(const dodoString &table) = 0;

                    /**
                     * set field type
                     * @param table defines table for which rules will be applied
                     * @param field defines field name
                     * @param type defines field type, @see data::base::sql::fieldEnum
                     */
                    virtual void setFieldType(const dodoString &table,
                                              const dodoString &field,
                                              short            type);

                    /**
                     * @return escaped string
                     * @param data defines a string to escape
                     */
                    static dodoString escapeFields(const dodoString &data);

                  protected:

                    dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare> fieldTypes;   ///< hash of 'db:table' => 'field => 'type''

                    dodoString request;                                                                                                 ///< SQL statement

                    /**
                     * construct `SELECT function` statement
                     */
                    virtual void functionCollect();

                    /**
                     * construct `select procedure` statement
                     */
                    virtual void procedureCollect();

                    /**
                     * construct `select` statement
                     */
                    virtual void selectCollect();

                    /**
                     * construct `join` statement
                     */
                    virtual void joinCollect();

                    /**
                     * construct `insert` statement
                     */
                    virtual void insertCollect();

                    /**
                     * construct `update` statement
                     */
                    virtual void updateCollect();

                    /**
                     * construct `delete` statement
                     */
                    virtual void delCollect();

                    /**
                     * construct `union`, `minus`, `intersect` statements
                     */
                    virtual void subCollect();

                    /**
                     * add additional statements for query
                     * @param qTypeToCheck defines type of additional info to check
                     * @param collectedString defines string that defines additional statement
                     */
                    virtual void additionalCollect(unsigned int     qTypeToCheck,
                                                   const dodoString &collectedString);

                    /**
                     * @return string of substrings, separated with separator, framed with frame and applied escapeFields to each
                     * @param fields defines substrings to join
                     * @param separator defines separator string
                     * @param frame defines frame string[frame='; substring => 'substring']
                     * @param limit defines maximum number of parts to divide string; -1 for unlimit
                     */
                    static dodoString joinFields(const dodoStringArray &fields,
                                                 const dodoString      &separator,
                                                 const dodoString      &frame,
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
                        STATEMENT_CALL,
                        STATEMENT_FROM,
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
                        STATEMENT_NULL,

                        STATEMENT_ENUMSIZE
                    };

                    static const dodoString statements[STATEMENT_ENUMSIZE];                                 ///< constructor statements

                    static const dodoString additionalRequestStatements[ADDITIONAL_REQUEST_ENUMSIZE];       ///< additional statements(`where`, `limit`, ...)

                    static const dodoString subrequestStatements[SUBREQUEST_ENUMSIZE];                      ///< statements for complex queries(`union`, ...)

                    static const dodoString joinStatements[JOIN_ENUMSIZE];                                  ///< statements for `join` queries
                };
            };
        };
    };
};
#endif
