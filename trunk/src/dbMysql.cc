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

#include <libdodo/dbMysql.h>

#ifdef MYSQL_EXT

using namespace dodo::db;

__mysqlSSLOptions::__mysqlSSLOptions()
{
}

//-------------------------------------------------------------------

__mysqlSSLOptions::__mysqlSSLOptions(const dodoString &a_key,
									 const dodoString &a_cert,
									 const dodoString &a_ca,
									 const dodoString &a_capath,
									 const dodoString &a_cipher) : key(a_key),
																   cert(a_cert),
																   ca(a_ca),
																   capath(a_capath),
																   cipher(a_cipher)
{
}

//-------------------------------------------------------------------

mysql::mysql() : empty(true),
				 type(CLIENT_MULTI_STATEMENTS)
{
	addSQL();
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
mysql::addSQL()
{
	sqlDbDepAddInsArr.push_back(" delayed ");
	sqlDbDepAddInsArr.push_back(" low_priority ");
	sqlDbDepAddInsArr.push_back(" high_priority ");

	sqlDbDepAddSelArr.push_back(" straight_join ");
	sqlDbDepAddSelArr.push_back(" sql_small_result ");
	sqlDbDepAddSelArr.push_back(" sql_big_result ");

	sqlDbDepAddUpArr.push_back(" low_priority ");

	sqlDbDepAddDelArr.push_back(" low_priority ");
	sqlDbDepAddDelArr.push_back(" quick ");
}

//-------------------------------------------------------------------

void
mysql::setMyAddInsSt(short statement)
{
	removeFlag(qDbDepInsShift, 1 << MYSQL_ADDREQUEST_INSERT_DELAYED);
	removeFlag(qDbDepInsShift, 1 << MYSQL_ADDREQUEST_INSERT_LOW_PRIORITY);
	removeFlag(qDbDepInsShift, 1 << MYSQL_ADDREQUEST_INSERT_HIGH_PRIORITY);

	addFlag(qDbDepInsShift, 1 << statement);
}

//-------------------------------------------------------------------

void
mysql::setMyAddUpSt(short statement)
{
	addFlag(qDbDepUpShift, 1 << statement);
}

//-------------------------------------------------------------------

void
mysql::setMyAddSelSt(short statement)
{
	addFlag(qDbDepSelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
mysql::setMyAddDelSt(short statement)
{
	addFlag(qDbDepDelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
mysql::unsetMyAddInsSt(short statement)
{
	removeFlag(qDbDepInsShift, 1 << statement);
}

//-------------------------------------------------------------------

void
mysql::unsetMyAddUpSt(short statement)
{
	removeFlag(qDbDepUpShift, 1 << statement);
}

//-------------------------------------------------------------------

void
mysql::unsetMyAddSelSt(short statement)
{
	removeFlag(qDbDepSelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
mysql::unsetMyAddDelSt(short statement)
{
	removeFlag(qDbDepDelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
mysql::connectSettings(unsigned long a_type,
					   const __mysqlSSLOptions &options)
{
	type = a_type;

	mysql_ssl_set(mysqlHandle,
				  options.key.size() == 0 ? NULL : options.key.c_str(),
				  options.cert.size() == 0 ? NULL : options.cert.c_str(),
				  options.ca.size() == 0 ? NULL : options.ca.c_str(),
				  options.capath.size() == 0 ? NULL : options.capath.c_str(),
				  options.cipher.size() == 0 ? NULL : options.cipher.c_str());
}

//-------------------------------------------------------------------

void
mysql::connect()
{
#ifndef DBMYSQL_WO_XEXEC
	operType = MYSQL_OPERATION_CONNECT;
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
							dbInfo.host.size() == 0 ? NULL : dbInfo.host.c_str(),
							dbInfo.user.size() == 0 ? NULL : dbInfo.user.c_str(),
							dbInfo.password.size() == 0 ? NULL : dbInfo.password.c_str(),
							dbInfo.db.size() == 0 ? NULL : dbInfo.db.c_str(),
							dbInfo.port,
							dbInfo.path.size() == 0 ? NULL : dbInfo.path.c_str(),
							type))
		throw baseEx(ERRMODULE_DBMYSQL, MYSQLEX_CONNECT, ERR_MYSQL, mysql_errno(mysqlHandle), mysql_error(mysqlHandle), __LINE__, __FILE__);

#ifndef DBMYSQL_WO_XEXEC
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
#ifndef DBMYSQL_WO_XEXEC
		operType = MYSQL_OPERATION_DISCONNECT;
		performXExec(preExec);
#endif

		if (!empty)
		{
			empty = true;
			mysql_free_result(mysqlResult);
		}

		mysql_close(mysqlHandle);

#ifndef DBMYSQL_WO_XEXEC
		performXExec(postExec);
#endif

		connected = false;
	}
}

//-------------------------------------------------------------------

void
mysql::_exec(const dodoString &query,
			 bool result)
{
	if (query.size() == 0)
	{
		if (autoFraming)
		{
			if (qType == ACCUMULATOR_REQUEST_INSERT || qType == ACCUMULATOR_REQUEST_UPDATE)
			{
				dodoString temp = dbInfo.db + ":" + pre_table;

				if (framingFields.find(temp) == framingFields.end())
				{
					request = "describe " + pre_table;

					if (mysql_real_query(mysqlHandle, request.c_str(), request.size()) != 0)
					{
						int mysqlErrno = mysql_errno(mysqlHandle);
						if (reconnect && (mysqlErrno == CR_SERVER_GONE_ERROR || mysqlErrno == CR_SERVER_LOST))
						{
							connect();
							if (mysql_real_query(mysqlHandle, request.c_str(), request.size()) != 0)
								throw baseEx(ERRMODULE_DBMYSQL, MYSQLEX__EXEC, ERR_MYSQL, mysqlErrno, mysql_error(mysqlHandle), __LINE__, __FILE__, request);
						}
						else
							throw baseEx(ERRMODULE_DBMYSQL, MYSQLEX__EXEC, ERR_MYSQL, mysqlErrno, mysql_error(mysqlHandle), __LINE__, __FILE__, request);
					}

					mysqlResult = mysql_store_result(mysqlHandle);
					if (mysqlResult == NULL)
						throw baseEx(ERRMODULE_DBMYSQL, MYSQLEX__EXEC, ERR_MYSQL, mysql_errno(mysqlHandle), mysql_error(mysqlHandle), __LINE__, __FILE__);

					empty = false;

					mysql_field_seek(mysqlResult, 0);

					dodoStringArray rowsPart;

					MYSQL_ROW mysqlRow;

					while ((mysqlRow = mysql_fetch_row(mysqlResult)) != NULL)
					{
						if (strcasestr(mysqlRow[1], "char") != NULL ||
							strcasestr(mysqlRow[1], "date") != NULL ||
							strcasestr(mysqlRow[1], "time") != NULL ||
							strcasestr(mysqlRow[1], "blob") != NULL ||
							strcasestr(mysqlRow[1], "text") != NULL ||
							strcasestr(mysqlRow[1], "enum") != NULL ||
							strcasestr(mysqlRow[1], "set") != NULL)
							rowsPart.push_back(mysqlRow[0]);
					}

					mysql_free_result(mysqlResult);

					empty = true;

					framingFields.insert(make_pair(temp, rowsPart));
				}
			}
		}

		queryCollect();
	}
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
			connect();
			if (mysql_real_query(mysqlHandle, request.c_str(), request.size()) != 0)
				throw baseEx(ERRMODULE_DBMYSQL, MYSQLEX__EXEC, ERR_MYSQL, mysqlErrno, mysql_error(mysqlHandle), __LINE__, __FILE__, request);
		}
		else
			throw baseEx(ERRMODULE_DBMYSQL, MYSQLEX__EXEC, ERR_MYSQL, mysqlErrno, mysql_error(mysqlHandle), __LINE__, __FILE__, request);
	}

	if (!show)
		return ;

	if (!empty)
	{
		mysql_free_result(mysqlResult);
		empty = true;
	}

	mysqlResult = mysql_store_result(mysqlHandle);
	if (mysqlResult == NULL)
		throw baseEx(ERRMODULE_DBMYSQL, MYSQLEX__EXEC, ERR_MYSQL, mysql_errno(mysqlHandle), mysql_error(mysqlHandle), __LINE__, __FILE__);

	empty = false;
}

//-------------------------------------------------------------------

dodoArray<dodo::dodoStringArray>
mysql::fetchRow() const
{
#ifndef DBMYSQL_WO_XEXEC
	operType = MYSQL_OPERATION_FETCHROW;
	performXExec(preExec);
#endif

	if (empty || !show)
		return __dodostringarrayarray__;

	mysql_field_seek(mysqlResult, 0);

	unsigned int numFields = mysql_num_fields(mysqlResult);

	dodoArray<dodoStringArray> rows;

#ifndef USE_DEQUE
	rows.reserve(mysql_num_rows(mysqlResult));
#endif

	unsigned long *length, j;

	dodoStringArray rowsPart;

	MYSQL_ROW mysqlRow;

	dodoString rowPart;

	while ((mysqlRow = mysql_fetch_row(mysqlResult)) != NULL)
	{
		length = mysql_fetch_lengths(mysqlResult);

		rowsPart.clear();

#ifndef USE_DEQUE
		rowsPart.reserve(numFields);
#endif

		for (j = 0; j < numFields; ++j)
		{
			rowPart.assign(mysqlRow[j] != NULL ? mysqlRow[j] : "NULL", mysqlRow[j] ? length[j] : 4);
			if (preventEscaping)
				rowsPart.push_back(rowPart);
			else
				rowsPart.push_back(unescapeFields(rowPart));
		}

		rows.push_back(rowsPart);
	}

#ifndef DBMYSQL_WO_XEXEC
	performXExec(postExec);
#endif

	return rows;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
mysql::fetchField() const
{
#ifndef DBMYSQL_WO_XEXEC
	operType = MYSQL_OPERATION_FETCHFIELD;
	performXExec(preExec);
#endif

	if (empty || !show)
		return __dodostringarray__;

	unsigned int numFields = mysql_num_fields(mysqlResult);
	MYSQL_FIELD *mysqlFields = mysql_fetch_fields(mysqlResult);

	dodoStringArray fields;

#ifndef USE_DEQUE
	fields.reserve(numFields);
#endif

	for (unsigned int i(0); i < numFields; ++i)
		fields.push_back(mysqlFields[i].name);

#ifndef DBMYSQL_WO_XEXEC
	performXExec(postExec);
#endif

	return fields;
}

//-------------------------------------------------------------------

__connectorStorage
mysql::fetch() const
{
	return __connectorStorage(fetchRow(), fetchField());
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
mysql::exec(const dodoString &query,
			bool result)
{
#ifndef DBMYSQL_WO_XEXEC
	operType = MYSQL_OPERATION_EXEC;
	performXExec(preExec);
#endif

	_exec(query, result);

#ifndef DBMYSQL_WO_XEXEC
	performXExec(postExec);
#endif

	cleanCollected();
}

//-------------------------------------------------------------------

#ifndef DBMYSQL_WO_XEXEC

int
mysql::addPostExec(inExec func,
				   void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_DBMYSQL, data);
}

//-------------------------------------------------------------------

int
mysql::addPreExec(inExec func,
				  void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_DBMYSQL, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
mysql::addPostExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_DBMYSQL, data, toInit);
}

//-------------------------------------------------------------------

int
mysql::addPreExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_DBMYSQL, data, toInit);
}

//-------------------------------------------------------------------

dodo::__xexecCounts
mysql::addExec(const dodoString &module,
			   void             *data,
			   void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_DBMYSQL, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
mysql::setCharset(const dodoString &charset)
{
	mysql_options(mysqlHandle, MYSQL_READ_DEFAULT_FILE, charset.c_str());
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
mysql::fetchAssoc() const
{
	if (empty || !show)
		return __dodostringmaparray__;

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

		rowFieldsPart.clear();

		for (j = 0; j < numFields; ++j)
		{
			rowPart.assign(mysqlRow[j] != NULL ? mysqlRow[j] : "NULL", mysqlRow[j] ? length[j] : 4);
			if (preventEscaping)
				rowFieldsPart.insert(make_pair(mysqlFields[j].name, rowPart));
			else
				rowFieldsPart.insert(make_pair(mysqlFields[j].name, unescapeFields(rowPart)));
		}

		rowsFields.push_back(rowFieldsPart);
	}

	return rowsFields;
}

//-------------------------------------------------------------------

void
mysql::renameDbCollect()
{
	request = "rename database " + pre_order + " to " + pre_having;
}

//-------------------------------------------------------------------

void
mysql::renameFieldCollect()
{
	request = "alter table " + pre_table + " change " + pre_tableTo + " " + fieldCollect(pre_fieldInfo);
}

//-------------------------------------------------------------------

void
mysql::renameField(const dodoString &field,
				   const __connectorField &to_field,
				   const dodoString &table)
{
	qType = ACCUMULATOR_REQUEST_RENAME_FIELD;
	pre_tableTo = field;
	pre_table = table;
	pre_fieldInfo = to_field;
	show = false;
}

#endif

//-------------------------------------------------------------------

