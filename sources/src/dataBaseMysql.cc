/***************************************************************************
 *            dataBaseMysql.cc
 *
 *  Thu Apr  30 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#include <libdodo/directives.h>

#ifdef MYSQL_EXT
#include <mysql.h>
#include <errmsg.h>
#include <string.h>

#include <libdodo/dataBaseMysql.h>
#include <libdodo/dataBaseMysqlEx.h>

namespace dodo {
    namespace data {
        namespace base {
            /**
             * @struct __mysql__
             * @brief defines internal handlers for MySQL DBMS interaction
             */
            struct __mysql__ {
                /**
                 * constructor
                 */
                __mysql__() : handle(NULL),
                              result(NULL)
                {
                }

                MYSQL         *handle;  ///< DB handle
                MYSQL_RES     *result;  ///< handle to result
            };
        };
    };
};

using namespace dodo::data::base;

mysql::__connection_options__::__connection_options__(const dodo::string &db,
                                                      const dodo::string &host,
                                                      const dodo::string &user,
                                                      const dodo::string &password,
                                                      const dodo::string &path,
                                                      unsigned int     port,
                                                      unsigned long    type) : type(type),
                                                                               db(db),
                                                                               host(host),
                                                                               user(user),
                                                                               password(password),
                                                                               path(path),
                                                                               port(port)
{
}

//-------------------------------------------------------------------

mysql::__connection_options__::__connection_options__() : type(TYPE_MULTI_STATEMENTS)
{
}

//-------------------------------------------------------------------

mysql::mysql() : handle(new __mysql__)

{
#ifndef DATABASE_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_DATABASEMYSQL);
#endif

    handle->handle = mysql_init(NULL);
}

//-------------------------------------------------------------------

mysql::mysql(const data::base::__connection_options__ &a_info) : handle(new __mysql__),
                                                                 info(*dynamic_cast<const mysql::__connection_options__ *>(&a_info))
{
#ifndef DATABASE_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_DATABASEMYSQL);
#endif

    handle->handle = mysql_init(NULL);

    unsigned long type = 0;

    if (info.type & __connection_options__::TYPE_MULTI_STATEMENTS)
        type |= CLIENT_MULTI_STATEMENTS;

    if (info.type & __connection_options__::TYPE_COMPRESS)
        type |= CLIENT_COMPRESS;

    if (mysql_ssl_set(handle->handle,
                      info.ssl.key.size() == 0 ? NULL : info.ssl.key.data(),
                      info.ssl.cert.size() == 0 ? NULL : info.ssl.cert.data(),
                      info.ssl.ca.size() == 0 ? NULL : info.ssl.ca.data(),
                      info.ssl.capath.size() == 0 ? NULL : info.ssl.capath.data(),
                      info.ssl.cipher.size() == 0 ? NULL : info.ssl.cipher.data()) == 0)
        type |= CLIENT_SSL;

    dodo_try {
        if (!mysql_real_connect(handle->handle,
                                info.host.size() == 0 ? NULL : info.host.data(),
                                info.user.size() == 0 ? NULL : info.user.data(),
                                info.password.size() == 0 ? NULL : info.password.data(),
                                info.db.size() == 0 ? NULL : info.db.data(),
                                info.port,
                                info.path.size() == 0 ? NULL : info.path.data(),
                                type))
            dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_MYSQL, exception::ERRNO_MYSQL, mysql_errno(handle->handle), mysql_error(handle->handle), __LINE__, __FILE__);
    } dodo_catch (exception::basic *e UNUSED) {
        mysql_close(handle->handle);

        delete handle;

        dodo_rethrow;
    }

#ifndef MYSQL_NO_OPT_RECONNECT
    if (reconnect) {
        my_bool rc = 1;

        mysql_options(handle->handle, MYSQL_OPT_RECONNECT, &rc);
    }
#endif
}

//-------------------------------------------------------------------

mysql::mysql(mysql &)
{
}

//-------------------------------------------------------------------

mysql::~mysql()
{
    if (handle->handle != NULL) {
        if (handle->result)
            mysql_free_result(handle->result);

        mysql_close(handle->handle);
    }

    delete handle;
}

//-------------------------------------------------------------------

