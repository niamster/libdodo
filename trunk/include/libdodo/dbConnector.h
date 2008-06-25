/***************************************************************************
 *            dbConnector.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DBCONNECTOR_H_
#define _DBCONNECTOR_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace db
	{
		/**
		 * @enum connectorFieldTypeEnum defines data types
		 * @note with data marked with '**' may have length
		 * with data marked with '**' needs to specify length
		 */
		enum connectorFieldTypeEnum
		{
			CONNECTOR_FIELDTYPE_TINYINT,                                                ///< *The signed range is -128 to 127. The unsigned range is 0 to 255
			CONNECTOR_FIELDTYPE_SMALLINT,                                               ///< *The signed range is -32768 to 32767. The unsigned range is 0 to 65535
			CONNECTOR_FIELDTYPE_MEDIUMINT,                                              ///< *The signed range is -8388608 to 8388607. The unsigned range is 0 to 16777215
			CONNECTOR_FIELDTYPE_INTEGER, CONNECTOR_FIELDTYPE_INT,                       ///< *The signed range is -2147483648 to 2147483647. The unsigned range is 0 to 4294967295
			CONNECTOR_FIELDTYPE_BIGINT,                                                 ///< *The signed range is -9223372036854775808 to 9223372036854775807. The unsigned range is 0 to 18446744073709551615
			CONNECTOR_FIELDTYPE_FLOAT,                                                  ///< *Allowable values are -3.402823466E+38 to -1.175494351E-38, 0, and 1.175494351E-38 to 3.402823466E+38
			CONNECTOR_FIELDTYPE_REAL, CONNECTOR_FIELDTYPE_DOUBLE,                       ///< *[DOUBLE in some systems] Allowable values are -1.7976931348623157E+308 to -2.2250738585072014E-308, 0, and 2.2250738585072014E-308 to 1.7976931348623157E+308
			CONNECTOR_FIELDTYPE_DECIMAL,                                                ///< **An unpacked(the number is stored as a string) fixed-point number
			CONNECTOR_FIELDTYPE_DATE,                                                   ///< The supported range is '1000-01-01' to '9999-12-31'
			CONNECTOR_FIELDTYPE_TIME,                                                   ///< The range is '-838:59:59' to '838:59:59'
			CONNECTOR_FIELDTYPE_TIMESTAMP,                                              ///< *The range is '1970-01-01 00:00:00' to partway through the year 2037. The first TIMESTAMP column in a table is automatically set to the date and time of the most recent operation if you don't assign it a value yourself
			/**
			 * TIMESTAMP(14)        YYYYMMDDHHMMSS
			 * TIMESTAMP(12)        YYMMDDHHMMSS
			 * TIMESTAMP(10)        YYMMDDHHMM
			 * TIMESTAMP(8)                 YYYYMMDD
			 * TIMESTAMP(6)                 YYMMDD
			 * TIMESTAMP(4)                 YYMM
			 * TIMESTAMP(2)                 YY
			 */
			CONNECTOR_FIELDTYPE_CHAR,                                                   ///< **The range of M is 0 to 255 characters; A fixed-length string that is always right-padded with spaces to the specified length when stored
			CONNECTOR_FIELDTYPE_VARCHAR,                                                ///< **The range of M is 0 to 255 characters. A variable-length string. Range represents the maximum column length
			CONNECTOR_FIELDTYPE_TINYBLOB,                                               ///< A column with a maximum length of 255 (2^8 - 1) characters
			CONNECTOR_FIELDTYPE_BLOB,                                                   ///< A column with a maximum length of 65, 535 (2^16 -1) characters
			CONNECTOR_FIELDTYPE_MEDIUMBLOB,                                             ///< A column with a maximum length of 16, 777, 215 (2^24 - 1) characters
			CONNECTOR_FIELDTYPE_LONGBLOB,                                               ///< A column with a maximum length of 4, 294, 967, 295 or 4GB (2^32 - 1) characters
			CONNECTOR_FIELDTYPE_TINYTEXT,                                               ///< A column with a maximum length of 255 (2^8 - 1) characters
			CONNECTOR_FIELDTYPE_TEXT,                                                   ///< A column with a maximum length of 65, 535 (2^16 -1) characters
			CONNECTOR_FIELDTYPE_MEDIUMTEXT,                                             ///< A column with a maximum length of 16, 777, 215 (2^24 - 1) characters
			CONNECTOR_FIELDTYPE_LONGTEXT,                                               ///< A column with a maximum length of 4, 294, 967, 295 or 4GB (2^32 - 1) characters
			CONNECTOR_FIELDTYPE_ENUM,                                                   ///< An enumeration. A string object that can have only one value, chosen from the list of values 'value1', 'value2', ..., NULL or the special '' error value. An column can have a maximum of 65, 535 distinct values
			CONNECTOR_FIELDTYPE_SET,                                                    ///< A string object that can have zero or more values, each of which must be chosen from the list of values 'value1', 'value2', ... A column can have a maximum of 64 members
		};

		/**
		 *  @enum connectorFieldFlagEnum defines additional field properties
		 */
		enum connectorFieldFlagEnum
		{
			CONNECTOR_FIELDFLAG_NULL = 2,                               ///< may be NULL
			CONNECTOR_FIELDFLAG_AUTO_INCREMENT = 4,                     ///< if is not set by request, will be incremented relatevly to previous
		};

		/**
		 * @enum connectorReferenceEnum defines reference type assigned to the field
		 */
		enum connectorReferenceEnum
		{
			CONNECTOR_REFERENCE_RESTRICT = 1,                   ///< does not allow the action of any of those parent rows
			CONNECTOR_REFERENCE_CASCADE,                        ///< a row in the parent table is deleted, automatically deletes also all those rows in the child table whose foreign key values are equal to the referenced key value in the parent row
			CONNECTOR_REFERENCE_SET_NULL,                       ///< sets NULL on the action of any of those parent rows indicates on set action
			CONNECTOR_REFERENCE_NO_ACTION,                      ///< noacton on the action of any of those parent rows indicates on set action
			CONNECTOR_REFERENCE_SET_DEFAULT                     ///< sets default on the action of any of those parent rows indicates on set action
		};

