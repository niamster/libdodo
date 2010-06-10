/***************************************************************************
 *            dataBasePostgresql.cc
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

#include <libdodo/directives.h>

#ifdef POSTGRESQL_EXT
#include <libpq-fe.h>
#include <stdlib.h>
#include <string.h>

#include <libdodo/dataBasePostgresql.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/dataBasePostgresqlEx.h>
#include <libdodo/xexec.h>

namespace dodo {
    namespace data {
        namespace base {
            /**
             * @struct __postgresql__
             * @brief defines internal handlers for MySQL DBMS interaction
             */
            struct __postgresql__ {
                /**
                 * constructor
                 */
                __postgresql__() : handle(NULL),
                                   result(NULL)
                {
                }

                PGconn             *handle; ///< DB handle
                PGresult           *result; ///< handlde to result
            };
        };
    };
};

using namespace dodo::data::base;

#ifdef POSTGRESQL_NO_ENCODINGTOCHAR
const dodo::string postgresql::encodingStatements[] = {
    "SQL_ASCII",
    "EUC_JP",
    "EUC_CN",
    "EUC_KR",
    "EUC_TW",
    "JOHAB",
    "UTF8",
    "MULE_INTERNAL",
    "LATIN1",
    "LATIN2",
    "LATIN3",
    "LATIN4",
    "LATIN5",
    "LATIN6",
    "LATIN7",
    "LATIN8",
    "LATIN9",
    "LATIN10",
    "WIN1256",
    "WIN1258",
    "WIN866",
    "WIN874",
    "KOI8",
    "WIN1251",
    "WIN1252",
    "ISO_8859_5",
    "ISO_8859_6",
    "ISO_8859_7",
    "ISO_8859_8",
    "WIN1250",
    "WIN1253",
    "WIN1254",
    "WIN1255",
    "WIN1257",
    "SJIS",
    "BIG5",
    "GBK",
    "UHC",
    "GB18030"
};
#endif

//-------------------------------------------------------------------

postgresql::__connection_options__::__connection_options__(const dodo::string &db,
                                                           const dodo::string &host,
                                                           const dodo::string &user,
                                                           const dodo::string &password,
                                                           unsigned int     port) : db(db),
                                                                                    host(host),
                                                                                    user(user),
                                                                                    password(password),
                                                                                    port(port)
{
}

//-------------------------------------------------------------------

postgresql::__connection_options__::__connection_options__()
{
}

//-------------------------------------------------------------------

postgresql::postgresql() : handle(new __postgresql__)
{
#ifndef DATABASE_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_DATABASEPOSTGRESQL);
#endif
}

//-------------------------------------------------------------------

postgresql::postgresql(const data::base::__connection_options__ &a_info) : handle(new __postgresql__),
                                                                           info(*dynamic_cast<const postgresql::__connection_options__ *>(&a_info))
{
#ifndef DATABASE_WO_XEXEC
    collectedData.setExecObject(xexec::OBJECT_DATABASEPOSTGRESQL);
#endif

    handle->handle = PQsetdbLogin(
        info.host.size() == 0 ? NULL : info.host.data(),
        tools::string::uiToString(info.port).data(),
        NULL,
        NULL,
        info.db.size() == 0 ? NULL : info.db.data(),
        info.user.size() == 0 ? NULL : info.user.data(),
        info.password.size() == 0 ? NULL : info.password.data());

    int status = PQstatus(handle->handle);

    try {
        if (status != CONNECTION_OK) {
            throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_POSTGRESQL, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);
        }
    } catch (...) {
        delete handle;

        throw;
    }
}

//-------------------------------------------------------------------

postgresql::postgresql(postgresql &)
{
}

//-------------------------------------------------------------------

postgresql::~postgresql()
{
    if (handle->handle != NULL) {
        if (handle->result)
            PQclear(handle->result);

        PQfinish(handle->handle);
    }

    delete handle;
}

//-------------------------------------------------------------------

