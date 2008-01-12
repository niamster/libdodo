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
		 * @param rows
		 * @param fields
		 */
		__dbStorage(dodoArray<dodoStringArray> rows, dodoStringArray fields);
		
		/**
		 * constructor
		 */
		__dbStorage();

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

		dodoString &pre_where;                          ///< where statement of the request(can be used as `as` for callFunction)
		dodoStringArray &pre_fieldsNames;               ///< names of fields of request(can be used for `insert_select` as fields' names where to store result; as arguments for callFunction)
		dodoArray<dodoStringArray> &pre_fieldsVal;      ///< array of fields' values(accordingly to pre_fieldsNames). if simple action - contains 1 element(1 array of values); multiply array in case when multyply insert requested(can be used for `insert_select` as fields' names from data requested)
		dodoString &pre_table;                          ///< table for request;(can be used for `insert_select` as table from what to take request); as 'table' for rename(delete)Field, rename(delete)Db, rename(delete)Table methods; as name of function for callFunction)
		dodoString &pre_tableTo;                        ///< string of table where to store request(insert_select)(can be used as 'field' for rename(delete)Field method)
		dodoString &pre_order;                          ///< order statement(can be used as 'db' for rename(delete)Field, rename(delete)Db, rename(delete)Table methods)
		dodoString &pre_having;                         ///< having statement(can be used as ['charset' for db creation method] [table/field/database for rename methods])
		dodoString &pre_group;                          ///< group statement
		dodoString &pre_limNumber;                      ///< limit of result
		dodoString &pre_limOffset;                      ///< offset of requested result
		dodoStringArray &pre_subQueries;                ///< subquery
		dodoStringArray &pre_joinTables;                ///< join tables
		dodoStringArray &pre_joinConds;                 ///< join conditions
		dodoArray<int> &pre_joinTypes;                  ///< join types

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
	 * @enum dbJoinTypeEnum define join types
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
	 * @enum dbFieldTypeEnum defines Data types;
	 * @note with '**' need range
	 * with '*' may have range
	 */
	enum dbFieldTypeEnum
	{
		DB_FIELDTYPE_TINYINT,                           ///< *; The signed range is -128 to 127. The unsigned range is 0 to 255.
		DB_FIELDTYPE_SMALLINT,                          ///< *; The signed range is -32768 to 32767. The unsigned range is 0 to 65535.
		DB_FIELDTYPE_MEDIUMINT,                         ///< *; The signed range is -8388608 to 8388607. The unsigned range is 0 to 16777215.
		DB_FIELDTYPE_INTEGER, DB_FIELDTYPE_INT,     ///< *; The signed range is -2147483648 to 2147483647. The unsigned range is 0 to 4294967295.
		DB_FIELDTYPE_BIGINT,                            ///< *; The signed range is -9223372036854775808 to 9223372036854775807. The unsigned range is 0 to 18446744073709551615.
		DB_FIELDTYPE_FLOAT,                             ///< *; Allowable values are -3.402823466E+38 to -1.175494351E-38, 0, and 1.175494351E-38 to 3.402823466E+38.
		DB_FIELDTYPE_REAL, DB_FIELDTYPE_DOUBLE,     ///< *; [DOUBLE in some systems] Allowable values are -1.7976931348623157E+308 to -2.2250738585072014E-308, 0, and 2.2250738585072014E-308 to 1.7976931348623157E+308.
		DB_FIELDTYPE_DECIMAL,                           ///< **; An unpacked(the number is stored as a string) fixed-point number.
		DB_FIELDTYPE_DATE,                              ///< The supported range is '1000-01-01' to '9999-12-31'.
		DB_FIELDTYPE_TIME,                              ///< The range is '-838:59:59' to '838:59:59'
		DB_FIELDTYPE_TIMESTAMP,                         ///< *; The range is '1970-01-01 00:00:00' to partway through the year 2037. The first TIMESTAMP column in a table is automatically set to the date and time of the most recent operation if you don't assign it a value yourself.
		/**
		 * TIMESTAMP(14) 	YYYYMMDDHHMMSS
		 * TIMESTAMP(12) 	YYMMDDHHMMSS
		 * TIMESTAMP(10) 	YYMMDDHHMM
		 * TIMESTAMP(8) 		YYYYMMDD
		 * TIMESTAMP(6) 		YYMMDD
		 * TIMESTAMP(4) 		YYMM
		 * TIMESTAMP(2) 		YY
		 */
		DB_FIELDTYPE_CHAR,                              ///< **; The range of M is 0 to 255 characters; A fixed-length string that is always right-padded with spaces to the specified length when stored.
		DB_FIELDTYPE_VARCHAR,                           ///< **; The range of M is 0 to 255 characters. A variable-length string. Range represents the maximum column length.
		DB_FIELDTYPE_TINYBLOB,                          ///< A column with a maximum length of 255 (2^8 - 1) characters.
		DB_FIELDTYPE_BLOB,                              ///< A column with a maximum length of 65, 535 (2^16 -1) characters.
		DB_FIELDTYPE_MEDIUMBLOB,                        ///< A column with a maximum length of 16, 777, 215 (2^24 - 1) characters.
		DB_FIELDTYPE_LONGBLOB,                          ///< A column with a maximum length of 4, 294, 967, 295 or 4GB (2^32 - 1) characters.
		DB_FIELDTYPE_TINYTEXT,                          ///< A column with a maximum length of 255 (2^8 - 1) characters.
		DB_FIELDTYPE_TEXT,                              ///< A column with a maximum length of 65, 535 (2^16 -1) characters.
		DB_FIELDTYPE_MEDIUMTEXT,                        ///< A column with a maximum length of 16, 777, 215 (2^24 - 1) characters.
		DB_FIELDTYPE_LONGTEXT,                          ///< A column with a maximum length of 4, 294, 967, 295 or 4GB (2^32 - 1) characters.
		DB_FIELDTYPE_ENUM,                              ///< An enumeration. A string object that can have only one value, chosen from the list of values 'value1', 'value2', ..., NULL or the special '' error value. An column can have a maximum of 65, 535 distinct values.
		DB_FIELDTYPE_SET,                               ///< A string object that can have zero or more values, each of which must be chosen from the list of values 'value1', 'value2', ... A column can have a maximum of 64 members.
	};
	
	/**
	 *  @enum dbFieldFlagEnum describes type of field [in field creation]
	 */
	enum dbFieldFlagEnum
	{
		DB_FIELDFLAG_NULL = 2,              ///< NULL type
		DB_FIELDFLAG_AUTO_INCREMENT = 4,    ///< if is not set by request, will be incremented relatevly to previous
	};

	/**
	 * @enum dbReferenceEnum describes reference type on field [in field creation]
	 */
	enum dbReferenceEnum
	{
		DB_REFERENCE_RESTRICT = 1,  ///< does not allow the action of any of those parent rows
		DB_REFERENCE_CASCADE,       ///< a row in the parent table is deleted, automatically deletes also all those rows in the child table whose foreign key values are equal to the referenced key value in the parent row
		DB_REFERENCE_SET_NULL,      ///< sets NULL on the action of any of those parent rows indicates on set action
		DB_REFERENCE_NO_ACTION,     ///< noacton on the action of any of those parent rows indicates on set action
		DB_REFERENCE_SET_DEFAULT    ///< sets default on the action of any of those parent rows indicates on set action
	};

	/**
	 * @struct __fieldInfo contains info for field creation
	 */
	struct __fieldInfo
	{
		/**
		 * constructor
		 */
		__fieldInfo();

		/**
		 * overloaded operator '='
		 */
		const __fieldInfo &operator=(const __fieldInfo &from);

		dodoString name;    ///< name of the field
		int type;           ///< type of field[see dbFieldFlagEnum]
		int length;         ///< length of field; is valuable for all except [DATE, TIME, *TEXT, *BLOB, SET, ENUM] => for those will be ignored
		int flag;           ///< default=NULL; set it with '|'[see dbFieldFlagEnum]

		/**
		 * @note for reference: set flag with (MATCH FULL or MATCH PARTIAL or MATCH SIMPLE); ON DELETE 'ref'; ON UPDATE 'ref';
		 * for [ON DELETE or ON UPDATE] use on flag (RESTRICT or CASCADE or SET NULL or NO ACTION or SET DEFAULT)
		 */
		dodoString refTable;            ///< table on what is reference
		dodoStringArray refFields;      ///< array of fields on what is references
		int onDelete;                   ///< reference on action[see dbReferenceEnum]
		int onUpdate;                   ///< reference on action[see dbReferenceEnum]

		dodoString defaultVal;          ///< default value of field
		dodoStringArray set_enum;       ///< array of statements for SET or ENUM if these type difined

		dodoString charset;             ///< field's collate charset
	};

	/*
	 * @struct __tableInfo contains info for table creation
	 */
	struct __tableInfo
	{
		/**
		 * constructor
		 */
		__tableInfo();

		/**
		 * overloaded operator '='
		 */
		const __tableInfo &operator=(const __tableInfo &from);

		dodoString name;                    ///< name of the table
		dodoArray<__fieldInfo> fields;      ///< array of fields[see __fieldInfo]

		dodoStringArray primKeys;           ///< array of primary keys in table (field names)
		dodoStringArray uniq;               ///< array of unique in table (field names)

		bool ifNotExists;                   ///< no warning message if table already exixts [false by default]
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
			 * @param name is function name
			 * @param arguments is array of arguments
			 * @param as is name of the result
			 */
			virtual void callFunction(const dodoString &name, const dodoStringArray &arguments, const dodoString &as = __dodostring__) = 0;

			/**
			 * @param table is table name; if length(table)==0 => 'from `table`' doesn't use
			 * @param fields is array of fields' names
			 * @param where is where statement for request
			 */
			virtual void select(const dodoString &table, const dodoStringArray &fieldsNames, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table is table name; if length(table)==0 => 'from `table`' doesn't use
			 * @param where is where statement for request
			 */
			virtual void selectAll(const dodoString &table, const dodoString &where = __dodostring__) = 0;

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
			 * appends join for request
			 * @param table defines what table to join
			 * @param condition defines condition for joining
			 * @param type defines join type[see dbBaseJoinTypeEnum]
			 */
			virtual void join(const dodoString &table, int type, const dodoString &condition) = 0;

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

			bool reconnect; ///< tries to recconect if exec failed with connection error[true by default]

		protected:

			bool connected; ///< connected or not

			/**
			 * collected data
			 * some of variables can be used not only as they are named. Some variables can hold another data, to save space
			 */
			dodoString pre_where;                       ///< where statement of the request(can be used as `as` for callFunction)
			dodoStringArray pre_fieldsNames;            ///< names of fields of request(can be used for `insert_select` as fields' names where to store result, as field(s) for createIndex; as arguments for callFunction)
			dodoArray<dodoStringArray> pre_fieldsVal;   ///< array of fields' values(accordingly to pre_fieldsNames). if simple action - contains 1 element(1 array of values); multiply array in case when multyply insert requested; (can be used for `insert_select` as fields' names from data requested)
			dodoString pre_table;                       ///< table for request;(can be used for `insert_select` as table from what to take request); can be used as 'table' for rename(delete)Field, rename(delete)Db, rename(delete)Table methods, create(delete)Index methods; as name for callFunction)
			dodoString pre_tableTo;                     ///< string of table where to store request(insert_select)(can be used as 'field' for rename(delete)Field method)
			dodoString pre_order;                       ///< order statement(can be used as 'db' for rename(delete)Field, rename(delete)Db, rename(delete)Table methods)
			dodoString pre_having;                      ///< having statement(can be used as ['charset' for db creation method] [table/field/database for rename methods], name for index in create(delete)Indexes)
			dodoString pre_group;                       ///< group statement
			dodoString pre_limNumber;                   ///< limit of result
			dodoString pre_limOffset;                   ///< offset of requested result
			dodoStringArray pre_subQueries;             ///< subquery
			dodoStringArray pre_joinTables;            ///< join tables
			dodoStringArray pre_joinConds;             ///< join conditions
			dodoArray<int> pre_joinTypes;              ///< join types

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
