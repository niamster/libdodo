/***************************************************************************
 *            dbMysql.h
 *
 *  Fri Jan  13 19:25:19 2006
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

#ifndef _POSTGRESQL_H_
#define _POSTGRESQL_H_

#include <libdodo/directives.h>

#ifdef POSTGRESQL_EXT

#include <libpq-fe.h>

#include <libdodo/tools.h>
#include <libdodo/dbPostgresqlEx.h>
#include <libdodo/dbSqlBase.h>
#include <libdodo/xexec.h>

namespace dodo
{

	/**
	 * @enum dbPostgresqlOperTypeEnum describes type of operation for hook
	 */
	enum dbPostgresqlOperTypeEnum
	{
		DBPOSTGRESQL_OPERATION_CONNECT,
		DBPOSTGRESQL_OPERATION_EXEC,
		DBPOSTGRESQL_OPERATION_DISCONNECT,
		DBPOSTGRESQL_OPERATION_FETCHROW,
		DBPOSTGRESQL_OPERATION_FETCHFIELD,
	};

	/**
	 * @class dbPostgresql is an interface to Postgresql db through sql-, database- independent interfaces
	 */
	class dbPostgresql : public dbSqlBase
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
			virtual void connect();

			/**
			 * disconnect from database
			 */
			virtual void disconnect();

			/**
			 * @return amount of affected rows(update, delete...)
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
			virtual dodoArray<dodoStringArray> fetchRow() const;

			/**
			 * @return array of fields got from request
			 */
			virtual dodoStringArray fetchField() const;

			/**
			 * @return structure that holds array of rows and array of fields got from request
			 */
			virtual __dbStorage fetch() const;

			/**
			 * @return array that holds assoc array['fiels'=>'value'] got from request
			 */
			virtual dodoStringMapArray fetchAssoc() const;

			/**
			 * executes collected request
			 * @param query contains query for DB. You may pass it if you don't use methods like select, update of libdodo
			 * @param result describes whether request returns result[show, select...] or not[delete, update]
			 * @note to insert|update|select using BLOB values use hint:
			 * 		make standart method calls to collect query, but instead of blob-values place $1 .. $n [identificators]
			 * 		call setBLOBValues method to set blob values according to id
			 * 		call exec method with query="dodo:hint:db:blob"
			 * 		YOU MUST
			 * 				set preventFraming and preventEscaping to true
			 * 				by yourself escape[using dbSqlBase::escapeFields] and frame with '' non-blob text data before inserting/updating
			 * 				by yourself escape[using dbSqlBase::unescapeFields] non-blob text data after selecting
			 */
			virtual void exec(const dodoString &query = __dodostring__, bool result = false);

#ifndef DBPOSTGRESQL_WO_XEXEC

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
			virtual __xexecCounts addExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPostExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPreExec(const dodoString &module, void *data, void *toInit = NULL);

#endif

#endif

			/**
			 * sets sessions charset
			 * @param charset indicates what type of charset would be used for session
			 */
			virtual void setCharset(const dodoString &charset);

			/**
			 * @return current session charset
			 */
			virtual int getCharset() const;

			/**
			 * @param values defines what blob-type values will be applied for dodo:hint:db:blob instead of identificators
			 */
			virtual void setBLOBValues(const dodoStringArray &values);

		protected:

			/**
			 * @return sql compliant data type
			 * @param type indicates the data type
			 */
			virtual dodoString sqlDataType(int type);
			
			/**
			 * constructs from collected data to RENAME sql statement
			 */
			virtual void renameDbCollect();
			
			/**
			 * constructs from collected data to RENAME sql statement
			 */
			virtual void renameFieldCollect();

			/**
			 * executes request
			 * @param query contains query for DB. You may pass it if you don't use methods like select, update of libdodo
			 * @param result describes whether request returns result[show, select...] or not[delete, update]
			 * @note pure postgresql actions
			 * in function without `_` hooks are calling
			 * to insert|update using BLOB values use hint:
			 * 		make standart method calls to collect query, but instead of blob-values place $1 .. $n [identificators]
			 * 		call setBLOBValues method to set blob values according to id
			 * 		call exec method with query="dodo:hint:db:blob"
			 * 		YOU MUST
			 * 				set preventFraming and preventEscaping to true
			 * 				by yourself escape[using dbSqlBase::escapeFields] and frame with '' non-blob text data before inserting/updating
			 * 				by yourself escape[using dbSqlBase::unescapeFields] non-blob text data after selecting
			 */
			virtual void _exec(const dodoString &query, bool result);

		private:

			bool empty;                 ///< for detectin' whether pgResult is empty or not

			PGconn *conn;               ///< handle for connection to PG SQL server
			PGresult *pgResult;         ///< holds result from request

			dodoStringArray blobs;      ///< to store blob data
	};

};

#endif

#endif
