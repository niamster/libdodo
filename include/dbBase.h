/***************************************************************************
 *            dbBase.h
 *
 *  Thu Aug 25 21:39:24 2005
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _DBBASE_H_
#define _DBBASE_H_

#include <directives.h>

#include <types.h>
#include <dbSqlBaseEx.h>
#include <tools.h>

namespace dodo
{	
	/**
	 * @enum AddEnum describes additional statement
	 */
	enum AddEnum
	{
		WHERE = 1,
		HAVING,
		GROUPBY,
		ORDERBY,
		LIMIT,
		OFFSET,
	};	

	/**
	 * @enum Empty indicates that no action will be performed
	 */	
	enum Empty
	{
		EMPTY = 0
	};	
	
	/**
	 * @enum qStEnum indicates what type of operaion will be performed
	 */
	enum qStEnum
	{
		UNION,
		UNION_ALL,
		MINUS,
		INTERSECT,
		
		SELECT,
		INSERT,
		INSERT_SELECT,
		UPDATE,
		DELETE,
		
		USE,
		
		TRUNCATE,
		
		RENAME_DB,
		RENAME_TABLE,
		RENAME_FIELD,
		
		DELETE_DB,
		DELETE_TABLE,
		DELETE_FIELD,
		
		CREATE_DB,
		CREATE_TABLE,
		CREATE_FIELD		
	};
	
	/**
	 * @enum addSelEnum describes additional statements for select
	 */
	enum addSelEnum
	{
		SELECT_DISTINCT = 1,
		SELECT_ALL
	};

	/**
	 * @enum addDelEnum describes additional statements for delete
	 */	
	enum addDelEnum
	{
		DELETE_IGNORE = 1,
	};

	/**
	 * @enum addUpEnum describes additional statements for update
	 */	
	enum addUpEnum
	{
		UPDATE_IGNORE = 1,
	};

	/**
	 * @enum addInsEnum describes additional statements for insert
	 */	
	enum addInsEnum
	{
		INSERT_IGNORE = 1,
	};
	
	/**
	 * @struct __sqlStorage contains data that could be retrieved from result [select...]
	 */
	struct __sqlStorage
	{
		/**
		 * constructor
		 * 
		 * @param rows
		 * @param fields
		 */
		__sqlStorage(std::vector<stringArr> rows, stringArr fields);
		
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
	 * @struct __collectedData contains data that could be retrieved from class(to modyficate)[contains references]
	 * @note u can access to data directly from class, but this is more comfortable
	 */
	struct __collectedData
	{
		/**
		 * constructor
		 * initiates references
		 */
		__collectedData(std::string &pre_where,
		stringArr &pre_fieldsNames,
		std::vector<stringArr> &pre_fieldsVal,
		std::string &pre_table,
		std::string &pre_tableTo,
		std::string &pre_order,
		std::string &pre_having,
		std::string &pre_group,
		std::string &pre_limNumber,
		std::string &pre_limOffset,
		stringArr &pre_subQ,
		int &qType,
		int &qShift,
		int &qSelShift,
		int &qInsShift,
		int &qUpShift,
		int &qDelShift		
		);
		
		std::string &pre_where;///< where statement of the request	
		stringArr &pre_fieldsNames;///< names of fields of request;(can be used for `insert_select` as fields' names where to store result)
		std::vector<stringArr> &pre_fieldsVal;///< array of fields' values(accordingly to pre_fieldsNames). if simple action - contains 1 element(1 array of values); multiply array in case when multyply insert requested; (can be used for `insert_select` as fields' names from data requested)
		std::string &pre_table;///< table for request;(can be used for `insert_select` as table from what to take request); also can be used as 'table' for rename(delete)Field,rename(delete)Db,rename(delete)Table methods)
		std::string &pre_tableTo;///< string of table where to store request(insert_select)(also can be used as 'field' for rename(delete)Field method)
		std::string &pre_order;///< order statement(also can be used as 'db' for rename(delete)Field,rename(delete)Db,rename(delete)Table methods)
		std::string &pre_having;///< having statement(also can be used as ['charset' for db creation method] [table/field/database for rename methods])
		std::string &pre_group;///< group statement
		std::string &pre_limNumber;///< limit of result
		std::string &pre_limOffset;///< offset of requested result
		stringArr &pre_subQ;///< subquery
		
		int &qType;///< type of operation
		
		int &qShift;///< indicates if AddEnum's values was set [can be or'ed with | ]
		
		int &qSelShift;///< additional select statements
		int &qInsShift;///< additional insert statements
		int &qUpShift;///< additional update statements
		int &qDelShift;///< additional delete statements		
	};
		
	/**
	 * @enum baseDataTypesEnum defines Data types; 
	 * with '**' need range; with '*' may have range
	 */
	enum baseDataTypesEnum
	{
		TINYINT,///< *; The signed range is -128 to 127. The unsigned range is 0 to 255.
		SMALLINT,///< *; The signed range is -32768 to 32767. The unsigned range is 0 to 65535.
		MEDIUMINT,///< *; The signed range is -8388608 to 8388607. The unsigned range is 0 to 16777215.
		INTEGER, INT,///< *; The signed range is -2147483648 to 2147483647. The unsigned range is 0 to 4294967295.
		BIGINT,///< *; The signed range is -9223372036854775808 to 9223372036854775807. The unsigned range is 0 to 18446744073709551615.
		FLOAT,///< *; Allowable values are -3.402823466E+38 to -1.175494351E-38, 0, and 1.175494351E-38 to 3.402823466E+38.
		REAL, DOUBLE,///< *; [DOUBLE in some systems] Allowable values are -1.7976931348623157E+308 to -2.2250738585072014E-308, 0, and 2.2250738585072014E-308 to 1.7976931348623157E+308.
		DECIMAL,///< **; An unpacked(the number is stored as a string) fixed-point number. 
		DATE,///< The supported range is '1000-01-01' to '9999-12-31'.
		TIME,///< The range is '-838:59:59' to '838:59:59'
		TIMESTAMP,///< *; The range is '1970-01-01 00:00:00' to partway through the year 2037. The first TIMESTAMP column in a table is automatically set to the date and time of the most recent operation if you don't assign it a value yourself.
		/**
		 * TIMESTAMP(14) 	YYYYMMDDHHMMSS
		 * TIMESTAMP(12) 	YYMMDDHHMMSS
		 * TIMESTAMP(10) 	YYMMDDHHMM
		 * TIMESTAMP(8) 		YYYYMMDD
		 * TIMESTAMP(6) 		YYMMDD
		 * TIMESTAMP(4) 		YYMM
		 * TIMESTAMP(2) 		YY
		 */
		CHAR,///< **; The range of M is 0 to 255 characters; A fixed-length string that is always right-padded with spaces to the specified length when stored.
		VARCHAR,///< **; The range of M is 0 to 255 characters. A variable-length string. Range represents the maximum column length.
		TINYBLOB,///< A column with a maximum length of 255 (2^8 - 1) characters.
		BLOB,///< A column with a maximum length of 65,535 (2^16 -1) characters.
		MEDIUMBLOB,///< A column with a maximum length of 16,777,215 (2^24 - 1) characters. 
		LONGBLOB,///< A column with a maximum length of 4,294,967,295 or 4GB (2^32 - 1) characters.
		ENUM,///< An enumeration. A string object that can have only one value, chosen from the list of values 'value1', 'value2', ..., NULL or the special '' error value. An column can have a maximum of 65,535 distinct values.
		SET,///< A string object that can have zero or more values, each of which must be chosen from the list of values 'value1', 'value2', ... A column can have a maximum of 64 members.
	};
	
	/**
	 *  @enum fieldFlagEnum describes type of field [in field creation]
	 */
	enum fieldFlagEnum
	{
		_NULL = 1,///< NULL type
		AUTO_INCREMENT,///< if is not set by request, will be incremented relatevly to previous
		KEY///< a key(depends on DB, but possibly index)
	};
	
	/**
	 * enum refFieldEnum describes reference type on field [in field creation]
	 */
	enum refFieldEnum
	{
		RESTRICT = 0,///< does not allow the action of any of those parent rows
		CASCADE,///< a row in the parent table is deleted, automatically deletes also all those rows in the child table whose foreign key values are equal to the referenced key value in the parent row
		SET_NULL,///< sets NULL on the action of any of those parent rows indicates on set action
		NO_ACTION,///< noacton on the action of any of those parent rows indicates on set action
		SET_DEFAULT///< sets default on the action of any of those parent rows indicates on set action
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
		const __fieldInfo &operator=(__fieldInfo &from);

		std::string name;///< name of the field
		int type;///< type of field; see baseDataTypesEnum
		int length;///< length of field; is valuable for all except [DATE, TIME, *TEXT, *BLOB, SET, ENUM] => for those will be ignored
		int flag;///< default = NULL; set it with '|'; see fieldFlagEnum
		
		/**
		* @note for reference: set flag with (MATCH FULL or MATCH PARTIAL or MATCH SIMPLE); ON DELETE 'ref'; ON UPDATE 'ref';
		* for [ON DELETE or ON UPDATE] use on flag (RESTRICT or CASCADE or SET NULL or NO ACTION or SET DEFAULT)
		*/
		std::string refTable;///< table on what is reference
		stringArr refFields;///< array of fields on what is references
		int onDelete;///< reference on action; see refFieldEnum
		int onUpdate;///< reference on action; see refFieldEnum

		std::string defaultVal;///< default value of field
		stringArr set_enum;///< array of statements for SET or ENUM if these type difined
		
		std::string comment;///< comment string for field
		std::string charset;///< field's charset
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
		const __tableInfo &operator=(__tableInfo &from);
			
		std::string name;///< name of the table
		std::vector<__fieldInfo> fields;///< array of fields; see__fieldInfo
		
		stringArr keys;///< array of keys in table (field names)
		stringArr primKeys;///< array of primary keys in table (field names)
		stringArr uniq;///< array of unique in table (field names)
		
		long autoIncr;///< auto-increment value (as for start)
		long avgRowLen;///< avarage row length
		
		std::string comment;///< comment string for table
		std::string charset;///< table's charset
	};
	
	/**
	 * @class dbBase provides wide abilities for sql manipulations
	 * 
	 * @note for xexec => you may use store/restore methods in hook if you want to leave data unchanged after it!
	 */
	class dbBase
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
			virtual void setDbInfo(const std::string &db, const std::string &host, unsigned int port, const std::string &user, const std::string &password, const std::string &path = __string__) const;

			/**
			 * sets info for database
			 * @param info is info for database
			 */
			virtual void setDbInfo(const __dbInfo &dbInfo) const;
			
			/**
			 * @param table is table name; if length(table)==0 => 'from `table`' doesn't use 
			 * @param fields is array of fields' names
			 * @param where is where statement for request
		     */
			virtual void select(const std::string &table, const stringArr &fieldsNames, const std::string &where = __string__) const;
			
			/**
			 * @param table is table name
			 * @param fields is array of fields' names
			 * 			key => field's name
			 * 			value => field's value
		     */
			virtual void insert(const std::string &table, const assocArr &fields);
			
			/**
			 * @param table is table name
			 * @param fields is array of (array of fields' names)
			 * 			key => field's name
			 * 			value => field's value
		     */
			virtual void insert(const std::string &table, const std::vector<assocArr> &fields);
			
			/**
			 * @param table is table name
			 * @param fieldsVal is array of fields' values
			 * @param fieldsNames is array of fields' names
		     */
			virtual void insert(const std::string &table, const stringArr &fieldsVal, const stringArr &fieldsNames = __stringarray__);
			
			/**
			 * @param table is table name
			 * @param fieldsVal - array of array of (array of fields' values)
			 * @param fieldsNames is array of fields' names
		     */
			virtual void insert(const std::string &table, const std::vector<stringArr> &fieldsVal, const stringArr &fieldsNames = __stringarray__);			
			
			/**
			 * @param table(To/From) is table name To/From
			 * @param fieldsNames is array of fields' names
			 */
			virtual void insertSelect(const std::string &tableTo, const std::string &tableFrom, const stringArr &fieldsNamesTo, const stringArr &fieldsNamesFrom = __stringarray__, const std::string &where = __string__);
			
			/**
			 * @param table is table name
			 * @param fields - array of (char*)/(string)
			 * 			key => field's name
			 * 			value => field's value
			 * @param where is where statement for request
		     */
			virtual void update(const std::string &table, const assocArr &fields, const std::string &where = __string__);			
			
			/**
			 * @param table is table name
			 * @param fieldsVal is array of fields' values
			 * @param fieldsNames is array of fields' names
			 * @param where is where statement for request
		     */
			virtual void update(const std::string &table, const stringArr &fieldsVal, const stringArr &fieldsNames, const std::string &where = __string__);
			
			/**
			 * @param table is table name
			 * @param where is where statement for request
			 */       
			virtual void del(const std::string &table, const std::string &where = __string__);
			
			/**
			 * store query, made from subquery with requested method
			 * @param sub is array of subqueries
			 * @param type is type of "subquering"
			 */
			virtual void subquery(const stringArr &sub, int type=UNION/*UNION_ALL,MINUS,INTERSECT*/);
			
			/**
			 * change database
			 * @param db is name of database where to change
			 */
			virtual void use(const std::string &db);
			
			/**
			 * rename database
			 * @param db is current name
			 * @param to_db is new name
			 */
			virtual void renameDb(const std::string &db, const std::string &to_db);
			
			/**
			 * renames table
			 * @param table is current name
			 * @param to_table is new name
			 */
			virtual void renameTable(const std::string &table, const std::string &to_table);
			
			/**
			 * renames field
			 * @param field is current name
			 * @param to_field is new name
			 * @param is table where field is stored
			 */ 
			virtual void renameField(const std::string &field, const std::string &to_field, const std::string &table);
			
			/**
			 * deletes database
			 * @param db indicates what database to delete
			 */
			virtual void deleteDb(const std::string &db);
			
			/**
			 * deletes table
			 * @param table indicates what table to delete
			 */
			virtual void deleteTable(const std::string &table);
			
			/**
			 * deletes field
			 * @param field indicates what field to delete
			 * @param table is table where field is stored
			 */ 
			virtual void deleteField(const std::string &field, const std::string &table);
			
			/**
			 * creates database
			 * @param db is name of new database
			 * @param charset indicates what type of charset will be used
			 */
			virtual void createDb(const std::string &db, const std::string &charset = __string__);
			
			/**
			 * creates table
			 * @param tableInfo => see __tableInfo
			 */
			virtual void createTable(__tableInfo &tableInfo);
			
			/**
			 * creates field
			 * @param fieldInfo => see __fieldInfo
			 * @param table indicates in what table create field
			 */ 
			virtual void createField(__fieldInfo &fieldInfo, const std::string &table);						
			
			/**
			 * truncates table
			 * @param table is name name of table to truncate
			 */
			 virtual void truncate(const std::string &table);
			 
			/**
			 * add where statement
			 * overwites previous is it was defined
			 * @param where is where statement
			 */
			virtual void where(const std::string &where) const;
			
			/**
			 * sets limit for request
			 * @param number indicates the number of rows to show
			 */
			virtual void limit(unsigned int number) const;
			
			/**
			 * sets offset for request
			 * @param number indicates offset of the reading result
			 */
			virtual void offset(unsigned int number) const;		
				
			/**
			 * sets order for request
			 * @param order is order statement
			 */
			virtual void order(const std::string &order) const;
			
			/**
			 * sets group for request
			 * @param group is group statement
			 */
			virtual void group(const std::string &group) const;
			
			/**
			 * sets having for request
			 * @param having is having statement
			 */
			virtual void having(const std::string &having) const;
			
			/**
			 * remove where statement
			 */
			virtual void unwhere() const;
			
			/**
			 * remove limit for request
			 */
			virtual void unlimit() const;
			
			/**
			 * remove offset for request
			 */
			virtual void unoffset() const;		
				
			/**
			 * remove order for request
			 */
			virtual void unorder() const;
			
			/**
			 * remove group for request
			 */
			virtual void ungroup() const;
			
			/**
			 * remove having for request
			 */
			virtual void unhaving() const;		
			
			/**
			 * sest additional parameters for INSERT
			 */
			virtual void setAddInsSt(unsigned int statement);
			
			/**
			 * sets additional parameters for UPDATE
			 */
			virtual void setAddUpSt(unsigned int statement);
			
			/**
			 * sets additional parameters for SELECT
			 */
			virtual void setAddSelSt(unsigned int statement);
			
			/**
			 * sets additional parameters for DELETE
			 */
			virtual void setAddDelSt(unsigned int statement);
			
			/**
			 * removes additional parameters for INSERT
			 */
			virtual void unsetAddInsSt(unsigned int statement);
			
			/**
			 * removes additional parameters for UPDATE
			 */
			virtual void unsetAddUpSt(unsigned int statement);
			
			/**
			 * removes additional parameters for SELECT
			 */
			virtual void unsetAddSelSt(unsigned int statement);
			
			/**
			 * removes additional parameters for DELETE
			 */
			virtual void unsetAddDelSt(unsigned int statement);
			
			/**
			 * @return type of request was(will be) performed
			 */
			virtual int getQType() const;
			
			/**
			 * @return structure with references to internal collected request data(not copy).
			 * @note it may be used for postExec and preExec functions.
			 */
			virtual __collectedData collectedData();
			
			/**
			 * set default values for table (if you want safelly reuse)
			 */
			inline void initTableInfo(__tableInfo &table);
			
			/**
			 * set default values for field (if you want safelly reuse)
			 */
			inline void initRowInfo(__fieldInfo &field);	
					
		protected:
		
			/**
			 * @return sql compliant data type
			 * @param type indicates the data type
			 */
			inline virtual std::string stringType(int type) const;
			
			/**
			 * @return : if type must have range = 1; if may have = 0; if mustn't have = -1;
			 * @param type indicates the data type
			 */
			inline virtual int chkRange(int type) const;
			
			/**
			 * @return sql compliant references statements
			 * @param type indicates the type of reference
			 */
			inline virtual std::string stringReference(int type) const;
			
			/**
			 * frees collected data
			 */
			virtual void cleanCollect() const;
			
			/**
			 * collected data
			 * some of variables can be used not only as they are named. Some variables can hold another data, to save space
			 */
			mutable std::string pre_where;///< where statement of the request	
			mutable stringArr pre_fieldsNames;///< names of fields of request;(can be used for `insert_select` as fields' names where to store result)
			mutable std::vector<stringArr> pre_fieldsVal;///< array of fields' values(accordingly to pre_fieldsNames). if simple action - contains 1 element(1 array of values); multiply array in case when multyply insert requested; (can be used for `insert_select` as fields' names from data requested)
			mutable std::string pre_table;///< table for request;(can be used for `insert_select` as table from what to take request); also can be used as 'table' for rename(delete)Field,rename(delete)Db,rename(delete)Table methods)
			mutable std::string pre_tableTo;///< string of table where to store request(insert_select)(also can be used as 'field' for rename(delete)Field method)
			mutable std::string pre_order;///< order statement(also can be used as 'db' for rename(delete)Field,rename(delete)Db,rename(delete)Table methods)
			mutable std::string pre_having;///< having statement(also can be used as ['charset' for db creation method] [table/field/database for rename methods])
			mutable std::string pre_group;///< group statement
			mutable std::string pre_limNumber;///< limit of result
			mutable std::string pre_limOffset;///< offset of requested result
			mutable stringArr pre_subQ;///< subquery
			
			mutable bool show;///< is request was with result(show,select)
						
			mutable int qType;///< type of operation
			
			mutable int qShift;///< indicates if AddEnum's values was set [can be or'ed with | ]
			
			mutable int qSelShift;///< additional select statements
			mutable int qInsShift;///< additional insert statements
			mutable int qUpShift;///< additional update statements
			mutable int qDelShift;///< additional delete statements					 
			 
			 
			mutable __tableInfo pre_tableInfo;///< info about table to create
			mutable __fieldInfo pre_fieldInfo;///< info about field to create
			
			mutable __dbInfo dbInfo;///< data to connect to server
			
			/*
			 * additional statements for query, db-dependent, that can be implemented in derived class; 
			 * these arrays take part after generalSQL statements !!! See (mysqlpp::addSQL()) for more details
			 * every in this class has 1 empty("") element
			 */
			mutable stringArr sqlDbDepAddSelArr;///< additional SELECT statement
			mutable stringArr sqlDbDepAddInsArr;///< additional INSERT statement
			mutable stringArr sqlDbDepAddUpArr;///< additional UPDATE statement
			mutable stringArr sqlDbDepAddDelArr;///< additional DELETE statement
			
			mutable int qDbDepSelShift;///< value to shift query template for specific			
			mutable int qDbDepInsShift;///< value to shift query template for specific
			mutable int qDbDepUpShift;///< value to shift query template for specific
			mutable int qDbDepDelShift;///< value to shift query template for specific
			
			/**
			 * @note share vars
			 */
			mutable assocArr::const_iterator i;///< iterator for "hash"
			mutable assocArr::const_iterator j;///< iterator for "hash"
			
			mutable std::vector<stringArr>::const_iterator k;///< iterator for "hash"
			mutable std::vector<stringArr>::const_iterator l;///< iterator for "hash"
	};
};

#endif /* _DBBASE_H_ */
