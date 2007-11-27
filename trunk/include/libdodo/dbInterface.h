/***************************************************************************
 *            dbInterface.h
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

namespace dodo
{

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
		__dbStorage(dodoArray<dodoStringArr> rows, dodoStringArr fields);

		dodoArray<dodoStringArr> rows;  ///< array of rows returned in request
		dodoStringArr fields;           ///< array of fields returned in request
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
	 * @class dbInterface is an interface to mysql db through sql-, database- independent interfaces;
	 * you should derive from it if you want to write 'plugin' for your database
	 */
	class dbInterface
	{
		public:

			/**
			 *constructor
			 */
			dbInterface();

			/**
			 * destructor
			 */
			virtual ~dbInterface() = 0;

			/**
			 * connect to database
			 */
			virtual void
			connect() = 0;

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
			virtual void select(const dodoString &table, const dodoStringArr &fieldsNames, const dodoString &where = __dodostring__) = 0;

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
			virtual void insert(const dodoString &table, const dodoStringArr &fieldsVal, const dodoStringArr &fieldsNames = __dodostringarray__) = 0;

			/**
			 * @param table is table name
			 * @param fieldsVal - array of array of (array of fields' values)
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insert(const dodoString &table, const dodoArray<dodoStringArr> &fieldsVal, const dodoStringArr &fieldsNames = __dodostringarray__) = 0;

			/**
			 * @param table(To/From) is table name To/From
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insertSelect(const dodoString &tableTo, const dodoString &tableFrom, const dodoStringArr &fieldsNamesTo, const dodoStringArr &fieldsNamesFrom = __dodostringarray__, const dodoString &where = __dodostring__) = 0;

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
			virtual void update(const dodoString &table, const dodoStringArr &fieldsVal, const dodoStringArr &fieldsNames, const dodoString &where = __dodostring__) = 0;

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
			virtual dodoArray<dodoStringArr> fetchRow() const = 0;

			/**
			 * @return array of fields got from request
			 */
			virtual dodoStringArr fetchField() const = 0;

			/**
			 * @return structure that holds array of rows and array of fields got from request
			 */
			virtual __dbStorage fetch() const = 0;

			/**
			 * @return array that holds assoc array['fiels'=>'value'] got from request
			 */
			virtual dodoStringMapArr fetchAssoc() const = 0;

			/**
			 * executes collected request
			 * @param query contains query for DB. You may pass it if you don't use methods like select, update of libdodo
			 * @param result describes whether request returns result[show, select...] or not[delete, update]
			 */
			virtual void
			exec(const dodoString &query = __dodostring__, bool result = false) = 0;

		protected:

			bool connected; ///< connected or not

	};

};

#endif
