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

using namespace dodo::db::sql;

const dodoString constructor::sqlQStArr[] =
{
	" union ",
	" union all ",
	" minus ",
	" intersect "
};

//-------------------------------------------------------------------

const dodoString constructor::sqlJoinArr[] =
{
	" join ",
	" left outer join ",
	" right outer join ",
	" full outer join ",
	" inner join ",
	" cross join "
};

//-------------------------------------------------------------------

const dodoString constructor::sqlAddArr[] =
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

const dodoString constructor::statements[] =
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
	"NULL",
};

//-------------------------------------------------------------------

constructor::constructor()
{
}

//-------------------------------------------------------------------

constructor::~constructor()
{
}

//-------------------------------------------------------------------

void
constructor::setFieldType(const dodoString &table,
						  const dodoString &field,
						  short type)
{
	fieldTypes[collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + table][field] = type;
}

//-------------------------------------------------------------------

void
constructor::additionalCollect(unsigned int qTypeTocheck,
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

void
constructor::callFunctionCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_SELECT];
	request.append(collectedData.table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
	request.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
constructor::callProcedureCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_CALL];
	request.append(collectedData.table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
	request.append(tools::misc::implode(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
constructor::selectCollect()
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
constructor::insertCollect()
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
						if (type->second == FIELDTYPE_TEXT || type->second == FIELDTYPE_BINARY)
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
						else
							request.append(*i + statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
					}
					else
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
				}
				type = types->second.find(*t);
				if (type != typesEnd)
				{
					if (type->second == FIELDTYPE_TEXT || type->second == FIELDTYPE_BINARY)
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
					else
						request.append(*i);
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
					if (type->second == FIELDTYPE_TEXT || type->second == FIELDTYPE_BINARY)
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
					else
						request.append(*i + statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
				}
				else
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
			}
			type = types->second.find(*t);
			if (type != typesEnd)
			{
				if (type->second == FIELDTYPE_TEXT || type->second == FIELDTYPE_BINARY)
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
				else
					request.append(*i);
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

void
constructor::insertSelectCollect()
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
constructor::updateCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_UPDATE];
	request.append(collectedData.table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_SET]);

	dodoArray<dodoStringArray>::iterator v = collectedData.values.begin();
	if (v != collectedData.values.end())
	{
		dodoStringArray::const_iterator i(collectedData.fields.begin()), j(v->begin());
		if (i != j)
		{
			unsigned int fn(collectedData.fields.size()), fv(v->size());

			unsigned int o(fn <= fv ? fn : fv);

			dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + collectedData.table);
			if (types != fieldTypes.end())
			{
				dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
				dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

				--o;
				for (unsigned int k(0); k < o; ++i, ++k, ++j)
				{
					request.append(*i);

					type = types->second.find(*i);
					if (type != typesEnd)
					{
						if (type->second == FIELDTYPE_TEXT || type->second == FIELDTYPE_BINARY)
						{
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
							request.append(escapeFields(*j));
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
						}
						else
						{
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
							request.append(*j);
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
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
					if (type->second == FIELDTYPE_TEXT || type->second == FIELDTYPE_BINARY)
					{
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
						request.append(escapeFields(*j));
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
					}
					else
					{
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
						request.append(*j);
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
				--o;
				for (unsigned int k(0); k < o; ++i, ++k, ++j)
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
constructor::delCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_DELETE];
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_FROM]);
	request.append(collectedData.table);
}

//-------------------------------------------------------------------

void
constructor::subCollect()
{
	request = tools::misc::implode(collectedData.subQueries, sqlQStArr[collectedData.qType - 1]);
}

//-------------------------------------------------------------------

void
constructor::joinCollect()
{
	dodoStringArray::iterator i = collectedData.joinTables.begin(), j = collectedData.joinTables.end();
	dodoStringArray::iterator o = collectedData.joinConds.begin(), p = collectedData.joinConds.end();
	dodoArray<int>::iterator m = collectedData.joinTypes.begin(), n = collectedData.joinTypes.end();
	for (; i != j; ++i, ++o, ++m)
	{
		if (*m >= 0 && *m < JOINTYPESTSTATEMENTS)
			request.append(sqlJoinArr[*m]);
		else
			throw exception::basic(exception::ERRMODULE_DBSQLCONSTRUCTOR, SQLCONSTRUCTOREX_JOINCOLLECT, exception::ERRNO_LIBDODO, SQLCONSTRUCTOREX_UNKNOWNJOINTYPE, DBSQLCONSTRUCTOREX_UNKNOWNJOINTYPE_STR, __LINE__, __FILE__);

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
constructor::queryCollect()
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

	if (request.size() == 0)
		throw exception::basic(exception::ERRMODULE_DBSQLCONSTRUCTOR, SQLCONSTRUCTOREX_QUERYCOLLECT, exception::ERRNO_LIBDODO, SQLCONSTRUCTOREX_EMPTYREQUEST, DBSQLCONSTRUCTOREX_EMPTYREQUEST_STR, __LINE__, __FILE__);

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
constructor::escapeFields(const dodoString &data)
{
	dodoString temp;

	unsigned long size = data.size();

	for (unsigned long i = 0; i < size; ++i)
	{
		if (data[i] == '\\')
			temp.append("\\\\");
		else
		{
			if (data[i] == '\'')
				temp.append("\\'");
			else
				temp.append(1, data[i]);
		}
	}

	return temp;
}

//-------------------------------------------------------------------

