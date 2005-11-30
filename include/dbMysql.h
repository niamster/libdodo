/***************************************************************************
 *            dbMysql.h
 *
 *  Thu Apr  30 13:45:19 2005
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

#ifndef _DBMYSQL_H_
#define _DBMYSQL_H_

#include <directives.h>

#ifdef MYSQL_EXT
	
	#include <string.h>
		
	#include <sys/socket.h>
	#include <mysql.h>
	
	#include <dbMysqlEx.h>
	#include <dbSqlBase.h>
	#include <tools.h>
	#include <xexec.h>
	
	namespace dodo
	{
		
		/**
		 * @enum dbMysqlOperTypeEnum describes type of operation for hook
		 */
		enum dbMysqlOperTypeEnum
		{
			DBMYSQL_OPER_CONNECT,
			DBMYSQL_OPER_EXEC,
			DBMYSQL_OPER_DISCONNECT
		};
		
		/**
		 * @enum mysqlAddSelEnum describes mySQL additional statement for SELECT
		 */
		enum mysqlAddSelEnum
		{
			SELECT_STRAIGHT_JOIN = 1,
			SELECT_SMALL_RESULT,
			SELECT_BIG_RESULT,
		};

		/**
		 * @enum mysqlAddDelEnum describes mySQL additional statement for DELETE
		 */		
		enum mysqlAddDelEnum
		{
			DELETE_LOW_PRIORITY = 1,
			DELETE_QUICK
		};

		/**
		 * @enum mysqlAddUpEnum describes mySQL additional statement for UPDATE
		 */		
		enum mysqlAddUpEnum
		{
			UPDATE_LOW_PRIORITY = 1,
		};

		/**
		 * @enum mysqlAddInsEnum describes mySQL additional statement for INSERT
		 */		
		enum mysqlAddInsEnum
		{
			INSERT_DELAYED = 1,
			INSERT_LOW_PRIORITY,
			INSERT_HIGH_PRIORITY,
		};
		
		/**
		 * @struct __mysqlSSLOptions describes mySQL options to establish ssl connection
		 * @note any unused SSL parameters may be given empty
		 */
		struct __mysqlSSLOptions
		{
			std::string key;///< the pathname to the key file
			std::string cert;///< the pathname to the certificate file.
			std::string ca;///< the pathname to the certificate authority file.
			std::string capath;///< the pathname to a directory that contains trusted SSL CA certificates in pem format.
			std::string cipher;///< a list of allowable ciphers to use for SSL encryption.
		};
		
		/**
	 	 * @class dbMysql is an interface to mysql db through sql-,database- independent interfaces
		 */
		class dbMysql : public dbSqlBase, public xexec
		{
			private :
				/**
				 * constructor
				 * to prevent from copying
				 */
				dbMysql(dbMysql &a_mypp);
			
			public:
					
				/**
				 * @return self, casted to base class - dodoBase; 
				 * usefull to cast from child to parent;
				 * used in hooks
				 */	
				virtual dodoBase * const getSelf();
				
				/**
				 * constructor
				 */
				dbMysql();
				
				/**
				 * destructor
				 */
				virtual ~dbMysql();	
			
				/**
				 * connect to database
				 * @param type is type of connection - see mySQL documentation for more!
				 * @param options is options for ssl connection. see __mysqlSSLOptions for more details
				 * 	CLIENT_COMPRESS 	Use compression protocol.
				 *	CLIENT_FOUND_ROWS 	Return the number of found (matched) rows, not the number of affected rows.
				 *	CLIENT_IGNORE_SPACE 	Allow spaces after function names. Makes all functions names reserved words.
				 *	CLIENT_INTERACTIVE 	Allow interactive_timeout seconds (instead of wait_timeout seconds) of inactivity before closing the connection. The client's session wait_timeout variable is set to the value of the session interactive_timeout variable.
				 *	CLIENT_LOCAL_FILES 	Enable LOAD DATA LOCAL handling.
				 *	CLIENT_MULTI_STATEMENTS 	Tell the server that the client may send multiple statements in a single string (separated by ?;?). If this flag is not set, multiple-statement execution is disabled. New in 4.1.
				 *	CLIENT_MULTI_RESULTS 	Tell the server that the client can handle multiple result sets from multiple-statement executions or stored procedures. This is automatically set if CLIENT_MULTI_STATEMENTS is set. New in 4.1.
				 *	CLIENT_SSL 	Use SSL (encrypted protocol). This option should not be set by application programs; it is set internally in the client library.
				 */	
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								connect(unsigned long type=CLIENT_MULTI_STATEMENTS, const __mysqlSSLOptions &options = __mysqlSSLOptions) const;
				
				/**
				 * disconnect from database
				 */
				virtual void disconnect() const;
				
				/**
				 * @return amount of affected rows(update,delete...)
				 */
				virtual unsigned int affectedRowsCount();
				
				/**
				 * @return amount of rows got from request(select ...)
				 */
				virtual unsigned int rowsCount();				
				
				/**
				 * @return amount of fields got from request(select ...)
				 */
				virtual unsigned int fieldsCount();
				
				/**
				 * @return array of rows got from request
				 */
				virtual std::vector<stringArr> fetchRow();
				
				/**
				 * @return array of fields got from request
				 */
				virtual stringArr fetchField();
				
				/**
				 * @return structure that holds array of rows and array of fields got from request
				 */
				virtual __sqlStorage fetch();
			
				/**
				 * set additional mysql-specific statement for INSERT
				 * @param statement describes additional statement
				 */
				virtual void setMyAddInsSt(mysqlAddInsEnum statement);			
				
				/**
				 * set additional mysql-specific statement for UPDATE
				 * @param statement describes additional statement
				 */
				virtual void setMyAddUpSt(mysqlAddUpEnum statement);
				
				/**
				 * set additional mysql-specific statement for SELECT
				 * @param statement describes additional statement
				 */
				virtual void setMyAddSelSt(mysqlAddSelEnum statement);
				
				/**
				 * set additional mysql-specific statement for DELETE
				 * @param statement describes additional statement
				 */
				virtual void setMyAddDelSt(mysqlAddDelEnum statement);
				
				/**
				 * unset additional mysql-specific statement for INSERT
				 * @param statement describes additional statement
				 */
				virtual void unsetMyAddInsSt(mysqlAddInsEnum statement);
				
				/**
				 * unset additional mysql-specific statement for UPDATE
				 * @param statement describes additional statement
				 */
				virtual void unsetMyAddUpSt(mysqlAddUpEnum statement);
				
				/**
				 * unset additional mysql-specific statement for SELECT
				 * @param statement describes additional statement
				 */
				virtual void unsetMyAddSelSt(mysqlAddSelEnum statement);
				
				/**
				 * unset additional mysql-specific statement for DELETE
				 * @param statement describes additional statement
				 */
				virtual void unsetMyAddDelSt(mysqlAddDelEnum statement);

				/**
				 * executes collected request
				 */				
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								exec() const;
				
				/**
				 * adds hook after the operation by callback
			 	 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */			
				virtual int addPostExec(inExec func, void *data) const;
				
				/**
				 * adds hook before the operation by callback
			 	 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPreExec(inExec func, void *data) const;
				
				#ifdef DL_EXT

					/**
					 * set function from module that will be executed before/after the main action call
					 * the type of hook[pre/post] is defined in module
					 * @return number in list where function is set
					 * @param func is a pointer to function
					 * @param data is pointer to data toy want to pass to hook
					 */			
					virtual int addExec(const std::string &module, void *data) const;
				
					/**
					 * adds hook after the operation by callback
				 	 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPostExec(const std::string &module, void *data) const;
					
					/**
					 * adds hook after the operation by callback
				 	 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPreExec(const std::string &module, void *data) const;
				
				#endif
				
				/**
				 * sets sessions charset
				 * @param charset indicates what type of charset would be used for session
				 */
				virtual void setCharset(const std::string &charset);
				
				/**
				 * sets connection timeout
				 * @param connection timeout in seconds
				 */
				virtual void setConnectTimeout(unsigned int time);
				 
				/**
				 * @return current session charset
				 */ 
				virtual std::string getCharset(); 
				 
			private:
			
				/**
				 * executes request
				 * pure mysql actions
				 * in function without `_` hooks are calling
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								_exec() const;		
				
				/**
				 * inits addidtional mySQL specific statements
				 */
				virtual void addSQL();
				
				mutable bool connected;///< connected or not
				mutable bool empty;///< for detectin' whether mysqlResult is empty or not
				
				mutable MYSQL *mysql;///< handler fo mysql connections
				mutable MYSQL_RES *mysqlRes;///< pointer to result
				mutable MYSQL_ROW mysqlRow;///< pointer to rows
				mutable MYSQL_FIELD *mysqlFields;///< pointer to fields
		};
	};
#endif

#endif
