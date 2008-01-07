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
	 * @enum dbEmptyEnum indicates that no action will be performed
	 */
	enum dbEmptyEnum
	{
		DB_EMPTY = 0
	};

	/**
	 * @struct __dbStorage contains data that could be retrieved from result [select...]
	 */
	struct __dbStorage
	{
		/**
		 * constructor
		 *
		 * @param rows
		 * @param fields
		 */
		__dbStorage(dodoArray<dodoStringArray> rows, dodoStringArray fields);

		dodoArray<dodoStringArray> rows;    ///< array of rows returned in request
		dodoStringArray fields;             ///< array of fields returned in request
	};

	/**
	 * @struct __xexexDbBaseCollectedData contains data that could be retrieved from class(to modificate)[contains references]
	 */
	struct __xexexDbBaseCollectedData
	{
		/**
		 * constructor
		 * initiates references
		 */
		__xexexDbBaseCollectedData(dodoString &pre_where,
								   dodoStringArray &pre_fieldsNames,
								   dodoArray<dodoStringArray> &pre_fieldsVal,
								   dodoString &pre_table,
								   dodoString &pre_tableTo,
								   dodoString &pre_order,
								   dodoString &pre_having,
								   dodoString &pre_group,
								   dodoString &pre_limNumber,
								   dodoString &pre_limOffset,
								   dodoStringArray &pre_subQ,
								   int &qType,
								   int &qShift,
								   int &qSelShift,
								   int &qInsShift,
								   int &qUpShift,
								   int &qDelShift,
								   int &operType,
								   void *executor);

		dodoString &pre_where;                          ///< where statement of the request
		dodoStringArray &pre_fieldsNames;               ///< names of fields of request;(can be used for `insert_select` as fields' names where to store result)
		dodoArray<dodoStringArray> &pre_fieldsVal;      ///< array of fields' values(accordingly to pre_fieldsNames). if simple action - contains 1 element(1 array of values); multiply array in case when multyply insert requested; (can be used for `insert_select` as fields' names from data requested)
		dodoString &pre_table;                          ///< table for request;(can be used for `insert_select` as table from what to take request); also can be used as 'table' for rename(delete)Field, rename(delete)Db, rename(delete)Table methods)
		dodoString &pre_tableTo;                        ///< string of table where to store request(insert_select)(also can be used as 'field' for rename(delete)Field method)
		dodoString &pre_order;                          ///< order statement(also can be used as 'db' for rename(delete)Field, rename(delete)Db, rename(delete)Table methods)
		dodoString &pre_having;                         ///< having statement(also can be used as ['charset' for db creation method] [table/field/database for rename methods])
		dodoString &pre_group;                          ///< group statement
		dodoString &pre_limNumber;                      ///< limit of result
		dodoString &pre_limOffset;                      ///< offset of requested result
		dodoStringArray &pre_subQ;                      ///< subquery

		int &qType;                                     ///< type of operation[see qStEnum]

		int &qShift;                                    ///< indicates if AddEnum's values was set [can be or'ed with | ][see AddEnum]

		int &qSelShift;                                 ///< additional select statements[see addSelEnum]
		int &qInsShift;                                 ///< additional insert statements[see addInsEnum]
		int &qUpShift;                                  ///< additional update statements[see addUpEnum]
		int &qDelShift;                                 ///< additional delete statements[see addDelEnum]

		int &operType;                                  ///< operation type set by main action; can be used in hook to determine type of action

		void *executor;                                 ///< class that executed hook
	};

	/**
	 * @struct __sqlInfo that contains connection optioins to server
	 */
	struct __dbInfo
	{

		/**
		 * constructor
		 */
		__dbInfo();

		/**
		 * constructor
		 * @note defines structure data with user data
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
	 * @class db is an interface to mysql db through sql-, database- independent interfaces;
	 * you should derive from it if you want to write 'plugin' for your database
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
			 * connect to database
			 */
			virtual void connect() = 0;

			/**
			 * disconnect from database
			 */
			virtual void disconnect() = 0;
			/**
			 * sets info for database
			 * @param db is database name
			 * @param host is host, to connect to server
			 * @param port is port, to connect to server
			 * @param user is username, to connect to server
			 * @param password is password, to connect to server
			 * @param socket is path to unix socket
			 */
			virtual void setDbInfo(const dodoString &db, const dodoString &host, unsigned int port, const dodoString &user, const dodoString &password, const dodoString &path = __dodostring__) = 0;

			/**
			 * sets info for database
			 * @param info is info for database
			 */
			virtual void setDbInfo(const __dbInfo &dbInfo) = 0;

			/**
			 * @param table is table name; if length(table)==0 => 'from `table`' doesn't use
			 * @param fields is array of fields' names
			 * @param where is where statement for request
			 */
			virtual void select(const dodoString &table, const dodoStringArray &fieldsNames, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table is table name
			 * @param fields is array of fields' names
			 * 			key => field's name
			 * 			value => field's value
			 */
			virtual void insert(const dodoString &table, const dodoStringMap &fields) = 0;

			/**
			 * @param table is table name
			 * @param fields is array of (array of fields' names)
			 * 			key => field's name
			 * 			value => field's value
			 */
			virtual void insert(const dodoString &table, const dodoArray<dodoStringMap> &fields) = 0;

			/**
			 * @param table is table name
			 * @param fieldsVal is array of fields' values
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insert(const dodoString &table, const dodoStringArray &fieldsVal, const dodoStringArray &fieldsNames = __dodostringarray__) = 0;

			/**
			 * @param table is table name
			 * @param fieldsVal - array of array of (array of fields' values)
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insert(const dodoString &table, const dodoArray<dodoStringArray> &fieldsVal, const dodoStringArray &fieldsNames = __dodostringarray__) = 0;

			/**
			 * @param table(To/From) is table name To/From
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insertSelect(const dodoString &tableTo, const dodoString &tableFrom, const dodoStringArray &fieldsNamesTo, const dodoStringArray &fieldsNamesFrom = __dodostringarray__, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table is table name
			 * @param fields - array of (char*)/(string)
			 * 			key => field's name
			 * 			value => field's value
			 * @param where is where statement for request
			 */
			virtual void update(const dodoString &table, const dodoStringMap &fields, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table is table name
			 * @param fieldsVal is array of fields' values
			 * @param fieldsNames is array of fields' names
			 * @param where is where statement for request
			 */
			virtual void update(const dodoString &table, const dodoStringArray &fieldsVal, const dodoStringArray &fieldsNames, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table is table name
			 * @param where is where statement for request
			 */
			virtual void del(const dodoString &table, const dodoString &where = __dodostring__) = 0;

			/**
			 * truncates table
			 * @param table is name name of table to truncate
			 */
			virtual void truncate(const dodoString &table) = 0;

			/**
			 * add where statement
			 * overwites previous is it was defined
			 * @param where is where statement
			 */
			virtual void where(const dodoString &where) = 0;

			/**
			 * sets limit for request
			 * @param number indicates the number of rows to show
			 */
			virtual void limit(unsigned int number) = 0;

			/**
			 * sets offset for request
			 * @param number indicates offset of the reading result
			 */
			virtual void offset(unsigned int number) = 0;

			/**
			 * sets order for request
			 * @param order is order statement
			 */
			virtual void order(const dodoString &order) = 0;

			/**
			 * sets group for request
			 * @param group is group statement
			 */
			virtual void group(const dodoString &group) = 0;

			/**
			 * sets having for request
			 * @param having is having statement
			 */
			virtual void having(const dodoString &having) = 0;

			/**
			 * remove where statement
			 */
			virtual void unwhere() = 0;

			/**
			 * remove limit for request
			 */
			virtual void unlimit() = 0;

			/**
			 * remove offset for request
			 */
			virtual void unoffset() = 0;

			/**
			 * remove order for request
			 */
			virtual void unorder() = 0;

			/**
			 * remove group for request
			 */
			virtual void ungroup() = 0;

			/**
			 * remove having for request
			 */
			virtual void unhaving() = 0;

			/**
			 * @return amount of affected rows(update, delete...)
			 */
			virtual unsigned int affectedRowsCount() const = 0;

			/**
			 * @return amount of rows got from request(select ...)
			 */
			virtual unsigned int rowsCount() const = 0;

			/**
			 * @return amount of fields got from request(select ...)
			 */
			virtual unsigned int fieldsCount() const = 0;

			/**
			 * @return array of rows got from request
			 */
			virtual dodoArray<dodoStringArray> fetchRow() const = 0;

			/**
			 * @return array of fields got from request
			 */
			virtual dodoStringArray fetchField() const = 0;

			/**
			 * @return structure that holds array of rows and array of fields got from request
			 */
			virtual __dbStorage fetch() const = 0;

			/**
			 * @return array that holds assoc array['fiels'=>'value'] got from request
			 */
			virtual dodoStringMapArray fetchAssoc() const = 0;

			/**
			 * executes collected request
			 * @param query contains query for DB. You may pass it if you don't use methods like select, update of libdodo
			 * @param result describes whether request returns result[show, select...] or not[delete, update]
			 */
			virtual void exec(const dodoString &query = __dodostring__, bool result = false) = 0;
			
			bool reconnect;///< tries to recconect if exec failed with connection error[true by default]

		protected:

			bool connected; ///< connected or not

			/**
			 * collected data
			 * some of variables can be used not only as they are named. Some variables can hold another data, to save space
			 */
			dodoString pre_where;                       ///< where statement of the request
			dodoStringArray pre_fieldsNames;            ///< names of fields of request;(can be used for `insert_select` as fields' names where to store result, as field(s) for createIndex )
			dodoArray<dodoStringArray> pre_fieldsVal;   ///< array of fields' values(accordingly to pre_fieldsNames). if simple action - contains 1 element(1 array of values); multiply array in case when multyply insert requested; (can be used for `insert_select` as fields' names from data requested)
			dodoString pre_table;                       ///< table for request;(can be used for `insert_select` as table from what to take request); also can be used as 'table' for rename(delete)Field, rename(delete)Db, rename(delete)Table methods, create(delete)Index methods)
			dodoString pre_tableTo;                     ///< string of table where to store request(insert_select)(also can be used as 'field' for rename(delete)Field method)
			dodoString pre_order;                       ///< order statement(also can be used as 'db' for rename(delete)Field, rename(delete)Db, rename(delete)Table methods)
			dodoString pre_having;                      ///< having statement(also can be used as ['charset' for db creation method] [table/field/database for rename methods], name for index in create(delete)Indexes)
			dodoString pre_group;                       ///< group statement
			dodoString pre_limNumber;                   ///< limit of result
			dodoString pre_limOffset;                   ///< offset of requested result
			dodoStringArray pre_subQ;                   ///< subquery

			int qType;                                  ///< type of operation

			int qShift;                                 ///< indicates if AddEnum's values was set [can be or'ed with | ]

			int qSelShift;                              ///< additional select statements
			int qInsShift;                              ///< additional insert statements
			int qUpShift;                               ///< additional update statements
			int qDelShift;                              ///< additional delete statements

			__xexexDbBaseCollectedData collectedData;   ///< data collected for xexec

	};

};

#endif
