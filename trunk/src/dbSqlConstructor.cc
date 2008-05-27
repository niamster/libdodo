/***************************************************************************
 *            dbSqlConstructor.cc
 *
 *  Mon Jul 18 19:30:55 2005
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

#include <libdodo/dbSqlConstructor.h>

using namespace dodo::db;

const unsigned int sqlConstructor::addInsEnumArr[] =
{
	ACCUMULATOR_ADDREQUEST_INSERT_IGNORE,
};

//-------------------------------------------------------------------

const unsigned int sqlConstructor::addUpEnumArr[] =
{
	ACCUMULATOR_ADDREQUEST_UPDATE_IGNORE,
};

//-------------------------------------------------------------------

const unsigned int sqlConstructor::addDelEnumArr[] =
{
	ACCUMULATOR_ADDREQUEST_DELETE_IGNORE,
};

//-------------------------------------------------------------------

const unsigned int sqlConstructor::addSelEnumArr[] =
{
	ACCUMULATOR_ADDREQUEST_SELECT_DISTINCT,
	ACCUMULATOR_ADDREQUEST_SELECT_ALL
};

//-------------------------------------------------------------------

const dodoString sqlConstructor::sqlAddInsArr[] =
{
	" ignore ",
};

//-------------------------------------------------------------------

const dodoString sqlConstructor::sqlAddUpArr[] =
{
	" ignore ",
};

//-------------------------------------------------------------------

const dodoString sqlConstructor::sqlAddDelArr[] =
{
	" ignore ",
};

//-------------------------------------------------------------------

const dodoString sqlConstructor::sqlAddSelArr[] =
{
	" distinct ",
	" all ",
};

//-------------------------------------------------------------------

const dodoString sqlConstructor::sqlQStArr[] =
{
	" union ",
	" union all ",
	" minus ",
	" intersect "
};

//-------------------------------------------------------------------

const dodoString sqlConstructor::sqlJoinArr[] =
{
	" join ",
	" left outer join ",
	" right outer join ",
	" full outer join ",
	" inner join ",
	" cross join "
};

//-------------------------------------------------------------------

const dodoString sqlConstructor::sqlAddArr[] =
{
	" where ",
	" having ",
	" group by ",
	" order by ",
	" limit ",
	" offset ",
	" as ",
};

//-------------------------------------------------------------------

const dodoString sqlConstructor::statements[] =
{
	"=",
	"='",
	"'",
	",",
	" ( ",
	" ) ",
	"select ",
	"call ",
	" from ",
	":",
	"',",
	"),",
	"insert ",
	" into ",
	" values ",
	") select ",
	"update ",
	" set ",
	"delete ",
};

//-------------------------------------------------------------------

sqlConstructor::sqlConstructor() : preventFraming(false),
								   preventEscaping(false),
								   autoFraming(true)
{
}

//-------------------------------------------------------------------

sqlConstructor::~sqlConstructor()
{
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::valuesName(const dodoStringArray &values,
						   const dodoStringArray &fields,
						   const dodoString &frame)
{
	dodoString temp;

	unsigned int fn(fields.size()), fv(values.size());

	unsigned int o(fn <= fv ? fn : fv);

	dodoStringArray::const_iterator i(fields.begin()), j(values.begin());
	if (i != j)
	{
		--o;
		for (unsigned int k(0); k < o; ++i, ++k, ++j)
		{
			temp.append(*i);
			temp.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
			temp.append(frame);
			temp.append(preventEscaping ? *j : escapeFields(*j));
			temp.append(frame);
			temp.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
		}
		temp.append(*i);
		temp.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
		temp.append(frame);
		temp.append(preventEscaping ? *j : escapeFields(*j));
		temp.append(frame);
	}

	return temp;
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::exists(const dodoString &statement)
{
	return dodoString("exists(" + statement + statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::notexists(const dodoString &statement)
{
	return dodoString("not exists(" + statement + statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
sqlConstructor::additionalCollect(unsigned int qTypeTocheck,
								  const dodoString &collectedString)
{
	if (qShift == ACCUMULATOR_NONE)
		return ;

	if (isSetFlag(qShift, 1 << qTypeTocheck))
	{
		request.append(sqlAddArr[qTypeTocheck - 1]);
		request.append(collectedString);
	}
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::insideAddCollect(const unsigned int sqlAddEnumArr[],
								 const dodoString sqlAddArr[],
								 int qTypeShift)
{
	if (qTypeShift == ACCUMULATOR_NONE)
		return __dodostring__;

	dodoString temp;

	for (unsigned int i = 0; i < sizeof(sqlAddEnumArr) / sizeof(unsigned int); ++i)
	{
		if (isSetFlag(qTypeShift, 1 << sqlAddEnumArr[i]))
			temp.append(sqlAddArr[sqlAddEnumArr[i] - 1]);
	}

	return temp;
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::insideAddCollect(const dodoStringArray &statements,
								 int qTypeShift)
{
	if (qTypeShift == ACCUMULATOR_NONE)
		return __dodostring__;

	dodoString temp;

	unsigned int k(1);

	dodoStringArray::const_iterator i(statements.begin()), j(statements.end());
	for (; i != j; ++i, ++k)
	{
		if (isSetFlag(qTypeShift, 1 << k))
			temp.append(*i);
	}

	return temp;

}

//-------------------------------------------------------------------

void
sqlConstructor::callFunctionCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_SELECT];
	request.append(pre_table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);

	if (preventEscaping)
	{
		if (preventFraming)
			request.append(tools::misc::implode(pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		else
			request.append(tools::misc::implode(pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
	}
	else
	{
		if (preventFraming)
			request.append(tools::misc::implode(pre_fields, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		else
			request.append(tools::misc::implode(pre_fields, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
	}

	request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
sqlConstructor::callProcedureCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_CALL];
	request.append(pre_table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);

	if (preventEscaping)
	{
		if (preventFraming)
			request.append(tools::misc::implode(pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		else
			request.append(tools::misc::implode(pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
	}
	else
	{
		if (preventFraming)
			request.append(tools::misc::implode(pre_fields, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		else
			request.append(tools::misc::implode(pre_fields, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
	}

	request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
sqlConstructor::selectCollect()
{
	dodoString temp = insideAddCollect(addSelEnumArr, sqlAddSelArr, qSelShift);
	temp.append(insideAddCollect(sqlDbDepAddSelArr, qDbDepSelShift));

	if (pre_table.size() > 0)
	{
		temp.append(tools::misc::implode(pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));

		request = statements[SQLCONSTRUCTOR_STATEMENT_SELECT];
		request.append(temp);
		request.append(statements[SQLCONSTRUCTOR_STATEMENT_FROM]);
		request.append(pre_table);
	}
	else
	{
		temp.append(tools::misc::implode(pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		request = statements[SQLCONSTRUCTOR_STATEMENT_SELECT];
		request.append(temp);
	}
}

//-------------------------------------------------------------------

void
sqlConstructor::insertCollect()
{
	dodoStringArray fieldsVPart;

	dodoArray<dodoStringArray>::iterator k(pre_values.begin()), l(pre_values.end());

	dodoMap<dodoString, dodoStringArray>::iterator y = framingFields.find(dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + pre_table);

	if (autoFraming && !preventFraming && y != framingFields.end() && pre_fields.size() != 0)
	{
		dodoStringArray::iterator t;

		dodoString temp;

		for (; k != l; ++k)
		{
			temp.clear();

			t = pre_fields.begin();

			dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
			for (; i != j; ++i, ++t)
			{
				if (tools::misc::isInArray(y->second, *t, true))
				{
					if (preventEscaping)
						temp.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + *i + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
					else
						temp.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
				}
				else
					temp.append(*i + statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
			}
			if (tools::misc::isInArray(y->second, *t, true))
			{
				if (preventEscaping)
					temp.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + *i + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
				else
					temp.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
			}
			else
				temp.append(*i);

			fieldsVPart.push_back(temp);
		}
	}
	else
	{
		for (; k != l; ++k)
		{
			if (preventEscaping)
			{
				if (preventFraming)
					fieldsVPart.push_back(tools::misc::implode(*k, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
				else
					fieldsVPart.push_back(tools::misc::implode(*k, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
			}
			else
			{
				if (preventFraming)
					fieldsVPart.push_back(tools::misc::implode(*k, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
				else
					fieldsVPart.push_back(tools::misc::implode(*k, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
			}
		}
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

	dodoString temp = insideAddCollect(addInsEnumArr, sqlAddInsArr, qInsShift);
	temp.append(insideAddCollect(sqlDbDepAddInsArr, qDbDepInsShift));

	dodoString temp1 = pre_table;

	if (pre_fields.size() != 0)
	{
		temp1.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
		temp1.append(tools::misc::implode(pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		temp1.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
	}

	request = statements[SQLCONSTRUCTOR_STATEMENT_INSERT];
	request.append(temp);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_INTO]);
	request.append(temp1);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_VALUES]);
	request.append(fieldsPart);
}

//-------------------------------------------------------------------

void
sqlConstructor::insertSelectCollect()
{
	dodoString fieldsPartTo = tools::misc::implode(pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
	dodoString fieldsPartFrom = tools::misc::implode(pre_values.front(), statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

	dodoString temp = insideAddCollect(addInsEnumArr, sqlAddInsArr, qInsShift);
	temp.append(insideAddCollect(sqlDbDepAddInsArr, qDbDepInsShift));

	dodoString tempS = insideAddCollect(addSelEnumArr, sqlAddSelArr, qSelShift);
	tempS.append(insideAddCollect(sqlDbDepAddSelArr, qDbDepSelShift));

	tempS.append(fieldsPartFrom);

	request = statements[SQLCONSTRUCTOR_STATEMENT_INSERT];
	request.append(temp);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_INTO]);
	request.append(pre_tableTo);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
	request.append(fieldsPartTo);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKETSELECT]);
	request.append(tempS);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_FROM]);
	request.append(pre_table);
}

//-------------------------------------------------------------------

void
sqlConstructor::updateCollect()
{
	dodoString setPart;

	dodoMap<dodoString, dodoStringArray>::iterator y = framingFields.find(dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + pre_table);

	if (autoFraming && !preventFraming && y != framingFields.end() && pre_fields.size() != 0)
	{
		unsigned int fn(pre_fields.size()), fv(pre_values.front().size());
		unsigned int o(fn <= fv ? fn : fv);

		dodoStringArray::iterator i(pre_fields.begin()), j(pre_values.front().begin());
		for (unsigned int k(0); k < o - 1; ++i, ++j, ++k)
		{
			if (tools::misc::isInArray(y->second, *i, true))
			{
				setPart.append(*i);
				setPart.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
				setPart.append(preventEscaping ? *j : escapeFields(*j));
				setPart.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
			}
			else
			{
				setPart.append(*i);
				setPart.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
				setPart.append(preventEscaping ? *j : escapeFields(*j));
				setPart.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
			}
		}
		if (tools::misc::isInArray(y->second, *i, true))
		{
			setPart.append(*i);
			setPart.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
			setPart.append(preventEscaping ? *j : escapeFields(*j));
			setPart.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
		}
		else
		{
			setPart.append(*i);
			setPart.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
			setPart.append(preventEscaping ? *j : escapeFields(*j));
		}
	}
	else
	{
		if (preventFraming)
			setPart = valuesName(pre_values.front(), pre_fields, __dodostring__);
		else
			setPart = valuesName(pre_values.front(), pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
	}

	insideAddCollect(addUpEnumArr, sqlAddUpArr, qUpShift);
	dodoString temp = insideAddCollect(sqlDbDepAddUpArr, qDbDepUpShift);

	temp.append(pre_table);

	request = statements[SQLCONSTRUCTOR_STATEMENT_UPDATE];
	request.append(temp);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_SET]);
	request.append(setPart);
}

//-------------------------------------------------------------------

void
sqlConstructor::delCollect()
{
	dodoString temp = insideAddCollect(addDelEnumArr, sqlAddDelArr, qDelShift);
	temp.append(insideAddCollect(sqlDbDepAddDelArr, qDbDepDelShift));

	request = statements[SQLCONSTRUCTOR_STATEMENT_DELETE];
	request.append(temp);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_FROM]);
	request.append(pre_table);
}

//-------------------------------------------------------------------

void
sqlConstructor::subCollect()
{
	request = tools::misc::implode(pre_subQueries, sqlQStArr[qType - 1]);
}

//-------------------------------------------------------------------

void
sqlConstructor::truncateCollect()
{
	request = "truncate " + pre_table;
}

//-------------------------------------------------------------------

void
sqlConstructor::delDbCollect()
{
	request = "drop database " + pre_order;
}

//-------------------------------------------------------------------

void
sqlConstructor::delTableCollect()
{
	request = "drop table " + pre_table;
}

//-------------------------------------------------------------------

void
sqlConstructor::delFieldCollect()
{
	request = "alter " + pre_order + " drop " + pre_table;
}

//-------------------------------------------------------------------

void
sqlConstructor::renameDbCollect()
{
	request = __dodostring__;
}

//-------------------------------------------------------------------

void
sqlConstructor::renameTableCollect()
{
	request = "alter table " + pre_table + " rename to " + pre_having;
}

//-------------------------------------------------------------------

void
sqlConstructor::renameFieldCollect()
{
	request = __dodostring__;
}

//-------------------------------------------------------------------

void
sqlConstructor::createDbCollect()
{
	request = "create database " + pre_order;
	if (pre_having.size() != 0)
		request.append(" character set " + pre_having);
}

//-------------------------------------------------------------------

void
sqlConstructor::createIndexCollect()
{
	request = "create index " + pre_having + " on " + pre_table + statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET] + tools::misc::implode(pre_fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]) + statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET];
}

//-------------------------------------------------------------------

void
sqlConstructor::deleteIndexCollect()
{
	request = "drop index " + pre_having + " on " + pre_table;
}

//-------------------------------------------------------------------

void
sqlConstructor::createTableCollect()
{
	request = "create table ";

	request.append(pre_tableInfo.ifNotExists ? "if not exists " : __dodostring__);

	request.append(pre_tableInfo.name + statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);

	{
		dodoArray<__connectorField>::iterator i(pre_tableInfo.fields.begin()), j(pre_tableInfo.fields.end());
		if (i != j)
		{
			--j;
			for (; i != j; ++i)
				request.append(fieldCollect(*i) + statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
			request.append(fieldCollect(*i));
		}
	}

	request.append(!pre_tableInfo.primKeys.empty() ? ", primary key (" + tools::misc::implode(pre_tableInfo.primKeys, statements[SQLCONSTRUCTOR_STATEMENT_COMA]) + statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET] : __dodostring__);
	request.append(!pre_tableInfo.uniq.empty() ? ", unique " + tools::misc::implode(pre_tableInfo.uniq, statements[SQLCONSTRUCTOR_STATEMENT_COMA]) : __dodostring__);

	request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
sqlConstructor::createFieldCollect()
{
	request = "alter table " + pre_table + " add " + fieldCollect(pre_fieldInfo);
}

//-------------------------------------------------------------------

void
sqlConstructor::joinCollect()
{
	dodoStringArray::iterator i = pre_joinTables.begin(), j = pre_joinTables.end();
	dodoStringArray::iterator o = pre_joinConds.begin(), p = pre_joinConds.end();
	dodoArray<int>::iterator m = pre_joinTypes.begin(), n = pre_joinTypes.end();
	for (; i != j; ++i, ++o, ++m)
	{
		if (*m > 0 && *m < CONNECTOR_JOINTYPEUBREQUESTSTATEMENTS)
			request.append(sqlJoinArr[*m]);
		else
			throw baseEx(ERRMODULE_DBSQLCONSTRUCTOR, SQLCONSTRUCTOREX_JOINCOLLECT, ERR_LIBDODO, SQLCONSTRUCTOREX_UNKNOWNJOINTYPE, DBSQLCONSTRUCTOREX_UNKNOWNJOINTYPE_STR, __LINE__, __FILE__);

		request.append(*i);

		if (o->size() > 0)
		{
			request.append(" on ");
			request.append(*o);
		}
	}
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::queryCollect()
{
	bool additionalActions = true;
	bool selectAction = false;

	switch (qType)
	{
		case ACCUMULATOR_REQUEST_SELECT:

			selectCollect();
			selectAction = true;

			joinCollect();

			break;

		case ACCUMULATOR_REQUEST_INSERT:

			insertCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_UPDATE:

			updateCollect();

			break;

		case ACCUMULATOR_REQUEST_DELETE:

			delCollect();

			break;

		case ACCUMULATOR_REQUEST_INSERT_SELECT:

			insertSelectCollect();
			selectAction = true;

			break;

		case CONNECTOR_SUBREQUEST_UNION:
		case CONNECTOR_SUBREQUEST_UNION_ALL:
		case CONNECTOR_SUBREQUEST_MINUS:
		case CONNECTOR_SUBREQUEST_INTERSECT:

			subCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_CALL_FUNCTION:

			callFunctionCollect();
			selectAction = true;

			break;

		case ACCUMULATOR_REQUEST_CALL_PROCEDURE:

			callProcedureCollect();

			break;

		case ACCUMULATOR_REQUEST_TRUNCATE:

			truncateCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_RENAME_DB:

			renameDbCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_CREATE_INDEX:

			createIndexCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_DELETE_INDEX:

			deleteIndexCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_RENAME_TABLE:

			renameTableCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_RENAME_FIELD:

			renameFieldCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_DELETE_DB:

			delDbCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_DELETE_TABLE:

			delTableCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_DELETE_FIELD:

			delFieldCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_CREATE_DB:

			createDbCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_CREATE_TABLE:

			createTableCollect();
			additionalActions = false;

			break;

		case ACCUMULATOR_REQUEST_CREATE_FIELD:

			createFieldCollect();
			additionalActions = false;

			break;

		default:

			additionalActions = false;
	}

#ifndef FAST

	if (request.size() == 0)
		throw baseEx(ERRMODULE_DBSQLCONSTRUCTOR, SQLCONSTRUCTOREX_QUERYCOLLECT, ERR_LIBDODO, SQLCONSTRUCTOREX_EMPTYREQUEST, DBSQLCONSTRUCTOREX_EMPTYREQUEST_STR, __LINE__, __FILE__);

#endif

	if (additionalActions)
	{
		additionalCollect(ACCUMULATOR_ADDREQUEST_AS, pre_where);
		additionalCollect(ACCUMULATOR_ADDREQUEST_WHERE, pre_where);
		if (selectAction)
		{
			additionalCollect(ACCUMULATOR_ADDREQUEST_GROUPBY, pre_group);
			additionalCollect(ACCUMULATOR_ADDREQUEST_HAVING, pre_having);
		}
		additionalCollect(ACCUMULATOR_ADDREQUEST_ORDERBY, pre_order);
		additionalCollect(ACCUMULATOR_ADDREQUEST_LIMIT, pre_limit);
		additionalCollect(ACCUMULATOR_ADDREQUEST_OFFSET, pre_offset);
	}

	return request;
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::unescapeFields(const dodoString &data)
{
	dodoString temp = data;

	tools::string::replace("\\'", statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE], temp);
	tools::string::replace("\\\\", "\\", temp);

	return temp;
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::escapeFields(const dodoString &data)
{
	dodoString temp = data;

	tools::string::replace("\\", "\\\\", temp);
	tools::string::replace(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE], "\\'", temp);

	return temp;
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::fieldCollect(const __connectorField &row)
{
	int type = row.type, flag = row.flag;
	dodoString resRow(row.name + " " + sqlDataType(type));

	if (preventEscaping)
		resRow.append(!row.set_enum.empty() ? statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET] + tools::misc::implode(row.set_enum, statements[SQLCONSTRUCTOR_STATEMENT_COMA]) + statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET] : __dodostring__);
	else
		resRow.append(!row.set_enum.empty() ? statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET] + tools::misc::implode(row.set_enum, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]) + statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET] : __dodostring__);
	resRow.append((chkRange(type) > 0 && row.length > 0) ? statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET] + tools::string::lToString(row.length) + statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET] : __dodostring__);
	resRow.append(row.charset.size() > 0 ? " collate " + row.charset : " ");
	resRow.append(isSetFlag(flag, CONNECTOR_FIELDFLAG_NULL) ? " null " : " not null ");
	resRow.append(row.defaultVal.size() > 0 ? "default '" + row.defaultVal + "' " : __dodostring__);
	resRow.append(isSetFlag(flag, CONNECTOR_FIELDFLAG_AUTO_INCREMENT) ? " primary key auto_increment" : __dodostring__);

	if (row.refTable.size() > 0)
	{
		resRow.append(" references " + row.refTable);
		resRow.append(!row.refFields.empty() ? statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET] + tools::misc::implode(row.set_enum, statements[SQLCONSTRUCTOR_STATEMENT_COMA]) + statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET] : __dodostring__);
		resRow.append(row.onDelete >= 0 ? " on delete " + stringReference(row.onDelete) : __dodostring__);
		resRow.append(row.onUpdate >= 0 ? " on update " + stringReference(row.onUpdate) : __dodostring__);
	}

	return resRow;
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::sqlDataType(int type)
{
	switch (type)
	{
		case CONNECTOR_FIELDTYPE_INT:
		case CONNECTOR_FIELDTYPE_INTEGER:

			return dodoString("INTEGER");

		case CONNECTOR_FIELDTYPE_DATE:

			return dodoString("DATE");

		case CONNECTOR_FIELDTYPE_VARCHAR:

			return dodoString("VARCHAR");

		case CONNECTOR_FIELDTYPE_TIMESTAMP:

			return dodoString("TIMESTAMP");

		case CONNECTOR_FIELDTYPE_TIME:

			return dodoString("TIME");

		case CONNECTOR_FIELDTYPE_TINYINT:

			return dodoString("TINYINT");

		case CONNECTOR_FIELDTYPE_SMALLINT:

			return dodoString("SMALLINT");

		case CONNECTOR_FIELDTYPE_MEDIUMINT:

			return dodoString("MEDIUMINT");

		case CONNECTOR_FIELDTYPE_BIGINT:

			return dodoString("BIGINT");

		case CONNECTOR_FIELDTYPE_FLOAT:

			return dodoString("FLOAT");

		case CONNECTOR_FIELDTYPE_REAL:
		case CONNECTOR_FIELDTYPE_DOUBLE:

			return dodoString("REAL");

		case CONNECTOR_FIELDTYPE_DECIMAL:

			return dodoString("DECIMAL");

		case CONNECTOR_FIELDTYPE_CHAR:

			return dodoString("CHAR");

		case CONNECTOR_FIELDTYPE_TINYBLOB:

			return dodoString("TINYBLOB");

		case CONNECTOR_FIELDTYPE_BLOB:

			return dodoString("BLOB");

		case CONNECTOR_FIELDTYPE_MEDIUMBLOB:

			return dodoString("MEDIUMBLOB");

		case CONNECTOR_FIELDTYPE_LONGBLOB:

			return dodoString("LONGBLOB");

		case CONNECTOR_FIELDTYPE_TINYTEXT:

			return dodoString("TINYTEXT");

		case CONNECTOR_FIELDTYPE_TEXT:

			return dodoString("TEXT");

		case CONNECTOR_FIELDTYPE_MEDIUMTEXT:

			return dodoString("MEDIUMTEXT");

		case CONNECTOR_FIELDTYPE_LONGTEXT:

			return dodoString("LONGTEXT");

		case CONNECTOR_FIELDTYPE_ENUM:

			return dodoString("ENUM");

		case CONNECTOR_FIELDTYPE_SET:

			return dodoString("SET");

		default:

			return __dodostring__;
	}
}

//-------------------------------------------------------------------

int
sqlConstructor::chkRange(int type)
{
	switch (type)
	{
		case CONNECTOR_FIELDTYPE_DATE:
		case CONNECTOR_FIELDTYPE_TIME:
		case CONNECTOR_FIELDTYPE_TINYBLOB:
		case CONNECTOR_FIELDTYPE_BLOB:
		case CONNECTOR_FIELDTYPE_MEDIUMBLOB:
		case CONNECTOR_FIELDTYPE_LONGBLOB:
		case CONNECTOR_FIELDTYPE_TINYTEXT:
		case CONNECTOR_FIELDTYPE_TEXT:
		case CONNECTOR_FIELDTYPE_MEDIUMTEXT:
		case CONNECTOR_FIELDTYPE_LONGTEXT:
		case CONNECTOR_FIELDTYPE_ENUM:
		case CONNECTOR_FIELDTYPE_SET:

			return -1;

		case CONNECTOR_FIELDTYPE_INTEGER:
		case CONNECTOR_FIELDTYPE_INT:
		case CONNECTOR_FIELDTYPE_TINYINT:
		case CONNECTOR_FIELDTYPE_SMALLINT:
		case CONNECTOR_FIELDTYPE_MEDIUMINT:
		case CONNECTOR_FIELDTYPE_BIGINT:
		case CONNECTOR_FIELDTYPE_FLOAT:
		case CONNECTOR_FIELDTYPE_REAL:
		case CONNECTOR_FIELDTYPE_DOUBLE:
		case CONNECTOR_FIELDTYPE_TIMESTAMP:

			return 0;

		case CONNECTOR_FIELDTYPE_VARCHAR:
		case CONNECTOR_FIELDTYPE_CHAR:
		case CONNECTOR_FIELDTYPE_DECIMAL:

			return 1;

		default:

			return -1;
	}
}

//-------------------------------------------------------------------

dodoString
sqlConstructor::stringReference(int type)
{
	switch (type)
	{
		case CONNECTOR_REFERENCE_RESTRICT:

			return dodoString("restrict");

		case CONNECTOR_REFERENCE_CASCADE:

			return dodoString("cascade");

		case CONNECTOR_REFERENCE_SET_NULL:

			return dodoString("set null");

		case CONNECTOR_REFERENCE_NO_ACTION:

			return dodoString("no action");

		case CONNECTOR_REFERENCE_SET_DEFAULT:

			return dodoString("set default");

		default:

			return __dodostring__;
	}
}

//-------------------------------------------------------------------