#define CONNECTOR_JOINTYPESTSTATEMENTS 6

		/**
		 * @enum connectorJoinTypeEnum defines join types
		 */
		enum connectorJoinTypeEnum
		{
			CONNECTOR_JOINTYPE_JOIN,
			CONNECTOR_JOINTYPE_LEFTOUTER,
			CONNECTOR_JOINTYPE_RIGHTOUTER,
			CONNECTOR_JOINTYPE_FULLOUTER,
			CONNECTOR_JOINTYPE_INNER,
			CONNECTOR_JOINTYPE_CROSS,
		};

#define CONNECTOR_SUBREQUESTSTATEMENTS 4

		/**
		 * @enum connectorSubrequestEnum defines type of sub request
		 */
		enum connectorSubrequestEnum
		{
			CONNECTOR_SUBREQUEST_UNION = 1,
			CONNECTOR_SUBREQUEST_UNION_ALL,
			CONNECTOR_SUBREQUEST_MINUS,
			CONNECTOR_SUBREQUEST_INTERSECT,
		};

		/**
		 * @struct __connectorField defines data for field creation
		 */
		struct __connectorField
		{
			/**
			 * constructor
			 */
			__connectorField();

			/**
			 * overloaded operator '='
			 * @param from defines object where get properties
			 */
			const __connectorField &operator=(const __connectorField &from);

			dodoString name;                            ///< name of the field
			int type;                                   ///< type of field[see accumulatorFieldTypeEnum]
			int length;                                 ///< length of the field[see accumulatorFieldTypeEnum notes]
			int flag;                                   ///< flags of the field; may be combined with '|'[see accumulatorFieldFlagEnum][CONNECTOR_FIELDFLAG_NULL by default]

			dodoString refTable;                        ///< table where reference points
			dodoStringArray refFields;                  ///< fields where reference points
			int onDelete;                               ///< reference action on delete operation[see dbReferenceEnum]
			int onUpdate;                               ///< reference action on update operation[see dbReferenceEnum]

			dodoString defaultVal;                      ///< default value of the field
			dodoStringArray set_enum;                   ///< statements for SET or ENUM types

			dodoString charset;                         ///< collate charset of the field
		};

		/*
		 * @struct __connectorTable defines data for table creation
		 */
		struct __connectorTable
		{
			/**
			 * constructor
			 */
			__connectorTable();

			/**
			 * overloaded operator '='
			 * @param from defines object where get properties
			 */
			const __connectorTable &operator=(const __connectorTable &from);

			dodoString name;                                            ///< name of the table
			dodoArray<__connectorField> fields;                         ///< fields[see __connectorField]

			dodoStringArray primKeys;                                   ///< primary keys of the table
			dodoStringArray uniq;                                       ///< fields of the table that may contain only unique values

			bool ifNotExists;                                           ///< do not throw exception if table already exists[false by default]
		};

		/**
		 * @struct __connectorStorage defines fetched data from db
		 */
		struct __connectorStorage
		{
			/**
			 * constructor
			 * @param rows defines rows of data
			 * @param fields defines names of fields
			 */
			__connectorStorage(dodoArray<dodoStringArray> rows, dodoStringArray fields);

			/**
			 * constructor
			 */
			__connectorStorage();

			dodoArray<dodoStringArray> rows;                    ///< rows of data
			dodoStringArray fields;                             ///< names of fields
		};

		/**
		 * @struct __sqlInfo defines connection options for the server
		 */
		struct __connectorInfo
		{

			/**
			 * constructor
			 */
			__connectorInfo();

			/**
			 * constructor
			 * @param db defines name of db
			 * @param host defines host
			 * @param user defines user
			 * @param password defines password
			 * @param path defines path to db or unix socket
			 * @param port defines port
			 */
			__connectorInfo(const dodoString &db, const dodoString &host, const dodoString &user, const dodoString &password, const dodoString &path, int port);

			dodoString db;                      ///< database name
			dodoString host;                    ///< hostname
			dodoString user;                    ///< username
			dodoString password;                ///< password
			dodoString path;                    ///< path to db or unix socket
			unsigned int port;                  ///< port
		};

		/**
		 * @enum dbOperationTypeEnum defines type of operation for hook
		 */
		enum dbOperationTypeEnum
		{
			DB_OPERATION_CONNECT,
			DB_OPERATION_EXEC,
			DB_OPERATION_DISCONNECT,
			DB_OPERATION_FETCHROW,
			DB_OPERATION_FETCHFIELD,
		};

		/**
		 * @class connector implements an interface to db through sql and database independent interfaces
		 */
		class connector
