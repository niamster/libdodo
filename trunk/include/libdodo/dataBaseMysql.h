/***************************************************************************
 *            dataBaseMysql.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DATABASEMYSQL_H_
#define _DATABASEMYSQL_H_ 1

#include <libdodo/directives.h>

#ifdef MYSQL_EXT
#include <libdodo/types.h>
#include <libdodo/dataBaseSqlConstructor.h>

namespace dodo {
	namespace data {
		namespace base {
			struct __mysql__;

			/**
			 * @struct __mysqlSslOptions__
			 * @brief defines SSL mySQL options
			 */
			struct __mysqlSslOptions__ {
				dodoString key;     ///< pathname to the key file
				dodoString cert;    ///< pathname to the certificate file
				dodoString ca;      ///< pathname to the certificate authority file
				dodoString capath;  ///< pathname to a directory that contains trusted SSL CA certificates in pem format
				dodoString cipher;  ///< allowed SSL ciphers
			};

			/**
			 * @class mysql
			 * @brief provides an interface to MySQL db
			 */
			class mysql : public sql::constructor {
			  private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				mysql(mysql &mypp);

			  public:

				/**
				 * constructor
				 */
				mysql();

				/**
				 * constructor
				 * @param dbInfo defines information for connection to db
				 */
				mysql(const __connectionInfo__ &dbInfo);

				/**
				 * destructor
				 */
				virtual ~mysql();

				/*
				 * set connection settings
				 * @param type defines type of connection[see mySQL documentation]
				 * @param options defines options of ssl connection[see __mysqlSslOptions__]
				 * @note type can be:
				 *  CLIENT_COMPRESS         Use compression protocol
				 *	CLIENT_MULTI_STATEMENTS Tell the server that the client may send multiple statements in a single string (separated by ?;?). If this flag is not set, multiple-statement execution is disabled. New in 4.1.
				 */
				void connectSettings(unsigned long             type,
											 const __mysqlSslOptions__ &options = __mysqlSslOptions__());

				/**
				 * connect to the database
				 * @param dbInfo defines information for connection to db
				 */
				virtual void connect(const __connectionInfo__ &dbInfo);

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
				virtual __tuples__ fetch() const;

				/**
				 * @return received rows and fields from the evaluated request using hash `key`=>`value`
				 */
				virtual dodoStringMapArray fetchFieldsToRows() const;

				/**
				 * execute request
				 * @param query defines query; you may define it if you don't use db methods like select, update
				 * @param result defines type of result; if true query return the result
				 */
				virtual void exec(const dodoString &query = __dodostring__,
								  bool             result = false);

				/**
				 * set sessions charset
				 * @param charset defines charset
				 */
				void setCharset(const dodoString &charset);

				/**
				 * @return current session charset
				 */
				dodoString getCharset() const;

				/**
				 * set connection timeout
				 * @param time defines connection timeout in seconds
				 */
				void setConnectTimeout(unsigned int time);

			  private:

				bool empty;             ///< true id mysqlRes is empty

				__mysql__ *handle;      ///< DB handle

				unsigned long type;     ///< connection type
			};
		};
	};
};
#endif

#endif
