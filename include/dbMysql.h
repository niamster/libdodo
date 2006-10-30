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

#include <directives.h>

#ifdef MYSQL_EXT
		
	#include <mysql.h>
	#include <sys/socket.h>

	#include <tools.h>	
	#include <dbMysqlEx.h>	
	#include <dbSqlBase.h>
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
			DBMYSQL_OPER_DISCONNECT,
			DBMYSQL_OPER_FETCHROW,
			DBMYSQL_OPER_FETCHFIELD,
		};
		
		/**
		 * @enum mysqlAddSelEnum describes mySQL additional statement for SELECT
		 */
		enum mysqlAddSelEnum
		{
			DBREQUEST_SELECT_STRAIGHT_JOIN = 1,
			DBREQUEST_SELECT_SMALL_RESULT,
			DBREQUEST_SELECT_BIG_RESULT,
		};

		/**
		 * @enum mysqlAddDelEnum describes mySQL additional statement for DELETE
		 */		
		enum mysqlAddDelEnum
		{
			DBREQUEST_DELETE_LOW_PRIORITY = 1,
			DBREQUEST_DELETE_QUICK
		};

		/**
		 * @enum mysqlAddUpEnum describes mySQL additional statement for UPDATE
		 */		
		enum mysqlAddUpEnum
		{
			DBREQUEST_UPDATE_LOW_PRIORITY = 1,
		};

		/**
		 * @enum mysqlAddInsEnum describes mySQL additional statement for INSERT
		 */		
		enum mysqlAddInsEnum
		{
			DBREQUEST_INSERT_DELAYED = 1,
			DBREQUEST_INSERT_LOW_PRIORITY,
			DBREQUEST_INSERT_HIGH_PRIORITY,
		};
		
		/**
		 * @struct __mysqlSSLOptions describes mySQL options to establish ssl connection
		 * @note any unused SSL parameters may be given empty
		 */
		struct __mysqlSSLOptions
		{
			/**
			 * constructor
			 */
			__mysqlSSLOptions();
			
			/**
			 * constructor
			 * @note defines structure data with user data
			 */
			__mysqlSSLOptions(const std::string &key, const std::string &cert = __string__, const std::string &ca = __string__, const std::string &capath = __string__, const std::string &cipher = __string__);
			
			std::string key;///< the pathname to the key file
			std::string cert;///< the pathname to the certificate file.
			std::string ca;///< the pathname to the certificate authority file.
			std::string capath;///< the pathname to a directory that contains trusted SSL CA certificates in pem format.
			std::string cipher;///< a list of allowable ciphers to use for SSL encryption.
		};
		
		/**
	 	 * @class dbMysql is an interface to mysql db through sql-,database- independent interfaces
		 */
		class dbMysql : public dbSqlBase
		
		#ifndef DBMYSQL_WO_XEXEC
										, public xexec
		#endif
		
		{
			private:
			
				/**
				 * constructor
				 * to prevent from copying
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
				 * sets connection settings
				 * @param type is type of connection[see mySQL documentation for more]
				 * @param options is options for ssl connection[see __mysqlSSLOptions for more details]
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
				 * connect to database
				 */	
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								connect();
				
				/**
				 * disconnect from database
				 */
				virtual void disconnect();
				
				/**
				 * @return amount of affected rows(update,delete...)
				 */
				virtual unsigned int affectedRowsCount() const;
				
				/**
				 * @return amount of rows got from request(select ...)
				 */
				virtual unsigned int rowsCount() const;				
				
				/**
				 * @return amount of fields got from request(select ...)
				 */
				virtual unsigned int fieldsCount() const;
				
				/**
				 * @return array of rows got from request
				 */
				virtual dodoArray<dodoStringArr> fetchRow() const;
				
				/**
				 * @return array of fields got from request
				 */
				virtual dodoStringArr fetchField() const;
				
				/**
				 * @return structure that holds array of rows and array of fields got from request
				 */
				virtual __dbStorage fetch() const;
				
				/**
				 * @return array that holds assoc array['fiels'=>'value'] got from request
				 */
				virtual dodoStringMapArr fetchAssoc() const;
							
				/**
				 * set additional mysql-specific statement for INSERT
				 * @param statement describes additional statement[see mysqlAddDelEnum]
				 */
				virtual void setMyAddInsSt(short statement);			
				
				/**
				 * set additional mysql-specific statement for UPDATE
				 * @param statement describes additional statement[see mysqlAddDelEnum]
				 */
				virtual void setMyAddUpSt(short statement);
				
				/**
				 * set additional mysql-specific statement for SELECT
				 * @param statement describes additional statement[see mysqlAddDelEnum]
				 */
				virtual void setMyAddSelSt(short statement);
				
				/**
				 * set additional mysql-specific statement for DELETE
				 * @param statement describes additional statement[see mysqlAddDelEnum]
				 */
				virtual void setMyAddDelSt(short statement);
				
				/**
				 * unset additional mysql-specific statement for INSERT
				 * @param statement describes additional statement[see mysqlAddDelEnum]
				 */
				virtual void unsetMyAddInsSt(short statement);
				
				/**
				 * unset additional mysql-specific statement for UPDATE
				 * @param statement describes additional statement[see mysqlAddDelEnum]
				 */
				virtual void unsetMyAddUpSt(short statement);
				
				/**
				 * unset additional mysql-specific statement for SELECT
				 * @param statement describes additional statement[see mysqlAddDelEnum]
				 */
				virtual void unsetMyAddSelSt(short statement);
				
				/**
				 * unset additional mysql-specific statement for DELETE
				 * @param statement describes additional statement[see mysqlAddDelEnum]
				 */
				virtual void unsetMyAddDelSt(short statement);

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
								exec(const std::string &query = __string__, bool result = false);
				
				#ifndef DBMYSQL_WO_XEXEC
				
					/**
					 * adds hook after the operation by callback
				 	 * @return number in list where function is set
					 * @param func is a pointer to function
					 * @param data is pointer to data toy want to pass to hook
					 */			
					virtual int addPostExec(inExec func, void *data);
					
					/**
					 * adds hook before the operation by callback
				 	 * @return number in list where function is set
					 * @param func is a pointer to function
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPreExec(inExec func, void *data);
					
					#ifdef DL_EXT
	
						/**
						 * set function from module that will be executed before/after the main action call
						 * the type of hook[pre/post] is defined in module
						 * @return number in list where function is set
						 * @param func is a pointer to function
						 * @param data is pointer to data toy want to pass to hook
					 	 * @param toInit indicates data that will path to initialize function
						 */			
						virtual xexecCounts addExec(const std::string &module, void *data, void *toInit = NULL);
					
						/**
						 * adds hook after the operation by callback
					 	 * @return number in list where function is set
						 * @param module is a path to module, whrere hook exists
						 * @param data is pointer to data toy want to pass to hook
					 	 * @param toInit indicates data that will path to initialize function
						 */
						virtual int addPostExec(const std::string &module, void *data, void *toInit = NULL);
						
						/**
						 * adds hook after the operation by callback
					 	 * @return number in list where function is set
						 * @param module is a path to module, whrere hook exists
						 * @param data is pointer to data toy want to pass to hook
					 	 * @param toInit indicates data that will path to initialize function
						 */
						virtual int addPreExec(const std::string &module, void *data, void *toInit = NULL);
					
					#endif
				
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
				virtual std::string getCharset() const;
				 
			protected:
			
				/**
				 * executes request
				 * @param query contains query for DB. You may pass it if you don't use methods like select, update of libdodo
				 * @param result describes whether request returns result[show, select...] or not[delete, update]
				 * @note pure mysql actions
				 * in function without `_` hooks are calling
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								_exec(const std::string &query, bool result);		
				
				/**
				 * inits addidtional mySQL specific statements
				 */
				virtual void addSQL();

			private:	
				
				bool empty;///< for detectin' whether mysqlResult is empty or not
				
				MYSQL *mysql;///< handler fo mysql connections
				MYSQL_RES *mysqlRes;///< pointer to result
				
				unsigned long type;
		};
		
	};
	
#endif

#endif
