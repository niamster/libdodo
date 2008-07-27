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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/dbPostgresql.h>

#ifdef POSTGRESQL_EXT

using namespace dodo::db;

#ifdef POSTGRESQL_NO_ENCODINGTOCHAR

const dodoString postgresql::encodingStatements[] = {
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

postgresql::postgresql() : empty(true)
{
#ifndef DB_WO_XEXEC

	execObject = XEXEC_OBJECT_DBPOSTGRESQL;

#endif
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

void
postgresql::connect(const __connectionInfo &info)
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
			PQclear(pgResult);
			empty = true;
		}

		PQfinish(pgHandle);

		connected = false;
	}

	pgHandle = PQsetdbLogin(
		collectedData.dbInfo.host.size() == 0 ? NULL : collectedData.dbInfo.host.c_str(),
		tools::string::uiToString(collectedData.dbInfo.port).c_str(),
		NULL,
		NULL,
		collectedData.dbInfo.db.size() == 0 ? NULL : collectedData.dbInfo.db.c_str(),
		collectedData.dbInfo.user.size() == 0 ? NULL : collectedData.dbInfo.user.c_str(),
		collectedData.dbInfo.password.size() == 0 ? NULL : collectedData.dbInfo.password.c_str());

	int status = PQstatus(pgHandle);

	if (status != CONNECTION_OK)
		throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_CONNECT, ERR_MYSQL, status, PQerrorMessage(pgHandle), __LINE__, __FILE__);

#ifndef DB_WO_XEXEC
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
#ifndef DB_WO_XEXEC
		operType = DB_OPERATION_DISCONNECT;
		performXExec(preExec);
#endif

		if (!empty)
		{
			PQclear(pgResult);
			empty = true;
		}

		PQfinish(pgHandle);

#ifndef DB_WO_XEXEC
		performXExec(postExec);
#endif

		connected = false;
	}
}

//-------------------------------------------------------------------

dodoArray<dodo::dodoStringArray>
postgresql::fetchRows() const
{

#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_FETCHROW;
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
				rowPart.assign(statements[SQLCONSTRUCTOR_STATEMENT_NULL]);
			else
				rowPart.assign(PQgetvalue(pgResult, i, j), PQgetlength(pgResult, i, j));

			rowsPart.push_back(rowPart);
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
postgresql::fetchFields() const
{
#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_FETCHFIELD;
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

#ifndef DB_WO_XEXEC
	performXExec(postExec);
#endif

	return fields;
}

//-------------------------------------------------------------------

__tuples
postgresql::fetch() const
{
	return __tuples(fetchRows(), fetchFields());
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
postgresql::getFieldsTypes(const dodoString &table)
{

	dodoString temp = collectedData.dbInfo.db + ":" + table;

	dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(temp);

	if (types == fieldTypes.end())
		types = fieldTypes.insert(make_pair(temp, dodoMap<dodoString, short, dodoMapICaseStringCompare>())).first;

	request = "select column_name, data_type from information_schema.columns where table_name='" + table + "'";

	if (!empty)
	{
		PQclear(pgResult);
		empty = true;
	}

	pgResult = PQexecParams(pgHandle, request.c_str(), 0, NULL, NULL, NULL, NULL, 1);
	if (pgResult == NULL)
		throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(pgHandle), __LINE__, __FILE__, request);

	int status = PQresultStatus(pgResult);

	switch (status)
	{
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:

			throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, ERR_MYSQL, status, PQerrorMessage(pgHandle), __LINE__, __FILE__);
	}

	empty = false;

	int rowsNum = PQntuples(pgResult);

	const char *columnType, *columnName;

	dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

	dodoStringArray rowsPart;

	for (int i(0); i < rowsNum; ++i)
	{
		columnName = PQgetvalue(pgResult, i, 0);
		columnType = PQgetvalue(pgResult, i, 1);

		field = types->second.find(columnName);

		if (field == fieldsEnd)
		{
			if (strcasestr(columnType, "char") != NULL ||
				strcasestr(columnType, "date") != NULL ||
				strcasestr(columnType, "text") != NULL)
				types->second.insert(make_pair(dodoString(columnName), FIELDTYPE_TEXT));
			else
			{
				if (strcasestr(columnType, "bytea") != NULL ||
					strcasestr(columnType, "array") != NULL ||
					strcasestr(columnType, "cidr") != NULL ||
					strcasestr(columnType, "macaddrcd") != NULL ||
					strcasestr(columnType, "inet") != NULL)
					types->second.insert(make_pair(dodoString(columnName), FIELDTYPE_BINARY));
				else
					types->second.insert(make_pair(dodoString(columnName), FIELDTYPE_NUMERIC));
			}
		}
		else
		{
			if (strcasestr(columnType, "char") != NULL ||
				strcasestr(columnType, "date") != NULL ||
				strcasestr(columnType, "text") != NULL)
				field->second = FIELDTYPE_TEXT;
			else
			{
				if (strcasestr(columnType, "bytea") != NULL ||
					strcasestr(columnType, "array") != NULL ||
					strcasestr(columnType, "cidr") != NULL ||
					strcasestr(columnType, "macaddrcd") != NULL ||
					strcasestr(columnType, "inet") != NULL)
					field->second = FIELDTYPE_BINARY;
				else
					field->second = FIELDTYPE_NUMERIC;
			}
		}
	}

	if (!empty)
	{
		PQclear(pgResult);
		empty = true;
	}
}