void
mysql::connect(const data::base::__connection_options__ &a_info)
{
#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_CONNECT);
#endif

    info = *dynamic_cast<const mysql::__connection_options__ *>(&a_info);

    if (handle->handle != NULL) {
        if (handle->result) {
            mysql_free_result(handle->result);
            handle->result = NULL;
        }

        mysql_close(handle->handle);
        handle->handle = NULL;
    }

    handle->handle = mysql_init(NULL);

    unsigned long type = 0;

    if (info.type & __connection_options__::TYPE_MULTI_STATEMENTS)
        type |= CLIENT_MULTI_STATEMENTS;

    if (info.type & __connection_options__::TYPE_COMPRESS)
        type |= CLIENT_COMPRESS;

    if (mysql_ssl_set(handle->handle,
                      info.ssl.key.size() == 0 ? NULL : info.ssl.key.data(),
                      info.ssl.cert.size() == 0 ? NULL : info.ssl.cert.data(),
                      info.ssl.ca.size() == 0 ? NULL : info.ssl.ca.data(),
                      info.ssl.capath.size() == 0 ? NULL : info.ssl.capath.data(),
                      info.ssl.cipher.size() == 0 ? NULL : info.ssl.cipher.data()) == 0)
        type |= CLIENT_SSL;

    if (!mysql_real_connect(handle->handle,
                            info.host.size() == 0 ? NULL : info.host.data(),
                            info.user.size() == 0 ? NULL : info.user.data(),
                            info.password.size() == 0 ? NULL : info.password.data(),
                            info.db.size() == 0 ? NULL : info.db.data(),
                            info.port,
                            info.path.size() == 0 ? NULL : info.path.data(),
                            type))
        dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_CONNECT, exception::ERRNO_MYSQL, mysql_errno(handle->handle), mysql_error(handle->handle), __LINE__, __FILE__);

#ifndef MYSQL_NO_OPT_RECONNECT
    if (reconnect) {
        my_bool rc = 1;

        mysql_options(handle->handle, MYSQL_OPT_RECONNECT, &rc);
    }
#endif

#ifndef DATABASE_WO_XEXEC
    performPostExec(OPERATION_CONNECT);
#endif
}

//-------------------------------------------------------------------

void
mysql::disconnect()
{
    if (handle->handle != NULL) {
#ifndef DATABASE_WO_XEXEC
        performPreExec(OPERATION_DISCONNECT);
#endif

        if (handle->result) {
            mysql_free_result(handle->result);
            handle->result = NULL;
        }

        mysql_close(handle->handle);
        handle->handle = NULL;

#ifndef DATABASE_WO_XEXEC
        performPostExec(OPERATION_DISCONNECT);
#endif
    }
}

//-------------------------------------------------------------------

void
mysql::fetchedRows(data::base::rows &a_rows) const
{
    sql::rows *rows = dynamic_cast<sql::rows *>(&a_rows);

#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_FETCHEDROWS);
#endif

    rows->fields.clear();
    rows->values.clear();

    if (!handle->result) // FIXME: dodo_throw exception?
        return;

    mysql_data_seek(handle->result, 0);
    mysql_field_seek(handle->result, 0);

    unsigned int numFields = mysql_num_fields(handle->result);
    MYSQL_FIELD *mysqlFields = mysql_fetch_fields(handle->result);

#ifndef USE_DEQUE
    rows->fields.reserve(numFields);

    rows->values.reserve(mysql_num_rows(handle->result));
#endif

    unsigned long *length, j;

    dodoStringArray values;

    MYSQL_ROW mysqlRow;

#ifndef USE_DEQUE
    values.reserve(numFields);
#endif

    for (unsigned int i(0); i < numFields; ++i)
        rows->fields.push_back(mysqlFields[i].name);

    while ((mysqlRow = mysql_fetch_row(handle->result)) != NULL) {
        length = mysql_fetch_lengths(handle->result);

        for (j = 0; j < numFields; ++j) {
            if (mysqlRow[j] != NULL)
                values.push_back(dodo::string(mysqlRow[j], length[j]));
            else
                values.push_back(statements[sql::constructor::STATEMENT_NULL]);
        }

        rows->values.push_back(values);

        values.clear();
    }

#ifndef DATABASE_WO_XEXEC
    performPostExec(OPERATION_FETCHEDROWS);
#endif
}

//-------------------------------------------------------------------

