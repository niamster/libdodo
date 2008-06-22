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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/dbSqlite.h>

#ifdef SQLITE_EXT

using namespace dodo::db;

sqlite::sqlite() : empty(true),
				   hint(SQLITE_HINT_NONE)

{
#ifndef DB_WO_XEXEC

	execObject = XEXEC_OBJECT_DBSQLITE;

#endif

#ifndef SQLITE_ENABLE_COLUMN_METADATA

	autoFraming = false;

#endif
}

//-------------------------------------------------------------------

sqlite::sqlite(sqlite &a_pp)
{
}

//-------------------------------------------------------------------

sqlite::~sqlite()
{
	if (connected)
	{
		if (!empty)
			sqlite3_finalize(sqliteResult);

		sqlite3_close(sqliteHandle);
	}
}

//-------------------------------------------------------------------

dodoString
sqlite::fieldCollect(const __connectorField &row)
{
	int type = row.type, flag = row.flag;
	dodoString resRow(row.name + " " + sqlDataType(type));

	if (preventEscaping)
		resRow.append(!row.set_enum.empty() ? " (" + tools::misc::implode(row.set_enum, ",") + ")" : __dodostring__);
	else
		resRow.append(!row.set_enum.empty() ? " (" + tools::misc::implode(row.set_enum, escapeFields, ",") + ")" : __dodostring__);
	resRow.append((chkRange(type) > 0 && row.length > 0) ? " (" + tools::string::lToString(row.length) + ") " : __dodostring__);
	resRow.append(row.charset.size() > 0 ? " collate " + row.charset : " ");
	resRow.append(isSetFlag(flag, CONNECTOR_FIELDFLAG_NULL) ? " null " : " not null ");
	resRow.append(row.defaultVal.size() > 0 ? "default '" + row.defaultVal + "' " : __dodostring__);
	resRow.append(isSetFlag(flag, CONNECTOR_FIELDFLAG_AUTO_INCREMENT) ? " primary key autoincrement" : __dodostring__);

	if (row.refTable.size() > 0)
	{
		resRow.append(" references " + row.refTable);
		resRow.append(!row.refFields.empty() ? "(" + tools::misc::implode(row.set_enum, ",") + " )" : __dodostring__);
		resRow.append(row.onDelete >= 0 ? " on delete " + stringReference(row.onDelete) : __dodostring__);
		resRow.append(row.onUpdate >= 0 ? " on update " + stringReference(row.onUpdate) : __dodostring__);
	}

	return resRow;
}

//-------------------------------------------------------------------

void
sqlite::connect()
{
	if (connected)
	{
		if (!empty)
		{
			empty = true;
			sqlite3_finalize(sqliteResult);
		}

		if (sqlite3_close(sqliteHandle) != SQLITE_OK)
			throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_CONNECT, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);

		connected = false;
	}

#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	if (sqlite3_open(collectedData.dbInfo.path.c_str(), &sqliteHandle) != SQLITE_OK)
	{
		sqlite3_close(sqliteHandle);

		throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_CONNECT, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);
	}

#ifndef DB_WO_XEXEC
	performXExec(postExec);
#endif

	connected = true;
}

//-------------------------------------------------------------------

void
sqlite::disconnect()
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
			sqlite3_finalize(sqliteResult);
		}

		if (sqlite3_close(sqliteHandle) != SQLITE_OK)
			throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_DISCONNECT, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);

#ifndef DB_WO_XEXEC
		performXExec(postExec);
#endif

		connected = false;
	}
}

//-------------------------------------------------------------------

void
sqlite::setBLOBValues(const dodoStringArray &values)
{
	blobs = values;
}

//-------------------------------------------------------------------

