/***************************************************************************
 *            dbBase.h
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

#ifndef _DBBASE_H_
#define _DBBASE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/tools.h>
#include <libdodo/db.h>

namespace dodo
{
	/**
	 * @enum dbBaseAddEnum describes additional statement
	 */
	enum dbBaseAddEnum
	{
		DBBASE_ADDREQUEST_WHERE = 1,
		DBBASE_ADDREQUEST_HAVING,
		DBBASE_ADDREQUEST_GROUPBY,
		DBBASE_ADDREQUEST_ORDERBY,
		DBBASE_ADDREQUEST_LIMIT,
		DBBASE_ADDREQUEST_OFFSET,
		DBBASE_ADDREQUEST_AS,
		DBBASE_ADDREQUEST_JOIN,
	};

	/**
	 * @enum dbBaseRequestEnum indicates what type of operaion will be performed
	 */
	enum dbBaseRequestEnum
	{
		DBBASE_REQUEST_UNION = 1,
		DBBASE_REQUEST_UNION_ALL,
		DBBASE_REQUEST_MINUS,
		DBBASE_REQUEST_INTERSECT,

		DBBASE_REQUEST_SELECT,
		DBBASE_REQUEST_INSERT,
		DBBASE_REQUEST_INSERT_SELECT,
		DBBASE_REQUEST_UPDATE,
		DBBASE_REQUEST_DELETE,

		DBBASE_REQUEST_TRUNCATE,

		DBBASE_REQUEST_RENAME_DB,
		DBBASE_REQUEST_RENAME_TABLE,
		DBBASE_REQUEST_RENAME_FIELD,

		DBBASE_REQUEST_DELETE_DB,
		DBBASE_REQUEST_DELETE_TABLE,
		DBBASE_REQUEST_DELETE_FIELD,

		DBBASE_REQUEST_CREATE_DB,
		DBBASE_REQUEST_CREATE_TABLE,
		DBBASE_REQUEST_CREATE_FIELD,

		DBBASE_REQUEST_CREATE_INDEX,
		DBBASE_REQUEST_DELETE_INDEX,

		DBBASE_REQUEST_CALL_FUNCTION
	};

	/**
	 * @enum dbBaseAddSelEnum describes additional statements for select
	 */
	enum dbBaseAddSelEnum
	{
		DBBASE_ADDREQUEST_SELECT_DISTINCT = 1,
		DBBASE_ADDREQUEST_SELECT_ALL
	};

	/**
	 * @enum dbBaseAddDelEnum describes additional statements for delete
	 */
	enum dbBaseAddDelEnum
	{
		DBBASE_ADDREQUEST_DELETE_IGNORE = 1,
	};

	/**
	 * @enum dbBaseAddUpEnum describes additional statements for update
	 */
	enum dbBaseAddUpEnum
	{
		DBBASE_ADDREQUEST_UPDATE_IGNORE = 1,
	};

	/**
	 * @enum addInsEnum describes additional statements for insert
	 */
	enum dbBaseAddInsEnum
	{
		DBBASE_ADDREQUEST_INSERT_IGNORE = 1,
	};

	/**
	 * @class dbBase provides wide abilities for sql manipulations
	 * @note for xexec => you may use store/restore methods in hook if you want to leave data unchanged after it!
	 */
	class dbBase : public db
	{
		public:

			/*
			 * constructor
			 */
			dbBase();

			/**
			 * destructor
			 */
			virtual ~dbBase();

			/**
			 * next functions just modify internal data, but don't make a query. that's usefull to make hooks [preExec or postExec actions]
			 * in any time you can unset additional statement with callin' same function with `un` preffix (unlimit,..)
			 */

			/**
			 * sets info for database
			 * @param db is database name
			 * @param host is host, to connect to server
			 * @param port is port, to connect to server
			 * @param user is username, to connect to server
			 * @param password is password, to connect to server
			 * @param socket is path to unix socket
			 */
			virtual void setDbInfo(const dodoString &db, const dodoString &host, unsigned int port, const dodoString &user, const dodoString &password, const dodoString &path = __dodostring__);

			/**
			 * sets info for database
			 * @param info is info for database
			 */
			virtual void setDbInfo(const __dbInfo &dbInfo);

			/**
			 * @param name is function name
			 * @param arguments is array of arguments
			 * @param as is name of the result
			 */
			void callFunction(const dodoString &name, const dodoStringArray &arguments, const dodoString &as = __dodostring__);

			/**
			 * @param table is table name; if length(table)==0 => 'from `table`' doesn't use
			 * @param fields is array of fields' names
			 * @param where is where statement for request
			 */
			virtual void select(const dodoString &table, const dodoStringArray &fieldsNames, const dodoString &where = __dodostring__);

			/**
			 * @param table is table name; if length(table)==0 => 'from `table`' doesn't use
			 * @param where is where statement for request
			 */
			virtual void selectAll(const dodoString &table, const dodoString &where = __dodostring__);

			/**
			 * @param table is table name
			 * @param fields is array of fields' names
			 * 			key => field's name
			 * 			value => field's value
			 */
			virtual void insert(const dodoString &table, const dodoStringMap &fields);

			/**
			 * @param table is table name
			 * @param fields is array of (array of fields' names)
			 * 			key => field's name
			 * 			value => field's value
			 */
			virtual void insert(const dodoString &table, const dodoArray<dodoStringMap> &fields);

			/**
			 * @param table is table name
			 * @param fieldsVal is array of fields' values
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insert(const dodoString &table, const dodoStringArray &fieldsVal, const dodoStringArray &fieldsNames = __dodostringarray__);

			/**
			 * @param table is table name
			 * @param fieldsVal - array of array of (array of fields' values)
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insert(const dodoString &table, const dodoArray<dodoStringArray> &fieldsVal, const dodoStringArray &fieldsNames = __dodostringarray__);

			/**
			 * @param table(To/From) is table name To/From
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insertSelect(const dodoString &tableTo, const dodoString &tableFrom, const dodoStringArray &fieldsNamesTo, const dodoStringArray &fieldsNamesFrom = __dodostringarray__, const dodoString &where = __dodostring__);

			/**
			 * @param table is table name
			 * @param fields - array of (char*)/(string)
			 * 			key => field's name
			 * 			value => field's value
			 * @param where is where statement for request
			 */
			virtual void update(const dodoString &table, const dodoStringMap &fields, const dodoString &where = __dodostring__);

			/**
			 * @param table is table name
			 * @param fieldsVal is array of fields' values
			 * @param fieldsNames is array of fields' names
			 * @param where is where statement for request
			 */
			virtual void update(const dodoString &table, const dodoStringArray &fieldsVal, const dodoStringArray &fieldsNames, const dodoString &where = __dodostring__);

			/**
			 * @param table is table name
			 * @param where is where statement for request
			 */
			virtual void del(const dodoString &table, const dodoString &where = __dodostring__);

			/**
			 * store query, made from subquery with requested method
			 * @param sub is array of subqueries
			 * @param type is type of "subquering"[see qStEnum]
			 */
			virtual void subquery(const dodoStringArray &sub, int type = DBBASE_REQUEST_UNION /*DBBASE_REQUEST_UNION_ALL, DBBASE_REQUEST_MINUS, DBBASE_REQUEST_INTERSECT*/);

			/**
			 * create index in table
			 * @param table is where to create index
			 * @param field is field's name to be index
			 */
			virtual void createIndex(const dodoString &table, const dodoString &field, const dodoString &name);

			/**
			 * create index in table
			 * @param table is where to create index
			 * @param fields is fields' name to be index
			 */
			virtual void createIndex(const dodoString &table, const dodoStringArray &fields, const dodoString &name);

			/**
			 * delete index in table
			 * @param table is where to delete index
			 * @param name is index's name
			 */
			virtual void deleteIndex(const dodoString &table, const dodoString &name);

			/**
			 * rename database
			 * @param db is current name
			 * @param to_db is new name
			 */
			virtual void renameDb(const dodoString &db, const dodoString &to_db);

			/**
			 * renames table
			 * @param table is current name
			 * @param to_table is new name
			 */
			virtual void renameTable(const dodoString &table, const dodoString &to_table);

			/**
			 * renames field
			 * @param field is current name
			 * @param to_field is new name
			 * @param is table where field is stored
			 */
			virtual void renameField(const dodoString &field, const dodoString &to_field, const dodoString &table);

			/**
			 * deletes database
			 * @param db indicates what database to delete
			 */
			virtual void deleteDb(const dodoString &db);

			/**
			 * deletes table
			 * @param table indicates what table to delete
			 */
			virtual void deleteTable(const dodoString &table);

			/**
			 * deletes field
			 * @param field indicates what field to delete
			 * @param table is table where field is stored
			 */
			virtual void deleteField(const dodoString &field, const dodoString &table);

			/**
			 * creates database
			 * @param db is name of new database
			 * @param charset indicates what type of charset will be used
			 */
			virtual void createDb(const dodoString &db, const dodoString &charset = __dodostring__);

			/**
			 * creates table
			 * @param tableInfo[see __tableInfo]
			 */
			virtual void createTable(const __tableInfo &tableInfo);

			/**
			 * creates field
			 * @param fieldInfo[see __fieldInfo]
			 * @param table indicates in what table create field
			 */
			virtual void createField(const __fieldInfo &fieldInfo, const dodoString &table);

			/**
			 * truncates table
			 * @param table is name name of table to truncate
			 */
			virtual void truncate(const dodoString &table);

			/**
			 * add where statement
			 * overwites previous is it was defined
			 * @param where is where statement
			 */
			virtual void where(const dodoString &where);

			/**
			 * sets limit for request
			 * @param number indicates the number of rows to show
			 */
			virtual void limit(unsigned int number);

			/**
			 * sets offset for request
			 * @param number indicates offset of the reading result
			 */
			virtual void offset(unsigned int number);

			/**
			 * sets order for request
			 * @param order is order statement
			 */
			virtual void order(const dodoString &order);

			/**
			 * sets group for request
			 * @param group is group statement
			 */
			virtual void group(const dodoString &group);

			/**
			 * sets having for request
			 * @param having is having statement
			 */
			virtual void having(const dodoString &having);

			/**
			 * appends join for request
			 * @param table defines what table to join
			 * @param type defines join type[see dbBaseJoinTypeEnum]
			 * @param condition defines condition for joining
			 */
			virtual void join(const dodoString &table, int type, const dodoString &condition);

			/**
			 * remove where statement
			 */
			virtual void unwhere();

			/**
			 * remove limit for request
			 */
			virtual void unlimit();

			/**
			 * remove offset for request
			 */
			virtual void unoffset();

			/**
			 * remove order for request
			 */
			virtual void unorder();

			/**
			 * remove group for request
			 */
			virtual void ungroup();

			/**
			 * remove having for request
			 */
			virtual void unhaving();

			/**
			 * sets additional parameters for INSERT
			 * @param statement indicates what kind of additional parameters to add[see addInsEnum]
			 */
			virtual void setAddInsSt(unsigned int statement);

			/**
			 * sets additional parameters for UPDATE
			 * @param statement indicates what kind of additional parameters to add[see addUpEnum]
			 */
			virtual void setAddUpSt(unsigned int statement);

			/**
			 * sets additional parameters for SELECT
			 * @param statement indicates what kind of additional parameters to add[see addSelEnum]
			 */
			virtual void setAddSelSt(unsigned int statement);

			/**
			 * sets additional parameters for DELETE
			 * @param statement indicates what kind of additional parameters to add[see addDelEnum]
			 */
			virtual void setAddDelSt(unsigned int statement);

			/**
			 * removes additional parameters for INSERT
			 * @param statement indicates what kind of additional parameters to remove[see addInsEnum]
			 */
			virtual void unsetAddInsSt(unsigned int statement);

			/**
			 * removes additional parameters for UPDATE
			 * @param statement indicates what kind of additional parameters to remove[see addUpEnum]
			 */
			virtual void unsetAddUpSt(unsigned int statement);

			/**
			 * removes additional parameters for SELECT
			 * @param statement indicates what kind of additional parameters to remove[see addSelEnum]
			 */
			virtual void unsetAddSelSt(unsigned int statement);

			/**
			 * removes additional parameters for DELETE
			 * @param statement indicates what kind of additional parameters to remove[see addDelEnum]
			 */
			virtual void unsetAddDelSt(unsigned int statement);

			/**
			 * set default values for table (if you want safelly reuse)
			 */
			static void initTableInfo(__tableInfo &table);

			/**
			 * set default values for field (if you want safelly reuse)
			 */
			static void initFieldInfo(__fieldInfo &field);

		protected:

			/**
			 * frees collected data
			 */
			virtual void cleanCollected();

			bool show;                              ///< is request was with result(show, select)

			__tableInfo pre_tableInfo;              ///< info about table to create
			__fieldInfo pre_fieldInfo;              ///< info about field to create

			__dbInfo dbInfo;                        ///< data to connect to server

			/*
			 * additional statements for query, db-dependent, that can be implemented in derived class;
			 * these arrays take part after generalSQL statements !!! See (mysqlpp::addSQL()) for more details
			 * every in this class has 1 empty("") element
			 */
			dodoStringArray sqlDbDepAddSelArr;      ///< additional SELECT statement
			dodoStringArray sqlDbDepAddInsArr;      ///< additional INSERT statement
			dodoStringArray sqlDbDepAddUpArr;       ///< additional UPDATE statement
			dodoStringArray sqlDbDepAddDelArr;      ///< additional DELETE statement

			int qDbDepSelShift;                     ///< value to shift query template for specific
			int qDbDepInsShift;                     ///< value to shift query template for specific
			int qDbDepUpShift;                      ///< value to shift query template for specific
			int qDbDepDelShift;                     ///< value to shift query template for specific
	};

};

#endif
