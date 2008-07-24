/***************************************************************************
 *            dbAccumulator.h
 *
 *  Thu Aug 25 21:39:24 2005
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

#ifndef _DBACCUMULATOR_H_
#define _DBACCUMULATOR_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/toolsString.h>
#include <libdodo/dbConnector.h>

namespace dodo
{
	namespace db
	{
		/**
		 * @struct __xexecDbAccumulatorCollectedData defines data that could be retrieved from the db object
		 */
		struct __xexecDbAccumulatorCollectedData
		{

#ifndef DB_WO_XEXEC

			/**
			 * constructor
			 * @param operType defines xexec operation
			 * @param executor defines class that executed hook
			 */
			__xexecDbAccumulatorCollectedData(int &operType, void *executor);

#endif

			dodoString where;                                           ///< `where` for the request(can be used as `as` for `callFunction`)
			dodoStringArray fields;                                     ///< `fields` for request(can be used as `fieldsTo` for `insert_select`; as `arguments` for `callFunction`; as `arguments` for `callProcedure`; as `fields`/`field` `createIndex`)
			dodoArray<dodoStringArray> values;                          ///< `values` for the request(can be used as `fieldsFrom` for `insert_select`)
			dodoString table;                                           ///< `table` for the request(can be used `tableTo` for `insert_select`; as `name` for `callFunction`; as `name` for `callProcedure`)
			dodoString tableTo;                                         ///< `tableTo` for the request
			dodoString order;                                           ///< `order` for the request
			dodoString having;                                          ///< `having` for the request
			dodoString group;                                           ///< `group` for the request
			dodoString limit;                                           ///< `limit` for the result
			dodoString offset;                                          ///< `offset` for the result
			dodoStringArray subQueries;                                 ///< `subquery`
			dodoStringArray joinTables;                                 ///< join tables
			dodoStringArray joinConds;                                  ///< join conditions
			dodoArray<int> joinTypes;                                   ///< join types

			__connectionInfo dbInfo;                                     ///< data info to connect to the server

			int qType;                                                  ///< type of operation
			int qShift;                                                 ///< additional actions[see accumulatorAddEnum]

#ifndef DB_WO_XEXEC

			int &operType;                                              ///< xexec operation

			void *executor;                                             ///< class that executed hook

#endif
		};

		/**
		 * @class accumulator provides wide abilities for db manipulations
		 */
		class accumulator : public connector
		{
			public:

				/*
				 * constructor
				 */
				accumulator();

				/**
				 * destructor
				 */
				virtual ~accumulator();

				/**
				 * set connection information for database
				 * @param dbInfo defines connection information
				 */
				virtual void setDbInfo(const __connectionInfo &dbInfo);

				/**
				 * call stored function
				 * @param name defines function name
				 * @param arguments defines arguments
				 * @param as defines name of the result row
				 */
				void callFunction(const dodoString &name, const dodoStringArray &arguments, const dodoString &as = __dodostring__);

				/**
				 * call stored procedure
				 * @param name is procedure name
				 * @param arguments is array of arguments
				 */
				void callProcedure(const dodoString &name, const dodoStringArray &arguments);

				/**
				 * @param table defines table name
				 * @param fields defines names of fields
				 * @param where defines `where` statement
				 * @note if (length(table) == 0) => 'from `table`' is not used
				 */
				virtual void select(const dodoString &table, const dodoStringArray &fields, const dodoString &where = __dodostring__);

				/**
				 * @param table defines table name
				 * @param where defines `where` statement
				 */
				virtual void selectAll(const dodoString &table, const dodoString &where = __dodostring__);

				/**
				 * @param table defines table name
				 * @param fields defines hash of field=>value
				 */
				virtual void insert(const dodoString &table, const dodoStringMap &fields);

				/**
				 * @param table defines table name
				 * @param fields defines set of hashes of field=>value
				 */
				virtual void insert(const dodoString &table, const dodoArray<dodoStringMap> &fields);

				/**
				 * @param table defines table name
				 * @param values defines values
				 * @param fields defines names of fields
				 */
				virtual void insert(const dodoString &table, const dodoStringArray &values, const dodoStringArray &fields = __dodostringarray__);

				/**
				 * @param table defines table name
				 * @param values defines values
				 * @param fields defines names of fields
				 */
				virtual void insert(const dodoString &table, const dodoArray<dodoStringArray> &values, const dodoStringArray &fields = __dodostringarray__);

				/**
				 * @param tableTo defines table where data will be stored
				 * @param tableFrom defines table where data will be fetched
				 * @param fieldsTo defines fields where data will be stored
				 * @param fieldsFrom defines fields that will be fetched
				 * @param where defines `where` statement
				 */
				virtual void insertSelect(const dodoString &tableTo, const dodoString &tableFrom, const dodoStringArray &fieldsTo, const dodoStringArray &fieldsFrom = __dodostringarray__, const dodoString &where = __dodostring__);

				/**
				 * @param table defines table name
				 * @param fields defines hash of field=>value
				 * @param where defines `where` statement
				 */
				virtual void update(const dodoString &table, const dodoStringMap &fields, const dodoString &where = __dodostring__);

				/**
				 * @param table defines table name
				 * @param values defines values
				 * @param fields defines names of fields
				 * @param where defines `where` statement
				 */
				virtual void update(const dodoString &table, const dodoStringArray &values, const dodoStringArray &fields, const dodoString &where = __dodostring__);

				/**
				 * @param table defines table name
				 * @param where defines `where` statement
				 */
				virtual void del(const dodoString &table, const dodoString &where = __dodostring__);

				/**
				 * store query, made from subquery with requested method
				 * @param subqueries defines subqueries
				 * @param type defines type of combining subqueries[see subrequestEnum]
				 */
				virtual void subquery(const dodoStringArray &subqueries, int type = SUBREQUEST_UNION);

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
				 * @param type defines join type[see accumulatorJoinTypeEnum]
				 */
				virtual void join(const dodoString &table, int type, const dodoString &condition);

				/**
				 * @enum accumulatorNoneEnum defines empty state of the db object
				 */
				enum accumulatorNoneEnum
				{
					ACCUMULATOR_NONE
				};

		#define ACCUMULATOR_ADDREQUESTSTATEMENTS 8

				/**
				 * @enum accumulatorAddRequestEnum defines additional properties for the request
				 */
				enum accumulatorAddRequestEnum
				{
					ACCUMULATOR_ADDREQUEST_WHERE = 1,
					ACCUMULATOR_ADDREQUEST_HAVING,
					ACCUMULATOR_ADDREQUEST_GROUPBY,
					ACCUMULATOR_ADDREQUEST_ORDERBY,
					ACCUMULATOR_ADDREQUEST_LIMIT,
					ACCUMULATOR_ADDREQUEST_OFFSET,
					ACCUMULATOR_ADDREQUEST_AS,
					ACCUMULATOR_ADDREQUEST_JOIN,
				};

				/**
				 * @enum accumulatorRequestEnum defines type of request
				 */
				enum accumulatorRequestEnum
				{
					ACCUMULATOR_REQUEST_SELECT = SUBREQUESTSTATEMENTS + 1,
					ACCUMULATOR_REQUEST_INSERT,
					ACCUMULATOR_REQUEST_INSERT_SELECT,
					ACCUMULATOR_REQUEST_UPDATE,
					ACCUMULATOR_REQUEST_DELETE,

					ACCUMULATOR_REQUEST_CALL_FUNCTION,
					ACCUMULATOR_REQUEST_CALL_PROCEDURE,
				};

			protected:

				/**
				 * free collected data
				 */
				virtual void cleanCollected();

				bool show;                                                      ///< if true try to get result from the request[select]

				__xexecDbAccumulatorCollectedData collectedData;                        ///< data collected for xexec
		};
	};
};

#endif