//-------------------------------------------------------------------

void
postgresql::exec(const dodoString &query,
				 bool result)
{
#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_EXEC;
	performXExec(preExec);
#endif

	int status;

	if (query.size() == 0)
		queryCollect();
	else
	{
		request = query;
		show = result;
	}

	if (!empty)
	{
		PQclear(pgResult);
		empty = true;
	}

	long size = blobs.size();

	if (size > 0)
	{
		char **values = new char*[size];
		int *lengths = new int[size];
		int *formats = new int[size];

		dodoList<__blob>::iterator i(blobs.begin()), j(blobs.end());
		for (int o = 0; i != j; ++i, ++o)
		{
			values[o] = (char *)i->value->c_str();
			lengths[o] = i->value->size();
			formats[o] = 1;
		}

		pgResult = PQexecParams(pgHandle, request.c_str(), size, NULL, values, lengths, formats, 0);

		delete [] values;
		delete [] lengths;
		delete [] formats;

		blobs.clear();
	}
	else
		pgResult = PQexecParams(pgHandle, request.c_str(), 0, NULL, NULL, NULL, NULL, 1);

	if (pgResult == NULL)
		throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(pgHandle), __LINE__, __FILE__);

	status = PQresultStatus(pgResult);
	switch (status)
	{
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:

			throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_EXEC, ERR_MYSQL, status, PQerrorMessage(pgHandle), __LINE__, __FILE__);
	}

	empty = false;

#ifndef DB_WO_XEXEC
	performXExec(postExec);
#endif

	cleanCollected();
	request.clear();
}

//-------------------------------------------------------------------

void
postgresql::updateCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_UPDATE];
	request.append(collectedData.table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_SET]);

	dodoArray<dodoStringArray>::iterator v = collectedData.values.begin();
	if (v != collectedData.values.end())
	{
		unsigned int fn(collectedData.fields.size()), fv(v->size());

		unsigned int o(fn <= fv ? fn : fv);

		dodoStringArray::const_iterator i(collectedData.fields.begin()), j(v->begin());
		if (i != j)
		{
			dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + collectedData.table);
			if (types != fieldTypes.end())
			{
				dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
				dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

				__blob blob;

				unsigned int k = 1;
				for (; k < o; ++i, ++k, ++j)
				{
					request.append(*i);

					type = types->second.find(*i);
					if (type != typesEnd)
					{
						if (type->second == FIELDTYPE_TEXT)
						{
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
							request.append(escapeFields(*j));
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
						}
						else
						{
							if (type->second == FIELDTYPE_BINARY)
							{
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
								request.append("$" + tools::string::uiToString(k));
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

								blob.reference = k;
								blob.value = &(*j);

								blobs.push_back(blob);
							}
							else
							{
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
								request.append(*j);
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
							}
						}
					}
					else
					{
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
						request.append(escapeFields(*j));
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
					}
				}
				request.append(*i);

				type = types->second.find(*i);
				if (type != typesEnd)
				{
					if (type->second == FIELDTYPE_TEXT)
					{
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
						request.append(escapeFields(*j));
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
					}
					else
					{
						if (type->second == FIELDTYPE_BINARY)
						{
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
							request.append("$" + tools::string::uiToString(k));
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

							blob.reference = k;
							blob.value = &(*j);

							blobs.push_back(blob);
						}
						else
						{
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
							request.append(*j);
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
						}
					}
				}
				else
				{
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
					request.append(escapeFields(*j));
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
				}
			}
			else
			{
				for (unsigned int k(1); k < o; ++i, ++k, ++j)
				{
					request.append(*i);
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
					request.append(escapeFields(*j));
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
				}
				request.append(*i);
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
				request.append(escapeFields(*j));
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
			}
		}
	}
}

