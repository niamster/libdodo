/***************************************************************************
 *            db.h
 *
 *  Sun Jan  15 19:45:19 2005
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

#ifndef _DBINTERFACE_H_
#define _DBINTERFACE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/xexec.h>

namespace dodo
{
	/**
	 * @enum dbEmptyEnum defines empty state of the db object
	 */
	enum dbEmptyEnum
	{
		DB_EMPTY = 0
	};

	/**
	 * @struct __dbStorage defines fetched data from db
	 */
	struct __dbStorage
	{
		/**
		 * constructor
		 * @param rows defines rows of data
		 * @param fields defines names of fields
		 */
		__dbStorage(dodoArray<dodoStringArray> rows, dodoStringArray fields);
		
		/**
		 * constructor
		 */
		__dbStorage();

		dodoArray<dodoStringArray> rows;    ///< rows of data
		dodoStringArray fields;             ///< names of fields
	};

	/**
	 * @struct __xexexDbBaseCollectedData defines data that could be retrieved from the db object
	 */
	struct __xexexDbBaseCollectedData
	{
		/**
		 * constructor
		 */
		__xexexDbBaseCollectedData(dodoString &pre_where,
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
		dodoStringArray &pre_fields;               ///< `fields` for request(can be used as `fieldsTo` for `insert_select`; as `arguments` for `callFunction`; as `arguments` for `callProcedure`; as `fields`/`field` `createIndex`)
		dodoArray<dodoStringArray> &pre_values;      ///< `values` for the request(can be used as `fieldsFrom` for `insert_select`)
		dodoString &pre_table;                          ///< `table` for the request(can be used `tableTo` for `insert_select`; as `name` for `callFunction`; as `name` for `callProcedure`)
		dodoString &pre_tableTo;                        ///< `tableTo` for the request(can be used as `field` for `deleteField`/`renameField`)
		dodoString &pre_order;                          ///< `order` for the request(can be used as `db` for `deleteField`/`renameField`/`deleteTable`/`renameTable`)
		dodoString &pre_having;                         ///< `having` for the request(can be used as `charset` for 'createDb'; as `to_db` for `renameDb`; as `to_table` for `renameTable`; as `name` for `createIndex`; as `field` for `deleteIndex`; as `to_field` for `renameField`)
		dodoString &pre_group;                          ///< `group` for the request
		dodoString &pre_limit;                      ///< `limit` for the result
		dodoString &pre_offset;                      ///< `offset` for the result
		dodoStringArray &pre_subQueries;                ///< `subquery`
		dodoStringArray &pre_joinTables;                ///< join tables
		dodoStringArray &pre_joinConds;                 ///< join conditions
		dodoArray<int> &pre_joinTypes;                  ///< join types

		int &qType;                                     ///< type of operation

		int &qShift;                                    ///< additional actions[see dbBaseAddEnum]

		int &qSelShift;                                 ///< additional select statements[see dbBaseAddSelEnum]
		int &qInsShift;                                 ///< additional insert statements[see dbBaseAddInsEnum]
		int &qUpShift;                                  ///< additional update statements[see dbBaseAddUpEnum]
		int &qDelShift;                                 ///< additional delete statements[see dbBaseAddDelEnum]

		int &operType;                                  ///< xexec operation

		void *executor;                                 ///< class that executed hook
	};

	/**
	 * @struct __sqlInfo defines connection options for the server
	 */
	struct __dbInfo
	{

		/**
		 * constructor
		 */
		__dbInfo();

		/**
		 * constructor
		 * @param db defines name of db
		 * @param host defines host
		 * @param user defines user
		 * @param password defines password
		 * @param path defines path to db or unix socket
		 * @param port defines port
		 */
		__dbInfo(const dodoString &db, const dodoString &host, const dodoString &user, const dodoString &password, const dodoString &path, int port);

		dodoString db;          ///< database name
		dodoString host;        ///< hostname
		dodoString user;        ///< username
		dodoString password;    ///< password
		dodoString path;        ///< path to db or unix socket
		unsigned int port;      ///< port
	};

	
	/**
	 * @enum dbJoinTypeEnum defines join types
	 */
	enum dbJoinTypeEnum
	{
		DB_JOINTYPE_JOIN,
		DB_JOINTYPE_LEFTOUTER,
		DB_JOINTYPE_RIGHTOUTER,
		DB_JOINTYPE_FULLOUTER,
		DB_JOINTYPE_INNER,
		DB_JOINTYPE_CROSS,
	};

	/**
	 * @class db implements an interface to db through sql and database independent interfaces
	 */
	class db : public xexec
	{
		public:

			/**
			 *constructor
			 */
			db();

			/**
			 * destructor
			 */
			virtual ~db() = 0;

			/**
			 * connect to the database
			 */
			virtual void connect() = 0;

			/**
			 * disconnect from the database
			 */
			virtual void disconnect() = 0;
			
			/**
			 * set connection information for database
			 * @param db defines name of db
			 * @param host defines host
			 * @param user defines user
			 * @param password defines password
			 * @param path defines path to db or unix socket
			 * @param port defines port
			 */
			virtual void setDbInfo(const dodoString &db, const dodoString &host, unsigned int port, const dodoString &user, const dodoString &password, const dodoString &path = __dodostring__) = 0;

			/**
			 * set connection information for database
			 * @param info defines connection information
			 */
			virtual void setDbInfo(const __dbInfo &dbInfo) = 0;

			/**
			 * call stored function
			 * @param name defines function name
			 * @param arguments defines arguments
			 * @param as defines name of the result row
			 */
			virtual void callFunction(const dodoString &name, const dodoStringArray &arguments, const dodoString &as = __dodostring__) = 0;

			/**
			 * call stored procedure
			 * @param name is procedure name
			 * @param arguments is array of arguments
			 */
			virtual void callProcedure(const dodoString &name, const dodoStringArray &arguments) = 0;
			
			/**
			 * @param table defines table name
			 * @param fields defines names of fields
			 * @param where defines `where` statement
			 * @note if (length(table) == 0) => 'from `table`' is not used
			 */
			virtual void select(const dodoString &table, const dodoStringArray &fields, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param where defines `where` statement
			 */
			virtual void selectAll(const dodoString &table, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param fields defines hash of field=>value
			 */
			virtual void insert(const dodoString &table, const dodoStringMap &fields) = 0;

			/**
			 * @param table defines table name
			 * @param fields defines set of hashes of field=>value
			 */
			virtual void insert(const dodoString &table, const dodoArray<dodoStringMap> &fields) = 0;

			/**
			 * @param table defines table name
			 * @param values defines values
			 * @param fields defines names of fields
			 */
			virtual void insert(const dodoString &table, const dodoStringArray &values, const dodoStringArray &fields = __dodostringarray__) = 0;

			/**
			 * @param table defines table name
			 * @param values defines values
			 * @param fields defines names of fields
			 */
			virtual void insert(const dodoString &table, const dodoArray<dodoStringArray> &values, const dodoStringArray &fields = __dodostringarray__) = 0;

			/**
			 * @param tableTo defines table where data will be stored
			 * @param tableFrom defines table where data will be fetched
			 * @param fieldsTo defines fields where data will be stored
			 * @param fieldsFrom defines fields that will be fetched
			 */
			virtual void insertSelect(const dodoString &tableTo, const dodoString &tableFrom, const dodoStringArray &fieldsTo, const dodoStringArray &fieldsFrom = __dodostringarray__, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param fields defines hash of field=>value
			 * @param where defines `where` statement
			 */
			virtual void update(const dodoString &table, const dodoStringMap &fields, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param values defines values
			 * @param fields defines names of fields
			 * @param where defines `where` statement
			 */
			virtual void update(const dodoString &table, const dodoStringArray &values, const dodoStringArray &fields, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param where defines `where` statement
			 */
			virtual void del(const dodoString &table, const dodoString &where = __dodostring__) = 0;

			/**
			 * truncate table
			 * @param table is name name of table to truncate
			 */
			virtual void truncate(const dodoString &table) = 0;

			/**
			 * set `where` statement
			 * @param where defines `where` statement
			 * @note overwrites previous definition[from methods that may define it]
			 */
			virtual void where(const dodoString &where) = 0;

			/**
			 * set `limit` property
			 * @param number defines `limit` value
			 */
			virtual void limit(unsigned int number) = 0;

			/**
			 * set `offset` property
			 * @param number defines `offset` value
			 */
			virtual void offset(unsigned int number) = 0;

			/**
			 * set `order` property
			 * @param order defines `order` value
			 */
			virtual void order(const dodoString &order) = 0;

			/**
			 * set `group` property
			 * @param group defines `group` value
			 */
			virtual void group(const dodoString &group) = 0;

			/**
			 * set having property
			 * @param having defines having value
			 */
			virtual void having(const dodoString &having) = 0;

			/**
			 * append join statement
			 * @param table defines table to join
			 * @param condition defines condition for joining
			 * @param type defines join type[see dbBaseJoinTypeEnum]
			 */
			virtual void join(const dodoString &table, int type, const dodoString &condition) = 0;

			/**
			 * remove `where` statement
			 */
			virtual void unwhere() = 0;

			/**
			 * remove `limit` property
			 */
			virtual void unlimit() = 0;

			/**
			 * remove `offset` property
			 */
			virtual void unoffset() = 0;

			/**
			 * remove `order` property
			 */
			virtual void unorder() = 0;

			/**
			 * remove `group` property
			 */
			virtual void ungroup() = 0;

			/**
			 * remove `having` property
			 */
			virtual void unhaving() = 0;

			/**
			 * @return amount of affected rows
			 */
			virtual unsigned int affectedRowsCount() const = 0;

			/**
			 * @return amount of rows got from the request
			 */
			virtual unsigned int rowsCount() const = 0;

			/**
			 * @return amount of fields got from the request
			 */
			virtual unsigned int fieldsCount() const = 0;

			/**
			 * @return rows got from the request
			 */
			virtual dodoArray<dodoStringArray> fetchRow() const = 0;

			/**
			 * @return fields got from the request
			 */
			virtual dodoStringArray fetchField() const = 0;

			/**
			 * @return result got from the request
			 */
			virtual __dbStorage fetch() const = 0;

			/**
			 * @return set of hashes of field=>value
			 */
			virtual dodoStringMapArray fetchAssoc() const = 0;

			/**
			 * execute collected request
			 * @param query contains query for DB
			 * @param result defines type of result(true for quering data, false for data operation)
			 * @note if query is empty request will be constructed from the data that was defined using object's methods 
			 */
			virtual void exec(const dodoString &query = __dodostring__, bool result = false) = 0;

			bool reconnect; ///< if true tries to reconect in case when `exec` failed with connection error[true by default]

		protected:

			bool connected; ///< true if connected to the db

			dodoString pre_where;                          ///< `where` for the request(can be used as `as` for `callFunction`)
			dodoStringArray pre_fields;               ///< `fields` for request(can be used as `fieldsTo` for `insert_select`; as `arguments` for `callFunction`; as `arguments` for `callProcedure`; as `fields`/`field` `createIndex`)
			dodoArray<dodoStringArray> pre_values;      ///< `values` for the request(can be used as `fieldsFrom` for `insert_select`)
			dodoString pre_table;                          ///< `table` for the request(can be used `tableTo` for `insert_select`; as `name` for `callFunction`; as `name` for `callProcedure`)
			dodoString pre_tableTo;                        ///< `tableTo` for the request(can be used as `field` for `deleteField`/`renameField`)
			dodoString pre_order;                          ///< `order` for the request(can be used as `db` for `deleteField`/`renameField`/`deleteTable`/`renameTable`)
			dodoString pre_having;                         ///< `having` for the request(can be used as `charset` for 'createDb'; as `to_db` for `renameDb`; as `to_table` for `renameTable`; as `name` for `createIndex`; as `field` for `deleteIndex`; as `to_field` for `renameField`)
			dodoString pre_group;                          ///< `group` for the request
			dodoString pre_limit;                      ///< `limit` for the result
			dodoString pre_offset;                      ///< `offset` for the result
			dodoStringArray pre_subQueries;                ///< `subquery`
			dodoStringArray pre_joinTables;                ///< join tables
			dodoStringArray pre_joinConds;                 ///< join conditions
			dodoArray<int> pre_joinTypes;                  ///< join types

			int qType;                                     ///< type of operation

			int qShift;                                    ///< additional actions[see dbBaseAddEnum]

			int qSelShift;                                 ///< additional select statements[see dbBaseAddSelEnum]
			int qInsShift;                                 ///< additional insert statements[see dbBaseAddInsEnum]
			int qUpShift;                                  ///< additional update statements[see dbBaseAddUpEnum]
			int qDelShift;                                 ///< additional delete statements[see dbBaseAddDelEnum]

			__xexexDbBaseCollectedData collectedData;   ///< data collected for xexec

	};

};

#endif
