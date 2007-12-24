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

using namespace dodo;

dbPostgresql::dbPostgresql() : empty(true)
{
}

//-------------------------------------------------------------------

dbPostgresql::dbPostgresql(dbPostgresql &a_mypp)
{
}

//-------------------------------------------------------------------

dbPostgresql::~dbPostgresql()
{
	if (connected)
	{
		if (!empty)
			PQclear(pgResult);

		PQfinish(conn);
	}
}

//-------------------------------------------------------------------

dodoString
dbPostgresql::stringType(int type)
{
	switch (type)
	{
		case DBBASE_FIELDTYPE_TINYBLOB:
		case DBBASE_FIELDTYPE_BLOB:
		case DBBASE_FIELDTYPE_MEDIUMBLOB:
		case DBBASE_FIELDTYPE_LONGBLOB:

			return dodoString("BYTEA");

		default:

			return dbSqlBase::stringType(type);
	}
}

//-------------------------------------------------------------------

void
dbPostgresql::connect()
{
	if (connected)
		disconnect();

		#ifndef DBPOSTGRESQL_WO_XEXEC
	operType = DBPOSTGRESQL_OPERATION_CONNECT;
	performXExec(preExec);
		#endif

	conn = PQsetdbLogin(
		dbInfo.host.size() == 0 ? NULL : dbInfo.host.c_str(),
		stringTools::iToString(dbInfo.port).c_str(),
		NULL,
		NULL,
		dbInfo.db.size() == 0 ? NULL : dbInfo.db.c_str(),
		dbInfo.user.size() == 0 ? NULL : dbInfo.user.c_str(),
		dbInfo.password.size() == 0 ? NULL : dbInfo.password.c_str());

	int status = PQstatus(conn);

	if (status != CONNECTION_OK)
		throw baseEx(ERRMODULE_DBPOSTGRESQL, DBPOSTGRESQLEX_CONNECT, ERR_MYSQL, status, PQerrorMessage(conn), __LINE__, __FILE__);

		#ifndef DBPOSTGRESQL_WO_XEXEC
	performXExec(postExec);
		#endif

	connected = true;
}

//-------------------------------------------------------------------

void
dbPostgresql::disconnect()
{
	if (connected)
	{
			#ifndef DBPOSTGRESQL_WO_XEXEC
		operType = DBPOSTGRESQL_OPERATION_DISCONNECT;
		performXExec(preExec);
			#endif

		if (!empty)
		{
			PQclear(pgResult);
			empty = true;
		}

		PQfinish(conn);

			#ifndef DBPOSTGRESQL_WO_XEXEC
		performXExec(postExec);
			#endif

		connected = false;
	}
}

//-------------------------------------------------------------------

void
dbPostgresql::setBLOBValues(const dodoStringArr &values)
{
	blobs = values;
}

//-------------------------------------------------------------------

void
dbPostgresql::_exec(const dodoString &query,
					bool result)
{
	bool blobHint;
	int status;

	if (query.size() == 0)
	{

		if (autoFraming)
		{
			if (qType == DBBASE_REQUEST_INSERT || qType == DBBASE_REQUEST_UPDATE)
			{
				dodoString temp = dbInfo.db + ":" + pre_table;

				if (!framingFields.isset(temp))
				{
					request = "select column_name, data_type from information_schema.columns where table_name='" + pre_table + "'";

					if (!empty)
					{
						PQclear(pgResult);
						empty = true;
					}

					pgResult = PQexecParams(conn, request.c_str(), 0, NULL, NULL, NULL, NULL, 1);
					if (pgResult == NULL)
						throw baseEx(ERRMODULE_DBPOSTGRESQL, DBPOSTGRESQLEX__EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(conn), __LINE__, __FILE__, request);

					status = PQresultStatus(pgResult);

					switch (status)
					{
						case PGRES_EMPTY_QUERY:
						case PGRES_BAD_RESPONSE:
						case PGRES_NONFATAL_ERROR:
						case PGRES_FATAL_ERROR:

							throw baseEx(ERRMODULE_DBPOSTGRESQL, DBPOSTGRESQLEX__EXEC, ERR_MYSQL, status, PQerrorMessage(conn), __LINE__, __FILE__);
					}

					empty = false;

					int rowsNum = PQntuples(pgResult);
					char *fieldType;

					dodoStringArr rowsPart;

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

					framingFields.insert(temp, rowsPart);
				}
			}
		}

		queryCollect();

		blobHint = false;
	}
	else
	{
		if (strstr(query.c_str(), "dodo:hint:db:blob") != NULL)
		{
			queryCollect();

			if (!show)
				blobHint = true;
		}
		else
		{
			request = query;
			show = result;

			blobHint = false;
		}
	}

	if (!empty)
	{
		PQclear(pgResult);
		empty = true;
	}

	if (blobHint)
	{
		switch (qType)
		{
			case DBBASE_REQUEST_UPDATE:
			case DBBASE_REQUEST_INSERT:

			{
				long size = blobs.size();

				char **values = new char*[size];
				int *lengths = new int[size];
				int *formats = new int[size];

				dodoStringArr::iterator i(blobs.begin()), j(blobs.end());
				for (int o = 0; i != j; ++i, ++o)
				{
					values[o] = (char *)i->c_str();
					lengths[o] = i->size();
					formats[o] = 1;
				}

				pgResult = PQexecParams(conn, request.c_str(), size, NULL, values, lengths, formats, 0);
				if (pgResult == NULL)
				{
					delete [] values;
					delete [] lengths;
					delete [] formats;

					throw baseEx(ERRMODULE_DBPOSTGRESQL, DBPOSTGRESQLEX__EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(conn), __LINE__, __FILE__);
				}

				delete [] values;
				delete [] lengths;
				delete [] formats;
			}

				break;

			default:

				throw baseEx(ERRMODULE_DBPOSTGRESQL, DBPOSTGRESQLEX__EXEC, ERR_LIBDODO, DBPOSTGRESQLEX_WRONG_HINT_USAGE, DBPOSTGRESQLEX_WRONG_HINT_USAGE_STR, __LINE__, __FILE__);
		}
	}
	else
	{
		pgResult = PQexecParams(conn, request.c_str(), 0, NULL, NULL, NULL, NULL, 1);
		if (pgResult == NULL)
			throw baseEx(ERRMODULE_DBPOSTGRESQL, DBPOSTGRESQLEX__EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(conn), __LINE__, __FILE__, request);
	}

	status = PQresultStatus(pgResult);

	switch (status)
	{
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:

			throw baseEx(ERRMODULE_DBPOSTGRESQL, DBPOSTGRESQLEX__EXEC, ERR_MYSQL, status, PQerrorMessage(conn), __LINE__, __FILE__);
	}

	empty = false;
}