//-------------------------------------------------------------------

void
postgresql::insertCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_INSERT];
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_INTO]);
	request.append(collectedData.table);
	if (collectedData.fields.size() != 0)
	{
		request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
		request.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
	}
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_VALUES]);

	dodoArray<dodoStringArray>::iterator k(collectedData.values.begin()), l(collectedData.values.end());
	if (k != l)
	{
		dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + collectedData.table);
		if (types != fieldTypes.end())
		{
			dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
			dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

			dodoStringArray::iterator t;

			__blob blob;

			unsigned int o = 0;

			--l;
			for (; k != l; ++k)
			{
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);

				t = collectedData.fields.begin();

				dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
				for (; i != j; ++i, ++t)
				{
					type = types->second.find(*t);
					if (type != typesEnd)
					{
						if (type->second == FIELDTYPE_TEXT)
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
						else
						{
							if (type->second == FIELDTYPE_BINARY)
							{
								++o;

								request.append("$" + tools::string::uiToString(o));
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

								blob.reference = o;
								blob.value = &(*i);

								blobs.push_back(blob);

							}
							else
								request.append(*i + statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
						}
					}
					else
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
				}
				type = types->second.find(*t);
				if (type != typesEnd)
				{
					if (type->second == FIELDTYPE_TEXT)
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
					else
					{
						if (type->second == FIELDTYPE_BINARY)
						{
							++o;

							request.append("$" + tools::string::uiToString(o));

							blob.reference = o;
							blob.value = &(*i);

							blobs.push_back(blob);

						}
						else
							request.append(*i);
					}
				}
				else
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);

				request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKETCOMA]);
			}
			request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);

			t = collectedData.fields.begin();

			dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
			for (; i != j; ++i, ++t)
			{
				type = types->second.find(*t);
				if (type != typesEnd)
				{
					if (type->second == FIELDTYPE_TEXT)
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
					else
					{
						if (type->second == FIELDTYPE_BINARY)
						{
							++o;

							request.append("$" + tools::string::uiToString(o));
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

							blob.reference = o;
							blob.value = &(*i);

							blobs.push_back(blob);

						}
						else
							request.append(*i + statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
					}
				}
				else
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
			}
			type = types->second.find(*t);
			if (type != typesEnd)
			{
				if (type->second == FIELDTYPE_TEXT)
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
				else
				{
					if (type->second == FIELDTYPE_BINARY)
					{
						++o;

						request.append("$" + tools::string::uiToString(o));

						blob.reference = o;
						blob.value = &(*i);

						blobs.push_back(blob);

					}
					else
						request.append(*i);
				}
			}
			else
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);

			request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
		}
		else
		{
			--l;
			for (; k != l; ++k)
			{
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
				request.append(tools::misc::implode(*k, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKETCOMA]);
			}
			request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
			request.append(tools::misc::implode(*k, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
			request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
		}
	}
}

//-------------------------------------------------------------------

dodo::dodoStringMapArray
postgresql::fetchFieldsToRows() const
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
				rowPart.assign(statements[SQLCONSTRUCTOR_STATEMENT_NULL]);
			else
				rowPart.assign(PQgetvalue(pgResult, i, j), PQgetlength(pgResult, i, j));

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

dodoString
postgresql::getCharset() const
{
#ifdef POSTGRESQL_NO_ENCODINGTOCHAR

	int encoding = PQclientEncoding(pgHandle);

	if (encoding >= 0 && encoding < _PG_LAST_ENCODING_)
		return encodingStatements[encoding];

	return __dodostring__;

#else

	return pg_encoding_to_char(PQclientEncoding(pgHandle));

#endif
}

#endif

//-------------------------------------------------------------------