void
postgresql::connect(const data::base::__connection_options__ &a_info)
{
#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_CONNECT);
#endif

    info = *dynamic_cast<const postgresql::__connection_options__ *>(&a_info);

    if (handle->handle != NULL) {
        if (handle->result) {
            PQclear(handle->result);
            handle->result = NULL;
        }

        PQfinish(handle->handle);

        handle->handle = NULL;
    }

    handle->handle = PQsetdbLogin(
        info.host.size() == 0 ? NULL : info.host.data(),
        tools::string::uiToString(info.port).data(),
        NULL,
        NULL,
        info.db.size() == 0 ? NULL : info.db.data(),
        info.user.size() == 0 ? NULL : info.user.data(),
        info.password.size() == 0 ? NULL : info.password.data());

    int status = PQstatus(handle->handle);

    if (status != CONNECTION_OK) {
        handle->handle = NULL;

        throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_CONNECT, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);
    }

#ifndef DATABASE_WO_XEXEC
    performPostExec(OPERATION_CONNECT);
#endif
}

//-------------------------------------------------------------------

void
postgresql::disconnect()
{
    if (handle->handle != NULL) {
#ifndef DATABASE_WO_XEXEC
        performPreExec(OPERATION_DISCONNECT);
#endif

        if (handle->result) {
            PQclear(handle->result);
            handle->result = NULL;
        }

        PQfinish(handle->handle);

#ifndef DATABASE_WO_XEXEC
        performPostExec(OPERATION_DISCONNECT);
#endif

        handle->handle = NULL;
    }
}

//-------------------------------------------------------------------

void
postgresql::fetchedRows(data::base::rows &a_rows) const
{
    sql::rows *rows = dynamic_cast<sql::rows *>(&a_rows);

#ifndef DATABASE_WO_XEXEC
    performPreExec(OPERATION_FETCHEDROWS);
#endif

    rows->fields.clear();
    rows->values.clear();

    if (!handle->result) // FIXME: throw exception?
        return;

    int rowsNum = PQntuples(handle->result);
    int fieldsNum = PQnfields(handle->result);

#ifndef USE_DEQUE
    rows->fields.reserve(fieldsNum);
    rows->values.reserve(rowsNum);
#endif

    dodoStringArray values;

#ifndef USE_DEQUE
    values.reserve(fieldsNum);
#endif

    for (int i(0); i < fieldsNum; ++i)
        rows->fields.push_back(PQfname(handle->result, i));

    int j;
    for (int i(0); i < rowsNum; ++i) {
        for (j = 0; j < fieldsNum; ++j) {
            if (PQgetisnull(handle->result, i, j) == 1)
                values.push_back(statements[sql::constructor::STATEMENT_NULL]);
            else
                values.push_back(dodo::string(PQgetvalue(handle->result, i, j), PQgetlength(handle->result, i, j)));
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
postgresql::fetchedRows() const
{
    if (handle->result == NULL)
        throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_FETCHEDROWS, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    return PQntuples(handle->result);
}

//-------------------------------------------------------------------

unsigned int
postgresql::affectedRows() const
{
    if (handle->result == NULL)
        throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_AFFECTEDROWS, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    return atoi(PQcmdTuples(handle->result));
}

//-------------------------------------------------------------------

void
postgresql::requestFieldsTypes(const dodo::string &table)
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_REQUESTFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    dodoMap<dodo::string, dodoMap<dodo::string, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(table);

    if (types == fieldTypes.end())
        types = fieldTypes.insert(std::make_pair(table, dodoMap<dodo::string, short, dodoMapICaseStringCompare>())).first;

    dodo::string request = "select column_name, data_type from information_schema.columns where table_name='" + table + "'";

    if (handle->result)
        PQclear(handle->result);

    handle->result = PQexecParams(handle->handle, request.data(), 0, NULL, NULL, NULL, NULL, 1);
    if (handle->result == NULL)
        throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_REQUESTFIELDSTYPES, exception::ERRNO_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(handle->handle), __LINE__, __FILE__, request);

    int status = PQresultStatus(handle->result);

    switch (status) {
        case PGRES_EMPTY_QUERY:
        case PGRES_BAD_RESPONSE:
        case PGRES_NONFATAL_ERROR:
        case PGRES_FATAL_ERROR:

            throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_REQUESTFIELDSTYPES, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);
    }

    int rowsNum = PQntuples(handle->result);

    const char *columnType, *columnName;

    dodoMap<dodo::string, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

    dodoStringArray rowsPart;

    for (int i(0); i < rowsNum; ++i) {
        columnName = PQgetvalue(handle->result, i, 0);
        columnType = PQgetvalue(handle->result, i, 1);

        field = types->second.find(columnName);

        if (field == fieldsEnd) {
            if (strcasestr(columnType, "char") != NULL ||
                strcasestr(columnType, "date") != NULL ||
                strcasestr(columnType, "text") != NULL)
                types->second.insert(std::make_pair(dodo::string(columnName), sql::FIELD_TEXT));
            else {
                if (strcasestr(columnType, "bytea") != NULL ||
                    strcasestr(columnType, "array") != NULL ||
                    strcasestr(columnType, "cidr") != NULL ||
                    strcasestr(columnType, "macaddrcd") != NULL ||
                    strcasestr(columnType, "inet") != NULL)
                    types->second.insert(std::make_pair(dodo::string(columnName), sql::FIELD_BINARY));
                else
                    types->second.insert(std::make_pair(dodo::string(columnName), sql::FIELD_NUMERIC));
            }
        } else {
            if (strcasestr(columnType, "char") != NULL ||
                strcasestr(columnType, "date") != NULL ||
                strcasestr(columnType, "text") != NULL)
                field->second = sql::FIELD_TEXT;
            else {
                if (strcasestr(columnType, "bytea") != NULL ||
                    strcasestr(columnType, "array") != NULL ||
                    strcasestr(columnType, "cidr") != NULL ||
                    strcasestr(columnType, "macaddrcd") != NULL ||
                    strcasestr(columnType, "inet") != NULL)
                    field->second = sql::FIELD_BINARY;
                else
                    field->second = sql::FIELD_NUMERIC;
            }
        }
    }

    PQclear(handle->result);
    handle->result = NULL;
}