//-------------------------------------------------------------------

dodoArray<dodoStringArr>
dbPostgresql::fetchRow() const
{

		#ifndef DBPOSTGRESQL_WO_XEXEC
	operType = DBPOSTGRESQL_OPERATION_FETCHROW;
	performXExec(preExec);
		#endif

	if (empty || !show)
		return __dodostringarrayarray__;

	int rowsNum = PQntuples(pgResult);
	int fieldsNum = PQnfields(pgResult);

	dodoArray<dodoStringArr> rows;

		#ifndef USE_DEQUE
	rows.reserve(rowsNum);
		#endif

	int j;

	dodoStringArr rowsPart;
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

dodoStringArr
dbPostgresql::fetchField() const
{
		#ifndef DBPOSTGRESQL_WO_XEXEC
	operType = DBPOSTGRESQL_OPERATION_FETCHFIELD;
	performXExec(preExec);
		#endif

	if (empty || !show)
		return __dodostringarray__;

	int fieldsNum = PQnfields(pgResult);

	dodoStringArr fields;

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

__dbStorage
dbPostgresql::fetch() const
{
	return __dbStorage(fetchRow(), fetchField());
}

//-------------------------------------------------------------------

unsigned int
dbPostgresql::rowsCount() const
{
	if (empty || !show)
		return 0;
	else
		return PQntuples(pgResult);
}

//-------------------------------------------------------------------

unsigned int
dbPostgresql::fieldsCount() const
{
	if (empty || !show)
		return 0;
	else
		return PQnfields(pgResult);
}

//-------------------------------------------------------------------

unsigned int
dbPostgresql::affectedRowsCount() const
{
	if (empty || show)
		return 0;
	else
		return atoi(PQcmdTuples(pgResult));
}

//-------------------------------------------------------------------


void
dbPostgresql::exec(const dodoString &query,
				   bool result)
{
		#ifndef DBPOSTGRESQL_WO_XEXEC
	operType = DBPOSTGRESQL_OPERATION_EXEC;
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
dbPostgresql::addPostExec(inExec func,
						  void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data);
}

//-------------------------------------------------------------------

int
dbPostgresql::addPreExec(inExec func,
						 void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data);
}

//-------------------------------------------------------------------

		#ifdef DL_EXT

int
dbPostgresql::addPostExec(const dodoString &module,
						  void             *data,
						  void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data, toInit);
}

//-------------------------------------------------------------------

int
dbPostgresql::addPreExec(const dodoString &module,
						 void             *data,
						 void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data, toInit);
}

//-------------------------------------------------------------------

__xexecCounts
dbPostgresql::addExec(const dodoString &module,
					  void             *data,
					  void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_DBPOSTGRESQL, data, toInit);
}

		#endif

//-------------------------------------------------------------------

	#endif

//-------------------------------------------------------------------

dodoStringMapArray
dbPostgresql::fetchAssoc() const
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
			if (PQgetisnull(pgResult, i, j) == 0)
				rowPart.assign("NULL");
			else
			{
				if (preventEscaping)
					rowPart.assign(PQgetvalue(pgResult, i, j), PQgetlength(pgResult, i, j));
				else
					rowPart.assign(unescapeFields(dodoString(PQgetvalue(pgResult, i, j), PQgetlength(pgResult, i, j))));
			}

			rowFieldsPart.insert(PQfname(pgResult, i), rowPart);
		}

		rowsFields.push_back(rowFieldsPart);
	}

	return rowsFields;
}

//-------------------------------------------------------------------

void
dbPostgresql::setCharset(const dodoString &charset)
{
	int status = PQsetClientEncoding(conn, charset.c_str());
	if (status == -1)
		throw baseEx(ERRMODULE_DBPOSTGRESQL, DBPOSTGRESQLEX_SETCHARSET, ERR_MYSQL, status, PQerrorMessage(conn), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
dbPostgresql::getCharset() const
{
	return PQclientEncoding(conn);
}

//-------------------------------------------------------------------

#endif
