/***************************************************************************
 *            dataBaseAccumulator.h
 *
 *  Thu Aug 25 2005
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

#ifndef _DATABASEACCUMULATOR_H_
#define _DATABASEACCUMULATOR_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/xexec.h>
#include <libdodo/dataBaseConnector.h>

namespace dodo {
    namespace data {
        namespace base {
            /**
             * @class accumulator
             * @brief provides wide abilities for db manipulations
             */
            class accumulator : public connector {
              public:

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
#endif

                    dodoString where;                           ///< `where` for the request(can be used as `as` for `callFunction`)
                    dodoStringArray fields;                     ///< `fields` for request(as `arguments` for `callFunction`; as `arguments` for `callProcedure`)
                    dodoArray<dodoStringArray> values;          ///< `values` for the request
                    dodoString table;                           ///< `table` for the request(as `name` for `callFunction`; as `name` for `callProcedure`)
                    dodoString order;                           ///< `order` for the request
                    dodoString having;                          ///< `having` for the request
                    dodoString group;                           ///< `group` for the request
                    dodoString limit;                           ///< `limit` for the result
                    dodoString offset;                          ///< `offset` for the result
                    dodoStringArray subQueries;                 ///< `subquery`
                    dodoStringArray joinTables;                 ///< join tables
                    dodoStringArray joinConds;                  ///< join conditions
                    dodoArray<int> joinTypes;                   ///< join types

                    __connection__ dbInfo;                      ///< data info to connect to the server

                    int type;                                   ///< type of operation
                    int additional;                             ///< additional actions, @see accumulator::additionalRequestEnum
                };

                /*
                 * constructor
                 */
                accumulator();

                /**
                 * destructor
                 */
                virtual ~accumulator();

                /**
                 * call stored function
                 * @param name defines function name
                 * @param arguments defines arguments
                 * @param as defines name of the result row
                 */
                virtual void function(const dodoString      &name,
                                      const dodoStringArray &arguments,
                                      const dodoString      &as = __dodostring__);

                /**
                 * call stored procedure
                 * @param name is procedure name
                 * @param arguments is array of arguments
                 */
                virtual void procedure(const dodoString      &name,
                                       const dodoStringArray &arguments);

                /**
                 * @param table defines table name
                 * @param fields defines names of fields
                 * @param where defines `where` statement
                 * @note if param table is empty 'from `table`' is not used
                 * if param fields is empty all fields are fetched
                 */
                virtual void select(const dodoString      &table,
                                    const dodoStringArray &fields = __dodostringarray__,
                                    const dodoString      &where = __dodostring__);

                /**
                 * @param table defines table name
                 * @param fields defines hash of field=>value
                 */
                virtual void insert(const dodoString    &table,
                                    const dodoStringMap &fields);

                /**
                 * @param table defines table name
                 * @param fields defines set of hashes of field=>value
                 */
                virtual void insert(const dodoString               &table,
                                    const dodoArray<dodoStringMap> &fields);

                /**
                 * @param table defines table name
                 * @param values defines values
                 * @param fields defines names of fields
                 */
                virtual void insert(const dodoString      &table,
                                    const dodoStringArray &values,
                                    const dodoStringArray &fields = __dodostringarray__);

                /**
                 * @param table defines table name
                 * @param values defines values
                 * @param fields defines names of fields
                 */
                virtual void insert(const dodoString                 &table,
                                    const dodoArray<dodoStringArray> &values,
                                    const dodoStringArray            &fields = __dodostringarray__);


                /**
                 * @param table defines table name
                 * @param fields defines hash of field=>value
                 * @param where defines `where` statement
                 */
                virtual void update(const dodoString    &table,
                                    const dodoStringMap &fields,
                                    const dodoString    &where = __dodostring__);

                /**
                 * @param table defines table name
                 * @param values defines values
                 * @param fields defines names of fields
                 * @param where defines `where` statement
                 */
                virtual void update(const dodoString      &table,
                                    const dodoStringArray &values,
                                    const dodoStringArray &fields,
                                    const dodoString      &where = __dodostring__);

                /**
                 * @param table defines table name
                 * @param where defines `where` statement
                 */
                virtual void del(const dodoString &table,
                                 const dodoString &where = __dodostring__);

                /**
                 * store query, made from subquery with requested method
                 * @param subqueries defines subqueries
                 * @param type defines type of combining subqueries, @see data::base::subrequestEnum
                 */
                virtual void subquery(const dodoStringArray &subqueries,
                                      int                   type = SUBREQUEST_UNION);

                /**
                 * set `limit` property
                 * @param number defines `limit` value
                 */
                virtual void limit(unsigned int number);

                /**
                 * set `offset` property
                 * @param number defines `offset` value
                 */
                virtual void offset(unsigned int number);

                /**
                 * set `order` property
                 * @param order defines `order` value
                 */
                virtual void order(const dodoString &order);

                /**
                 * set `group` property
                 * @param group defines `group` value
                 */
                virtual void group(const dodoString &group);

                /**
                 * set having property
                 * @param having defines having value
                 */
                virtual void having(const dodoString &having);

                /**
                 * append join statement
                 * @param table defines table to join
                 * @param condition defines condition for joining
                 * @param type defines join type, @see data::base::joinEnum
                 */
                virtual void join(const dodoString &table,
                                  int              type,
                                  const dodoString &condition);

                /**
                 * @enum stateNoneEnum defines empty state of the db object
                 */
                enum stateNoneEnum {
                    STATE_NONE
                };

                /**
                 * @enum additionalRequestEnum defines additional properties for the request
                 */
                enum additionalRequestEnum {
                    ADDITIONAL_REQUEST_WHERE = 1,
                    ADDITIONAL_REQUEST_HAVING,
                    ADDITIONAL_REQUEST_GROUPBY,
                    ADDITIONAL_REQUEST_ORDERBY,
                    ADDITIONAL_REQUEST_LIMIT,
                    ADDITIONAL_REQUEST_OFFSET,
                    ADDITIONAL_REQUEST_AS,
                    ADDITIONAL_REQUEST_JOIN,

                    ADDITIONAL_REQUEST_ENUMSIZE
                };

                /**
                 * @enum requestEnum defines type of request
                 */
                enum requestEnum {
                    REQUEST_SELECT = SUBREQUEST_ENUMSIZE + 1,
                    REQUEST_INSERT,
                    REQUEST_UPDATE,
                    REQUEST_DELETE,

                    REQUEST_FUNCTION,
                    REQUEST_PROCEDURE,
                };

              protected:

                /**
                 * free collected data
                 */
                void cleanCollected();

                bool show;                                                  ///< if true try to get result from the request[select]

                __collected_data__ collectedData;                           ///< data collected for xexec
            };
        };
    };
};
#endif
