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
	
	#include <dbSqliteEx.h>
	#include <dbSqlBase.h>
	#include <tools.h>
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
		
		/**
		 * @enum sqliteAddSelEnum describes SQLite additional statement for SELECT
		 */
		enum sqliteAddSelEnum
		{
		};

		/**
		 * @enum sqliteAddDelEnum describes SQLite additional statement for DELETE
		 */		
		enum sqliteAddDelEnum
		{
		};

		/**
		 * @enum sqliteAddUpEnum describes SQLite additional statement for UPDATE
		 */		
		enum sqliteAddUpEnum
		{
		};

		/**
		 * @enum sqliteAddInsEnum describes SQLite additional statement for INSERT
		 */		
		enum sqliteAddInsEnum
		{
		};
		
		/**
	 	 * @class dbSqlite is an interface to sqlite db through sql-,database- independent interfaces
		 */
		class dbSqlite : public dbSqlBase, public xexec
		{
			private :
				/**
				 * constructor
				 * to prevent from copying
				 */
				dbSqlite(dbSqlite &a_pp);
			
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
				 * set additional sqlite-specific statement for INSERT
				 * @param statement describes additional statement
				 */
				virtual void setLiteAddInsSt(sqliteAddInsEnum statement);			
				
				/**
				 * set additional sqlite-specific statement for UPDATE
				 * @param statement describes additional statement
				 */
				virtual void setLiteAddUpSt(sqliteAddUpEnum statement);
				
				/**
				 * set additional sqlite-specific statement for SELECT
				 * @param statement describes additional statement
				 */
				virtual void setLiteAddSelSt(sqliteAddSelEnum statement);
				
				/**
				 * set additional sqlite-specific statement for DELETE
				 * @param statement describes additional statement
				 */
				virtual void setLiteAddDelSt(sqliteAddDelEnum statement);
				
				/**
				 * unset additional sqlite-specific statement for INSERT
				 * @param statement describes additional statement
				 */
				virtual void unsetLiteAddInsSt(sqliteAddInsEnum statement);
				
				/**
				 * unset additional sqlite-specific statement for UPDATE
				 * @param statement describes additional statement
				 */
				virtual void unsetLiteAddUpSt(sqliteAddUpEnum statement);
				
				/**
				 * unset additional sqlite-specific statement for SELECT
				 * @param statement describes additional statement
				 */
				virtual void unsetLiteAddSelSt(sqliteAddSelEnum statement);
				
				/**
				 * unset additional sqlite-specific statement for DELETE
				 * @param statement describes additional statement
				 */
				virtual void unsetLiteAddDelSt(sqliteAddDelEnum statement);

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
				 
			private:
			
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
				
				/**
				 * inits additional SQLite specific statements
				 */
				virtual void addSQL();
				
				mutable bool connected;///< connected or not
				mutable bool empty;///< for detectin' whether result is empty or not
				
				std::vector<stringArr> rows;///< to store rows
				stringArr fields;///< to store fields
		};
	};
#endif

#endif
