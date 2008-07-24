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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/dbSqlConstructor.h>

using namespace dodo::db;

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
								   preventEscaping(false)

#ifdef ENABLE_SQL_AUTOFRAMING

								   ,
								   autoFraming(true),
								   manualAutoFraming(false)

#endif
{
}

//-------------------------------------------------------------------

sqlConstructor::~sqlConstructor()
{
}

//-------------------------------------------------------------------

void
sqlConstructor::setAutoFramingRule(const dodoString &db,
                                   const dodoString &table,
                                   const dodoStringArray &fields)
{
	framingFields[db + ":" + table] = fields;
}

//-------------------------------------------------------------------

void
sqlConstructor::removeAutoFramingRule(const dodoString &db,
                                      const dodoString &table)
{
	framingFields.erase(db + ":" + table);
}

//-------------------------------------------------------------------

void
sqlConstructor::removeAutoFramingRules()
{
	framingFields.clear();
}

//-------------------------------------------------------------------

void
sqlConstructor::additionalCollect(unsigned int qTypeTocheck,
								  const dodoString &collectedString)
{
	if (collectedData.qShift == ACCUMULATOR_NONE)
		return ;

	if (isSetFlag(collectedData.qShift, 1 << qTypeTocheck))
	{
		request.append(sqlAddArr[qTypeTocheck - 1]);
		request.append(collectedString);
	}
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

void
sqlConstructor::callFunctionCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_SELECT];
	request.append(collectedData.table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);

	if (preventEscaping)
	{
		if (preventFraming)
			request.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		else
			request.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
	}
	else
	{
		if (preventFraming)
			request.append(tools::misc::implode(collectedData.fields, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		else
			request.append(tools::misc::implode(collectedData.fields, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
	}

	request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
sqlConstructor::callProcedureCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_CALL];
	request.append(collectedData.table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);

	if (preventEscaping)
	{
		if (preventFraming)
			request.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		else
			request.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
	}
	else
	{
		if (preventFraming)
			request.append(tools::misc::implode(collectedData.fields, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		else
			request.append(tools::misc::implode(collectedData.fields, escapeFields, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
	}

	request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
sqlConstructor::selectCollect()
{
	if (collectedData.table.size() > 0)
	{
		request = statements[SQLCONSTRUCTOR_STATEMENT_SELECT];
		request.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		request.append(statements[SQLCONSTRUCTOR_STATEMENT_FROM]);
		request.append(collectedData.table);
	}
	else
	{
		request = statements[SQLCONSTRUCTOR_STATEMENT_SELECT];
		request.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
	}
}

//-------------------------------------------------------------------

void
sqlConstructor::insertCollect()
{
	dodoStringArray fieldsVPart;

	dodoArray<dodoStringArray>::iterator k(collectedData.values.begin()), l(collectedData.values.end());

	dodoMap<dodoString, dodoStringArray>::iterator y = framingFields.find(collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + collectedData.table);

#ifdef ENABLE_SQL_AUTOFRAMING

	if (autoFraming && !preventFraming && y != framingFields.end() && collectedData.fields.size() != 0)
	{
		dodoStringArray::iterator t;

		dodoString temp;

		for (; k != l; ++k)
		{
			temp.clear();

			t = collectedData.fields.begin();

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

#endif

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

	dodoString temp = collectedData.table;

	if (collectedData.fields.size() != 0)
	{
		temp.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
		temp.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		temp.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
	}

	request = statements[SQLCONSTRUCTOR_STATEMENT_INSERT];
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_INTO]);
	request.append(temp);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_VALUES]);
	request.append(fieldsPart);
}

//-------------------------------------------------------------------

void
sqlConstructor::insertSelectCollect()
{
	dodoString fieldsPartTo = tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

	dodoString fieldsPartFrom;

	dodoArray<dodoStringArray>::iterator i = collectedData.values.begin();
	if (i != collectedData.values.end())
		fieldsPartFrom = tools::misc::implode(*i, statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

	request = statements[SQLCONSTRUCTOR_STATEMENT_INSERT];
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_INTO]);
	request.append(collectedData.tableTo);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
	request.append(fieldsPartTo);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKETSELECT]);
	request.append(fieldsPartFrom);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_FROM]);
	request.append(collectedData.table);
}

