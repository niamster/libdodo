/***************************************************************************
 *            dbSqlite.h
 *
 *  Sat Dec 10 06:45:19 2005
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

#ifndef _DBSQLITE_H_
#define _DBSQLITE_H_

#include <directives.h>

#ifdef SQLITE_EXT
	
	#include <sqlite3.h>
	
	#include <dbSqliteEx.h>
	#include <dbSqlBase.h>
	#include <tools.h>
	#include <dbInterface.h>
	#include <xexec.h>
	
	namespace dodo
	{
		
		/**
		 * @enum dbSqliteOperTypeEnum describes type of operation for hook
		 */
		enum dbSqliteOperTypeEnum
		{
			DBSQLITE_OPER_CONNECT,
			DBSQLITE_OPER_EXEC,
			DBSQLITE_OPER_DISCONNECT,
			DBSQLITE_OPER_FETCHROW,
			DBSQLITE_OPER_FETCHFIELD,
		};
		
		class dbSqlite;///< to make struct with this class before class declaration
		
		/**
		 * @struct __sqliteCallbackData passes to callback function
		 */
		struct __sqliteCallbackData
		{
			dbSqlite *data;
			bool first;
		};
		
		/**
	 	 * @class dbSqlite is an interface to sqlite db through sql-,database- independent interfaces
		 */
		class dbSqlite : public dbSqlBase, public dbInterface
		
		#ifndef DBSQLITE_WO_XEXEC
										, public xexec
		#endif
		
		{
			private:
			
				/**
				 * constructor
				 * to prevent from copying
				 */
				dbSqlite(dbSqlite &a_pp);
			
			public:
				
				/**
				 * constructor
				 */
				dbSqlite();
				
				/**
				 * destructor
				 */
				virtual ~dbSqlite();	
			
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
				 */				
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								exec() const;
				
				#ifndef DBSQLITE_WO_XEXEC
				
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

				mutable int rowsNum;///< number of columns in the query result
				mutable int fieldsNum;///< number of fields in the query result
				 
			protected:
			
				/**
				 * executes request
				 * pure sqlite actions
				 * in function without `_` hooks are calling
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								_exec() const;
					
			private:
				
				mutable sqlite3 *lite;///< handle to DB
				
				
				static int sqlite_callback(void *data, int argc, char **argv, char **azColName);///< callback function to work with got sql's data 
				mutable __sqliteCallbackData callBackData;
				
				mutable stringArr rowPart;///< to set temporary row content
				mutable dodoStringMap rowFieldsPart;///< to store rows with fields' names
				
				/**
				 * @note share vars
				 */
				mutable stringArr::const_iterator i;///< iterator for "hash"
				mutable stringArr::const_iterator j;///< iterator for "hash"
				mutable std::vector<stringArr>::const_iterator k;///< iterator for array of hashes
				mutable std::vector<stringArr>::const_iterator l;///< iterator for array of hashes				
		};

	};
	
#endif

#endif
