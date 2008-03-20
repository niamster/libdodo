/***************************************************************************
 *            dbMysql.h
 *
 *  Thu Apr  30 13:45:19 2005
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

#ifndef _DBMYSQL_H_
#define _DBMYSQL_H_

#include <libdodo/directives.h>

#ifdef MYSQL_EXT

#include <mysql.h>
#include <errmsg.h>
#include <sys/socket.h>

#include <libdodo/tools.h>
#include <libdodo/dbMysqlEx.h>
#include <libdodo/dbSqlBase.h>
#include <libdodo/xexec.h>

namespace dodo
{

	/**
	 * @enum dbMysqlOperTypeEnum defines type of operation for hook
	 */
	enum dbMysqlOperTypeEnum
	{
		DBMYSQL_OPERATION_CONNECT,
		DBMYSQL_OPERATION_EXEC,
		DBMYSQL_OPERATION_DISCONNECT,
		DBMYSQL_OPERATION_FETCHROW,
		DBMYSQL_OPERATION_FETCHFIELD,
	};

	/**
	 * @enum dbMysqlAddSelEnum defines additional mySQL statement for `select`
	 */
	enum dbMysqlAddSelEnum
	{
		DBMYSQL_ADDREQUEST_SELECT_STRAIGHT_JOIN = 1,
		DBMYSQL_ADDREQUEST_SELECT_SMALL_RESULT,
		DBMYSQL_ADDREQUEST_SELECT_BIG_RESULT,
	};

	/**
	 * @enum dbMysqlAddDelEnum defines additional mySQL statement for `delete`
	 */
	enum dbMysqlAddDelEnum
	{
		DBMYSQL_ADDREQUEST_DELETE_LOW_PRIORITY = 1,
		DBMYSQL_ADDREQUEST_DELETE_QUICK
	};

	/**
	 * @enum mysqlAddUpEnum defines additional mySQL statement for `update`
	 */
	enum mysqlAddUpEnum
	{
		DBMYSQL_REQUEST_UPDATE_LOW_PRIORITY = 1,
	};

	/**
	 * @enum dbMysqlAddInsEnum defines additional mySQL statement for `insert`
	 */
	enum dbMysqlAddInsEnum
	{
		DBMYSQL_ADDREQUEST_INSERT_DELAYED = 1,
		DBMYSQL_ADDREQUEST_INSERT_LOW_PRIORITY,
		DBMYSQL_ADDREQUEST_INSERT_HIGH_PRIORITY,
	};

	/**
	 * @struct __mysqlSSLOptions defines SSL mySQL options
	 */
	struct __mysqlSSLOptions
	{
		/**
		 * constructor
		 */
		__mysqlSSLOptions();

		/**
		 * constructor
		 * @param key defines pathname to the key file
		 * @param cert defines pathname to the certificate file
		 * @param ca defines pathname to the certificate authority file
		 * @param capath defines pathname to the directory that contains trusted SSL CA certificates in pem format
		 * @param cipher defines allowed SSL ciphers
		 */
		__mysqlSSLOptions(const dodoString &key, const dodoString &cert = __dodostring__, const dodoString &ca = __dodostring__, const dodoString &capath = __dodostring__, const dodoString &cipher = __dodostring__);

		dodoString key;         ///< pathname to the key file
		dodoString cert;        ///< pathname to the certificate file
		dodoString ca;          ///< pathname to the certificate authority file
		dodoString capath;      ///< pathname to a directory that contains trusted SSL CA certificates in pem format
		dodoString cipher;      ///< allowed SSL ciphers
	};

	/**
	 * @class dbMysql provides an interface to mySQL db
	 */
	class dbMysql : public dbSqlBase
	{
		private:

			/**
			 * constructor
			 * prevent copying
			 */
			dbMysql(dbMysql &a_mypp);

		public:

			/**
			 * constructor
			 */
			dbMysql();

			/**
			 * destructor
			 */
			virtual ~dbMysql();
			
			/*
			 * set connection settings
			 * @param type defines type of connection[see mySQL documentation]
			 * @param options defines options of ssl connection[see __mysqlSSLOptions]
			 * @note type can be:
			 * 	CLIENT_COMPRESS 	Use compression protocol.
			 *	CLIENT_FOUND_ROWS 	Return the number of found (matched) rows, not the number of affected rows.
			 *	CLIENT_IGNORE_SPACE 	Allow spaces after function names. Makes all functions names reserved words.
			 *	CLIENT_INTERACTIVE 	Allow interactive_timeout seconds (instead of wait_timeout seconds) of inactivity before closing the connection. The client's session wait_timeout variable is set to the value of the session interactive_timeout variable.
			 *	CLIENT_LOCAL_FILES 	Enable LOAD DATA LOCAL handling.
			 *	CLIENT_MULTI_STATEMENTS 	Tell the server that the client may send multiple statements in a single string (separated by ?;?). If this flag is not set, multiple-statement execution is disabled. New in 4.1.
			 *	CLIENT_MULTI_RESULTS 	Tell the server that the client can handle multiple result sets from multiple-statement executions or stored procedures. This is automatically set if CLIENT_MULTI_STATEMENTS is set. New in 4.1.
			 *	CLIENT_SSL 	Use SSL (encrypted protocol). This option should not be set by application programs; it is set internally in the client library.
			 */
			void connectSettings(unsigned long type, const __mysqlSSLOptions &options = __mysqlSSLOptions());

			/**
			 * connect to the database
			 */
			virtual void connect();

			/**
			 * disconnect from the database
			 */
			virtual void disconnect();

			/**
			 * @return amount of affected rows from the evaluated request
			 */
			virtual unsigned int affectedRowsCount() const;

			/**
			 * @return amount of received rows from the evaluated request
			 */
			virtual unsigned int rowsCount() const;

			/**
			 * @return amount of received fields from the evaluated request
			 */
			virtual unsigned int fieldsCount() const;

			/**
			 * @return received rows from the evaluated request
			 */
			virtual dodoArray<dodoStringArray> fetchRow() const;

			/**
			 * @return received fields from the evaluated request
			 */
			virtual dodoStringArray fetchField() const;

			/**
			 * @return structure received rows and fields from the evaluated request
			 */
			virtual __dbStorage fetch() const;

			/**
			 * @return received rows and fields from the evaluated request using hash `key`=>`value`
			 */
			virtual dodoStringMapArray fetchAssoc() const;

			/**
			 * set additional mySQL specific statement for `insert`
			 * @param statement defines additional statement[see mysqlAddInsEnum]
			 */
			virtual void setMyAddInsSt(short statement);

			/**
			 * set additional mySQL specific statement for `update`
			 * @param statement defines additional statement[see mysqlAddUpEnum]
			 */
			virtual void setMyAddUpSt(short statement);

			/**
			 * set additional mySQL specific statement for `select`
			 * @param statement defines additional statement[see mysqlAddSelEnum]
			 */
			virtual void setMyAddSelSt(short statement);

			/**
			 * set additional mySQL specific statement for `delete`
			 * @param statement defines additional statement[see mysqlAddDelEnum]
			 */
			virtual void setMyAddDelSt(short statement);

			/**
			 * remove additional mySQL specific statement for `insert`
			 * @param statement defines additional statement[see mysqlAddInsEnum]
			 */
			virtual void unsetMyAddInsSt(short statement);

			/**
			 * remove additional mySQL specific statement for `update`
			 * @param statement defines additional statement[see mysqlAddUpEnum]
			 */
			virtual void unsetMyAddUpSt(short statement);

			/**
			 * remove additional mySQL specific statement for `select`
			 * @param statement defines additional statement[see mysqlAddSelEnum]
			 */
			virtual void unsetMyAddSelSt(short statement);

			/**
			 * remove additional mySQL specific statement for `delete`
			 * @param statement defines additional statement[see mysqlAddDelEnum]
			 */
			virtual void unsetMyAddDelSt(short statement);

			/**
			 * execute request
			 * @param query defines query; you may define it if you don't use db methods like select, update
			 * @param result defines type of result; if true query return the result
			 */
			virtual void exec(const dodoString &query = __dodostring__, bool result = false);

#ifndef DBMYSQL_WO_XEXEC

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * add hook before the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPreExec(inExec func, void *data);

#ifdef DL_EXT

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPostExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPreExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set hook from the library that will be executed before/after the operation
			 * @return number in list where function is set
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 * @note type of hook[pre/post] is defined in the library
			 */
			virtual __xexecCounts addExec(const dodoString &path, void *data, void *toInit = NULL);

#endif

#endif

			/**
			 * set sessions charset
			 * @param charset defines charset
			 */
			virtual void setCharset(const dodoString &charset);

			/**
			 * @return current session charset
			 */
			virtual dodoString getCharset() const;

			/**
			 * set connection timeout
			 * @param time defines connection timeout in seconds
			 */
			virtual void setConnectTimeout(unsigned int time);
			
			/**
			 * rename field
			 * @param field defines current name of the field
			 * @param to_field defines new name of the field
			 * @param table defines table that contains the field
			 */
			virtual void renameField(const dodoString &field, const __fieldInfo &to_field, const dodoString &table);

		protected:

			/**
			 * execute request
			 * @param query defines query; you may define it if you don't use db methods like select, update
			 * @param result defines type of result; if true query return the result
			 */
			virtual void _exec(const dodoString &query, bool result);

			/**
			 * init additional mySQL specific statements
			 */
			virtual void addSQL();
			
			/**
			 * construct `rename database` statement
			 */
			virtual void renameDbCollect();

			/**
			 * construct `alter table` statement
			 */
			virtual void renameFieldCollect();

		private:

			bool empty;             ///< true id mysqlRes is empty

			MYSQL *mysql;           ///< DB handle
			MYSQL_RES *mysqlRes;    ///< handle to result

			unsigned long type;    ///< connection type
	};

};

#endif

#endif