//-------------------------------------------------------------------

void
sqlConstructor::updateCollect()
{
	dodoString setPart;

	dodoMap<dodoString, dodoStringArray>::iterator y = framingFields.find(collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + collectedData.table);

#ifdef ENABLE_SQL_AUTOFRAMING

	if (autoFraming && !preventFraming && y != framingFields.end() && collectedData.fields.size() != 0)
	{

		dodoArray<dodoStringArray>::iterator v = collectedData.values.begin();
		if (v != collectedData.values.end())
		{
			unsigned int fn(collectedData.fields.size()), fv(v->size());
			unsigned int o(fn <= fv ? fn : fv);

			dodoStringArray::iterator i(collectedData.fields.begin()), j(v->begin());
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
	}
	else

#endif

	{
		dodoArray<dodoStringArray>::iterator v = collectedData.values.begin();
		if (v != collectedData.values.end())
		{
			if (preventFraming)
				setPart = valuesName(*v, collectedData.fields, __dodostring__);
			else
				setPart = valuesName(*v, collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
		}
	}

	request = statements[SQLCONSTRUCTOR_STATEMENT_UPDATE];
	request.append(collectedData.table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_SET]);
	request.append(setPart);
}

//-------------------------------------------------------------------

void
sqlConstructor::delCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_DELETE];
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_FROM]);
	request.append(collectedData.table);
}

//-------------------------------------------------------------------

void
sqlConstructor::subCollect()
{
	request = tools::misc::implode(collectedData.subQueries, sqlQStArr[collectedData.qType - 1]);
}

//-------------------------------------------------------------------

void
sqlConstructor::joinCollect()
{
	dodoStringArray::iterator i = collectedData.joinTables.begin(), j = collectedData.joinTables.end();
	dodoStringArray::iterator o = collectedData.joinConds.begin(), p = collectedData.joinConds.end();
	dodoArray<int>::iterator m = collectedData.joinTypes.begin(), n = collectedData.joinTypes.end();
	for (; i != j; ++i, ++o, ++m)
	{
		if (*m >= 0 && *m < JOINTYPESTSTATEMENTS)
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

	switch (collectedData.qType)
	{
		case ACCUMULATOR_REQUEST_SELECT:

			selectCollect();
			selectAction = true;

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

		case SUBREQUEST_UNION:
		case SUBREQUEST_UNION_ALL:
		case SUBREQUEST_MINUS:
		case SUBREQUEST_INTERSECT:

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

		default:

			additionalActions = false;
	}

#ifndef FAST

	if (request.size() == 0)
		throw baseEx(ERRMODULE_DBSQLCONSTRUCTOR, SQLCONSTRUCTOREX_QUERYCOLLECT, ERR_LIBDODO, SQLCONSTRUCTOREX_EMPTYREQUEST, DBSQLCONSTRUCTOREX_EMPTYREQUEST_STR, __LINE__, __FILE__);

#endif

	if (additionalActions)
	{
		if (selectAction && isSetFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_JOIN))
			joinCollect();
		additionalCollect(ACCUMULATOR_ADDREQUEST_AS, collectedData.where);
		additionalCollect(ACCUMULATOR_ADDREQUEST_WHERE, collectedData.where);
		if (selectAction)
		{
			additionalCollect(ACCUMULATOR_ADDREQUEST_GROUPBY, collectedData.group);
			additionalCollect(ACCUMULATOR_ADDREQUEST_HAVING, collectedData.having);
		}
		additionalCollect(ACCUMULATOR_ADDREQUEST_ORDERBY, collectedData.order);
		additionalCollect(ACCUMULATOR_ADDREQUEST_LIMIT, collectedData.limit);
		additionalCollect(ACCUMULATOR_ADDREQUEST_OFFSET, collectedData.offset);
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