#ifndef DB_WO_XEXEC
						: public xexec
#endif
		{
			public:

				/**
				 *constructor
				 */
				connector();

				/**
				 * destructor
				 */
				virtual ~connector() = 0;

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
				 * @param dbInfo defines connection information
				 */
				virtual void setDbInfo(const __connectorInfo &dbInfo) = 0;

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
				 * @param where defines `where` statement
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
				 * store query, made from subquery with requested method
				 * @param subqueries defines subqueries
				 * @param type defines type of combining subqueries[see connectorSubrequestEnum]
				 */
				virtual void subquery(const dodoStringArray &subqueries, int type = CONNECTOR_SUBREQUEST_UNION) = 0;

				/**
				 * create index in the table
				 * @param table defines table where create index
				 * @param field defines field that would be index
				 * @param name defines name of the index
				 */
				virtual void createIndex(const dodoString &table, const dodoString &field, const dodoString &name) = 0;

				/**
				 * create index in the table
				 * @param table defines table where create index
				 * @param fields defines fields that would be index
				 * @param name defines name of the index
				 */
				virtual void createIndex(const dodoString &table, const dodoStringArray &fields, const dodoString &name) = 0;

				/**
				 * delete index in the table
				 * @param table defines table where delete index
				 * @param name defines name of the index
				 */
				virtual void deleteIndex(const dodoString &table, const dodoString &name) = 0;

				/**
				 * rename database
				 * @param db defines current name of the database
				 * @param to_db defines new name of the database
				 */
				virtual void renameDb(const dodoString &db, const dodoString &to_db) = 0;

				/**
				 * rename table
				 * @param table defines current name of the table
				 * @param to_table defines new name of the table
				 */
				virtual void renameTable(const dodoString &table, const dodoString &to_table) = 0;

				/**
				 * rename field
				 * @param field defines current name of the field
				 * @param to_field defines new name of the field
				 * @param table defines table that contains the field
				 */
				virtual void renameField(const dodoString &field, const dodoString &to_field, const dodoString &table) = 0;

				/**
				 * delete database
				 * @param db defines the database
				 */
				virtual void deleteDb(const dodoString &db) = 0;

				/**
				 * delete table
				 * @param table defines the table
				 */
				virtual void deleteTable(const dodoString &table) = 0;

				/**
				 * delete field
				 * @param field defines field that will be deleted
				 * @param table defines table that contains the field
				 */
				virtual void deleteField(const dodoString &field, const dodoString &table) = 0;

				/**
				 * create database
				 * @param db the name of the new database
				 * @param charset defines charset of the database
				 */
				virtual void createDb(const dodoString &db, const dodoString &charset = __dodostring__) = 0;

				/**
				 * create table
				 * @param tableInfo defines table definition[see __connectorTable]
				 */
				virtual void createTable(const __connectorTable &tableInfo) = 0;

				/**
				 * create field
				 * @param fieldInfo defines field definition[see __connectorField]
				 * @param table defines table that will contain the field
				 */
				virtual void createField(const __connectorField &fieldInfo, const dodoString &table) = 0;

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
				 * @param type defines join type[see connectorJoinTypeEnum]
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
				virtual __connectorStorage fetch() const = 0;

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

				bool reconnect;                 ///< if true tries to reconect in case when `exec` failed with connection error[true by default]

			protected:

				bool connected;                 ///< true if connected to the db
		};
	};
};

#endif
