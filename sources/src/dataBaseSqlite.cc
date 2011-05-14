/***************************************************************************
 *            dataBaseSqlite.cc
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

#ifdef SQLITE3_EXT
#include <sqlite3.h>
#include <string.h>

#include <libdodo/dataBaseSqlite.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/dataBaseSqliteEx.h>
#include <libdodo/xexec.h>
#include <libdodo/toolsString.h>

namespace dodo {
    namespace data {
        namespace base {
            /**
             * @struct __sqlite__
             * @brief defines internal handlers for SQLite DBMS interaction
             */
            struct __sqlite__ {
                /**
                 * constructor
                 */
                __sqlite__() : handle(NULL),
                               result(NULL)
                {
                }

                sqlite3        *handle; ///< DB handle
                sqlite3_stmt   *result; ///< handlde to result
            };
        };
    };
};

using namespace dodo::data::base;

sqlite::__connection_options__::__connection_options__(const dodo::string &path) : path(path)
{
}

//-------------------------------------------------------------------

sqlite::__connection_options__::__connection_options__()
{
}

//-------------------------------------------------------------------

sqlite::sqlite() : handle(new __sqlite__)
{
#ifndef DATABASE_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_DATABASESQLITE);
#endif
}

//-------------------------------------------------------------------

sqlite::sqlite(const data::base::__connection_options__ &a_info) : handle(new __sqlite__),
                                                         info(*dynamic_cast<const sqlite::__connection_options__ *>(&a_info))
{
#ifndef DATABASE_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_DATABASESQLITE);
#endif

    dodo_try {
        if (sqlite3_open(info.path.data(), &handle->handle) != SQLITE_OK) {
            sqlite3_close(handle->handle);

            dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_SQLITE, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);
        }
    } dodo_catch (exception::basic *e UNUSED) {
        delete handle;

        dodo_rethrow;
    }
}

//-------------------------------------------------------------------

sqlite::sqlite(sqlite &)
{
}

//-------------------------------------------------------------------

sqlite::~sqlite()
{
    if (handle->handle != NULL) {
        if (handle->result)
            sqlite3_finalize(handle->result);

        sqlite3_close(handle->handle);
    }

    delete handle;
}

//-------------------------------------------------------------------

void
sqlite::connect(const data::base::__connection_options__ &a_info)
{
    info = *dynamic_cast<const sqlite::__connection_options__ *>(&a_info);

#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_CONNECT);
#endif

    if (handle->handle != NULL) {
        if (handle->result) {
            sqlite3_finalize(handle->result);
            handle->result = NULL;
        }

        if (sqlite3_close(handle->handle) != SQLITE_OK)
            dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_CONNECT, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

        handle->handle = NULL;
    }

    if (sqlite3_open(info.path.data(), &handle->handle) != SQLITE_OK) {
        sqlite3_close(handle->handle);

        dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_CONNECT, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);
    }

#ifndef DATABASE_WO_XEXEC
    performPostExec(OPERATION_CONNECT);
#endif
}

//-------------------------------------------------------------------

void
sqlite::disconnect()
{
    if (handle->handle != NULL) {
#ifndef DATABASE_WO_XEXEC
        performPreExec(OPERATION_DISCONNECT);
#endif

        if (handle->result) {
            sqlite3_finalize(handle->result);
            handle->result = NULL;
        }

        if (sqlite3_close(handle->handle) != SQLITE_OK)
            dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_DISCONNECT, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
        performPostExec(OPERATION_DISCONNECT);
#endif

        handle->handle = NULL;
    }
}

//-------------------------------------------------------------------

