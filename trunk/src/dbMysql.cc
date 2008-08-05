/***************************************************************************
 *            dbMysql.cc
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

#include <libdodo/dbMysql.h>

#ifdef MYSQL_EXT

using namespace dodo::db;

mysql::mysql() : empty(true),
				 type(CLIENT_MULTI_STATEMENTS)

{
#ifndef DB_WO_XEXEC

	execObject = XEXEC_OBJECT_DBMYSQL;

#endif
}

//-------------------------------------------------------------------

mysql::mysql(mysql &a_mypp)
{
}

//-------------------------------------------------------------------

mysql::~mysql()
{
	if (connected)
	{
		if (!empty)
			mysql_free_result(mysqlResult);

		mysql_close(mysqlHandle);
	}
}

//-------------------------------------------------------------------

void
mysql::connectSettings(unsigned long a_type,
					   const __mysqlSslOptions &options)
{
	type = a_type;

	if (mysql_ssl_set(mysqlHandle,
					  options.key.size() == 0 ? NULL : options.key.c_str(),
					  options.cert.size() == 0 ? NULL : options.cert.c_str(),
					  options.ca.size() == 0 ? NULL : options.ca.c_str(),
					  options.capath.size() == 0 ? NULL : options.capath.c_str(),
					  options.cipher.size() == 0 ? NULL : options.cipher.c_str()) == 0)
		type |= CLIENT_SSL;
}

//-------------------------------------------------------------------

void
mysql::connect(const __connectionInfo &info)
{
	collectedData.dbInfo = info;

#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	if (connected)
	{
		if (!empty)
		{
			empty = true;
			mysql_free_result(mysqlResult);
		}

		mysql_close(mysqlHandle);

		connected = false;
	}

	mysqlHandle = mysql_init(NULL);

	if (!mysql_real_connect(mysqlHandle,
							collectedData.dbInfo.host.size() == 0 ? NULL : collectedData.dbInfo.host.c_str(),
							collectedData.dbInfo.user.size() == 0 ? NULL : collectedData.dbInfo.user.c_str(),
							collectedData.dbInfo.password.size() == 0 ? NULL : collectedData.dbInfo.password.c_str(),
							collectedData.dbInfo.db.size() == 0 ? NULL : collectedData.dbInfo.db.c_str(),
							collectedData.dbInfo.port,
							collectedData.dbInfo.path.size() == 0 ? NULL : collectedData.dbInfo.path.c_str(),
							type))
		throw exception::basic(exception::ERRMODULE_DBMYSQL, MYSQLEX_CONNECT, exception::ERRNO_MYSQL, mysql_errno(mysqlHandle), mysql_error(mysqlHandle), __LINE__, __FILE__);

#ifndef MYSQL_NO_OPT_RECONNECT

	if (reconnect)
	{
		my_bool rc = 1;

		mysql_options(mysqlHandle, MYSQL_OPT_RECONNECT, &rc);
	}

#endif

#ifndef DB_WO_XEXEC
	performXExec(postExec);
#endif

	connected = true;
}

//-------------------------------------------------------------------

void
mysql::disconnect()
{
	if (connected)
	{
#ifndef DB_WO_XEXEC
		operType = DB_OPERATION_DISCONNECT;
		performXExec(preExec);
#endif

		if (!empty)
		{
			empty = true;
			mysql_free_result(mysqlResult);
		}

		mysql_close(mysqlHandle);

#ifndef DB_WO_XEXEC
		performXExec(postExec);
#endif

		connected = false;
	}
}

//-------------------------------------------------------------------

dodoArray<dodo::dodoStringArray>
mysql::fetchRows() const
{
#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_FETCHROW;
	performXExec(preExec);
#endif

	if (empty || !show)
		return __dodostringarrayarray__;

	mysql_data_seek(mysqlResult, 0);

	unsigned int numFields = mysql_num_fields(mysqlResult);

	dodoArray<dodoStringArray> rows;

#ifndef USE_DEQUE
	rows.reserve(mysql_num_rows(mysqlResult));
#endif

	unsigned long *length, j;

	dodoStringArray rowsPart;

	MYSQL_ROW mysqlRow;

#ifndef USE_DEQUE
	rowsPart.reserve(numFields);
#endif

	while ((mysqlRow = mysql_fetch_row(mysqlResult)) != NULL)
	{
		length = mysql_fetch_lengths(mysqlResult);

		rowsPart.clear();

		for (j = 0; j < numFields; ++j)
		{
			if (mysqlRow[j] != NULL)
				rowsPart.push_back(dodoString(mysqlRow[j], length[j]));
			else
				rowsPart.push_back(statements[SQLCONSTRUCTOR_STATEMENT_NULL]);
		}

		rows.push_back(rowsPart);
	}

#ifndef DB_WO_XEXEC
	performXExec(postExec);
#endif

	return rows;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
mysql::fetchFields() const
{
#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_FETCHFIELD;
	performXExec(preExec);
#endif

	if (empty || !show)
		return __dodostringarray__;

	mysql_field_seek(mysqlResult, 0);

	unsigned int numFields = mysql_num_fields(mysqlResult);
	MYSQL_FIELD *mysqlFields = mysql_fetch_fields(mysqlResult);

	dodoStringArray fields;

#ifndef USE_DEQUE
	fields.reserve(numFields);
#endif

	for (unsigned int i(0); i < numFields; ++i)
		fields.push_back(mysqlFields[i].name);

#ifndef DB_WO_XEXEC
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
	if (empty || !show)
		return 0;
	else
		return mysql_num_rows(mysqlResult);
}

//-------------------------------------------------------------------

unsigned int
mysql::fieldsCount() const
{
	if (empty || !show)
		return 0;
	else
		return mysql_num_fields(mysqlResult);
}

//-------------------------------------------------------------------

unsigned int
mysql::affectedRowsCount() const
{
	if (empty || show)
		return 0;
	else
		return mysql_affected_rows(mysqlHandle);
}

//-------------------------------------------------------------------

void
mysql::getFieldsTypes(const dodoString &table)
{
	dodoString temp = collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + table;

	dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(temp);

	if (types == fieldTypes.end())
		types = fieldTypes.insert(make_pair(temp, dodoMap<dodoString, short, dodoMapICaseStringCompare>())).first;

	request = "describe " + table;

	if (mysql_real_query(mysqlHandle, request.c_str(), request.size()) != 0)
	{
		int mysqlErrno = mysql_errno(mysqlHandle);
		if (reconnect && (mysqlErrno == CR_SERVER_GONE_ERROR || mysqlErrno == CR_SERVER_LOST))
		{
			connect(collectedData.dbInfo);
			if (mysql_real_query(mysqlHandle, request.c_str(), request.size()) != 0)
				throw exception::basic(exception::ERRMODULE_DBMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(mysqlHandle), __LINE__, __FILE__, request);
		}
		else
			throw exception::basic(exception::ERRMODULE_DBMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(mysqlHandle), __LINE__, __FILE__, request);
	}

	mysqlResult = mysql_store_result(mysqlHandle);
	if (mysqlResult == NULL)
		throw exception::basic(exception::ERRMODULE_DBMYSQL, MYSQLEX_GETFIELDSTYPES, exception::ERRNO_MYSQL, mysql_errno(mysqlHandle), mysql_error(mysqlHandle), __LINE__, __FILE__);

	empty = false;

	MYSQL_ROW mysqlRow;

	dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

	while ((mysqlRow = mysql_fetch_row(mysqlResult)) != NULL)
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
				types->second.insert(make_pair(dodoString(mysqlRow[0]), sql::FIELDTYPE_TEXT));
			else
			{
				if (strcasestr(mysqlRow[1], "blob") != NULL)
					types->second.insert(make_pair(dodoString(mysqlRow[0]), sql::FIELDTYPE_BINARY));
				else
					types->second.insert(make_pair(dodoString(mysqlRow[0]), sql::FIELDTYPE_NUMERIC));
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
				field->second = sql::FIELDTYPE_TEXT;
			else
			{
				if (strcasestr(mysqlRow[1], "blob") != NULL)
					field->second = sql::FIELDTYPE_BINARY;
				else
					field->second = sql::FIELDTYPE_NUMERIC;
			}
		}
	}

	mysql_free_result(mysqlResult);

	empty = true;
}

//-------------------------------------------------------------------

void
mysql::exec(const dodoString &query,
			bool result)
{
#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_EXEC;
	performXExec(preExec);
#endif

	if (query.size() == 0)
		queryCollect();
	else
	{
		request = query;
		show = result;
	}

	if (mysql_real_query(mysqlHandle, request.c_str(), request.size()) != 0)
	{
		int mysqlErrno = mysql_errno(mysqlHandle);
		if (reconnect && (mysqlErrno == CR_SERVER_GONE_ERROR || mysqlErrno == CR_SERVER_LOST))
		{
			connect(collectedData.dbInfo);
			if (mysql_real_query(mysqlHandle, request.c_str(), request.size()) != 0)
				throw exception::basic(exception::ERRMODULE_DBMYSQL, MYSQLEX_EXEC, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(mysqlHandle), __LINE__, __FILE__, request);
		}
		else
			throw exception::basic(exception::ERRMODULE_DBMYSQL, MYSQLEX_EXEC, exception::ERRNO_MYSQL, mysqlErrno, mysql_error(mysqlHandle), __LINE__, __FILE__, request);
	}

	if (show)
	{
		if (!empty)
		{
			mysql_free_result(mysqlResult);
			empty = true;
		}

		mysqlResult = mysql_store_result(mysqlHandle);
		if (mysqlResult == NULL)
			throw exception::basic(exception::ERRMODULE_DBMYSQL, MYSQLEX_EXEC, exception::ERRNO_MYSQL, mysql_errno(mysqlHandle), mysql_error(mysqlHandle), __LINE__, __FILE__);

		empty = false;
	}

#ifndef DB_WO_XEXEC
	performXExec(postExec);
#endif

	cleanCollected();
	request.clear();
}

//-------------------------------------------------------------------

void
mysql::setCharset(const dodoString &charset)
{
	mysql_options(mysqlHandle, MYSQL_SET_CHARSET_NAME, charset.c_str());
}

//-------------------------------------------------------------------

void
mysql::setConnectTimeout(unsigned int time)
{
	mysql_options(mysqlHandle, MYSQL_OPT_CONNECT_TIMEOUT, (char *)&time);
}

//-------------------------------------------------------------------

dodoString
mysql::getCharset() const
{
	return mysql_character_set_name(mysqlHandle);
}

//-------------------------------------------------------------------

dodo::dodoStringMapArray
mysql::fetchFieldsToRows() const
{
	if (empty || !show)
		return __dodostringmaparray__;

	mysql_data_seek(mysqlResult, 0);
	mysql_field_seek(mysqlResult, 0);

	unsigned int numFields = mysql_num_fields(mysqlResult);
	MYSQL_FIELD *mysqlFields = mysql_fetch_fields(mysqlResult);

	dodoStringMapArray rowsFields;

#ifndef USE_DEQUE
	rowsFields.reserve(mysql_num_rows(mysqlResult));
#endif

	dodoStringMap rowFieldsPart;
	dodoString rowPart;

	unsigned long *length, j;

	MYSQL_ROW mysqlRow;

	while ((mysqlRow = mysql_fetch_row(mysqlResult)) != NULL)
	{
		length = mysql_fetch_lengths(mysqlResult);

		for (j = 0; j < numFields; ++j)
		{
			if (mysqlRow[j] != NULL)
				rowFieldsPart.insert(make_pair(mysqlFields[j].name, dodoString(mysqlRow[j], length[j])));
			else
				rowFieldsPart.insert(make_pair(mysqlFields[j].name, statements[SQLCONSTRUCTOR_STATEMENT_NULL]));
		}
	}

	rowsFields.push_back(rowFieldsPart);

	return rowsFields;
}

#endif

//-------------------------------------------------------------------

