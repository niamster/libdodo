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
		 * @struct __xexexDbAccumulatorCollectedData defines data that could be retrieved from the db object
		 */
		struct __xexexDbAccumulatorCollectedData
		{
			/**
			 * constructor
			 */
			__xexexDbAccumulatorCollectedData(dodoString &pre_where,
											  dodoStringArray &pre_fields,
											  dodoArray<dodoStringArray> &pre_values,
											  dodoString &pre_table,
											  dodoString &pre_tableTo,
											  dodoString &pre_order,
											  dodoString &pre_having,
											  dodoString &pre_group,
											  dodoString &pre_limit,
											  dodoString &pre_offset,
											  dodoStringArray &pre_subQueries,
											  dodoStringArray &pre_joinTables,
											  dodoStringArray &pre_joinConds,
											  dodoArray<int> &pre_joinTypes,
											  int &qType,
											  int &qShift,
											  int &qSelShift,
											  int &qInsShift,
											  int &qUpShift,
											  int &qDelShift,
											  int &operType,
											  void *executor);

			dodoString &pre_where;                          ///< `where` for the request(can be used as `as` for `callFunction`)
			dodoStringArray &pre_fields;                    ///< `fields` for request(can be used as `fieldsTo` for `insert_select`; as `arguments` for `callFunction`; as `arguments` for `callProcedure`; as `fields`/`field` `createIndex`)
			dodoArray<dodoStringArray> &pre_values;         ///< `values` for the request(can be used as `fieldsFrom` for `insert_select`)
			dodoString &pre_table;                          ///< `table` for the request(can be used `tableTo` for `insert_select`; as `name` for `callFunction`; as `name` for `callProcedure`)
			dodoString &pre_tableTo;                        ///< `tableTo` for the request(can be used as `field` for `deleteField`/`renameField`)
			dodoString &pre_order;                          ///< `order` for the request(can be used as `db` for `deleteField`/`renameField`/`deleteTable`/`renameTable`)
			dodoString &pre_having;                         ///< `having` for the request(can be used as `charset` for 'createDb'; as `to_db` for `renameDb`; as `to_table` for `renameTable`; as `name` for `createIndex`; as `field` for `deleteIndex`; as `to_field` for `renameField`)
			dodoString &pre_group;                          ///< `group` for the request
			dodoString &pre_limit;                          ///< `limit` for the result
			dodoString &pre_offset;                         ///< `offset` for the result
			dodoStringArray &pre_subQueries;                ///< `subquery`
			dodoStringArray &pre_joinTables;                ///< join tables
			dodoStringArray &pre_joinConds;                 ///< join conditions
			dodoArray<int> &pre_joinTypes;                  ///< join types

			int &qType;                                     ///< type of operation

			int &qShift;                                    ///< additional actions[see accumulatorAddEnum]

			int &qSelShift;                                 ///< additional select statements[see accumulatorAddSelEnum]
			int &qInsShift;                                 ///< additional insert statements[see accumulatorAddInsEnum]
			int &qUpShift;                                  ///< additional update statements[see accumulatorAddUpEnum]
			int &qDelShift;                                 ///< additional delete statements[see accumulatorAddDelEnum]

			int &operType;                                  ///< xexec operation

			void *executor;                                 ///< class that executed hook
		};

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
			ACCUMULATOR_REQUEST_SELECT = CONNECTOR_SUBREQUESTSTATEMENTS + 1,
			ACCUMULATOR_REQUEST_INSERT,
			ACCUMULATOR_REQUEST_INSERT_SELECT,
			ACCUMULATOR_REQUEST_UPDATE,
			ACCUMULATOR_REQUEST_DELETE,

			ACCUMULATOR_REQUEST_TRUNCATE,

			ACCUMULATOR_REQUEST_RENAME_DB,
			ACCUMULATOR_REQUEST_RENAME_TABLE,
			ACCUMULATOR_REQUEST_RENAME_FIELD,

			ACCUMULATOR_REQUEST_DELETE_DB,
			ACCUMULATOR_REQUEST_DELETE_TABLE,
			ACCUMULATOR_REQUEST_DELETE_FIELD,

			ACCUMULATOR_REQUEST_CREATE_DB,
			ACCUMULATOR_REQUEST_CREATE_TABLE,
			ACCUMULATOR_REQUEST_CREATE_FIELD,

			ACCUMULATOR_REQUEST_CREATE_INDEX,
			ACCUMULATOR_REQUEST_DELETE_INDEX,

			ACCUMULATOR_REQUEST_CALL_FUNCTION,
			ACCUMULATOR_REQUEST_CALL_PROCEDURE,
		};

		#define ACCUMULATOR_ADDREQUESTSELECTSTATEMENTS 2

		/**
		 * @enum accumulatorAddSelEnum defines additional properties for the `select` request
		 */
		enum accumulatorAddSelEnum
		{
			ACCUMULATOR_ADDREQUEST_SELECT_DISTINCT = 1,
			ACCUMULATOR_ADDREQUEST_SELECT_ALL
		};

		#define ACCUMULATOR_ADDREQUESTDELETESTATEMENTS 1

		/**
		 * @enum accumulatorAddDelEnum defines additional properties for the `delete` request
		 */
		enum accumulatorAddDelEnum
		{
			ACCUMULATOR_ADDREQUEST_DELETE_IGNORE = 1,
		};

		#define ACCUMULATOR_ADDREQUESTUPDATESTATEMENTS 1

		/**
		 * @enum accumulatorAddUpEnum defines additional properties for the `update` request
		 */
		enum accumulatorAddUpEnum
		{
			ACCUMULATOR_ADDREQUEST_UPDATE_IGNORE = 1,
		};

		#define ACCUMULATOR_ADDREQUESTINSERTSTATEMENTS 1

		/**
		 * @enum accumulatorAddInsEnum defines additional properties for the `insert` request
		 */
		enum accumulatorAddInsEnum
		{
			ACCUMULATOR_ADDREQUEST_INSERT_IGNORE = 1,
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
				 * @param db defines name of db
				 * @param host defines host
				 * @param user defines user
				 * @param password defines password
				 * @param path defines path to db or unix socket
				 * @param port defines port
				 */
				virtual void setDbInfo(const dodoString &db, const dodoString &host, unsigned int port, const dodoString &user, const dodoString &password, const dodoString &path = __dodostring__);

				/**
				 * set connection information for database
				 * @param dbInfo defines connection information
				 */
				virtual void setDbInfo(const __connectorInfo &dbInfo);

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
				 * @param type defines type of combining subqueries[see connectorSubrequestEnum]
				 */
				virtual void subquery(const dodoStringArray &subqueries, int type = CONNECTOR_SUBREQUEST_UNION);

				/**
				 * create index in the table
				 * @param table defines table where create index
				 * @param field defines field that would be index
				 * @param name defines name of the index
				 */
				virtual void createIndex(const dodoString &table, const dodoString &field, const dodoString &name);

				/**
				 * create index in the table
				 * @param table defines table where create index
				 * @param fields defines fields that would be index
				 * @param name defines name of the index
				 */
				virtual void createIndex(const dodoString &table, const dodoStringArray &fields, const dodoString &name);

				/**
				 * delete index in the table
				 * @param table defines table where delete index
				 * @param name defines name of the index
				 */
				virtual void deleteIndex(const dodoString &table, const dodoString &name);

				/**
				 * rename database
				 * @param db defines current name of the database
				 * @param to_db defines new name of the database
				 */
				virtual void renameDb(const dodoString &db, const dodoString &to_db);

				/**
				 * rename table
				 * @param table defines current name of the table
				 * @param to_table defines new name of the table
				 */
				virtual void renameTable(const dodoString &table, const dodoString &to_table);

				/**
				 * rename field
				 * @param field defines current name of the field
				 * @param to_field defines new name of the field
				 * @param table defines table that contains the field
				 */
				virtual void renameField(const dodoString &field, const dodoString &to_field, const dodoString &table);

				/**
				 * delete database
				 * @param db defines the database
				 */
				virtual void deleteDb(const dodoString &db);

				/**
				 * delete table
				 * @param table defines the table
				 */
				virtual void deleteTable(const dodoString &table);

				/**
				 * delete field
				 * @param field defines field that will be deleted
				 * @param table defines table that contains the field
				 */
				virtual void deleteField(const dodoString &field, const dodoString &table);

				/**
				 * create database
				 * @param db the name of the new database
				 * @param charset defines charset of the database
				 */
				virtual void createDb(const dodoString &db, const dodoString &charset = __dodostring__);

				/**
				 * create table
				 * @param tableInfo defines table definition[see __connectorTable]
				 */
				virtual void createTable(const __connectorTable &tableInfo);

				/**
				 * create field
				 * @param fieldInfo defines field definition[see __connectorField]
				 * @param table defines table that will contain the field
				 */
				virtual void createField(const __connectorField &fieldInfo, const dodoString &table);

				/**
				 * truncate table
				 * @param table is name name of table to truncate
				 */
				virtual void truncate(const dodoString &table);

				/**
				 * set `where` statement
				 * @param where defines `where` statement
				 * @note overwrites previous definition[from methods that may define it]
				 */
				virtual void where(const dodoString &where);

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
				 * remove `where` statement
				 */
				virtual void unwhere();

				/**
				 * remove `limit` property
				 */
				virtual void unlimit();

				/**
				 * remove `offset` property
				 */
				virtual void unoffset();

				/**
				 * remove `order` property
				 */
				virtual void unorder();

				/**
				 * remove `group` property
				 */
				virtual void ungroup();

				/**
				 * remove `having` property
				 */
				virtual void unhaving();

				/**
				 * set additional parameters for `insert` request
				 * @param statement defines additional parameters of the statement; may be combined with '|'[see accumulatorAddInsEnum]
				 */
				virtual void setAddInsSt(unsigned int statement);

				/**
				 * set additional parameters for `update` request
				 * @param statement defines additional parameters of the statement; may be combined with '|'[see accumulatorAddUpEnum]
				 */
				virtual void setAddUpSt(unsigned int statement);

				/**
				 * sets additional parameters for `select` request
				 * @param statement defines additional parameters of the statement; may be combined with '|'[see accumulatorAddSelEnum]
				 */
				virtual void setAddSelSt(unsigned int statement);

				/**
				 * sets additional parameters for `delete` request
				 * @param statement defines additional parameters of the statement; may be combined with '|'[see accumulatorAddDelEnum]
				 */
				virtual void setAddDelSt(unsigned int statement);

				/**
				 * remove additional parameters from `insert` request
				 * @param statement defines additional parameters of the statemente; may be combined with '|'[see accumulatorAddInsEnum]
				 */
				virtual void unsetAddInsSt(unsigned int statement);

				/**
				 * remove additional parameters from `update` request
				 * @param statement defines additional parameters of the statement; may be combined with '|'[see accumulatorAddUpEnum]
				 */
				virtual void unsetAddUpSt(unsigned int statement);

				/**
				 * remove additional parameters from `select` request
				 * @param statement defines additional parameters of the statement; may be combined with '|'[see accumulatorAddSelEnum]
				 */
				virtual void unsetAddSelSt(unsigned int statement);

				/**
				 * remove additional parameters from `delete` request
				 * @param statement defines additional parameters of the statement; may be combined with '|'[see accumulatorAddDelEnum]
				 */
				virtual void unsetAddDelSt(unsigned int statement);

				/**
				 * set default values for table
				 * @param table defines table structure that will be initialized
				 */
				static void initTableInfo(__connectorTable &table);

				/**
				 * set default values for field
				 * @param field defines field structure that will be initialized
				 */
				static void initFieldInfo(__connectorField &field);

			protected:

				/**
				 * free collected data
				 */
				virtual void cleanCollected();

				bool show;                                      ///< if true try to get result from the request[select]

				__connectorTable pre_tableInfo;                 ///< table structure for creation
				__connectorField pre_fieldInfo;                 ///< field structure for creation

				__connectorInfo dbInfo;                         ///< data info to connect to the server

				/*
				 * additional statements for query, db-dependent
				 * can be implemented in derived class
				 * these arrays take part after generalSQL statements
				 * every of these objects has one empty("") element
				 */
				dodoStringArray sqlDbDepAddSelArr;                  ///< additional `select` statement
				dodoStringArray sqlDbDepAddInsArr;                  ///< additional `insert` statement
				dodoStringArray sqlDbDepAddUpArr;                   ///< additional `update` statement
				dodoStringArray sqlDbDepAddDelArr;                  ///< additional `delete` statement

				int qDbDepSelShift;                                 ///< additional db-dependent `select` actions[see sqlDbDepAddSelArr]
				int qDbDepInsShift;                                 ///< additional db-dependent `insert` actions[see sqlDbDepAddInsArr]
				int qDbDepUpShift;                                  ///< additional db-dependent `update` actions[see sqlDbDepAddUpArr]
				int qDbDepDelShift;                                 ///< additional db-dependent `delete` actions[see sqlDbDepAddDelArr]

				dodoString pre_where;                               ///< `where` for the request(can be used as `as` for `callFunction`)
				dodoStringArray pre_fields;                         ///< `fields` for request(can be used as `fieldsTo` for `insert_select`; as `arguments` for `callFunction`; as `arguments` for `callProcedure`; as `fields`/`field` `createIndex`)
				dodoArray<dodoStringArray> pre_values;              ///< `values` for the request(can be used as `fieldsFrom` for `insert_select`)
				dodoString pre_table;                               ///< `table` for the request(can be used `tableTo` for `insert_select`; as `name` for `callFunction`; as `name` for `callProcedure`)
				dodoString pre_tableTo;                             ///< `tableTo` for the request(can be used as `field` for `deleteField`/`renameField`)
				dodoString pre_order;                               ///< `order` for the request(can be used as `db` for `deleteField`/`renameField`/`deleteTable`/`renameTable`)
				dodoString pre_having;                              ///< `having` for the request(can be used as `charset` for 'createDb'; as `to_db` for `renameDb`; as `to_table` for `renameTable`; as `name` for `createIndex`; as `field` for `deleteIndex`; as `to_field` for `renameField`)
				dodoString pre_group;                               ///< `group` for the request
				dodoString pre_limit;                               ///< `limit` for the result
				dodoString pre_offset;                              ///< `offset` for the result
				dodoStringArray pre_subQueries;                     ///< `subquery`
				dodoStringArray pre_joinTables;                     ///< join tables
				dodoStringArray pre_joinConds;                      ///< join conditions
				dodoArray<int> pre_joinTypes;                       ///< join types

				int qType;                                          ///< type of operation

				int qShift;                                         ///< additional actions[see accumulatorAddEnum]

				int qSelShift;                                      ///< additional select statements[see accumulatorAddSelEnum]
				int qInsShift;                                      ///< additional insert statements[see accumulatorAddInsEnum]
				int qUpShift;                                       ///< additional update statements[see accumulatorAddUpEnum]
				int qDelShift;                                      ///< additional delete statements[see accumulatorAddDelEnum]

				__xexexDbAccumulatorCollectedData collectedData;    ///< data collected for xexec
		};
	};
};

#endif