unsigned int
mysql::fetchedRows() const
{
    if (handle->result == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_FETCHEDROWS, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    return mysql_num_rows(handle->result);
}

//-------------------------------------------------------------------

unsigned int
mysql::affectedRows() const
{
    if (handle->handle == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_AFFECTEDROWS, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    return mysql_affected_rows(handle->handle);
}

//-------------------------------------------------------------------

void
mysql::requestFieldsTypes(const dodo::string &table)
{
    if (handle->handle == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_REQUESTFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    dodoMap<dodo::string, dodoMap<dodo::string, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(table);

    if (types == fieldTypes.end())
        types = fieldTypes.insert(std::make_pair(table, dodoMap<dodo::string, short, dodoMapICaseStringCompare>())).first;

    dodo::string request = "describe " + table;

    if (mysql_real_query(handle->handle, request.data(), request.size()) != 0) {
        int mysqlErrno = mysql_errno(handle->handle);
        if (reconnect && (mysqlErrno == CR_SERVER_GONE_ERROR || mysqlErrno == CR_SERVER_LOST)) {
            connect(info);
            if (mysql_real_query(handle->handle, request.data(), request.size()) != 0)
                dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_REQUESTFIELDSTYPES, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(handle->handle), __LINE__, __FILE__, request);
        } else
            dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_REQUESTFIELDSTYPES, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(handle->handle), __LINE__, __FILE__, request);
    }

    handle->result = mysql_store_result(handle->handle);
    if (handle->result == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_REQUESTFIELDSTYPES, exception::ERRNO_MYSQL, mysql_errno(handle->handle), mysql_error(handle->handle), __LINE__, __FILE__);

    MYSQL_ROW mysqlRow;

    dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

    while ((mysqlRow = mysql_fetch_row(handle->result)) != NULL) {
        field = types->second.find(mysqlRow[0]);

        if (field == fieldsEnd) {
            if (strcasestr(mysqlRow[1], "char") != NULL ||
                strcasestr(mysqlRow[1], "date") != NULL ||
                strcasestr(mysqlRow[1], "time") != NULL ||
                strcasestr(mysqlRow[1], "text") != NULL ||
                strcasestr(mysqlRow[1], "enum") != NULL ||
                strcasestr(mysqlRow[1], "set") != NULL) {
                types->second.insert(std::make_pair(dodo::string(mysqlRow[0]), sql::FIELD_TEXT));
            } else {
                if (strcasestr(mysqlRow[1], "blob") != NULL)
                    types->second.insert(std::make_pair(dodo::string(mysqlRow[0]), sql::FIELD_BINARY));
                else
                    types->second.insert(std::make_pair(dodo::string(mysqlRow[0]), sql::FIELD_NUMERIC));
            }
        } else {
            if (strcasestr(mysqlRow[1], "char") != NULL ||
                strcasestr(mysqlRow[1], "date") != NULL ||
                strcasestr(mysqlRow[1], "time") != NULL ||
                strcasestr(mysqlRow[1], "text") != NULL ||
                strcasestr(mysqlRow[1], "enum") != NULL ||
                strcasestr(mysqlRow[1], "set") != NULL) {
                field->second = sql::FIELD_TEXT;
            } else {
                if (strcasestr(mysqlRow[1], "blob") != NULL)
                    field->second = sql::FIELD_BINARY;
                else
                    field->second = sql::FIELD_NUMERIC;
            }
        }
    }

    mysql_free_result(handle->result);
    handle->result = NULL;
}

//-------------------------------------------------------------------

void
mysql::exec()
{
    exec(sql::query(construct()));
}

//-------------------------------------------------------------------

void
mysql::exec(const query &a_query)
{
    dodo_try {
        collectedData.query = dynamic_cast<const sql::query *>(&a_query);

        if (handle->handle == NULL)
            dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_EXEC, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
        performPreExec(OPERATION_EXEC);
#endif
        if (handle->result) {
            mysql_free_result(handle->result);
            handle->result = NULL;
        }

        if (mysql_real_query(handle->handle, collectedData.query->sql.data(), collectedData.query->sql.size()) != 0) {
            int mysqlErrno = mysql_errno(handle->handle);
            if (reconnect && (mysqlErrno == CR_SERVER_GONE_ERROR || mysqlErrno == CR_SERVER_LOST)) {
                connect(info);
                if (mysql_real_query(handle->handle, collectedData.query->sql.data(), collectedData.query->sql.size()) != 0)
                    dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_EXEC, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(handle->handle), __LINE__, __FILE__, collectedData.query->sql);
            } else {
                dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_EXEC, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(handle->handle), __LINE__, __FILE__, collectedData.query->sql);
            }
        }

        if (mysql_field_count(handle->handle) != 0) {
            handle->result = mysql_store_result(handle->handle);
            if (handle->result == NULL)
                dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_EXEC, exception::ERRNO_MYSQL, mysql_errno(handle->handle), mysql_error(handle->handle), __LINE__, __FILE__);
        }

#ifndef DATABASE_WO_XEXEC
        performPostExec(OPERATION_EXEC);
#endif

        collectedData.clear();
    } dodo_catch (exception::basic *e UNUSED) {
        collectedData.clear();

        dodo_rethrow;
    }
}

//-------------------------------------------------------------------

void
mysql::setCharset(const dodo::string &charset)
{
    if (handle->handle == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_SETCHARSET, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    mysql_options(handle->handle, MYSQL_SET_CHARSET_NAME, charset.data());
}

//-------------------------------------------------------------------

void
mysql::setConnectionTimeout(unsigned int time)
{
    if (handle->handle == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_SETCONNECTIONTIMEOUT, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    mysql_options(handle->handle, MYSQL_OPT_CONNECT_TIMEOUT, (char *)&time);
}

//-------------------------------------------------------------------

dodo::string
mysql::charset() const
{
    if (handle->handle == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASEMYSQL, MYSQLEX_CHARSET, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    return mysql_character_set_name(handle->handle);
}

#endif

//-------------------------------------------------------------------