void
sqlite::fetchedRows(data::base::rows &a_rows) const
{
    sql::rows *rows = dynamic_cast<sql::rows *>(&a_rows);

    if (handle->handle == NULL || handle->result == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_FETCHEDROWS, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_FETCHEDROWS);
#endif

    rows->fields.clear();
    rows->values.clear();

    sqlite3_reset(handle->result);

    unsigned int numFields = sqlite3_column_count(handle->result);

    if (numFields == 0)
        return;

    bool iterate = true;
    unsigned int i = 0;

    rows->fields.reserve(numFields);

    for (i = 0; i < numFields; ++i)
        rows->fields.push_back(sqlite3_column_name(handle->result, i));

    dodoStringArray values;

    while (iterate) {
        switch (sqlite3_step(handle->result)) {
            case SQLITE_BUSY:

                continue;

            case SQLITE_DONE:

                iterate = false;

                break;

            case SQLITE_ERROR:

                dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_FETCHEDROWS, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

            case SQLITE_ROW:

                values.clear();
                values.reserve(numFields);

                for (i = 0; i < numFields; ++i) {
                    switch (sqlite3_column_type(handle->result, i)) {
                        case SQLITE_INTEGER:

                            values.push_back(tools::string::iToString(sqlite3_column_int(handle->result, i)));

                            break;

                        case SQLITE_FLOAT:

                            values.push_back(tools::string::dToString(sqlite3_column_double(handle->result, i)));

                            break;

                        case SQLITE_TEXT:

                            values.push_back(dodo::string((const char *)sqlite3_column_text(handle->result, i), sqlite3_column_bytes(handle->result, i)));

                            break;

                        case SQLITE_BLOB:

                            values.push_back(dodo::string((const char *)sqlite3_column_blob(handle->result, i), sqlite3_column_bytes(handle->result, i)));

                            break;

                        case SQLITE_NULL:
                        default:

                            values.push_back(statements[STATEMENT_NULL]);

                            break;
                    }
                }

                rows->values.push_back(values);

                break;
        }
    }

#ifndef DATABASE_WO_XEXEC
    performPostExec(OPERATION_FETCHEDROWS);
#endif
}

//-------------------------------------------------------------------

