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

#include "directives.h"
#include "types.h"
#include "sqlBaseEx.h"
#include "dodoBase.h"
#include "tools.h"

namespace dodo
{
	///////////////////////////////////
	enum Empty
	{
		EMPTY = 0
	};
	///////////////////////////////////
	//	These statements need additional statement
	///////////////////////////////////	
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
	 * some statements can't be togather in one query. They are marcked specially
	 * for more security before applying one, remove other;
	 * example: 
	 * in select statement : [ALL | DISTINCT]
 	 * so to set ALL u should remove from flag DISTINCTROW
	 * ----
	 * qSelShift &= ~DISTINCT-1;
	 * qSelShift |= ALL;
	 * ----
	 * or to use functions removeF and addF
	 */
	enum qStEnum///do not shuffle!!
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
	enum addSelEnum
	{
		SELECT_DISTINCT = 1,///dup1
		SELECT_ALL///dup1
	};
	enum addDelEnum
	{
		DELETE_IGNORE = 1,
	};
	enum addUpEnum
	{
		UPDATE_IGNORE = 1,
	};
	enum addInsEnum
	{
		INSERT_IGNORE = 1,
	};
	/**
	 * arrays of positions of "statement" for complete realization. In sql wersion - see sqlBase;
	 */	
	static unsigned int addInsEnumArr[1] = 
	{
		INSERT_IGNORE,
	};
	static unsigned int addUpEnumArr[1] = 
	{
		UPDATE_IGNORE,
	};
	static unsigned int addDelEnumArr[1] = 
	{
		DELETE_IGNORE,
	};
	static unsigned int addSelEnumArr[2] = 
	{
		SELECT_DISTINCT,
		SELECT_ALL
	};
	/**
	 * data that could be retrieved from result
	 */
	struct __sqlStorage
	{
		__sqlStorage(std::vector<stringArr> rows, stringArr fields);
		std::vector<stringArr> rows;
		stringArr fields;
	};	
	
	/**
	 * data to connect to server
	 */
	struct __sqlInfo
	{
		std::string db;
		std::string host;
		std::string user;
		std::string password;
		std::string socket;
		unsigned int port;
	};
	/**
	 * data that could be retrieved from class(to modyficate)[contains references]
	 */
	struct __collectedData
	{
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
		
		std::string &pre_where;	
		stringArr &pre_fieldsNames;
		std::vector<stringArr> &pre_fieldsVal;
		std::string &pre_table;
		std::string &pre_tableTo;
		std::string &pre_order;
		std::string &pre_having;
		std::string &pre_group;
		std::string &pre_limNumber;
		std::string &pre_limOffset;
		stringArr &pre_subQ;
		
		int &qType;
		
		int &qShift;
		
		int &qSelShift;
		int &qInsShift;
		int &qUpShift;
		int &qDelShift;		
	};
	/**
	 * structure that contains backup of collected data
	 */
	struct __collectedDataP
	{	
		std::string pre_where;	
		stringArr pre_fieldsNames;
		std::vector<stringArr> pre_fieldsVal;
		std::string pre_table;
		std::string pre_tableTo;
		std::string pre_order;
		std::string pre_having;
		std::string pre_group;
		std::string pre_limNumber;
		std::string pre_limOffset;
		stringArr pre_subQ;
		
		int qType;
		
		int qShift;
		