//-------------------------------------------------------------------

void
postgresql::exec()
{
    exec(sql::query(construct()));
}

//-------------------------------------------------------------------

void
postgresql::exec(const query &a_query)
    try
    {
        collectedData.query = dynamic_cast<const sql::query *>(&a_query);

        if (handle->handle == NULL)
            throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_EXEC, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);

#ifndef DATABASE_WO_XEXEC
        performPreExec(OPERATION_EXEC);
#endif

        int status;

        if (handle->result)
            PQclear(handle->result);

        unsigned long size = blobs.size();

        if (size > 0) {
            char **values = new char*[size];
            int *lengths = new int[size];
            int *formats = new int[size];

            dodoList<__blob__>::iterator i(blobs.begin()), j(blobs.end());
            for (int o = 0; i != j; ++i, ++o) {
                values[o] = (char *)i->value->data();
                lengths[o] = i->value->size();
                formats[o] = 1;
            }

            handle->result = PQexecParams(handle->handle, collectedData.query->sql.data(), size, NULL, values, lengths, formats, 0);

            blobs.clear();
        } else {
            handle->result = PQexecParams(handle->handle, collectedData.query->sql.data(), 0, NULL, NULL, NULL, NULL, 1);
        }

        if (handle->result == NULL)
            throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_EXEC, exception::ERRNO_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(handle->handle), __LINE__, __FILE__, collectedData.query->sql);

        status = PQresultStatus(handle->result);
        switch (status) {
            case PGRES_EMPTY_QUERY:
            case PGRES_BAD_RESPONSE:
            case PGRES_NONFATAL_ERROR:
            case PGRES_FATAL_ERROR:

                throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_EXEC, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);

            /* case PGRES_COMMAND_OK: */

            /*     PQclear(handle->result); */
            /*     handle->result = NULL; */

            /*     break; */
        }

#ifndef DATABASE_WO_XEXEC
        performPostExec(OPERATION_EXEC);
#endif

        collectedData.clear();
    } catch (...) {
        collectedData.clear();
    }

//-------------------------------------------------------------------

void
postgresql::insert(const data::base::rows      &rows,
                   const data::base::condition &condition)
{
    constructor::insert(rows, condition);
}

//-------------------------------------------------------------------

void
postgresql::update(const data::base::rows      &rows,
                   const data::base::condition &condition)
{
    constructor::update(rows, condition);
}

//-------------------------------------------------------------------