unsigned int
sqlite::fetchedRows() const
{
    if (handle->result == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_FETCHEDROWS, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

    sqlite3_reset(handle->result);

    if (sqlite3_column_count(handle->result) == 0) /// FIXME: dodo_throw exception?
        return 0;

    bool iterate = true;
    unsigned int numRows = 0;

    while (iterate) {
        switch (sqlite3_step(handle->result)) {
            case SQLITE_BUSY:

                continue;

            case SQLITE_DONE:

                iterate = false;

                break;

            case SQLITE_ERROR:

                dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_FETCHEDROWS, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

            case SQLITE_ROW:

                ++numRows;

                break;
        }
    }

    return numRows;
}

//-------------------------------------------------------------------

unsigned int
sqlite::affectedRows() const
{
    if (handle->handle == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_AFFECTEDROWS, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

    return sqlite3_changes(handle->handle);
}

//-------------------------------------------------------------------

void
sqlite::requestFieldsTypes(const dodo::string &table)
{
    if (handle->handle == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_REQUESTFIELDSTYPES, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

    dodoMap<dodo::string, dodoMap<dodo::string, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(table);

    if (types == fieldTypes.end())
        types = fieldTypes.insert(std::make_pair(table, dodoMap<dodo::string, short, dodoMapICaseStringCompare>())).first;

    if (handle->result) {
        sqlite3_finalize(handle->result);
        handle->result = NULL;
    }

#ifdef SQLITE_ENABLE_COLUMN_METADATA
    dodo::string request = "select * from " + table + " limit 1";

    if (sqlite3_prepare(handle->handle, request.data(), request.size(), &handle->result, NULL) != SQLITE_OK)
        dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_REQUESTFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__, request);

    if (handle->result == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_REQUESTFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

    unsigned int numFields = sqlite3_column_count(handle->result);

    const char *columnType, *columnName;

    dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

    for (unsigned int i(0); i < numFields; ++i) {
        columnName = sqlite3_column_name(handle->result, i);

        if (sqlite3_table_column_metadata(handle->handle,
                                          NULL,
                                          table.data(),
                                          columnName,
                                          &columnType,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL) != SQLITE_OK) {
            sqlite3_finalize(handle->result);

            dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_REQUESTFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__, request);
        }

        field = types->second.find(columnName);

        if (field == fieldsEnd) {
            if (strcasestr(columnType, "char") != NULL ||
                strcasestr(columnType, "date") != NULL ||
                strcasestr(columnType, "time") != NULL ||
                strcasestr(columnType, "text") != NULL ||
                strcasestr(columnType, "enum") != NULL ||
                strcasestr(columnType, "set") != NULL)
                types->second.insert(std::make_pair(dodo::string(columnName), sql::FIELD_TEXT));
            else {
                if (strcasestr(columnType, "blob") != NULL)
                    types->second.insert(std::make_pair(dodo::string(columnName), sql::FIELD_BINARY));
                else
                    types->second.insert(std::make_pair(dodo::string(columnName), sql::FIELD_NUMERIC));
            }
        } else {
            if (strcasestr(columnType, "char") != NULL ||
                strcasestr(columnType, "date") != NULL ||
                strcasestr(columnType, "time") != NULL ||
                strcasestr(columnType, "text") != NULL ||
                strcasestr(columnType, "enum") != NULL ||
                strcasestr(columnType, "set") != NULL)
                field->second = sql::FIELD_TEXT;
            else {
                if (strcasestr(columnType, "blob") != NULL)
                    field->second = sql::FIELD_BINARY;
                else
                    field->second = sql::FIELD_NUMERIC;
            }
        }
    }
#else
    dodo::string request = "pragma table_info(" + table + ")";

    if (sqlite3_prepare(handle->handle, request.data(), request.size(), &handle->result, NULL) != SQLITE_OK)
        dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_REQUESTFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__, request);

    if (handle->result == NULL)
        dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_REQUESTFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

    bool iterate = true;

    const char *columnType, *columnName;

    dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

    while (iterate) {
        switch (sqlite3_step(handle->result)) {
            case SQLITE_BUSY:

                continue;

            case SQLITE_DONE:

                iterate = false;

                break;

            case SQLITE_ERROR:

                dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_REQUESTFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

            case SQLITE_ROW:

                columnName = (const char *)sqlite3_column_text(handle->result, 1);
                columnType = (const char *)sqlite3_column_text(handle->result, 2);

                field = types->second.find(columnName);

                if (field == fieldsEnd) {
                    if (strcasestr(columnType, "char") != NULL ||
                        strcasestr(columnType, "date") != NULL ||
                        strcasestr(columnType, "time") != NULL ||
                        strcasestr(columnType, "text") != NULL ||
                        strcasestr(columnType, "enum") != NULL ||
                        strcasestr(columnType, "set") != NULL)
                        types->second.insert(std::make_pair(dodo::string(columnName), sql::FIELD_TEXT));
                    else {
                        if (strcasestr(columnType, "blob") != NULL)
                            types->second.insert(std::make_pair(dodo::string(columnName), sql::FIELD_BINARY));
                        else
                            types->second.insert(std::make_pair(dodo::string(columnName), sql::FIELD_NUMERIC));
                    }
                } else {
                    if (strcasestr(columnType, "char") != NULL ||
                        strcasestr(columnType, "date") != NULL ||
                        strcasestr(columnType, "time") != NULL ||
                        strcasestr(columnType, "text") != NULL ||
                        strcasestr(columnType, "enum") != NULL ||
                        strcasestr(columnType, "set") != NULL)
                        field->second = sql::FIELD_TEXT;
                    else {
                        if (strcasestr(columnType, "blob") != NULL)
                            field->second = sql::FIELD_BINARY;
                        else
                            field->second = sql::FIELD_NUMERIC;
                    }
                }

                break;
        }
    }
#endif

    sqlite3_finalize(handle->result);
    handle->result = NULL;
}

//-------------------------------------------------------------------

void
sqlite::exec()
{
    exec(sql::query(construct()));
}

//-------------------------------------------------------------------

void
sqlite::exec(const query &a_query)
{
    dodo_try {
        collectedData.query = dynamic_cast<const sql::query *>(&a_query);

        if (handle->handle == NULL)
            dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_EXEC, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
        performPreExec(OPERATION_EXEC);
#endif

        if (handle->result) {
            sqlite3_finalize(handle->result);
            handle->result = NULL;
        }

        if (sqlite3_prepare(handle->handle, collectedData.query->sql.data(), collectedData.query->sql.size(), &handle->result, NULL) != SQLITE_OK)
            dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_EXEC, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__, collectedData.query->sql);

        dodo::slList<__blob__>::iterator i(blobs.begin()), j(blobs.end());
        for (; i != j; ++i) {
            if (sqlite3_bind_blob(handle->result, i->reference, i->value->data(), i->value->size(), SQLITE_TRANSIENT) != SQLITE_OK) {
                blobs.clear();

                dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_EXEC, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);
            }
        }

        blobs.clear();

        if (handle->result == NULL)
            dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_EXEC, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

        int status = sqlite3_step(handle->result);
        if (status != SQLITE_DONE && status != SQLITE_ROW)
            dodo_throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_EXEC, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

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
sqlite::insert(const data::base::rows      &rows,
                   const data::base::condition &condition)
{
    constructor::insert(rows, condition);
}

//-------------------------------------------------------------------

void
sqlite::update(const data::base::rows      &rows,
                   const data::base::condition &condition)
{
    constructor::update(rows, condition);
}

//-------------------------------------------------------------------

dodo::string
sqlite::update()
{
    dodo::string request = statements[STATEMENT_UPDATE];
    request += dodo::string(collectedData.condition._table);
    request += dodo::string(statements[STATEMENT_SET]);

    dodoArray<dodoStringArray>::iterator v = collectedData.rows.values.begin();
    if (v != collectedData.rows.values.end()) {
        unsigned int fn(collectedData.rows.fields.size()), fv(v->size());

        unsigned int o(fn <= fv ? fn : fv);

        dodoStringArray::const_iterator i(collectedData.rows.fields.begin()), j(v->begin());
        if (i != j) {
            dodoMap<dodo::string, dodoMap<dodo::string, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.condition._table);
            if (types != fieldTypes.end()) {
                dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator type;
                dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

                __blob__ blob;

                unsigned int k = 1;
                for (; k < o; ++i, ++k, ++j) {
                    request += dodo::string(*i);

                    type = types->second.find(*i);
                    if (type != typesEnd) {
                        if (type->second == sql::FIELD_TEXT) {
                            request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                            request += dodo::string(escapeFields(*j));
                            request += dodo::string(statements[STATEMENT_APOSTROPHECOMA]);
                        } else {
                            if (type->second == sql::FIELD_BINARY) {
                                request += dodo::string(statements[STATEMENT_EQUAL]);
                                request += dodo::string("$" + tools::string::uiToString(k));
                                request += dodo::string(statements[STATEMENT_COMA]);

                                blob.reference = k;
                                blob.value = &(*j);

                                blobs.push(blob);
                            } else {
                                request += dodo::string(statements[STATEMENT_EQUAL]);
                                request += dodo::string(*j);
                                request += dodo::string(statements[STATEMENT_COMA]);
                            }
                        }
                    } else {
                        request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                        request += dodo::string(escapeFields(*j));
                        request += dodo::string(statements[STATEMENT_APOSTROPHECOMA]);
                    }
                }
                request += dodo::string(*i);

                type = types->second.find(*i);
                if (type != typesEnd) {
                    if (type->second == sql::FIELD_TEXT) {
                        request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                        request += dodo::string(escapeFields(*j));
                        request += dodo::string(statements[STATEMENT_APOSTROPHE]);
                    } else {
                        if (type->second == sql::FIELD_BINARY) {
                            request += dodo::string(statements[STATEMENT_EQUAL]);
                            request += dodo::string("$" + tools::string::uiToString(k));
                            request += dodo::string(statements[STATEMENT_COMA]);

                            blob.reference = k;
                            blob.value = &(*j);

                            blobs.push(blob);
                        } else {
                            request += dodo::string(statements[STATEMENT_EQUAL]);
                            request += dodo::string(*j);
                            request += dodo::string(statements[STATEMENT_COMA]);
                        }
                    }
                } else {
                    request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                    request += dodo::string(escapeFields(*j));
                    request += dodo::string(statements[STATEMENT_APOSTROPHE]);
                }
            } else {
                for (unsigned int k(1); k < o; ++i, ++k, ++j) {
                    request += dodo::string(*i);
                    request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                    request += dodo::string(escapeFields(*j));
                    request += dodo::string(statements[STATEMENT_APOSTROPHECOMA]);
                }
                request += dodo::string(*i);
                request += dodo::string(statements[STATEMENT_EQUALAPOSTROPHE]);
                request += dodo::string(escapeFields(*j));
                request += dodo::string(statements[STATEMENT_APOSTROPHE]);
            }
        }
    }

    return request;
}