		int qSelShift;
		int qInsShift;
		int qUpShift;
		int qDelShift;		
	};
	/**
	 * Data types; with '**' need range; with '*' may have range
	 */
	enum baseDataTypesEnum
	{
		TINYINT,///*; The signed range is -128 to 127. The unsigned range is 0 to 255.
		SMALLINT,///*; The signed range is -32768 to 32767. The unsigned range is 0 to 65535.
		MEDIUMINT,///*; The signed range is -8388608 to 8388607. The unsigned range is 0 to 16777215.
		INT,///*; The signed range is -2147483648 to 2147483647. The unsigned range is 0 to 4294967295.
		BIGINT,///*; The signed range is -9223372036854775808 to 9223372036854775807. The unsigned range is 0 to 18446744073709551615.
		FLOAT,///*; Allowable values are -3.402823466E+38 to -1.175494351E-38, 0, and 1.175494351E-38 to 3.402823466E+38.
		DOUBLE,///*; Allowable values are -1.7976931348623157E+308 to -2.2250738585072014E-308, 0, and 2.2250738585072014E-308 to 1.7976931348623157E+308.
		DECIMAL,///**; An unpacked(the number is stored as a string) fixed-point number. 
		DATE,///The supported range is '1000-01-01' to '9999-12-31'.
		TIME,///The range is '-838:59:59' to '838:59:59'
		TIMESTAMP,///*; The range is '1970-01-01 00:00:00' to partway through the year 2037. The first TIMESTAMP column in a table is automatically set to the date and time of the most recent operation if you don't assign it a value yourself.
		/**
		 * TIMESTAMP(14) 	YYYYMMDDHHMMSS
		 * TIMESTAMP(12) 	YYMMDDHHMMSS
		 * TIMESTAMP(10) 	YYMMDDHHMM
		 * TIMESTAMP(8) 		YYYYMMDD
		 * TIMESTAMP(6) 		YYMMDD
		 * TIMESTAMP(4) 		YYMM
		 * TIMESTAMP(2) 		YY
		 */
		CHAR,///**; The range of M is 0 to 255 characters; A fixed-length string that is always right-padded with spaces to the specified length when stored.
		VARCHAR,///**; The range of M is 0 to 255 characters. A variable-length string. Range represents the maximum column length.
		TINYBLOB,///A column with a maximum length of 255 (2^8 - 1) characters.
		BLOB,///A column with a maximum length of 65,535 (2^16 -1) characters.
		MEDIUMBLOB,///A column with a maximum length of 16,777,215 (2^24 - 1) characters. 
		LONGBLOB,///A column with a maximum length of 4,294,967,295 or 4GB (2^32 - 1) characters.
		ENUM,///An enumeration. A string object that can have only one value, chosen from the list of values 'value1', 'value2', ..., NULL or the special '' error value. An column can have a maximum of 65,535 distinct values.
		SET,///A string object that can have zero or more values, each of which must be chosen from the list of values 'value1', 'value2', ... A column can have a maximum of 64 members.
	};
	/**
	 *  type of field
	 */
	enum fieldTypeEnum
	{
		_NULL = 1,
		AUTO_INCREMENT,
		KEY
	};
	/**
	 * reference onfield
	 */
	enum refFieldEnum
	{
		RESTRICT = 0,
		CASCADE,
		SET_NULL,
		NO_ACTION,
		SET_DEFAULT
	};
	/**
	 * structure that holds info for field creation
	 */
	struct __fieldInfo
	{
		__fieldInfo();
		const __fieldInfo &operator=(__fieldInfo &from);

		std::string name;
		int type;///use baseDataTypesEnum
		int length;///is valuable for all except [DATE, TIME, *TEXT, *BLOB, SET, ENUM] => for those will be ignored
		int flag;/// default = NULL; set it with '|' : |= NOT_NULL, AUTO_INCR, KEY; 
		/**
		* for reference: set flag with (MATCH FULL or MATCH PARTIAL or MATCH SIMPLE); ON DELETE 'ref'; ON UPDATE 'ref';
		* for [ON DELETE or ON UPDATE] use on flag (RESTRICT or CASCADE or SET NULL or NO ACTION or SET DEFAULT)
		*/
		std::string refTable;///table on what is reference. next are only valuable if it set!
		stringArr refFields;
		int onDelete;///look for refFieldEnum
		int onUpdate;///look for refFieldEnum

		std::string defaultVal;///default value
		stringArr set_enum;///array of statement for SET or ENUM
		
		std::string comment;///comment for field
		std::string charset;///charset
	};
	/*
	 * structure that holds info about table
	 */
	struct __tableInfo
	{
		__tableInfo();
		const __tableInfo &operator=(__tableInfo &from);
			
		std::string name;
		std::vector<__fieldInfo> fields;///array of __fieldInfo
		
		stringArr keys;///array of keys in table (field names)
		stringArr primKeys;///array of primary keys in table (field names)
		stringArr uniq;///array of unique in table (field names)
		
		long autoIncr;///auto-increment value (as for start)
		long avgRowLen;///avarage row length
		
		std::string comment;///comment for table
		std::string charset;///charset
	};
	/**
	 * class to provide wide abilities for sql manipulations
	 * 
	 * NOTE for xexec = u may use store/restore in hook if you want to leave data unchanged after it!
	 */
	class dbBase : public dodoBase
	{
		public:
		
			/**
			 * returns this, casted to dodoBase *
			 */
			virtual dodoBase *getSelf();
			/*
			 * constructors and destructors
		 	 */
			dbBase();	
			virtual ~dbBase();

