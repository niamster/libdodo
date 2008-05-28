/***************************************************************************
 *            dbPostgresql.cc
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

#include <libdodo/dbPostgresql.h>

#ifdef POSTGRESQL_EXT

using namespace dodo::db;

postgresql::postgresql() : empty(true),
						   hint(POSTGRESQL_HINT_NONE)
{
}

//-------------------------------------------------------------------

postgresql::postgresql(postgresql &a_mypp)
{
}

//-------------------------------------------------------------------

postgresql::~postgresql()
{
	if (connected)
	{
		if (!empty)
			PQclear(pgResult);

		PQfinish(pgHandle);
	}
}

//-------------------------------------------------------------------

dodoString
postgresql::sqlDataType(int type)
{
	switch (type)
	{
		case CONNECTOR_FIELDTYPE_TINYBLOB:
		case CONNECTOR_FIELDTYPE_BLOB:
		case CONNECTOR_FIELDTYPE_MEDIUMBLOB:
		case CONNECTOR_FIELDTYPE_LONGBLOB:

			return dodoString("BYTEA");

		default:

			return sqlConstructor::sqlDataType(type);
	}
}

//-------------------------------------------------------------------

void
postgresql::connect()
{
#ifndef DBPOSTGRESQL_WO_XEXEC
	operType = POSTGRESQL_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	if (connected)
	{
		if (!empty)
		{
			PQclear(pgResult);
			empty = true;
		}

		PQfinish(pgHandle);

		connected = false;
	}

	pgHandle = PQsetdbLogin(
		dbInfo.host.size() == 0 ? NULL : dbInfo.host.c_str(),
		tools::string::iToString(dbInfo.port).c_str(),
		NULL,
		NULL,
		dbInfo.db.size() == 0 ? NULL : dbInfo.db.c_str(),
		dbInfo.user.size() == 0 ? NULL : dbInfo.user.c_str(),
		dbInfo.password.size() == 0 ? NULL : dbInfo.password.c_str());

	int status = PQstatus(pgHandle);

	if (status != CONNECTION_OK)
		throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_CONNECT, ERR_MYSQL, status, PQerrorMessage(pgHandle), __LINE__, __FILE__);

#ifndef DBPOSTGRESQL_WO_XEXEC
	performXExec(postExec);
#endif

	connected = true;
}

//-------------------------------------------------------------------

void
postgresql::disconnect()
{
	if (connected)
	{
#ifndef DBPOSTGRESQL_WO_XEXEC
		operType = POSTGRESQL_OPERATION_DISCONNECT;
		performXExec(preExec);
#endif

		if (!empty)
		{
			PQclear(pgResult);
			empty = true;
		}

		PQfinish(pgHandle);

#ifndef DBPOSTGRESQL_WO_XEXEC
		performXExec(postExec);
#endif

		connected = false;
	}
}

//-------------------------------------------------------------------

void
postgresql::setBLOBValues(const dodoStringArray &values)
{
	blobs = values;
}

//-------------------------------------------------------------------

void
postgresql::_exec(const dodoString &query,
				  bool result)
{
	bool blobHint;
	int status;

	if (query.size() == 0)
	{

		if (autoFraming)
		{
			if (collectedData.qType == ACCUMULATOR_REQUEST_INSERT || collectedData.qType == ACCUMULATOR_REQUEST_UPDATE)
			{
				dodoString temp = dbInfo.db + ":" + collectedData.table;

				if (framingFields.find(temp) == framingFields.end())
				{
					request = "select column_name, data_type from information_schema.columns where table_name='" + collectedData.table + "'";

					if (!empty)
					{
						PQclear(pgResult);
						empty = true;
					}

					pgResult = PQexecParams(pgHandle, request.c_str(), 0, NULL, NULL, NULL, NULL, 1);
					if (pgResult == NULL)
						throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX__EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(pgHandle), __LINE__, __FILE__, request);

					status = PQresultStatus(pgResult);

					switch (status)
					{
						case PGRES_EMPTY_QUERY:
						case PGRES_BAD_RESPONSE:
						case PGRES_NONFATAL_ERROR:
						case PGRES_FATAL_ERROR:

							throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX__EXEC, ERR_MYSQL, status, PQerrorMessage(pgHandle), __LINE__, __FILE__);
					}

					empty = false;

					int rowsNum = PQntuples(pgResult);
					char *fieldType;

					dodoStringArray rowsPart;

					for (int i(0); i < rowsNum; ++i)
					{
						fieldType = PQgetvalue(pgResult, i, 1);

						if (strcasestr(fieldType, "char") != NULL ||
							strcasestr(fieldType, "date") != NULL ||
							strcasestr(fieldType, "bytea") != NULL ||
							strcasestr(fieldType, "array") != NULL ||
							strcasestr(fieldType, "text") != NULL ||
							strcasestr(fieldType, "cidr") != NULL ||
							strcasestr(fieldType, "macaddrcd ") != NULL ||
							strcasestr(fieldType, "inet") != NULL)
							rowsPart.push_back(PQgetvalue(pgResult, i, 0));
					}

					if (!empty)
					{
						PQclear(pgResult);
						empty = true;
					}

					framingFields.insert(make_pair(temp, rowsPart));
				}
			}
		}

		queryCollect();
	}

	if (!empty)
	{
		PQclear(pgResult);
		empty = true;
	}

	if (isSetFlag(hint, POSTGRESQL_HINT_BLOB))
	{
		removeFlag(hint, POSTGRESQL_HINT_BLOB);

		switch (collectedData.qType)
		{
			case ACCUMULATOR_REQUEST_UPDATE:
			case ACCUMULATOR_REQUEST_INSERT:

			{
				long size = blobs.size();

				char **values = new char*[size];
				int *lengths = new int[size];
				int *formats = new int[size];

				dodoStringArray::iterator i(blobs.begin()), j(blobs.end());
				for (int o = 0; i != j; ++i, ++o)
				{
					values[o] = (char *)i->c_str();
					lengths[o] = i->size();
					formats[o] = 1;
				}

				pgResult = PQexecParams(pgHandle, request.c_str(), size, NULL, values, lengths, formats, 0);

				delete [] values;
				delete [] lengths;
				delete [] formats;

				if (pgResult == NULL)
					throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX__EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(pgHandle), __LINE__, __FILE__);
			}

				break;

			case ACCUMULATOR_REQUEST_SELECT:

				pgResult = PQexecParams(pgHandle, request.c_str(), 0, NULL, NULL, NULL, NULL, 1);

				break;

			default:

				throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX__EXEC, ERR_LIBDODO, POSTGRESQLEX_WRONGHINTUSAGE, DBPOSTGRESQLEX_WRONGHINTUSAGE_STR, __LINE__, __FILE__);
		}
	}
	else
	{
		pgResult = PQexec(pgHandle, request.c_str());
		if (pgResult == NULL)
			throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX__EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(pgHandle), __LINE__, __FILE__, request);
	}

	status = PQresultStatus(pgResult);

	switch (status)
	{
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:

			throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX__EXEC, ERR_MYSQL, status, PQerrorMessage(pgHandle), __LINE__, __FILE__);
	}

	empty = false;
}

//-------------------------------------------------------------------

dodoArray<dodo::dodoStringArray>
postgresql::fetchRow() const
{

#ifndef DBPOSTGRESQL_WO_XEXEC
	operType = POSTGRESQL_OPERATION_FETCHROW;
	performXExec(preExec);
#endif

	if (empty || !show)
		return __dodostringarrayarray__;

	int rowsNum = PQntuples(pgResult);
	int fieldsNum = PQnfields(pgResult);

	dodoArray<dodoStringArray> rows;

#ifndef USE_DEQUE
	rows.reserve(rowsNum);
#endif

	int j;

	dodoStringArray rowsPart;
	dodoString rowPart;

	for (int i(0); i < rowsNum; ++i)
	{
		rowsPart.clear();

#ifndef USE_DEQUE
		rowsPart.reserve(fieldsNum);
#endif

		for (j = 0; j < fieldsNum; ++j)
		{
			if (PQgetisnull(pgResult, i, j) == 1)
				rowPart.assign("NULL");
			else
			{
				if (preventEscaping)
					rowPart.assign(PQgetvalue(pgResult, i, j), PQgetlength(pgResult, i, j));
				else
					rowPart.assign(unescapeFields(dodoString(PQgetvalue(pgResult, i, j), PQgetlength(pgResult, i, j))));
			}

			rowsPart.push_back(rowPart);
		}

		rows.push_back(rowsPart);
	}

#ifndef DBPOSTGRESQL_WO_XEXEC
	performXExec(postExec);
#endif

	return rows;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
postgresql::fetchField() const
{
#ifndef DBPOSTGRESQL_WO_XEXEC
	operType = POSTGRESQL_OPERATION_FETCHFIELD;
	performXExec(preExec);
#endif

	if (empty || !show)
		return __dodostringarray__;

	int fieldsNum = PQnfields(pgResult);

	dodoStringArray fields;

#ifndef USE_DEQUE
	fields.reserve(fieldsNum);
#endif

	for (int i(0); i < fieldsNum; ++i)
		fields.push_back(PQfname(pgResult, i));

#ifndef DBPOSTGRESQL_WO_XEXEC
	performXExec(postExec);
#endif

	return fields;
}

//-------------------------------------------------------------------

__connectorStorage
postgresql::fetch() const
{
	return __connectorStorage(fetchRow(), fetchField());
}

//-------------------------------------------------------------------

unsigned int
postgresql::rowsCount() const
{
	if (empty || !show)
		return 0;
	else
		return PQntuples(pgResult);
}

//-------------------------------------------------------------------

unsigned int
postgresql::fieldsCount() const
{
	if (empty || !show)
		return 0;
	else
		return PQnfields(pgResult);
}

//-------------------------------------------------------------------

unsigned int
postgresql::affectedRowsCount() const
{
	if (empty || show)
		return 0;
	else
		return atoi(PQcmdTuples(pgResult));
}

//-------------------------------------------------------------------

void
postgresql::exec(const dodoString &query,
				 bool result)
{
#ifndef DBPOSTGRESQL_WO_XEXEC
	operType = POSTGRESQL_OPERATION_EXEC;
	performXExec(preExec);
#endif

	_exec(query, result);

#ifndef DBPOSTGRESQL_WO_XEXEC
	performXExec(postExec);
#endif

	cleanCollected();
}

//-------------------------------------------------------------------

#ifndef DBPOSTGRESQL_WO_XEXEC

int
postgresql::addPostExec(inExec func,
						void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data);
}

//-------------------------------------------------------------------

int
postgresql::addPreExec(inExec func,
					   void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
postgresql::addPostExec(const dodoString &module,
						void             *data,
						void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data, toInit);
}

//-------------------------------------------------------------------

int
postgresql::addPreExec(const dodoString &module,
					   void             *data,
					   void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data, toInit);
}

//-------------------------------------------------------------------

dodo::__xexecCounts
postgresql::addExec(const dodoString &module,
					void             *data,
					void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

dodo::dodoStringMapArray
postgresql::fetchAssoc() const
{
	if (empty || !show)
		return __dodostringmaparray__;

	int rowsNum = PQntuples(pgResult);
	int fieldsNum = PQnfields(pgResult);

	dodoStringMapArray rowsFields;

#ifndef USE_DEQUE
	rowsFields.reserve(rowsNum);
#endif

	dodoStringMap rowFieldsPart;
	dodoString rowPart;

	int j;

	for (int i(0); i < rowsNum; ++i)
	{
		rowFieldsPart.clear();

		for (j = 0; j < fieldsNum; ++j)
		{
			if (PQgetisnull(pgResult, i, j) == 1)
				rowPart.assign("NULL");
			else
			{
				if (preventEscaping)
					rowPart.assign(PQgetvalue(pgResult, i, j), PQgetlength(pgResult, i, j));
				else
					rowPart.assign(unescapeFields(dodoString(PQgetvalue(pgResult, i, j), PQgetlength(pgResult, i, j))));
			}

			rowFieldsPart.insert(make_pair(PQfname(pgResult, j), rowPart));
		}

		rowsFields.push_back(rowFieldsPart);
	}

	return rowsFields;
}

//-------------------------------------------------------------------

void
postgresql::setCharset(const dodoString &charset)
{
	int status = PQsetClientEncoding(pgHandle, charset.c_str());
	if (status == -1)
		throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_SETCHARSET, ERR_MYSQL, status, PQerrorMessage(pgHandle), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
postgresql::getCharset() const
{
	return PQclientEncoding(pgHandle);
}
//-------------------------------------------------------------------

void
postgresql::renameDbCollect()
{
	request = "alter database " + collectedData.order + " rename to " + collectedData.having;
}

//-------------------------------------------------------------------

void
postgresql::renameFieldCollect()
{
	request = "alter table " + collectedData.table + " rename column " + collectedData.tableTo + " to " + collectedData.having;
}

#endif

//-------------------------------------------------------------------