dodo::string
postgresql::update()
{
    dodo::string request = statements[sql::constructor::STATEMENT_UPDATE];
    request += dodo::string(collectedData.condition._table);
    request += dodo::string(statements[sql::constructor::STATEMENT_SET]);

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
                            request += dodo::string(statements[sql::constructor::STATEMENT_EQUALAPOSTROPHE]);
                            request += dodo::string(escapeFields(*j));
                            request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHECOMA]);
                        } else {
                            if (type->second == sql::FIELD_BINARY) {
                                request += dodo::string(statements[sql::constructor::STATEMENT_EQUAL]);
                                request += dodo::string("$" + tools::string::uiToString(k));
                                request += dodo::string(statements[sql::constructor::STATEMENT_COMA]);

                                blob.reference = k;
                                blob.value = &(*j);

                                blobs.push_back(blob);
                            } else {
                                request += dodo::string(statements[sql::constructor::STATEMENT_EQUAL]);
                                request += dodo::string(*j);
                                request += dodo::string(statements[sql::constructor::STATEMENT_COMA]);
                            }
                        }
                    } else {
                        request += dodo::string(statements[sql::constructor::STATEMENT_EQUALAPOSTROPHE]);
                        request += dodo::string(escapeFields(*j));
                        request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHECOMA]);
                    }
                }
                request += dodo::string(*i);

                type = types->second.find(*i);
                if (type != typesEnd) {
                    if (type->second == sql::FIELD_TEXT) {
                        request += dodo::string(statements[sql::constructor::STATEMENT_EQUALAPOSTROPHE]);
                        request += dodo::string(escapeFields(*j));
                        request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE]);
                    } else {
                        if (type->second == sql::FIELD_BINARY) {
                            request += dodo::string(statements[sql::constructor::STATEMENT_EQUAL]);
                            request += dodo::string("$" + tools::string::uiToString(k));
                            request += dodo::string(statements[sql::constructor::STATEMENT_COMA]);

                            blob.reference = k;
                            blob.value = &(*j);

                            blobs.push_back(blob);
                        } else {
                            request += dodo::string(statements[sql::constructor::STATEMENT_EQUAL]);
                            request += dodo::string(*j);
                            request += dodo::string(statements[sql::constructor::STATEMENT_COMA]);
                        }
                    }
                } else {
                    request += dodo::string(statements[sql::constructor::STATEMENT_EQUALAPOSTROPHE]);
                    request += dodo::string(escapeFields(*j));
                    request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE]);
                }
            } else {
                for (unsigned int k(1); k < o; ++i, ++k, ++j) {
                    request += dodo::string(*i);
                    request += dodo::string(statements[sql::constructor::STATEMENT_EQUALAPOSTROPHE]);
                    request += dodo::string(escapeFields(*j));
                    request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHECOMA]);
                }
                request += dodo::string(*i);
                request += dodo::string(statements[sql::constructor::STATEMENT_EQUALAPOSTROPHE]);
                request += dodo::string(escapeFields(*j));
                request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE]);
            }
        }
    }

    return request;
}

//-------------------------------------------------------------------