			/**
			 * next functions just modify internal data, but don't make a query. that's usefull to make preExec or postExec
			 * it is safe to call them any times. u'll never get '... limit 10 limit 3' in  this case data will be replaced => '... limit 3'
			 * this is to all functions? that collect data. If u set `where statement` with select u can replace it with 'where' function;
			 * and in any time u can unset additional statement with callin' same function with `un` preffix (unlimit,..)
			 */
			/**
			 * sets info for mysql database
			 */
			void setSqlInfo(const std::string &db, const std::string &host, unsigned int port, const std::string &user, const std::string &password, const std::string &socket = __string__) const;
			/**
			 * table - (char*)/(string); if length(table)==0 => 'from `table`' doesn't use 
			 * fields - array of (char*)/(string)
			 * where - (char*)/(string)
		     */
			virtual void select(const std::string &table, const stringArr &fieldsNames, const std::string &where = __string__) const;
			/**
			 * table - (char*)/(string)
			 * fields - array of (char*)/(string)
			 * key => field's name
			 * value => field's value
		     */
			virtual void insert(const std::string &table, const assocArr &fields);
			/**
			 * table - (char*)/(string)
			 * fields - array of array((char*)/(string))
			 * 			key => field's name
			 * 			value => field's value
		     */
			virtual void insert(const std::string &table, const std::vector<assocArr> &fields);
			/**
			 * table - (char*)/(string)
			 * fieldsVal - array of (char*)/(string)
			 * fieldsNames - array of (char*)/(string)
		     */
			virtual void insert(const std::string &table, const stringArr &fieldsVal, const stringArr &fieldsNames = __stringarray__);
			/**
			 * table - (char*)/(string)
			 * fieldsVal - array of array((char*)/(string))
			 * fieldsNames - array of (char*)/(string)
		     */
			virtual void insert(const std::string &table, const std::vector<stringArr> &fieldsVal, const stringArr &fieldsNames = __stringarray__);			
			/**
			 * table(To/From) - (char*)/(string)
			 * fieldsNames - array of (char*)/(string)
			 */
			virtual void insertSelect(const std::string &tableTo, const std::string &tableFrom, const stringArr &fieldsNamesTo, const stringArr &fieldsNamesFrom = __stringarray__, const std::string &where = __string__);
			/**
			 * table - (char*)/(string)
			 * fields - array of (char*)/(string)
			 * key => field's name
			 * value => field's value
			 * where - (char*)/(string)
		     */
			virtual void update(const std::string &table, const assocArr &fields, const std::string &where = __string__);			
			/**
			 * table - (char*)/(string)
			 * fieldsVal - array of (char*)/(string)
			 * fieldsNames - array of (char*)/(string)
			 * where - (char*)/(string)
		     */
			virtual void update(const std::string &table, const stringArr &fieldsVal, const stringArr &fieldsNames, const std::string &where = __string__);
			/**
			 * table - (char*)/(string)
			 * where - (char*)/(string)
			 */       
			virtual void del(const std::string &table, const std::string &where = __string__);
			/**
			 * subquery
			 */
			virtual void subquery(const stringArr &sub, int type=UNION/*UNION_ALL,MINUS,INTERSECT*/);
			/**
			 * change db
			 */
			virtual void use(const std::string &db);
			/**
			 * renames database
			 */
			virtual void renameDb(const std::string &db, const std::string &to_db);
			/**
			 * renames table
			 */
			virtual void renameTable(const std::string &table, const std::string &to_table);
			/**
			 * renames field
			 */ 
			virtual void renameField(const std::string &field, const std::string &to_field, const std::string &table);
			/**
			 * deletes database
			 */
			virtual void deleteDb(const std::string &db);
			/**
			 * deletes table
			 */
			virtual void deleteTable(const std::string &table);
			/**
			 * deletes field
			 */ 
			virtual void deleteField(const std::string &field, const std::string &table);
			/**
			 * creates database
			 */
			virtual void createDb(const std::string &db, const std::string &charset = __string__);
			/**
			 * creates table
			 */
			virtual void createTable(__tableInfo &tableInfo);
			/**
			 * creates field
			 */ 
			virtual void createField(__fieldInfo &fieldInfo, const std::string &table);						
			/**
			 * truncates table
			 */
			 virtual void truncate(const std::string &table);
			/**
			 * add where statement
			 */
			virtual void where(const std::string &where) const;
			/**
			 * sets limit for request
			 */
			virtual void limit(unsigned int number) const;
			/**
			 * sets offset for request
			 */
			virtual void offset(unsigned int number) const;			
			/**
			 * sets order for request
			 */
			virtual void order(const std::string &order) const;
			/**
			 * sets group for request
			 */
			virtual void group(const std::string &group) const;
			/**
			 * sets having for request
			 */
			virtual void having(const std::string &having) const;
			/**
			 * unsets where statement
			 */
			virtual void unwhere() const;
			/**
			 * unsets limit for request
			 */
			virtual void unlimit() const;
			/**
			 * unsets offset for request
			 */
			virtual void unoffset() const;			
			/**
			 * unsets order for request
			 */
			virtual void unorder() const;
			/**
			 * unsets group for request
			 */
			virtual void ungroup() const;
			/**
			 * unsets having for request
			 */
			virtual void unhaving() const;		
			/**
			 * functions to set(unset) additional parameters for standart qTypes
			 */
			virtual void setAddInsSt(unsigned int statement);
			virtual void setAddUpSt(unsigned int statement);
			virtual void setAddSelSt(unsigned int statement);
			virtual void setAddDelSt(unsigned int statement);
			
