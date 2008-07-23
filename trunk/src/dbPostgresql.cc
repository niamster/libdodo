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
postgresql::connect()
{
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
				rowPart.assign("NULL");
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

__connectorStorage
postgresql::fetch() const
{
	return __connectorStorage(fetchRows(), fetchFields());
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
#ifndef DB_WO_XEXEC
	operType = DB_OPERATION_EXEC;
	performXExec(preExec);
#endif

	bool blobHint;
	int status;

	if (query.size() == 0)
		queryCollect();

	if (!empty)
	{
		PQclear(pgResult);
		empty = true;
	}

	switch (collectedData.qType)
	{
		case ACCUMULATOR_REQUEST_UPDATE:
		case ACCUMULATOR_REQUEST_INSERT:
		{
			dodoArray<dodoStringArray>::iterator k(collectedData.values.begin()), l(collectedData.values.end());

			int size = 0;

			for (;k!=l;++k)
				size += k->size();

			char **values = new char*[size];
			int *lengths = new int[size];
			int *formats = new int[size];

			k = collectedData.values.begin();

			int o = 0;
			dodoStringArray::iterator i, j;
			for (; k != l; ++k)
			{
				i = k->begin();
				j = k->end();
				for (;i!=j;++i,++o)
				{
					values[o] = (char *)i->c_str();
					lengths[o] = i->size();
					formats[o] = 1;
				}
			}

			pgResult = PQexecParams(pgHandle, request.c_str(), size, NULL, values, lengths, formats, 0);

			delete [] values;
			delete [] lengths;
			delete [] formats;

			if (pgResult == NULL)
				throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(pgHandle), __LINE__, __FILE__);
		}

		break;

		case ACCUMULATOR_REQUEST_SELECT:

			pgResult = PQexecParams(pgHandle, request.c_str(), 0, NULL, NULL, NULL, NULL, 1);
			if (pgResult == NULL)
				throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(pgHandle), __LINE__, __FILE__);

			break;

		default:

			pgResult = PQexec(pgHandle, request.c_str());
			if (pgResult == NULL)
				throw baseEx(ERRMODULE_DBPOSTGRESQL, POSTGRESQLEX_EXEC, ERR_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(pgHandle), __LINE__, __FILE__, request);
	}

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
				rowPart.assign("NULL");
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

//-------------------------------------------------------------------

void
postgresql::updateCollect()
{
	dodoString setPart; 

	dodoArray<dodoStringArray>::iterator v = collectedData.values.begin();
	if (v != collectedData.values.end())
	{
		dodoStringArray refs;

		static dodoString ref = "$";

		for (unsigned long i=1, size=v->size(); i<=size; ++i)
			refs.push_back(ref + tools::string::ulToString(i));

		setPart = valuesName(refs, collectedData.fields, __dodostring__);
	}

	insideAddCollect(addUpEnumArr, sqlAddUpArr, ACCUMULATOR_ADDREQUESTUPDATESTATEMENTS, collectedData.qUpShift);
	dodoString temp = insideAddCollect(sqlDbDepAddUpArr, qDbDepUpShift);

	temp.append(collectedData.table);

	request = statements[SQLCONSTRUCTOR_STATEMENT_UPDATE];
	request.append(temp);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_SET]);
	request.append(setPart);
}

//-------------------------------------------------------------------

void
postgresql::insertCollect()
{
	dodoStringArray fieldsVPart;

	dodoArray<dodoStringArray>::iterator k(collectedData.values.begin()), l(collectedData.values.end());
	
	dodoString refs;
	
	static dodoString ref = "$";

	unsigned long vals = 1;
	unsigned long r = 0, size = 0;
	for (; k != l; ++k)
	{
		refs.clear();

		for (r=vals, size=k->size(); r<size; ++r)
		{
			refs.append(ref);
			refs.append(tools::string::ulToString(r));
			refs.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
		}
		refs.append(ref);
		refs.append(tools::string::ulToString(r));

		fieldsVPart.push_back(refs);
		
		vals += k->size();
	}

	dodoString fieldsPart;

	dodoStringArray::iterator i(fieldsVPart.begin()), j(fieldsVPart.end());
	if (i != j)
	{
		--j;
		for (; i != j; ++i)
		{
			fieldsPart.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
			fieldsPart.append(*i);
			fieldsPart.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKETCOMA]);
		}
		fieldsPart.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
		fieldsPart.append(*i);
		fieldsPart.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
	}

	dodoString temp = insideAddCollect(addInsEnumArr, sqlAddInsArr, ACCUMULATOR_ADDREQUESTINSERTSTATEMENTS, collectedData.qInsShift);
	temp.append(insideAddCollect(sqlDbDepAddInsArr, qDbDepInsShift));

	dodoString temp1 = collectedData.table;

	if (collectedData.fields.size() != 0)
	{
		temp1.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
		temp1.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		temp1.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
	}

	request = statements[SQLCONSTRUCTOR_STATEMENT_INSERT];
	request.append(temp);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_INTO]);
	request.append(temp1);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_VALUES]);
	request.append(fieldsPart);
}

#endif

//-------------------------------------------------------------------

