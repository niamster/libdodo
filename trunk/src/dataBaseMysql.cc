/***************************************************************************
 *            dataBaseMysql.cc
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

#include <libdodo/directives.h>

#ifdef MYSQL_EXT
#include <mysql.h>
#include <errmsg.h>

#include <libdodo/dataBaseMysql.h>
#include <libdodo/dataBaseMysqlEx.h>

namespace dodo
{
	namespace data
	{
		namespace base
		{
			struct __mysqlHandle
			{
				MYSQL	  *handle; ///< DB handle
				MYSQL_RES *result; ///< handle to result
			};
		};
	};
};

using namespace dodo::data::base;

mysql::mysql() : empty(true),
				 type(CLIENT_MULTI_STATEMENTS),
				 handle(new __mysqlHandle)

{
#ifndef DATABASE_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_DATABASEMYSQL);
#endif

	handle->handle = mysql_init(NULL);
}

//-------------------------------------------------------------------

mysql::mysql(const __connectionInfo &info) : empty(true),
											 type(CLIENT_MULTI_STATEMENTS),
											 handle(new __mysqlHandle)

{
#ifndef DATABASE_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_DATABASEMYSQL);
#endif

	collectedData.dbInfo = info;

	handle->handle = mysql_init(NULL);

	if (!mysql_real_connect(handle->handle,
							collectedData.dbInfo.host.size() == 0 ? NULL : collectedData.dbInfo.host.c_str(),
							collectedData.dbInfo.user.size() == 0 ? NULL : collectedData.dbInfo.user.c_str(),
							collectedData.dbInfo.password.size() == 0 ? NULL : collectedData.dbInfo.password.c_str(),
							collectedData.dbInfo.db.size() == 0 ? NULL : collectedData.dbInfo.db.c_str(),
							collectedData.dbInfo.port,
							collectedData.dbInfo.path.size() == 0 ? NULL : collectedData.dbInfo.path.c_str(),
							type))
	{
		delete handle;

		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_MYSQL, exception::ERRNO_MYSQL, mysql_errno(handle->handle), mysql_error(handle->handle), __LINE__, __FILE__);
	}

#ifndef MYSQL_NO_OPT_RECONNECT
	if (reconnect)
	{
		my_bool rc = 1;

		mysql_options(handle->handle, MYSQL_OPT_RECONNECT, &rc);
	}
#endif
}

//-------------------------------------------------------------------

mysql::mysql(mysql &a_mypp)
{
}

//-------------------------------------------------------------------

mysql::~mysql()
{
	if (handle->handle != NULL)
	{
		if (!empty)
		{
			mysql_free_result(handle->result);
		}

		mysql_close(handle->handle);
	}

	delete handle;
}

//-------------------------------------------------------------------

void
mysql::connectSettings(unsigned long           a_type,
					   const __mysqlSslOptions &options)
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	type = a_type;

	if (mysql_ssl_set(handle->handle,
					  options.key.size() == 0 ? NULL : options.key.c_str(),
					  options.cert.size() == 0 ? NULL : options.cert.c_str(),
					  options.ca.size() == 0 ? NULL : options.ca.c_str(),
					  options.capath.size() == 0 ? NULL : options.capath.c_str(),
					  options.cipher.size() == 0 ? NULL : options.cipher.c_str()) == 0)
	{
		type |= CLIENT_SSL;
	}
}

//-------------------------------------------------------------------

void
mysql::connect(const __connectionInfo &info)
{
	collectedData.dbInfo = info;

#ifndef DATABASE_WO_XEXEC
	operType = DATABASE_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	if (handle->handle != NULL)
	{
		if (!empty)
		{
			empty = true;
			mysql_free_result(handle->result);
		}

		mysql_close(handle->handle);

		handle->handle = NULL;
	}

	handle->handle = mysql_init(NULL);

	if (!mysql_real_connect(handle->handle,
							collectedData.dbInfo.host.size() == 0 ? NULL : collectedData.dbInfo.host.c_str(),
							collectedData.dbInfo.user.size() == 0 ? NULL : collectedData.dbInfo.user.c_str(),
							collectedData.dbInfo.password.size() == 0 ? NULL : collectedData.dbInfo.password.c_str(),
							collectedData.dbInfo.db.size() == 0 ? NULL : collectedData.dbInfo.db.c_str(),
							collectedData.dbInfo.port,
							collectedData.dbInfo.path.size() == 0 ? NULL : collectedData.dbInfo.path.c_str(),
							type))
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_CONNECT, exception::ERRNO_MYSQL, mysql_errno(handle->handle), mysql_error(handle->handle), __LINE__, __FILE__);
	}

#ifndef MYSQL_NO_OPT_RECONNECT
	if (reconnect)
	{
		my_bool rc = 1;

		mysql_options(handle->handle, MYSQL_OPT_RECONNECT, &rc);
	}
#endif

#ifndef DATABASE_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
mysql::disconnect()
{
	if (handle->handle != NULL)
	{
#ifndef DATABASE_WO_XEXEC
		operType = DATABASE_OPERATION_DISCONNECT;
		performXExec(preExec);
#endif

		if (!empty)
		{
			empty = true;
			mysql_free_result(handle->result);
		}

		mysql_close(handle->handle);

#ifndef DATABASE_WO_XEXEC
		performXExec(postExec);
#endif

		handle->handle = NULL;
	}
}

//-------------------------------------------------------------------

dodoArray<dodo::dodoStringArray>
mysql::fetchRows() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

#ifndef DATABASE_WO_XEXEC
	operType = DATABASE_OPERATION_FETCHROW;
	performXExec(preExec);
#endif

	dodoArray<dodoStringArray> rows;

	if (empty || !show)
	{
		return rows;
	}

	mysql_data_seek(handle->result, 0);

	unsigned int numFields = mysql_num_fields(handle->result);

#ifndef USE_DEQUE
	rows.reserve(mysql_num_rows(handle->result));
#endif

	unsigned long *length, j;

	dodoStringArray rowsPart;

	MYSQL_ROW mysqlRow;

#ifndef USE_DEQUE
	rowsPart.reserve(numFields);
#endif

	while ((mysqlRow = mysql_fetch_row(handle->result)) != NULL)
	{
		length = mysql_fetch_lengths(handle->result);

		rowsPart.clear();

		for (j = 0; j < numFields; ++j)
		{
			if (mysqlRow[j] != NULL)
			{
				rowsPart.push_back(dodoString(mysqlRow[j], length[j]));
			}
			else
			{
				rowsPart.push_back(statements[SQLCONSTRUCTOR_STATEMENT_NULL]);
			}
		}

		rows.push_back(rowsPart);
	}

#ifndef DATABASE_WO_XEXEC
	performXExec(postExec);
#endif

	return rows;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
mysql::fetchFields() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

#ifndef DATABASE_WO_XEXEC
	operType = DATABASE_OPERATION_FETCHFIELD;
	performXExec(preExec);
#endif

	dodoStringArray fields;

	if (empty || !show)
	{
		return fields;
	}

	mysql_field_seek(handle->result, 0);

	unsigned int numFields = mysql_num_fields(handle->result);
	MYSQL_FIELD *mysqlFields = mysql_fetch_fields(handle->result);

#ifndef USE_DEQUE
	fields.reserve(numFields);
#endif

	for (unsigned int i(0); i < numFields; ++i)
	{
		fields.push_back(mysqlFields[i].name);
	}

#ifndef DATABASE_WO_XEXEC
	performXExec(postExec);
#endif

	return fields;
}

//-------------------------------------------------------------------

__tuples
mysql::fetch() const
{
	return __tuples(fetchRows(), fetchFields());
}

//-------------------------------------------------------------------

unsigned int
mysql::rowsCount() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	if (empty || !show)
	{
		return 0;
	}
	else
	{
		return mysql_num_rows(handle->result);
	}
}

//-------------------------------------------------------------------

unsigned int
mysql::fieldsCount() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	if (empty || !show)
	{
		return 0;
	}
	else
	{
		return mysql_num_fields(handle->result);
	}
}

//-------------------------------------------------------------------

unsigned int
mysql::affectedRowsCount() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	if (empty || show)
	{
		return 0;
	}
	else
	{
		return mysql_affected_rows(handle->handle);
	}
}

//-------------------------------------------------------------------

void
mysql::getFieldsTypes(const dodoString &table)
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	dodoString temp = collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + table;

	dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(temp);

	if (types == fieldTypes.end())
	{
		types = fieldTypes.insert(make_pair(temp, dodoMap<dodoString, short, dodoMapICaseStringCompare>())).first;
	}

	request = "describe " + table;

	if (mysql_real_query(handle->handle, request.c_str(), request.size()) != 0)
	{
		int mysqlErrno = mysql_errno(handle->handle);
		if (reconnect && (mysqlErrno == CR_SERVER_GONE_ERROR || mysqlErrno == CR_SERVER_LOST))
		{
			connect(collectedData.dbInfo);
			if (mysql_real_query(handle->handle, request.c_str(), request.size()) != 0)
			{
				throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(handle->handle), __LINE__, __FILE__, request);
			}
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(handle->handle), __LINE__, __FILE__, request);
		}
	}

	handle->result = mysql_store_result(handle->handle);
	if (handle->result == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_MYSQL, mysql_errno(handle->handle), mysql_error(handle->handle), __LINE__, __FILE__);
	}

	empty = false;

	MYSQL_ROW mysqlRow;

	dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

	while ((mysqlRow = mysql_fetch_row(handle->result)) != NULL)
	{
		field = types->second.find(mysqlRow[0]);

		if (field == fieldsEnd)
		{
			if (strcasestr(mysqlRow[1], "char") != NULL ||
				strcasestr(mysqlRow[1], "date") != NULL ||
				strcasestr(mysqlRow[1], "time") != NULL ||
				strcasestr(mysqlRow[1], "text") != NULL ||
				strcasestr(mysqlRow[1], "enum") != NULL ||
				strcasestr(mysqlRow[1], "set") != NULL)
			{
				types->second.insert(make_pair(dodoString(mysqlRow[0]), sql::FIELDTYPE_TEXT));
			}
			else
			{
				if (strcasestr(mysqlRow[1], "blob") != NULL)
				{
					types->second.insert(make_pair(dodoString(mysqlRow[0]), sql::FIELDTYPE_BINARY));
				}
				else
				{
					types->second.insert(make_pair(dodoString(mysqlRow[0]), sql::FIELDTYPE_NUMERIC));
				}
			}
		}
		else
		{
			if (strcasestr(mysqlRow[1], "char") != NULL ||
				strcasestr(mysqlRow[1], "date") != NULL ||
				strcasestr(mysqlRow[1], "time") != NULL ||
				strcasestr(mysqlRow[1], "text") != NULL ||
				strcasestr(mysqlRow[1], "enum") != NULL ||
				strcasestr(mysqlRow[1], "set") != NULL)
			{
				field->second = sql::FIELDTYPE_TEXT;
			}
			else
			{
				if (strcasestr(mysqlRow[1], "blob") != NULL)
				{
					field->second = sql::FIELDTYPE_BINARY;
				}
				else
				{
					field->second = sql::FIELDTYPE_NUMERIC;
				}
			}
		}
	}

	mysql_free_result(handle->result);

	empty = true;
}

//-------------------------------------------------------------------

void
mysql::exec(const dodoString &query,
			bool             result)
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

#ifndef DATABASE_WO_XEXEC
	operType = DATABASE_OPERATION_EXEC;
	performXExec(preExec);
#endif

	if (query.size() == 0)
	{
		queryCollect();
	}
	else
	{
		request = query;
		show = result;
	}

	if (mysql_real_query(handle->handle, request.c_str(), request.size()) != 0)
	{
		int mysqlErrno = mysql_errno(handle->handle);
		if (reconnect && (mysqlErrno == CR_SERVER_GONE_ERROR || mysqlErrno == CR_SERVER_LOST))
		{
			connect(collectedData.dbInfo);
			if (mysql_real_query(handle->handle, request.c_str(), request.size()) != 0)
			{
				throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_EXEC, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(handle->handle), __LINE__, __FILE__, request);
			}
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_EXEC, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(handle->handle), __LINE__, __FILE__, request);
		}
	}

	if (show)
	{
		if (!empty)
		{
			mysql_free_result(handle->result);
			empty = true;
		}

		handle->result = mysql_store_result(handle->handle);
		if (handle->result == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_EXEC, exception::ERRNO_MYSQL, mysql_errno(handle->handle), mysql_error(handle->handle), __LINE__, __FILE__);
		}

		empty = false;
	}

#ifndef DATABASE_WO_XEXEC
	performXExec(postExec);
#endif

	cleanCollected();
	request.clear();
}

//-------------------------------------------------------------------

void
mysql::setCharset(const dodoString &charset)
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	mysql_options(handle->handle, MYSQL_SET_CHARSET_NAME, charset.c_str());
}

//-------------------------------------------------------------------

void
mysql::setConnectTimeout(unsigned int time)
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	mysql_options(handle->handle, MYSQL_OPT_CONNECT_TIMEOUT, (char *)&time);
}

//-------------------------------------------------------------------

dodoString
mysql::getCharset() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	return mysql_character_set_name(handle->handle);
}

//-------------------------------------------------------------------

dodo::dodoStringMapArray
mysql::fetchFieldsToRows() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, MYSQLEX_NOTOPENED, DATABASEMYSQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	dodoStringMapArray rowsFields;

	if (empty || !show)
	{
		return rowsFields;
	}

	mysql_data_seek(handle->result, 0);
	mysql_field_seek(handle->result, 0);

	unsigned int numFields = mysql_num_fields(handle->result);
	MYSQL_FIELD *mysqlFields = mysql_fetch_fields(handle->result);

#ifndef USE_DEQUE
	rowsFields.reserve(mysql_num_rows(handle->result));
#endif

	dodoStringMap rowFieldsPart;
	dodoString rowPart;

	unsigned long *length, j;

	MYSQL_ROW mysqlRow;

	while ((mysqlRow = mysql_fetch_row(handle->result)) != NULL)
	{
		length = mysql_fetch_lengths(handle->result);

		for (j = 0; j < numFields; ++j)
		{
			if (mysqlRow[j] != NULL)
			{
				rowFieldsPart.insert(make_pair(mysqlFields[j].name, dodoString(mysqlRow[j], length[j])));
			}
			else
			{
				rowFieldsPart.insert(make_pair(mysqlFields[j].name, statements[SQLCONSTRUCTOR_STATEMENT_NULL]));
			}
		}
	}

	rowsFields.push_back(rowFieldsPart);

	return rowsFields;
}
#endif

//-------------------------------------------------------------------