dodoArray<dodo::dodoStringArray>
sqlite::fetchRow() const
{
#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_FETCHROW;
	performXExec(preExec);
#endif

	if (!show)
		return __dodostringarrayarray__;

	sqlite3_reset(sqliteResult);

	unsigned int numFields = sqlite3_column_count(sqliteResult);

	bool iterate = true;
	unsigned int i = 0;

	dodoArray<dodoStringArray> rows;

#ifndef USE_DEQUE
	rows.reserve(sqlite3_data_count(sqliteResult));
#endif

	dodoStringArray rowsPart;
	dodoString rowPart;

	int result;

	while (iterate)
	{
		result = sqlite3_step(sqliteResult);
		switch (result)
		{
			case SQLITE_BUSY:

				continue;

			case SQLITE_DONE:

				iterate = false;

				break;

			case SQLITE_ERROR:

				throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_FETCHROW, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);

			case SQLITE_ROW:

				rowsPart.clear();

#ifndef USE_DEQUE
				rowsPart.reserve(numFields);
#endif

				for (i = 0; i < numFields; ++i)
					switch (sqlite3_column_type(sqliteResult, i))
					{
						case SQLITE_INTEGER:

							rowsPart.push_back(tools::string::lToString(sqlite3_column_int(sqliteResult, i)));

							break;

						case SQLITE_FLOAT:

							rowsPart.push_back(tools::string::dToString(sqlite3_column_double(sqliteResult, i)));

							break;

						case SQLITE_TEXT:

							rowPart = (const char *)sqlite3_column_text(sqliteResult, i);
							if (preventEscaping)
								rowsPart.push_back(rowPart);
							else
								rowsPart.push_back(unescapeFields(rowPart));

							break;

						case SQLITE_BLOB:

							rowPart.assign((const char *)sqlite3_column_blob(sqliteResult, i), sqlite3_column_bytes(sqliteResult, i));
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

#ifndef DB_WO_XEXEC
	performXExec(postExec);
#endif

	return rows;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
sqlite::fetchField() const
{
#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_FETCHFIELD;
	performXExec(preExec);
#endif

	if (!show)
		return __dodostringarray__;

	unsigned int numFields = sqlite3_column_count(sqliteResult);

	dodoStringArray fields;

#ifndef USE_DEQUE
	fields.reserve(numFields);
#endif

	for (unsigned int i(0); i < numFields; ++i)
		fields.push_back(sqlite3_column_name(sqliteResult, i));

#ifndef DB_WO_XEXEC
	performXExec(postExec);
#endif

	return fields;
}

//-------------------------------------------------------------------

__connectorStorage
sqlite::fetch() const
{
	return __connectorStorage(fetchRow(), fetchField());
}

//-------------------------------------------------------------------

unsigned int
sqlite::rowsCount() const
{
	if (show)
	{
		sqlite3_reset(sqliteResult);

		unsigned int numRows = 0;
		bool iterate = true;
		int result;

		while (iterate)
		{
			result = sqlite3_step(sqliteResult);
			switch (result)
			{
				case SQLITE_BUSY:

					continue;

				case SQLITE_DONE:

					iterate = false;

					break;

				case SQLITE_ERROR:

					throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_FETCHROW, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);

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
sqlite::fieldsCount() const
{
	if (show)
		return sqlite3_column_count(sqliteResult);
	else
		return 0;
}

//-------------------------------------------------------------------

unsigned int
sqlite::affectedRowsCount() const
{
	if (!show)
		return sqlite3_changes(sqliteHandle);
	else
		return 0;
}

//-------------------------------------------------------------------


void
sqlite::exec(const dodoString &query,
			 bool result)
{
#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_EXEC;
	performXExec(preExec);
#endif

	if (query.size() == 0)
	{
		if (autoFraming)
		{
#ifdef SQLITE_ENABLE_COLUMN_METADATA

			if (collectedData.qType == ACCUMULATOR_REQUEST_INSERT || collectedData.qType == ACCUMULATOR_REQUEST_UPDATE)
			{
				dodoString temp = dbInfo.db + ":" + collectedData.table;

				if (framingFields.find(temp) == framingFields.end())
				{
					request = "select * from " + collectedData.table + " limit 1";

					if (!empty)
					{
						sqlite3_finalize(sqliteResult);
						empty = true;
					}

					if (sqlite3_prepare(sqliteHandle, request.c_str(), request.size(), &sqliteResult, NULL) != SQLITE_OK)
						throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_EXEC, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__, request);

					if (sqliteResult == NULL)
						throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_EXEC, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);

					empty = false;

					unsigned int numFields = sqlite3_column_count(sqliteResult);

					const char *columnType, *columnName;

					dodoStringArray temp1;

					for (unsigned int i(0); i < numFields; ++i)
					{
						columnName = sqlite3_column_name(sqliteResult, i);

						if (sqlite3_table_column_metadata(sqliteHandle,
														  NULL,
														  collectedData.table.c_str(),
														  columnName,
														  &columnType,
														  NULL,
														  NULL,
														  NULL,
														  NULL) != SQLITE_OK)
							throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_EXEC, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__, request);

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
						sqlite3_finalize(sqliteResult);
						empty = true;
					}

					framingFields.insert(make_pair(temp, temp1));
				}
			}

#endif
		}

		queryCollect();
	}

	if (!empty)
	{
		sqlite3_finalize(sqliteResult);
		empty = true;
	}

	if (sqlite3_prepare(sqliteHandle, request.c_str(), request.size(), &sqliteResult, NULL) != SQLITE_OK)
		throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_EXEC, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__, request);

	if (isSetFlag(hint, SQLITE_HINT_BLOB))
	{
		removeFlag(hint, SQLITE_HINT_BLOB);

		switch (collectedData.qType)
		{
			case ACCUMULATOR_REQUEST_UPDATE:
			case ACCUMULATOR_REQUEST_INSERT:

			{
				dodoStringArray::iterator i(blobs.begin()), j(blobs.end());
				for (int o = 1; i != j; ++i, ++o)
					if (sqlite3_bind_blob(sqliteResult, o, i->c_str(), i->size(), SQLITE_TRANSIENT) != SQLITE_OK)
						throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_EXEC, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);
			}

			break;

			default:

				throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_EXEC, ERR_LIBDODO, SQLITEEX_WRONGHINTUSAGE, DBSQLITEEX_WRONGHINTUSAGE_STR, __LINE__, __FILE__);

		}
	}

	if (sqliteResult == NULL)
		throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_EXEC, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);

	empty = false;

	if (!show)
		if (sqlite3_step(sqliteResult) != SQLITE_DONE)
			throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_FETCHROW, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);

