/***************************************************************************
 *            dbInterface.h
 *
 *  Sun Jan  15 19:45:19 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  aint with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _DBINTERFACE_H_
#define _DBINTERFACE_H_

#include <directives.h>
#include <types.h>

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
		__dbStorage(std::vector<stringArr> rows, stringArr fields);
		
		std::vector<stringArr> rows;///< array of rows returned in request
		stringArr fields;///< array of fields returned in request
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
		__dbInfo(const std::string &db, const std::string &host, const std::string &user, const std::string &password, const std::string &path, int port);
		
		std::string db;///< database name
		std::string host;///< hostname
		std::string user;///< username
		std::string password;///< password
		std::string path;///< path to db or unix socket
		unsigned int port;///< port
	};
	
	/**
 	 * @class dbInterface is an interface to mysql db through sql-,database- independent interfaces; 
 	 * you should derive from it if you want to write 'plugin' for your database
	 */
	class dbInterface
	{
		public:
		
			/**
			 * constructor
			 */
			dbInterface();
			
			/**
			 * destructor
			 */
			virtual ~dbInterface();
		
			/**
			 * connect to database
			 */	
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connect() const = 0;
			
			/**
			 * disconnect from database
			 */
			virtual void disconnect() const = 0;
			/**
			 * sets info for database
			 * @param db is database name
			 * @param host is host, to connect to server
			 * @param port is port, to connect to server
			 * @param user is username, to connect to server
			 * @param password is password, to connect to server
			 * @param socket is path to unix socket
			 */
			virtual void setDbInfo(const std::string &db, const std::string &host, unsigned int port, const std::string &user, const std::string &password, const std::string &path = __string__) const = 0;

			/**
			 * sets info for database
			 * @param info is info for database
			 */
			virtual void setDbInfo(const __dbInfo &dbInfo) const = 0;
			
			/**
			 * @param table is table name; if length(table)==0 => 'from `table`' doesn't use 
			 * @param fields is array of fields' names
			 * @param where is where statement for request
		     */
			virtual void select(const std::string &table, const stringArr &fieldsNames, const std::string &where = __string__) const = 0;
			
			/**
			 * @param table is table name
			 * @param fields is array of fields' names
			 * 			key => field's name
			 * 			value => field's value
		     */
			virtual void insert(const std::string &table, const assocArr &fields) = 0;
			
			/**
			 * @param table is table name
			 * @param fields is array of (array of fields' names)
			 * 			key => field's name
			 * 			value => field's value
		     */
			virtual void insert(const std::string &table, const std::vector<assocArr> &fields) = 0;
			
			/**
			 * @param table is table name
			 * @param fieldsVal is array of fields' values
			 * @param fieldsNames is array of fields' names
		     */
			virtual void insert(const std::string &table, const stringArr &fieldsVal, const stringArr &fieldsNames = __stringarray__) = 0;
			
			/**
			 * @param table is table name
			 * @param fieldsVal - array of array of (array of fields' values)
			 * @param fieldsNames is array of fields' names
		     */
			virtual void insert(const std::string &table, const std::vector<stringArr> &fieldsVal, const stringArr &fieldsNames = __stringarray__) = 0;			
			
			/**
			 * @param table(To/From) is table name To/From
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insertSelect(const std::string &tableTo, const std::string &tableFrom, const stringArr &fieldsNamesTo, const stringArr &fieldsNamesFrom = __stringarray__, const std::string &where = __string__) = 0;
			
			/**
			 * @param table is table name
			 * @param fields - array of (char*)/(string)
			 * 			key => field's name
			 * 			value => field's value
			 * @param where is where statement for request
		     */
			virtual void update(const std::string &table, const assocArr &fields, const std::string &where = __string__) = 0;			
			
			/**
			 * @param table is table name
			 * @param fieldsVal is array of fields' values
			 * @param fieldsNames is array of fields' names
			 * @param where is where statement for request
		     */
			virtual void update(const std::string &table, const stringArr &fieldsVal, const stringArr &fieldsNames, const std::string &where = __string__) = 0;
			
			/**
			 * @param table is table name
			 * @param where is where statement for request
			 */       
			virtual void del(const std::string &table, const std::string &where = __string__) = 0;
			
			/**
			 * truncates table
			 * @param table is name name of table to truncate
			 */
			 virtual void truncate(const std::string &table) = 0;
			 
			/**
			 * add where statement
			 * overwites previous is it was defined
			 * @param where is where statement
			 */
			virtual void where(const std::string &where) const = 0;
			
			/**
			 * sets limit for request
			 * @param number indicates the number of rows to show
			 */
			virtual void limit(unsigned int number) const = 0;
			
			/**
			 * sets offset for request
			 * @param number indicates offset of the reading result
			 */
			virtual void offset(unsigned int number) const = 0;		
				
			/**
			 * sets order for request
			 * @param order is order statement
			 */
			virtual void order(const std::string &order) const = 0;
			
			/**
			 * sets group for request
			 * @param group is group statement
			 */
			virtual void group(const std::string &group) const = 0;
			
			/**
			 * sets having for request
			 * @param having is having statement
			 */
			virtual void having(const std::string &having) const = 0;
			
			/**
			 * remove where statement
			 */
			virtual void unwhere() const = 0;
			
			/**
			 * remove limit for request
			 */
			virtual void unlimit() const = 0;
			
			/**
			 * remove offset for request
			 */
			virtual void unoffset() const = 0;		
				
			/**
			 * remove order for request
			 */
			virtual void unorder() const = 0;
			
			/**
			 * remove group for request
			 */
			virtual void ungroup() const = 0;
			
			/**
			 * remove having for request
			 */
			virtual void unhaving() const = 0;
						
			/**
			 * @return amount of affected rows(update,delete...)
			 */
			virtual unsigned int affectedRowsCount() = 0;
			
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
			virtual std::vector<stringArr> fetchRow() const = 0;
			
			/**
			 * @return array of fields got from request
			 */
			virtual stringArr fetchField() const = 0;
			
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
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							exec(const std::string &query = __string__, bool result = false) const = 0;
									
		protected:
		
			mutable bool connected;///< connected or not
		
	};
	
};

#endif
