/***************************************************************************
 *            dataBaseSqlite.cc
 *
 *  Thu Apr  30 2005
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

sqlite::sqlite() : handle(new __sqlite__),
                   empty(true)
{
#ifndef DATABASE_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_DATABASESQLITE);
#endif
}

//-------------------------------------------------------------------

sqlite::sqlite(const __connection__ &info) : handle(new __sqlite__),
                                             empty(true)
{
#ifndef DATABASE_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_DATABASESQLITE);
#endif

    collectedData.dbInfo = info;

    if (sqlite3_open(collectedData.dbInfo.path.data(), &handle->handle) != SQLITE_OK) {
        sqlite3_close(handle->handle);

        delete handle;

        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_SQLITE, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);
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
        if (!empty)
            sqlite3_finalize(handle->result);

        sqlite3_close(handle->handle);
    }

    delete handle;
}

//-------------------------------------------------------------------

void
sqlite::connect(const __connection__ &info)
{
    collectedData.dbInfo = info;

#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_CONNECT);
#endif

    if (handle->handle != NULL) {
        if (!empty) {
            empty = true;
            sqlite3_finalize(handle->result);
        }

        if (sqlite3_close(handle->handle) != SQLITE_OK)
            throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_CONNECT, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

        handle->handle = NULL;
    }

    if (sqlite3_open(collectedData.dbInfo.path.data(), &handle->handle) != SQLITE_OK) {
        sqlite3_close(handle->handle);

        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_CONNECT, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);
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

        if (!empty) {
            empty = true;
            sqlite3_finalize(handle->result);
        }

        if (sqlite3_close(handle->handle) != SQLITE_OK)
            throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_DISCONNECT, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
        performPostExec(OPERATION_DISCONNECT);
#endif

        handle->handle = NULL;
    }
}

//-------------------------------------------------------------------

dodoArray<dodo::dodoStringArray>
sqlite::fetchRows() const
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_FETCHROWS);
#endif

    dodoArray<dodoStringArray> rows;

    if (!show)
        return rows;

    sqlite3_reset(handle->result);

    unsigned int numFields = sqlite3_column_count(handle->result);

    bool iterate = true;
    unsigned int i = 0;

#ifndef USE_DEQUE
    rows.reserve(sqlite3_data_count(handle->result));
#endif

    dodoStringArray rowsPart;

    while (iterate) {
        switch (sqlite3_step(handle->result)) {
            case SQLITE_BUSY:

                continue;

            case SQLITE_DONE:

                iterate = false;

                break;

            case SQLITE_ERROR:

                throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_FETCHROWS, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

            case SQLITE_ROW:

                rowsPart.clear();

#ifndef USE_DEQUE
                rowsPart.reserve(numFields);
#endif

                for (i = 0; i < numFields; ++i) {
                    switch (sqlite3_column_type(handle->result, i)) {
                        case SQLITE_INTEGER:

                            rowsPart.push_back(tools::string::iToString(sqlite3_column_int(handle->result, i)));

                            break;

                        case SQLITE_FLOAT:

                            rowsPart.push_back(tools::string::dToString(sqlite3_column_double(handle->result, i)));

                            break;

                        case SQLITE_TEXT:

                            rowsPart.push_back(dodoString((const char *)sqlite3_column_text(handle->result, i), sqlite3_column_bytes(handle->result, i)));

                            break;

                        case SQLITE_BLOB:

                            rowsPart.push_back(dodoString((const char *)sqlite3_column_blob(handle->result, i), sqlite3_column_bytes(handle->result, i)));

                            break;

                        case SQLITE_NULL:
                        default:

                            rowsPart.push_back(statements[STATEMENT_NULL]);

                            break;
                    }
                }

                rows.push_back(rowsPart);

                break;
        }
    }

#ifndef DATABASE_WO_XEXEC
    performPostExec(OPERATION_FETCHROWS);
#endif

    return rows;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
sqlite::fetchFields() const
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_FETCHFIELDS);
#endif

    dodoStringArray fields;

    if (!show)
        return fields;

    unsigned int numFields = sqlite3_column_count(handle->result);

#ifndef USE_DEQUE
    fields.reserve(numFields);
#endif

    for (unsigned int i(0); i < numFields; ++i)
        fields.push_back(sqlite3_column_name(handle->result, i));

#ifndef DATABASE_WO_XEXEC
    performPostExec(OPERATION_FETCHFIELDS);
#endif

    return fields;
}

//-------------------------------------------------------------------

__tuples__
sqlite::fetch() const
{
    return __tuples__(fetchRows(), fetchFields());
}

//-------------------------------------------------------------------

unsigned int
sqlite::requestedRows() const
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

    if (show) {
        sqlite3_reset(handle->result);

        unsigned int numRows = 0;
        bool iterate = true;
        int result;

        while (iterate) {
            result = sqlite3_step(handle->result);
            switch (result) {
                case SQLITE_BUSY:

                    continue;

                case SQLITE_DONE:

                    iterate = false;

                    break;

                case SQLITE_ERROR:

                    throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_FETCHROWS, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

                case SQLITE_ROW:

                    ++numRows;

                    break;
            }
        }

        return numRows;
    } else
        return 0;
}

//-------------------------------------------------------------------

unsigned int
sqlite::requestedFields() const
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

    if (show)
        return sqlite3_column_count(handle->result);
    else
        return 0;
}

//-------------------------------------------------------------------

unsigned int
sqlite::affectedRows() const
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

    if (!show)
        return sqlite3_changes(handle->handle);
    else
        return 0;
}

//-------------------------------------------------------------------

void
sqlite::requestFieldsTypes(const dodoString &table)
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

    dodoString temp = collectedData.dbInfo.db + ":" + table;

    dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(temp);

    if (types == fieldTypes.end())
        types = fieldTypes.insert(make_pair(temp, dodoMap<dodoString, short, dodoMapICaseStringCompare>())).first;

#ifdef SQLITE_ENABLE_COLUMN_METADATA
    request = "select * from " + table + " limit 1";

    if (!empty) {
        sqlite3_finalize(handle->result);
        empty = true;
    }

    if (sqlite3_prepare(handle->handle, request.data(), request.size(), &handle->result, NULL) != SQLITE_OK)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__, request);

    if (handle->result == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

    empty = false;

    unsigned int numFields = sqlite3_column_count(handle->result);

    const char *columnType, *columnName;

    dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

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
            if (!empty) {
                sqlite3_finalize(handle->result);
                empty = true;
            }

            throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__, request);
        }

        field = types->second.find(columnName);

        if (field == fieldsEnd) {
            if (strcasestr(columnType, "char") != NULL ||
                strcasestr(columnType, "date") != NULL ||
                strcasestr(columnType, "time") != NULL ||
                strcasestr(columnType, "text") != NULL ||
                strcasestr(columnType, "enum") != NULL ||
                strcasestr(columnType, "set") != NULL)
                types->second.insert(make_pair(dodoString(columnName), sql::FIELD_TEXT));
            else {
                if (strcasestr(columnType, "blob") != NULL)
                    types->second.insert(make_pair(dodoString(columnName), sql::FIELD_BINARY));
                else
                    types->second.insert(make_pair(dodoString(columnName), sql::FIELD_NUMERIC));
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

    if (!empty) {
        sqlite3_finalize(handle->result);
        empty = true;
    }
#else
    request = "pragma table_info(" + table + ")";

    if (!empty) {
        sqlite3_finalize(handle->result);
        empty = true;
    }

    if (sqlite3_prepare(handle->handle, request.data(), request.size(), &handle->result, NULL) != SQLITE_OK)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__, request);

    if (handle->result == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

    empty = false;

    bool iterate = true;

    const char *columnType, *columnName;

    dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

    while (iterate) {
        switch (sqlite3_step(handle->result)) {
            case SQLITE_BUSY:

                continue;

            case SQLITE_DONE:

                iterate = false;

                break;

            case SQLITE_ERROR:

                throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

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
                        types->second.insert(make_pair(dodoString(columnName), sql::FIELD_TEXT));
                    else {
                        if (strcasestr(columnType, "blob") != NULL)
                            types->second.insert(make_pair(dodoString(columnName), sql::FIELD_BINARY));
                        else
                            types->second.insert(make_pair(dodoString(columnName), sql::FIELD_NUMERIC));
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

    if (!empty) {
        sqlite3_finalize(handle->result);
        empty = true;
    }
#endif

    request.clear();
}

//-------------------------------------------------------------------

void
sqlite::exec(const dodoString &query,
             bool             result)
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_EXEC);
#endif

    if (query.size() == 0)
        construct();
    else {
        request = query;
        show = result;
    }

    if (!empty) {
        sqlite3_finalize(handle->result);
        empty = true;
    }

    if (sqlite3_prepare(handle->handle, request.data(), request.size(), &handle->result, NULL) != SQLITE_OK)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_EXEC, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__, request);

    dodoList<__blob__>::iterator i(blobs.begin()), j(blobs.end());
    for (; i != j; ++i) {
        if (sqlite3_bind_blob(handle->result, i->reference, i->value->data(), i->value->size(), SQLITE_TRANSIENT) != SQLITE_OK) {
            blobs.clear();

            throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_EXEC, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);
        }
    }

    blobs.clear();

    if (handle->result == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_EXEC, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

    empty = false;

    if (!show)
        if (sqlite3_step(handle->result) != SQLITE_DONE)
            throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_EXEC, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
    performPostExec(OPERATION_EXEC);
#endif

    cleanCollected();
    request.clear();
}

//-------------------------------------------------------------------

void
sqlite::updateCollect()
{
    request = statements[STATEMENT_UPDATE];
    request.append(collectedData.table);
    request.append(statements[STATEMENT_SET]);

    dodoArray<dodoStringArray>::iterator v = collectedData.values.begin();
    if (v != collectedData.values.end()) {
        unsigned int fn(collectedData.fields.size()), fv(v->size());

        unsigned int o(fn <= fv ? fn : fv);

        dodoStringArray::const_iterator i(collectedData.fields.begin()), j(v->begin());
        if (i != j) {
            dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.dbInfo.db + statements[STATEMENT_COLON] + collectedData.table);
            if (types != fieldTypes.end()) {
                dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
                dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

                __blob__ blob;

                unsigned int k = 1;
                for (; k < o; ++i, ++k, ++j) {
                    request.append(*i);

                    type = types->second.find(*i);
                    if (type != typesEnd) {
                        if (type->second == sql::FIELD_TEXT) {
                            request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                            request.append(escapeFields(*j));
                            request.append(statements[STATEMENT_APOSTROPHECOMA]);
                        } else {
                            if (type->second == sql::FIELD_BINARY) {
                                request.append(statements[STATEMENT_EQUAL]);
                                request.append("$" + tools::string::uiToString(k));
                                request.append(statements[STATEMENT_COMA]);

                                blob.reference = k;
                                blob.value = &(*j);

                                blobs.push_back(blob);
                            } else {
                                request.append(statements[STATEMENT_EQUAL]);
                                request.append(*j);
                                request.append(statements[STATEMENT_COMA]);
                            }
                        }
                    } else {
                        request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                        request.append(escapeFields(*j));
                        request.append(statements[STATEMENT_APOSTROPHECOMA]);
                    }
                }
                request.append(*i);

                type = types->second.find(*i);
                if (type != typesEnd) {
                    if (type->second == sql::FIELD_TEXT) {
                        request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                        request.append(escapeFields(*j));
                        request.append(statements[STATEMENT_APOSTROPHE]);
                    } else {
                        if (type->second == sql::FIELD_BINARY) {
                            request.append(statements[STATEMENT_EQUAL]);
                            request.append("$" + tools::string::uiToString(k));
                            request.append(statements[STATEMENT_COMA]);

                            blob.reference = k;
                            blob.value = &(*j);

                            blobs.push_back(blob);
                        } else {
                            request.append(statements[STATEMENT_EQUAL]);
                            request.append(*j);
                            request.append(statements[STATEMENT_COMA]);
                        }
                    }
                } else {
                    request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                    request.append(escapeFields(*j));
                    request.append(statements[STATEMENT_APOSTROPHE]);
                }
            } else {
                for (unsigned int k(1); k < o; ++i, ++k, ++j) {
                    request.append(*i);
                    request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                    request.append(escapeFields(*j));
                    request.append(statements[STATEMENT_APOSTROPHECOMA]);
                }
                request.append(*i);
                request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
                request.append(escapeFields(*j));
                request.append(statements[STATEMENT_APOSTROPHE]);
            }
        }
    }
}

//-------------------------------------------------------------------

void
sqlite::insertCollect()
{
    request = statements[STATEMENT_INSERT];
    request.append(statements[STATEMENT_INTO]);
    request.append(collectedData.table);
    if (collectedData.fields.size() != 0) {
        request.append(statements[STATEMENT_LEFTBRACKET]);
        request.append(tools::misc::join(collectedData.fields, statements[STATEMENT_COMA]));
        request.append(statements[STATEMENT_RIGHTBRACKET]);
    }
    request.append(statements[STATEMENT_VALUES]);

    dodoArray<dodoStringArray>::iterator k(collectedData.values.begin()), l(collectedData.values.end());
    if (k != l) {
        dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.dbInfo.db + statements[STATEMENT_COLON] + collectedData.table);
        if (types != fieldTypes.end()) {
            dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
            dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

            dodoStringArray::iterator t;

            __blob__ blob;

            unsigned int o = 0;

            --l;
            for (; k != l; ++k) {
                request.append(statements[STATEMENT_LEFTBRACKET]);

                t = collectedData.fields.begin();

                dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
                for (; i != j; ++i, ++t) {
                    type = types->second.find(*t);
                    if (type != typesEnd) {
                        if (type->second == sql::FIELD_TEXT)
                            request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                        else {
                            if (type->second == sql::FIELD_BINARY) {
                                ++o;

                                request.append("$" + tools::string::uiToString(o));
                                request.append(statements[STATEMENT_COMA]);

                                blob.reference = o;
                                blob.value = &(*i);

                                blobs.push_back(blob);
                            } else
                                request.append(*i + statements[STATEMENT_COMA]);
                        }
                    } else
                        request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                }
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == sql::FIELD_TEXT)
                        request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
                    else {
                        if (type->second == sql::FIELD_BINARY) {
                            ++o;

                            request.append("$" + tools::string::uiToString(o));

                            blob.reference = o;
                            blob.value = &(*i);

                            blobs.push_back(blob);
                        } else
                            request.append(*i);
                    }
                } else
                    request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

                request.append(statements[STATEMENT_RIGHTBRACKETCOMA]);
            }
            request.append(statements[STATEMENT_LEFTBRACKET]);

            t = collectedData.fields.begin();

            dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
            for (; i != j; ++i, ++t) {
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == sql::FIELD_TEXT)
                        request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
                    else {
                        if (type->second == sql::FIELD_BINARY) {
                            ++o;

                            request.append("$" + tools::string::uiToString(o));
                            request.append(statements[STATEMENT_COMA]);

                            blob.reference = o;
                            blob.value = &(*i);

                            blobs.push_back(blob);
                        } else
                            request.append(*i + statements[STATEMENT_COMA]);
                    }
                } else
                    request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
            }
            type = types->second.find(*t);
            if (type != typesEnd) {
                if (type->second == sql::FIELD_TEXT)
                    request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
                else {
                    if (type->second == sql::FIELD_BINARY) {
                        ++o;

                        request.append("$" + tools::string::uiToString(o));

                        blob.reference = o;
                        blob.value = &(*i);

                        blobs.push_back(blob);
                    } else
                        request.append(*i);
                }
            } else
                request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

            request.append(statements[STATEMENT_RIGHTBRACKET]);
        } else {
            --l;
            for (; k != l; ++k) {
                request.append(statements[STATEMENT_LEFTBRACKET]);
                request.append(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
                request.append(statements[STATEMENT_RIGHTBRACKETCOMA]);
            }
            request.append(statements[STATEMENT_LEFTBRACKET]);
            request.append(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
            request.append(statements[STATEMENT_RIGHTBRACKET]);
        }
    }
}

//-------------------------------------------------------------------

dodo::dodoStringMapArray
sqlite::fetchFieldsToRows() const
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, SQLITEEX_NOTOPENED, DATABASESQLITEEX_NOTOPENED_STR, __LINE__, __FILE__);

    dodoStringMapArray rowsFields;

    if (!show)
        return rowsFields;

    sqlite3_reset(handle->result);

    unsigned int numFields = sqlite3_column_count(handle->result);

    bool iterate = true;
    unsigned int i = 0;

    dodoStringMap rowFieldsPart;
    dodoString rowPart;

#ifndef USE_DEQUE
    rowsFields.reserve(sqlite3_data_count(handle->result));
#endif

    while (iterate) {
        switch (sqlite3_step(handle->result)) {
            case SQLITE_BUSY:

                continue;

            case SQLITE_DONE:

                iterate = false;

                break;

            case SQLITE_ERROR:

                throw exception::basic(exception::MODULE_DATABASESQLITE, SQLITEEX_FETCHFIELDSTOROWS, exception::ERRNO_SQLITE, sqlite3_errcode(handle->handle), sqlite3_errmsg(handle->handle), __LINE__, __FILE__);

            case SQLITE_ROW:

                rowFieldsPart.clear();

                for (i = 0; i < numFields; ++i) {
                    switch (sqlite3_column_type(handle->result, i)) {
                        case SQLITE_INTEGER:

                            rowFieldsPart.insert(make_pair(sqlite3_column_name(handle->result, i), tools::string::iToString(sqlite3_column_int(handle->result, i))));

                            break;

                        case SQLITE_FLOAT:

                            rowFieldsPart.insert(make_pair(sqlite3_column_name(handle->result, i), tools::string::dToString(sqlite3_column_double(handle->result, i))));

                            break;

                        case SQLITE_TEXT:

                            rowFieldsPart.insert(make_pair(sqlite3_column_name(handle->result, i), dodoString((const char *)sqlite3_column_text(handle->result, i), sqlite3_column_bytes(handle->result, i))));

                            break;

                        case SQLITE_BLOB:

                            rowFieldsPart.insert(make_pair(sqlite3_column_name(handle->result, i), dodoString((const char *)sqlite3_column_blob(handle->result, i), sqlite3_column_bytes(handle->result, i))));

                            break;

                        case SQLITE_NULL:
                        default:

                            rowFieldsPart.insert(make_pair(dodoString(sqlite3_column_name(handle->result, i)), statements[STATEMENT_NULL]));

                            break;
                    }
                }

                rowsFields.push_back(rowFieldsPart);

                break;
        }
    }

    return rowsFields;
}
#endif

//-------------------------------------------------------------------