#ifndef DB_WO_XEXEC
	performXExec(postExec);
#endif

	cleanCollected();
}

//-------------------------------------------------------------------

dodo::dodoStringMapArray
sqlite::fetchAssoc() const
{
	if (!show)
		return __dodostringmaparray__;

	sqlite3_reset(sqliteResult);

	unsigned int numFields = sqlite3_column_count(sqliteResult);

	bool iterate = true;
	unsigned int i = 0;

	dodoStringMapArray rowsFields;
	dodoStringMap rowFieldsPart;
	dodoString rowPart;

#ifndef USE_DEQUE
	rowsFields.reserve(sqlite3_data_count(sqliteResult));
#endif

	int result;

	while (iterate)
	{
		result = sqlite3_step(sqliteResult);
		switch (result)
		{
			case SQLITE_BUSY:

				continue;

			case SQLITE_DONE:

				iterate = false;

				break;

			case SQLITE_ERROR:

				throw baseEx(ERRMODULE_DBSQLITE, SQLITEEX_FETCHASSOC, ERR_SQLITE, sqlite3_errcode(sqliteHandle), sqlite3_errmsg(sqliteHandle), __LINE__, __FILE__);

			case SQLITE_ROW:

				rowFieldsPart.clear();

				for (i = 0; i < numFields; ++i)
					switch (sqlite3_column_type(sqliteResult, i))
					{
						case SQLITE_INTEGER:

							rowFieldsPart.insert(make_pair(sqlite3_column_name(sqliteResult, i), tools::string::lToString(sqlite3_column_int(sqliteResult, i))));

							break;

						case SQLITE_FLOAT:

							rowFieldsPart.insert(make_pair(sqlite3_column_name(sqliteResult, i), tools::string::dToString(sqlite3_column_double(sqliteResult, i))));

							break;

						case SQLITE_TEXT:

							rowPart = (const char *)sqlite3_column_text(sqliteResult, i);
							if (preventEscaping)
								rowFieldsPart.insert(make_pair(sqlite3_column_name(sqliteResult, i), rowPart));
							else
								rowFieldsPart.insert(make_pair(sqlite3_column_name(sqliteResult, i), unescapeFields(rowPart)));

							break;

						case SQLITE_BLOB:

							rowPart.assign((const char *)sqlite3_column_blob(sqliteResult, i), sqlite3_column_bytes(sqliteResult, i));
							if (preventEscaping)
								rowFieldsPart.insert(make_pair(sqlite3_column_name(sqliteResult, i), rowPart));
							else
								rowFieldsPart.insert(make_pair(sqlite3_column_name(sqliteResult, i), unescapeFields(rowPart)));

							break;

						case SQLITE_NULL:
						default:

							rowFieldsPart.insert(make_pair(dodoString(sqlite3_column_name(sqliteResult, i)), "NULL"));

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