dodo::string
postgresql::insert()
{
    dodo::string request = statements[sql::constructor::STATEMENT_INSERT];
    request += dodo::string(statements[sql::constructor::STATEMENT_INTO]);
    request += dodo::string(collectedData.condition._table);
    if (collectedData.rows.fields.size() != 0) {
        request += dodo::string(statements[sql::constructor::STATEMENT_LEFTBRACKET]);
        request += dodo::string(tools::misc::join(collectedData.rows.fields, statements[sql::constructor::STATEMENT_COMA]));
        request += dodo::string(statements[sql::constructor::STATEMENT_RIGHTBRACKET]);
    }
    request += dodo::string(statements[sql::constructor::STATEMENT_VALUES]);

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
                request += dodo::string(statements[sql::constructor::STATEMENT_LEFTBRACKET]);

                t = collectedData.rows.fields.begin();

                dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
                for (; i != j; ++i, ++t) {
                    type = types->second.find(*t);
                    if (type != typesEnd) {
                        if (type->second == sql::FIELD_TEXT)
                            request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[sql::constructor::STATEMENT_APOSTROPHECOMA]);
                        else {
                            if (type->second == sql::FIELD_BINARY) {
                                ++o;

                                request += dodo::string("$" + tools::string::uiToString(o));
                                request += dodo::string(statements[sql::constructor::STATEMENT_COMA]);

                                blob.reference = o;
                                blob.value = &(*i);

                                blobs.push_back(blob);
                            } else
                                request += dodo::string(*i + statements[sql::constructor::STATEMENT_COMA]);
                        }
                    } else
                        request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[sql::constructor::STATEMENT_APOSTROPHECOMA]);
                }
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == sql::FIELD_TEXT)
                        request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[sql::constructor::STATEMENT_APOSTROPHE]);
                    else {
                        if (type->second == sql::FIELD_BINARY) {
                            ++o;

                            request += dodo::string("$" + tools::string::uiToString(o));

                            blob.reference = o;
                            blob.value = &(*i);

                            blobs.push_back(blob);
                        } else
                            request += dodo::string(*i);
                    }
                } else
                    request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[sql::constructor::STATEMENT_APOSTROPHE]);

                request += dodo::string(statements[sql::constructor::STATEMENT_RIGHTBRACKETCOMA]);
            }
            request += dodo::string(statements[sql::constructor::STATEMENT_LEFTBRACKET]);

            t = collectedData.rows.fields.begin();

            dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
            for (; i != j; ++i, ++t) {
                type = types->second.find(*t);
                if (type != typesEnd) {
                    if (type->second == sql::FIELD_TEXT)
                        request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[sql::constructor::STATEMENT_APOSTROPHECOMA]);
                    else {
                        if (type->second == sql::FIELD_BINARY) {
                            ++o;

                            request += dodo::string("$" + tools::string::uiToString(o));
                            request += dodo::string(statements[sql::constructor::STATEMENT_COMA]);

                            blob.reference = o;
                            blob.value = &(*i);

                            blobs.push_back(blob);
                        } else
                            request += dodo::string(*i + statements[sql::constructor::STATEMENT_COMA]);
                    }
                } else
                    request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[sql::constructor::STATEMENT_APOSTROPHECOMA]);
            }
            type = types->second.find(*t);
            if (type != typesEnd) {
                if (type->second == sql::FIELD_TEXT)
                    request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[sql::constructor::STATEMENT_APOSTROPHE]);
                else {
                    if (type->second == sql::FIELD_BINARY) {
                        ++o;

                        request += dodo::string("$" + tools::string::uiToString(o));

                        blob.reference = o;
                        blob.value = &(*i);

                        blobs.push_back(blob);
                    } else
                        request += dodo::string(*i);
                }
            } else
                request += dodo::string(statements[sql::constructor::STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[sql::constructor::STATEMENT_APOSTROPHE]);

            request += dodo::string(statements[sql::constructor::STATEMENT_RIGHTBRACKET]);
        } else {
            --l;
            for (; k != l; ++k) {
                request += dodo::string(statements[sql::constructor::STATEMENT_LEFTBRACKET]);
                request += dodo::string(joinFields(*k, statements[sql::constructor::STATEMENT_COMA], statements[sql::constructor::STATEMENT_APOSTROPHE]));
                request += dodo::string(statements[sql::constructor::STATEMENT_RIGHTBRACKETCOMA]);
            }
            request += dodo::string(statements[sql::constructor::STATEMENT_LEFTBRACKET]);
            request += dodo::string(joinFields(*k, statements[sql::constructor::STATEMENT_COMA], statements[sql::constructor::STATEMENT_APOSTROPHE]));
            request += dodo::string(statements[sql::constructor::STATEMENT_RIGHTBRACKET]);
        }
    }

    return request;
}

//-------------------------------------------------------------------

void
postgresql::setCharset(const dodo::string &charset)
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_SETCHARSET, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);

    int status = PQsetClientEncoding(handle->handle, charset.data());
    if (status == -1)
        throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_SETCHARSET, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

dodo::string
postgresql::charset() const
{
    if (handle->handle == NULL)
        throw exception::basic(exception::MODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_CHARSET, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);

#ifdef POSTGRESQL_NO_ENCODINGTOCHAR
    int encoding = PQclientEncoding(handle->handle);

    if (encoding >= 0 && encoding < ENCODING_ENUMSIZE)
        return encodingStatements[encoding];

    return __dodostring__;
#else
    return pg_encoding_to_char(PQclientEncoding(handle->handle));
#endif
}
#endif

//-------------------------------------------------------------------
