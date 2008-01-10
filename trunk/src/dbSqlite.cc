/***************************************************************************
 *            dbSqlite.cc
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

#include <libdodo/dbSqlite.h>

#ifdef SQLITE_EXT

using namespace dodo;


dbSqlite::dbSqlite() : empty(true)
{
}

//-------------------------------------------------------------------

dbSqlite::dbSqlite(dbSqlite &a_pp)
{
}

//-------------------------------------------------------------------

dbSqlite::~dbSqlite()
{
	if (connected)
	{
		if (!empty)
			sqlite3_finalize(liteStmt);

		sqlite3_close(lite);
	}
}

//-------------------------------------------------------------------

dodoString
dbSqlite::fieldCollect(const __fieldInfo &row)
{
	int type = row.type, flag = row.flag;
	dodoString resRow(row.name + " " + sqlDataType(type));

	if (preventEscaping)
		resRow.append(!row.set_enum.empty() ? " (" + tools::implode(row.set_enum, ",") + ")" : __dodostring__);
	else
		resRow.append(!row.set_enum.empty() ? " (" + tools::implode(row.set_enum, escapeFields, ",") + ")" : __dodostring__);
	resRow.append((chkRange(type) > 0 && row.length > 0) ? " (" + stringTools::lToString(row.length) + ") " : __dodostring__);
	resRow.append(row.charset.size() > 0 ? " collate " + row.charset : " ");
	resRow.append((DBBASE_FIELDFLAG_NULL & flag) == DBBASE_FIELDFLAG_NULL ? " null " : " not null ");
	resRow.append(row.defaultVal.size() > 0 ? "default '" + row.defaultVal + "' " : __dodostring__);
	resRow.append((DBBASE_FIELDFLAG_AUTO_INCREMENT & flag) == DBBASE_FIELDFLAG_AUTO_INCREMENT ? " primary key autoincrement" : __dodostring__);

	if (row.refTable.size() > 0)
	{
		resRow.append(" references " + row.refTable);
		resRow.append(!row.refFields.empty() ? "(" + tools::implode(row.set_enum, ",") + " )" : __dodostring__);
		resRow.append(row.onDelete >= 0 ? " on delete " + stringReference(row.onDelete) : __dodostring__);
		resRow.append(row.onUpdate >= 0 ? " on update " + stringReference(row.onUpdate) : __dodostring__);
	}

	return resRow;
}

//-------------------------------------------------------------------

void
dbSqlite::connect()
{
	if (connected)
	{
		if (!empty)
		{
			empty = true;
			sqlite3_finalize(liteStmt);
		}

		if (sqlite3_close(lite) != SQLITE_OK)
			throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX_CONNECT, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);

		connected = false;
	}

#ifndef DBSQLITE_WO_XEXEC
	operType = DBSQLITE_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	if (sqlite3_open(dbInfo.path.c_str(), &lite) != SQLITE_OK)
	{
		sqlite3_close(lite);

		throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX_CONNECT, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);
	}

#ifndef DBSQLITE_WO_XEXEC
	performXExec(postExec);
#endif

	connected = true;
}

//-------------------------------------------------------------------

void
dbSqlite::disconnect()
{
	if (connected)
	{
#ifndef DBSQLITE_WO_XEXEC
		operType = DBSQLITE_OPERATION_DISCONNECT;
		performXExec(preExec);
#endif

		if (!empty)
		{
			empty = true;
			sqlite3_finalize(liteStmt);
		}

		if (sqlite3_close(lite) != SQLITE_OK)
			throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX_DISCONNECT, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);

#ifndef DBSQLITE_WO_XEXEC
		performXExec(postExec);
#endif

		connected = false;
	}
}

//-------------------------------------------------------------------

void
dbSqlite::setBLOBValues(const dodoStringArray &values)
{
	blobs = values;
}

//-------------------------------------------------------------------

void
dbSqlite::_exec(const dodoString &query,
				bool result)
{
	bool blobHint;

	if (query.size() == 0)
	{
		if (autoFraming)
		{
#ifdef SQLITE_ENABLE_COLUMN_METADATA

			if (qType == DBBASE_REQUEST_INSERT || qType == DBBASE_REQUEST_UPDATE)
			{
				dodoString temp = dbInfo.db + ":" + pre_table;

				if (!framingFields.isset(temp))
				{
					request = "select * from " + pre_table + " limit 1";

					if (!empty)
					{
						sqlite3_finalize(liteStmt);
						empty = true;
					}

					if (sqlite3_prepare(lite, request.c_str(), request.size(), &liteStmt, NULL) != SQLITE_OK)
						throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__, request);

					if (liteStmt == NULL)
						throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);

					empty = false;

					unsigned int numFields = sqlite3_column_count(liteStmt);

					const char *columnType, *columnName;

					dodoStringArray temp1;

					for (unsigned int i(0); i < numFields; ++i)
					{
						columnName = sqlite3_column_name(liteStmt, i);

						if (sqlite3_table_column_metadata(lite,
														  NULL,
														  pre_table.c_str(),
														  columnName,
														  &columnType,
														  NULL,
														  NULL,
														  NULL,
														  NULL) != SQLITE_OK)
							throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__, request);

						if (strcasestr(columnType, "char") != NULL ||
							strcasestr(columnType, "date") != NULL ||
							strcasestr(columnType, "time") != NULL ||
							strcasestr(columnType, "blob") != NULL ||
							strcasestr(columnType, "text") != NULL ||
							strcasestr(columnType, "enum") != NULL ||
							strcasestr(columnType, "set") != NULL)
							temp1.push_back(columnName);
					}

					if (!empty)
					{
						sqlite3_finalize(liteStmt);
						empty = true;
					}

					framingFields.insert(temp, temp1);
				}
			}

#else

			autoFraming = false;

#endif

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
		sqlite3_finalize(liteStmt);
		empty = true;
	}

	if (sqlite3_prepare(lite, request.c_str(), request.size(), &liteStmt, NULL) != SQLITE_OK)
		throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__, request);

	if (blobHint)
	{
		switch (qType)
		{
			case DBBASE_REQUEST_UPDATE:
			case DBBASE_REQUEST_INSERT:

			{
				dodoStringArray::iterator i(blobs.begin()), j(blobs.end());
				for (int o = 1; i != j; ++i, ++o)
					if (sqlite3_bind_blob(liteStmt, o, i->c_str(), i->size(), SQLITE_TRANSIENT) != SQLITE_OK)
						throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);
			}

				break;

			default:

				throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX__EXEC, ERR_LIBDODO, DBSQLITEEX_WRONGHINTUSAGE, DBSQLITEEX_WRONGHINTUSAGE_STR, __LINE__, __FILE__);

		}
	}

	if (liteStmt == NULL)
		throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);

	empty = false;

	if (!show)
		if (sqlite3_step(liteStmt) != SQLITE_DONE)
			throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX_FETCHROW, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

dodoArray<dodoStringArray>
dbSqlite::fetchRow() const
{
#ifndef DBSQLITE_WO_XEXEC
	operType = DBSQLITE_OPERATION_FETCHROW;
	performXExec(preExec);
#endif

	if (!show)
		return __dodostringarrayarray__;

	sqlite3_reset(liteStmt);

	unsigned int numFields = sqlite3_column_count(liteStmt);

	bool iterate = true;
	unsigned int i = 0;

	dodoArray<dodoStringArray> rows;

#ifndef USE_DEQUE
	rows.reserve(sqlite3_data_count(liteStmt));
#endif

	dodoStringArray rowsPart;
	dodoString rowPart;

	int result;

	while (iterate)
	{
		result = sqlite3_step(liteStmt);
		switch (result)
		{
			case SQLITE_BUSY:

				continue;

			case SQLITE_DONE:

				iterate = false;

				break;

			case SQLITE_ERROR:

				throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX_FETCHROW, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);

			case SQLITE_ROW:

				rowsPart.clear();

#ifndef USE_DEQUE
				rowsPart.reserve(numFields);
#endif

				for (i = 0; i < numFields; ++i)
					switch (sqlite3_column_type(liteStmt, i))
					{
						case SQLITE_INTEGER:

							rowsPart.push_back(stringTools::lToString(sqlite3_column_int(liteStmt, i)));

							break;

						case SQLITE_FLOAT:

							rowsPart.push_back(stringTools::dToString(sqlite3_column_double(liteStmt, i)));

							break;

						case SQLITE_TEXT:

							rowPart = (const char *)sqlite3_column_text(liteStmt, i);
							if (preventEscaping)
								rowsPart.push_back(rowPart);
							else
								rowsPart.push_back(unescapeFields(rowPart));

							break;

						case SQLITE_BLOB:

							rowPart.assign((const char *)sqlite3_column_blob(liteStmt, i), sqlite3_column_bytes(liteStmt, i));
							if (preventEscaping)
								rowsPart.push_back(rowPart);
							else
								rowsPart.push_back(unescapeFields(rowPart));

							break;

						case SQLITE_NULL:
						default:

							rowsPart.push_back("NULL");

							break;
					}

				rows.push_back(rowsPart);

				break;
		}
	}

#ifndef DBSQLITE_WO_XEXEC
	performXExec(postExec);
#endif

	return rows;
}

//-------------------------------------------------------------------

dodoStringArray
dbSqlite::fetchField() const
{
#ifndef DBSQLITE_WO_XEXEC
	operType = DBSQLITE_OPERATION_FETCHFIELD;
	performXExec(preExec);
#endif

	if (!show)
		return __dodostringarray__;

	unsigned int numFields = sqlite3_column_count(liteStmt);

	dodoStringArray fields;

#ifndef USE_DEQUE
	fields.reserve(numFields);
#endif

	for (unsigned int i(0); i < numFields; ++i)
		fields.push_back(sqlite3_column_name(liteStmt, i));

#ifndef DBSQLITE_WO_XEXEC
	performXExec(postExec);
#endif

	return fields;
}

//-------------------------------------------------------------------

__dbStorage
dbSqlite::fetch() const
{
	return __dbStorage(fetchRow(), fetchField());
}

//-------------------------------------------------------------------

unsigned int
dbSqlite::rowsCount() const
{
	if (show)
	{
		sqlite3_reset(liteStmt);

		unsigned int numRows = 0;
		bool iterate = true;
		int result;

		while (iterate)
		{
			result = sqlite3_step(liteStmt);
			switch (result)
			{
				case SQLITE_BUSY:

					continue;

				case SQLITE_DONE:

					iterate = false;

					break;

				case SQLITE_ERROR:

					throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX_FETCHROW, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);

				case SQLITE_ROW:

					++numRows;

					break;
			}
		}

		return numRows;
	}
	else
		return 0;
}

//-------------------------------------------------------------------

unsigned int
dbSqlite::fieldsCount() const
{
	if (show)
		return sqlite3_column_count(liteStmt);
	else
		return 0;
}

//-------------------------------------------------------------------

unsigned int
dbSqlite::affectedRowsCount() const
{
	if (!show)
		return sqlite3_changes(lite);
	else
		return 0;
}

//-------------------------------------------------------------------


void
dbSqlite::exec(const dodoString &query,
			   bool result)
{
#ifndef DBSQLITE_WO_XEXEC
	operType = DBSQLITE_OPERATION_EXEC;
	performXExec(preExec);
#endif

	_exec(query, result);

#ifndef DBSQLITE_WO_XEXEC
	performXExec(postExec);
#endif

	cleanCollected();
}

//-------------------------------------------------------------------

#ifndef DBSQLITE_WO_XEXEC

int
dbSqlite::addPostExec(inExec func,
					  void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_DBSQLITE, data);
}

//-------------------------------------------------------------------

int
dbSqlite::addPreExec(inExec func,
					 void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_DBSQLITE, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
dbSqlite::addPostExec(const dodoString &module,
					  void             *data,
					  void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_DBSQLITE, data, toInit);
}

//-------------------------------------------------------------------

int
dbSqlite::addPreExec(const dodoString &module,
					 void             *data,
					 void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_DBSQLITE, data, toInit);
}

//-------------------------------------------------------------------

__xexecCounts
dbSqlite::addExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_DBSQLITE, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

dodoStringMapArray
dbSqlite::fetchAssoc() const
{
	if (!show)
		return __dodostringmaparray__;

	sqlite3_reset(liteStmt);

	unsigned int numFields = sqlite3_column_count(liteStmt);

	bool iterate = true;
	unsigned int i = 0;

	dodoStringMapArray rowsFields;
	dodoStringMap rowFieldsPart;
	dodoString rowPart;

#ifndef USE_DEQUE
	rowsFields.reserve(sqlite3_data_count(liteStmt));
#endif

	int result;

	while (iterate)
	{
		result = sqlite3_step(liteStmt);
		switch (result)
		{
			case SQLITE_BUSY:

				continue;

			case SQLITE_DONE:

				iterate = false;

				break;

			case SQLITE_ERROR:

				throw baseEx(ERRMODULE_DBSQLITE, DBSQLITEEX_FETCHASSOC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite), __LINE__, __FILE__);

			case SQLITE_ROW:

				rowFieldsPart.clear();

				for (i = 0; i < numFields; ++i)
					switch (sqlite3_column_type(liteStmt, i))
					{
						case SQLITE_INTEGER:

							rowFieldsPart.insert(sqlite3_column_name(liteStmt, i), stringTools::lToString(sqlite3_column_int(liteStmt, i)));

							break;

						case SQLITE_FLOAT:

							rowFieldsPart.insert(sqlite3_column_name(liteStmt, i), stringTools::dToString(sqlite3_column_double(liteStmt, i)));

							break;

						case SQLITE_TEXT:

							rowPart = (const char *)sqlite3_column_text(liteStmt, i);
							if (preventEscaping)
								rowFieldsPart.insert(sqlite3_column_name(liteStmt, i), rowPart);
							else
								rowFieldsPart.insert(sqlite3_column_name(liteStmt, i), unescapeFields(rowPart));

							break;

						case SQLITE_BLOB:

							rowPart.assign((const char *)sqlite3_column_blob(liteStmt, i), sqlite3_column_bytes(liteStmt, i));
							if (preventEscaping)
								rowFieldsPart.insert(sqlite3_column_name(liteStmt, i), rowPart);
							else
								rowFieldsPart.insert(sqlite3_column_name(liteStmt, i), unescapeFields(rowPart));

							break;

						case SQLITE_NULL:
						default:

							rowFieldsPart.insert(sqlite3_column_name(liteStmt, i), "NULL");

							break;
					}

				rowsFields.push_back(rowFieldsPart);

				break;
		}
	}

	return rowsFields;
}

#endif

//-------------------------------------------------------------------