			virtual void unsetAddInsSt(unsigned int statement);
			virtual void unsetAddUpSt(unsigned int statement);
			virtual void unsetAddSelSt(unsigned int statement);
			virtual void unsetAddDelSt(unsigned int statement);
			/**
			 * detect what type of request was(will be) performed
			 */
			virtual int getQType() const;
			/**
			 * function that return structure with pointers to internal data(not copy).
			 * it is used for postExec and preExec functions. For modyfication.
			 */
			virtual __collectedData getCollectedData();			
			/**
			 * functions to store collected data into temp struct and restore from it.
			 */
			virtual void store();
			virtual void restore();
			/**
			 * set default values
			 */
			inline void initTableInfo(__tableInfo &table);
			inline void initRowInfo(__fieldInfo &field);			
		protected:
		
			/**
			 * resolve baseDataTypesEnum into string
			 */
			inline virtual std::string stringType(int type) const;
			/**
			 * check for range; return : if must 1; may have 0; mustn't have -1;
			 */
			inline virtual int chkRange(int type) const;
			/**
			 * resolve refFieldEnum into string
			 */
			inline virtual std::string stringReference(int type) const;
			/**
			 * backuped collected data
			 */
		
			mutable __collectedDataP backup;		
			/**
			 * frees collected data
			 */
			virtual void cleanCollect() const;///clean collected data
			/**
			 * collected data
			 * some of variables can be used not only as they are named. See comments/ Some variables can hold another data, to save space
			 */
			mutable std::string pre_where;///string of where statement
			mutable stringArr pre_fieldsNames;///array of string with fields' names(can be used for `insert_select` as pre_fieldsNamesTo)
			mutable std::vector<stringArr> pre_fieldsVal;///array of string with fields' values(accordingly to pre_fieldsNames)(can be used for `insert_select` as pre_fieldsNamesFrom)
			mutable std::string pre_table;///string of table name(can be used for `insert_select` as tableFrom)also can be used as 'table' for rename(delete)Field,rename(delete)Db,rename(delete)Table)
			mutable std::string pre_tableTo;///string of tableTo name(insert_select)(also can be used as 'field' for rename(delete)Field)
			mutable std::string pre_order;///string of order statement(also can be used as 'db' for rename(delete)Field,rename(delete)Db,rename(delete)Table)
			mutable std::string pre_having;///string of having statement(also can be used as ['charset' for db creation] [table/field/database for rename])
			mutable std::string pre_group;///string of group statement
			mutable std::string pre_limOffset;///number for offset in limit
			mutable std::string pre_limNumber;///number for limit
			mutable stringArr pre_subQ;///array of statements for subquery
			mutable __tableInfo pre_tableInfo;///for creation
			mutable __fieldInfo pre_fieldInfo;///for creation
			
			mutable __sqlInfo sqlInfo;///data to connect to server
			
			mutable bool show;//whether request was with result(show,select)
			
			mutable int qType; ///query type(for exec)
			
			mutable int qShift;///value to shift query template for specific
			
			mutable int qSelShift;
			mutable int qInsShift;
			mutable int qUpShift;
			mutable int qDelShift;	
			
			///additional statements for query, db-dependent, that can be implemented in derived class; 
			///these arrays take part after generalSQL statements !!! See mysqlinterface(mysqlpp::addSQL()) for more details
			///every in this class has 1 empty("") element
			mutable std::list<std::string> sqlDbDepAddSelArr;
			mutable std::list<std::string> sqlDbDepAddInsArr;
			mutable std::list<std::string> sqlDbDepAddUpArr;
			mutable std::list<std::string> sqlDbDepAddDelArr;
			
			mutable int qDbDepSelShift;///value to shift query template for specific			
			mutable int qDbDepInsShift;///value to shift query template for specific
			mutable int qDbDepUpShift;///value to shift query template for specific
			mutable int qDbDepDelShift;///value to shift query template for specific			
	};
};

#endif /* _DBBASE_H_ */
