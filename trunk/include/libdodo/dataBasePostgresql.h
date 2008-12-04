/***************************************************************************
 *            dataBasePostgresql.h
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

#ifndef _DATABASEPOSTGRESQL_H_
#define _DATABASEPOSTGRESQL_H_ 1

#include <libdodo/directives.h>

#ifdef POSTGRESQL_EXT

#include <libpq-fe.h>
#include <stdlib.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/dataBasePostgresqlEx.h>
#include <libdodo/dataBaseSqlConstructor.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace data
	{
		namespace base
		{
			/**
			 * @class postgresql
			 * @brief provides an interface to postgresql db
			 */
			class postgresql : public sql::constructor
			{
				private:

					/**
					 * copy constructor
					 * @note to prevent copying
					 */
					postgresql(postgresql &pgpp);

				public:

					/**
					 * constructor
					 */
					postgresql();

					/**
					 * constructor
					 * @param dbInfo defines information for connection to db
					 */
					postgresql(const __connectionInfo &dbInfo);

					/**
					 * destructor
					 */
					virtual ~postgresql();

					/**
					 * connect to the database
					 * @param dbInfo defines information for connection to db
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
					 * construct `insert` statement
					 */
					virtual void insertCollect();

					/**
					 * construct `update` statement
					 */
					virtual void updateCollect();

#ifdef POSTGRESQL_NO_ENCODINGTOCHAR

#define POSTGRESQL_ENCODINGSTATEMENTS 39

					/**
					 * @enum encodingIdentefiersEnum defines postgresql encoding identifiers
					 * @note fetched from mb/pg_wchar.h
					 */
					enum encodingIdentefiersEnum
					{
						PG_SQL_ASCII = 0,                           /* SQL/ASCII */
						PG_EUC_JP,                                  /* EUC for Japanese */
						PG_EUC_CN,                                  /* EUC for Chinese */
						PG_EUC_KR,                                  /* EUC for Korean */
						PG_EUC_TW,                                  /* EUC for Taiwan */
						PG_JOHAB,                                   /* EUC for Korean JOHAB */
						PG_UTF8,                                    /* Unicode UTF8 */
						PG_MULE_INTERNAL,                           /* Mule internal code */
						PG_LATIN1,                                  /* ISO-8859-1 Latin 1 */
						PG_LATIN2,                                  /* ISO-8859-2 Latin 2 */
						PG_LATIN3,                                  /* ISO-8859-3 Latin 3 */
						PG_LATIN4,                                  /* ISO-8859-4 Latin 4 */
						PG_LATIN5,                                  /* ISO-8859-9 Latin 5 */
						PG_LATIN6,                                  /* ISO-8859-10 Latin6 */
						PG_LATIN7,                                  /* ISO-8859-13 Latin7 */
						PG_LATIN8,                                  /* ISO-8859-14 Latin8 */
						PG_LATIN9,                                  /* ISO-8859-15 Latin9 */
						PG_LATIN10,                                 /* ISO-8859-16 Latin10 */
						PG_WIN1256,                                 /* windows-1256 */
						PG_WIN1258,                                 /* Windows-1258 */
						PG_WIN866,                                  /* (MS-DOS CP866) */
						PG_WIN874,                                  /* windows-874 */
						PG_KOI8R,                                   /* KOI8-R */
						PG_WIN1251,                                 /* windows-1251 */
						PG_WIN1252,                                 /* windows-1252 */
						PG_ISO_8859_5,                              /* ISO-8859-5 */
						PG_ISO_8859_6,                              /* ISO-8859-6 */
						PG_ISO_8859_7,                              /* ISO-8859-7 */
						PG_ISO_8859_8,                              /* ISO-8859-8 */
						PG_WIN1250,                                 /* windows-1250 */
						PG_WIN1253,                                 /* windows-1253 */
						PG_WIN1254,                                 /* windows-1254 */
						PG_WIN1255,                                 /* windows-1255 */
						PG_WIN1257,                                 /* windows-1257 */
																	/* PG_ENCODING_BE_LAST points to the above entry */

																	/* followings are for client encoding only */
						PG_SJIS,                                    /* Shift JIS (Winindows-932) */
						PG_BIG5,                                    /* Big5 (Windows-950) */
						PG_GBK,                                     /* GBK (Windows-936) */
						PG_UHC,                                     /* UHC (Windows-949) */
						PG_GB18030,                                 /* GB18030 */

						_PG_LAST_ENCODING_                          /* mark only */
					};

					const static dodoString encodingStatements[POSTGRESQL_ENCODINGSTATEMENTS];                 ///< encoding string representation according to encodingIdentefiersEnum
#endif

					/**
					 * @struct __blob
					 * @brief defines blob value
					 */
					struct __blob
					{
						unsigned int reference;                         ///< reference in request
						const dodoString *value;                        ///< pointer to blob value
					};

					dodoList<__blob> blobs;                 ///< references to blob data

				private:

					bool empty;                                                     ///< true id pgResult is empty

					PGconn *pgHandle;                                               ///< DB handle
					PGresult *pgResult;                                             ///< handlde to result
			};
		};
	};
};

#endif

#endif
