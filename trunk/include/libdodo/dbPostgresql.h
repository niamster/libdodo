/***************************************************************************
 *            dbPostgresql.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DBPOSTGRESQL_H_
#define _DBPOSTGRESQL_H_

#include <libdodo/directives.h>

#ifdef POSTGRESQL_EXT

#include <libpq-fe.h>
#include <stdlib.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/dbPostgresqlEx.h>
#include <libdodo/dbSqlConstructor.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace db
	{
		/**
		 * @enum postgresqlHintEnum define DB hints
		 */
		enum postgresqlHintEnum
		{
			POSTGRESQL_HINT_NONE = 0,
			POSTGRESQL_HINT_BLOB = 2,             ///< insert|update using BLOB values
		};

		/**
		 * @class postgresql provides an interface to postgresql db
		 */
		class postgresql : public sqlConstructor
		{
			private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				postgresql(postgresql &a_pgpp);

			public:

				/**
				 * constructor
				 */
				postgresql();

				/**
				 * destructor
				 */
				virtual ~postgresql();

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
				virtual __connectorStorage fetch() const;

				/**
				 * @return received rows and fields from the evaluated request using hash `key`=>`value`
				 */
				virtual dodoStringMapArray fetchAssoc() const;

				/**
				 * execute request
				 * @param query defines query; you may define it if you don't use db methods like select, update
				 * @param result defines type of result; if true query return the result
				 * @note to insert|update using BLOB values use hint:
				 *              make standart method calls to collect query, but instead of blob-values place $1 .. $n [identificators]
				 *              call setBLOBValues method to set blob values according to id
				 *              define hint class attribute as POSTGRESQL_HINT_BLOB[you have to define it each time before this call]
				 *              YOU MUST
				 *                              define preventFraming and preventEscaping as true
				 *                              by yourself escape[using sqlConstructor::escapeFields] and frame with '' non-blob text data before inserting/updating
				 *                              by yourself escape[using sqlConstructor::unescapeFields] non-blob text data after selecting
				 * to select BLOB values use hint:
				 *              define hint class attribute as POSTGRESQL_HINT_BLOB[you have to define it each time before this call]
				 */
				virtual void exec(const dodoString &query = __dodostring__, bool result = false);

				unsigned short hint;                 ///< DB hint[see postgresqlHintEnum]

				/**
				 * set BLOB data for the request
				 * @param values defines blob-type values that will be applied for dodo:hint:db:blob instead of identificators
				 */
				virtual void setBLOBValues(const dodoStringArray &values);

				/**
				 * set sessions charset
				 * @param charset defines charset
				 */
				virtual void setCharset(const dodoString &charset);

				/**
				 * @return current session charset
				 */
				virtual dodoString getCharset() const;

			protected:

				/**
				 * @return SQL compliant data type
				 * @param type defines data type
				 */
				virtual dodoString sqlDataType(int type);

				/**
				 * construct `rename database` statement
				 */
				virtual void renameDbCollect();

				/**
				 * construct `alter table` statement
				 */
				virtual void renameFieldCollect();

			private:

				bool empty;                                     ///< true id pgResult is empty

				PGconn *pgHandle;                               ///< DB handle
				PGresult *pgResult;                             ///< handlde to result

				dodoStringArray blobs;                          ///< blob data
		};
	};
};

#endif

#endif