//-------------------------------------------------------------------

dodo::string
sqlite::insert()
{
    dodo::string request = statements[STATEMENT_INSERT];
    request += dodo::string(statements[STATEMENT_INTO]);
    request += dodo::string(collectedData.condition._table);
    if (collectedData.rows.fields.size() != 0) {
        request += dodo::string(statements[STATEMENT_LEFTBRACKET]);
        request += dodo::string(tools::misc::join(collectedData.rows.fields, statements[STATEMENT_COMA]));
        request += dodo::string(statements[STATEMENT_RIGHTBRACKET]);
    }
    request += dodo::string(statements[STATEMENT_VALUES]);

    dodoArray<dodoStringArray>::iterator k(collectedData.rows.values.begin()), l(collectedData.rows.values.end());
    if (k != l) {
        dodoMap<dodo::string, dodoMap<dodo::string, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.condition._table);
        if (types != fieldTypes.end()) {
            dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator type;
            dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

            dodoStringArray::iterator t;

            __blob__ blob;

            unsigned int o = 0;

            --l;
            for (; k != l; ++k) {
                request += dodo::string(statements[STATEMENT_LEFTBRACKET]);

                t = collectedData.rows.fields.begin();

                dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
                for (; i != j; ++i, ++t) {
                    type = types->second.find(*t);
                    if (type != typesEnd) {
                        if (type->second == sql::FIELD_TEXT)
                            request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                        else {
                            if (type->second == sql::FIELD_BINARY) {
                                ++o;

                                request += dodo::string("$" + tools::string::uiToString(o));
                                request += dodo::string(statements[STATEMENT_COMA]);

                                blob.reference = o;
                                blob.value = &(*i);

                                blobs.push(blob);
                            } else
                                request += dodo::string(*i + statements[STATEMENT_COMA]);
                        }
                    } else
                        request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                }
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == sql::FIELD_TEXT)
                        request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
                    else {
                        if (type->second == sql::FIELD_BINARY) {
                            ++o;

                            request += dodo::string("$" + tools::string::uiToString(o));

                            blob.reference = o;
                            blob.value = &(*i);

                            blobs.push(blob);
                        } else
                            request += dodo::string(*i);
                    }
                } else
                    request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

                request += dodo::string(statements[STATEMENT_RIGHTBRACKETCOMA]);
            }
            request += dodo::string(statements[STATEMENT_LEFTBRACKET]);

            t = collectedData.rows.fields.begin();

            dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
            for (; i != j; ++i, ++t) {
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == sql::FIELD_TEXT)
                        request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                    else {
                        if (type->second == sql::FIELD_BINARY) {
                            ++o;

                            request += dodo::string("$" + tools::string::uiToString(o));
                            request += dodo::string(statements[STATEMENT_COMA]);

                            blob.reference = o;
                            blob.value = &(*i);

                            blobs.push(blob);
                        } else
                            request += dodo::string(*i + statements[STATEMENT_COMA]);
                    }
                } else
                    request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
            }
            type = types->second.find(*t);
            if (type != typesEnd) {
                if (type->second == sql::FIELD_TEXT)
                    request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
                else {
                    if (type->second == sql::FIELD_BINARY) {
                        ++o;

                        request += dodo::string("$" + tools::string::uiToString(o));

                        blob.reference = o;
                        blob.value = &(*i);

                        blobs.push(blob);
                    } else
                        request += dodo::string(*i);
                }
            } else
                request += dodo::string(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

            request += dodo::string(statements[STATEMENT_RIGHTBRACKET]);
        } else {
            --l;
            for (; k != l; ++k) {
                request += dodo::string(statements[STATEMENT_LEFTBRACKET]);
                request += dodo::string(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
                request += dodo::string(statements[STATEMENT_RIGHTBRACKETCOMA]);
            }
            request += dodo::string(statements[STATEMENT_LEFTBRACKET]);
            request += dodo::string(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
            request += dodo::string(statements[STATEMENT_RIGHTBRACKET]);
        }
    }

    return request;
}
#endif

//-------------------------------------------------------------------

