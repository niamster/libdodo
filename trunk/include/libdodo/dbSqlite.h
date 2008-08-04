/***************************************************************************
 *            dbSqlite.h
 *
 *  Sat Dec 10 06:45:19 2005
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

#ifndef _DBSQLITE_H_
#define _DBSQLITE_H_

#include <libdodo/directives.h>

#ifdef SQLITE_EXT

#include <sqlite3.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/dbSqliteEx.h>
#include <libdodo/dbSqlConstructor.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace db
	{
		/**
		 * @class sqlite provides an interface to sqlite db
		 * @note for field type autodetection sqlite must be compiled with defined SQLITE_ENABLE_COLUMN_METADATA directive
		 */
		class sqlite : public sql::constructor
		{
			private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				sqlite(sqlite &a_pp);

			public:

				/**
				 * constructor
				 */
				sqlite();

				/**
				 * destructor
				 */
				virtual ~sqlite();

				/**
				 * connect to the database
				 */
				virtual void connect(const __connectionInfo &dbInfo);

				/**
				 * disconnect from the database
				 */
				virtual void disconnect();

				/**
				 * automaticaly detect fields types
				 * @param table defines table for which rules will be applied
				 */
				virtual void getFieldsTypes(const dodoString &table);

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
				virtual dodoArray<dodoStringArray> fetchRows() const;

				/**
				 * @return received fields from the evaluated request
				 */
				virtual dodoStringArray fetchFields() const;

				/**
				 * @return structure received rows and fields from the evaluated request
				 */
				virtual __tuples fetch() const;

				/**
				 * @return received rows and fields from the evaluated request using hash `key`=>`value`
				 */
				virtual dodoStringMapArray fetchFieldsToRows() const;

				/**
				 * execute request
				 * @param query defines query; you may define it if you don't use db methods like select, update
				 * @param result defines type of result; if true query return the result
				 */
				virtual void exec(const dodoString &query = __dodostring__, bool result = false);

			protected:

				/**
				 * construct `insert` statement
				 */
				virtual void insertCollect();

				/**
				 * construct `update` statement
				 */
				virtual void updateCollect();

				/**
				 * @struct __blob defines blob value
				 */
				struct __blob
				{
					unsigned int reference; ///< reference in request
					const dodoString *value; ///< pointer to blob value
				};

				dodoList<__blob> blobs; ///< references to blob data

			private:

				sqlite3 *sqliteHandle;                                  ///< DB handle
				sqlite3_stmt *sqliteResult;                             ///< handlde to result

				bool empty;                                             ///< true if liteStmt is empty
		};
	};
};

#endif

#endif
