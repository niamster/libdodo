/***************************************************************************
 *            dataBasePostgresql.h
 *
 *  Fri Jan  13 2006
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
#include <libdodo/types.h>
#include <libdodo/dataBaseSqlConstructor.h>

namespace dodo {
    namespace data {
        namespace base {
            struct __postgresql__;

            /**
             * @class postgresql
             * @brief provides an interface to postgresql db
             */
            class postgresql : public sql::constructor {
              public:

                /**
                 * @struct __connection_options__
                 * @brief defines connection options for the server
                 */
                struct __connection_options__ : public data::base::__connection_options__ {
                    /**
                     * constructor
                     */
                    __connection_options__();

                    /**
                     * constructor
                     * @param db defines name of db
                     * @param host defines host
                     * @param user defines user
                     * @param password defines password
                     * @param path defines path to db or unix socket
                     * @param port defines port
                     */
                    __connection_options__(const dodoString &db,
                                           const            dodoString &host,
                                           const            dodoString &user,
                                           const            dodoString &password,
                                           unsigned int                port = 0);

                    dodoString   db;        ///< database name
                    dodoString   host;      ///< hostname
                    dodoString   user;      ///< username
                    dodoString   password;  ///< password
                    unsigned int port;      ///< port
                };

              private:

                /**
                 * copy constructor
                 * @note to prevent copying
                 */
                postgresql(postgresql &);

              public:

                /**
                 * constructor
                 */
                postgresql();

                /**
                 * constructor
                 * @param info defines information for connection to db
                 */
                postgresql(const data::base::__connection_options__ &info);

                /**
                 * destructor
                 */
                virtual ~postgresql();

                /**
                 * connect to the database
                 * @param info defines information for connection to db
                 */
                virtual void connect(const data::base::__connection_options__ &info);

                /**
                 * disconnect from the database
                 */
                virtual void disconnect();

                /**
                 * automaticaly detect fields types
                 * @param table defines table for which rules will be applied
                 */
                virtual void requestFieldsTypes(const dodoString &table);

                /**
                 * @return amount of affected rows from the evaluated request
                 */
                virtual unsigned int affectedRows() const;

                /**
                 * @return amount of received rows from the evaluated request
                 */
                virtual unsigned int fetchedRows() const;

                /**
                 * @param rows defines rows got from the request
                 */
                virtual void fetchedRows(data::base::rows &rows) const;

                /**
                 * execute request for data base
                 * @param query contains query for data base
                 */
                virtual void exec(const data::base::query &query);

                /**
                 * execute collected request for data base
                 */
                virtual void exec();

                /**
                 * set sessions charset
                 * @param charset defines charset
                 */
                void setCharset(const dodoString &charset);

                /**
                 * @return current session charset
                 */
                dodoString charset() const;

                /**
                 * @param rows defines rows for insertion into data base
                 * @param condition defines row insertion condition
                 */
                virtual void insert(const data::base::rows      &rows,
                                    const data::base::condition &condition);

                /**
                 * @param rows defines values of row for update
                 * @param condition defines row update condition
                 */
                virtual void update(const data::base::rows      &rows,
                                    const data::base::condition &condition);

              protected:

                /**
                 * @return insert SQL statement
                 */
                virtual dodoString insert();

                /**
                 * @return update SQL statement
                 */
                virtual dodoString update();

#ifdef POSTGRESQL_NO_ENCODINGTOCHAR
                /**
                 * @enum encodingIdentefiersEnum defines postgresql encoding identifiers
                 * @note fetched from mb/pg_wchar.h
                 */
                enum encodingIdentefiersEnum {
                    ENCODING_SQL_ASCII = 0,                                                             /* SQL/ASCII */
                    ENCODING_EUC_JP,                                                                    /* EUC for Japanese */
                    ENCODING_EUC_CN,                                                                    /* EUC for Chinese */
                    ENCODING_EUC_KR,                                                                    /* EUC for Korean */
                    ENCODING_EUC_TW,                                                                    /* EUC for Taiwan */
                    ENCODING_JOHAB,                                                                     /* EUC for Korean JOHAB */
                    ENCODING_UTF8,                                                                      /* Unicode UTF8 */
                    ENCODING_MULE_INTERNAL,                                                             /* Mule internal code */
                    ENCODING_LATIN1,                                                                    /* ISO-8859-1 Latin 1 */
                    ENCODING_LATIN2,                                                                    /* ISO-8859-2 Latin 2 */
                    ENCODING_LATIN3,                                                                    /* ISO-8859-3 Latin 3 */
                    ENCODING_LATIN4,                                                                    /* ISO-8859-4 Latin 4 */
                    ENCODING_LATIN5,                                                                    /* ISO-8859-9 Latin 5 */
                    ENCODING_LATIN6,                                                                    /* ISO-8859-10 Latin6 */
                    ENCODING_LATIN7,                                                                    /* ISO-8859-13 Latin7 */
                    ENCODING_LATIN8,                                                                    /* ISO-8859-14 Latin8 */
                    ENCODING_LATIN9,                                                                    /* ISO-8859-15 Latin9 */
                    ENCODING_LATIN10,                                                                   /* ISO-8859-16 Latin10 */
                    ENCODING_WIN1256,                                                                   /* windows-1256 */
                    ENCODING_WIN1258,                                                                   /* Windows-1258 */
                    ENCODING_WIN866,                                                                    /* (MS-DOS CP866) */
                    ENCODING_WIN874,                                                                    /* windows-874 */
                    ENCODING_KOI8R,                                                                     /* KOI8-R */
                    ENCODING_WIN1251,                                                                   /* windows-1251 */
                    ENCODING_WIN1252,                                                                   /* windows-1252 */
                    ENCODING_ISO_8859_5,                                                                /* ISO-8859-5 */
                    ENCODING_ISO_8859_6,                                                                /* ISO-8859-6 */
                    ENCODING_ISO_8859_7,                                                                /* ISO-8859-7 */
                    ENCODING_ISO_8859_8,                                                                /* ISO-8859-8 */
                    ENCODING_WIN1250,                                                                   /* windows-1250 */
                    ENCODING_WIN1253,                                                                   /* windows-1253 */
                    ENCODING_WIN1254,                                                                   /* windows-1254 */
                    ENCODING_WIN1255,                                                                   /* windows-1255 */
                    ENCODING_WIN1257,                                                                   /* windows-1257 */

                                                                                                        /* followings are for client encoding only */
                    ENCODING_SJIS,                                                                      /* Shift JIS (Winindows-932) */
                    ENCODING_BIG5,                                                                      /* Big5 (Windows-950) */
                    ENCODING_GBK,                                                                       /* GBK (Windows-936) */
                    ENCODING_UHC,                                                                       /* UHC (Windows-949) */
                    ENCODING_GB18030,                                                                   /* GB18030 */

                    ENCODING_ENUMSIZE
                };

                const static dodoString encodingStatements[ENCODING_ENUMSIZE];                          ///< encoding string representation according to encodingIdentefiersEnum
#endif

                /**
                 * @struct __blob__
                 * @brief defines blob value
                 */
                struct __blob__ {
                    unsigned int     reference;                                                         ///< reference in request
                    const dodoString *value;                                                            ///< pointer to blob value
                };

                dodoList<__blob__> blobs;                                                               ///< references to blob data

              private:

                __postgresql__ *handle;                                                                 ///< DB handle

                __connection_options__ info; ///< DB connection information
            };
        };
    };
};
#endif
#endif
