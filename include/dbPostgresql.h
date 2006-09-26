/***************************************************************************
 *            dbMysql.h
 *
 *  Fri Jan  13 19:25:19 2006
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

#ifndef _POSTGRESQL_H_
#define _POSTGRESQL_H_

#include <directives.h>

#ifdef POSTGRESQL_EXT

	#include <libpq-fe.h>
	
	#include <dbPostgresqlEx.h>	
	#include <dbSqlBase.h>
	#include <xexec.h>
	
	namespace dodo
	{
		
		/**
		 * @enum dbPostgresqlOperTypeEnum describes type of operation for hook
		 */
		enum dbPostgresqlOperTypeEnum
		{
			DBPOSTGRESQL_OPER_CONNECT,
			DBPOSTGRESQL_OPER_EXEC,
			DBPOSTGRESQL_OPER_DISCONNECT,
			DBPOSTGRESQL_OPER_FETCHROW,
			DBPOSTGRESQL_OPER_FETCHFIELD,
		};
		
		/**
	 	 * @class dbPostgresql is an interface to Postgresql db through sql-,database- independent interfaces
		 */
		class dbPostgresql : public dbSqlBase
		
		#ifndef DBPOSTGRESQL_WO_XEXEC
										, public xexec
		#endif
		
		{
			private:
			
				/**
				 * constructor
				 * to prevent from copying
				 */
				dbPostgresql(dbPostgresql &a_pgpp);
			
			public:
				
				/**
				 * constructor
				 */
				dbPostgresql();
				
				/**
				 * destructor
				 */
				virtual ~dbPostgresql();	
			
				/**
				 * connect to database
				 */	
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								connect() const;
				
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
				virtual unsigned int rowsCount() const;				
				
				/**
				 * @return amount of fields got from request(select ...)
				 */
				virtual unsigned int fieldsCount() const;
				
				/**
				 * @return array of rows got from request
				 */
				virtual std::vector<stringArr> fetchRow() const;
				
				/**
				 * @return array of fields got from request
				 */
				virtual stringArr fetchField() const;
				
				/**
				 * @return structure that holds array of rows and array of fields got from request
				 */
				virtual __dbStorage fetch() const;
				
				/**
				 * @return array that holds assoc array['fiels'=>'value'] got from request
				 */
				virtual dodoStringMapArr fetchAssoc() const;
				
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
								exec(const std::string &query = __string__, bool result = false) const;
				
				#ifndef DBPOSTGRESQL_WO_XEXEC
				
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
					 	 * @param toInit indicates data that will path to initialize function
						 */			
						virtual xexecCounts addExec(const std::string &module, void *data, void *toInit = NULL) const;
					
						/**
						 * adds hook after the operation by callback
					 	 * @return number in list where function is set
						 * @param module is a path to module, whrere hook exists
						 * @param data is pointer to data toy want to pass to hook
					 	 * @param toInit indicates data that will path to initialize function
						 */
						virtual int addPostExec(const std::string &module, void *data, void *toInit = NULL) const;
						
						/**
						 * adds hook after the operation by callback
					 	 * @return number in list where function is set
						 * @param module is a path to module, whrere hook exists
						 * @param data is pointer to data toy want to pass to hook
					 	 * @param toInit indicates data that will path to initialize function
						 */
						virtual int addPreExec(const std::string &module, void *data, void *toInit = NULL) const;
					
					#endif
				
				#endif
				
				/**
				 * sets sessions charset
				 * @param charset indicates what type of charset would be used for session
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif				 
								setCharset(const std::string &charset);
				 
				/**
				 * @return current session charset
				 */ 
				virtual int getCharset() const;
				 
			protected:
			
				/**
				 * executes request
				 * @param query contains query for DB. You may pass it if you don't use methods like select, update of libdodo
				 * @param result describes whether request returns result[show, select...] or not[delete, update]
				 * @note pure postgresql actions
				 * in function without `_` hooks are calling
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								_exec(const std::string &query, bool result) const;
				
			private:
				
				mutable int rowsNum;///< number of columns in the query result
				mutable int fieldsNum;///< number of fields in the query result
					
				mutable bool empty;///< for detectin' whether pgResult is empty or not
	
				mutable PGconn *conn;///< handle for connection to PG SQL server
				mutable PGresult *pgResult;///< holds result from request
				mutable int status;///< status of operation
				
				mutable std::string rowPart;///< temp storage for data
				mutable stringArr rowsPart;///< to store rows
				mutable dodoStringMap rowFieldsPart;///< to store rows with fields' names
		};
		
	};
	
#endif

#endif
