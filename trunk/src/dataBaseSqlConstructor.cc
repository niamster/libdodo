/***************************************************************************
 *            dataBaseSqlConstructor.cc
 *
 *  Mon Jul 18 2005
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

#include <libdodo/directives.h>

#include <libdodo/dataBaseSqlConstructor.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/dataBaseSqlConstructorEx.h>
#include <libdodo/dataBaseAccumulator.h>

using namespace dodo::data::base::sql;

const dodoString constructor::subrequestStatements[] = {
	" union ",
	" union all ",
	" minus ",
	" intersect "
};

//-------------------------------------------------------------------

const dodoString constructor::joinStatements[] = {
	" join ",
	" left outer join ",
	" right outer join ",
	" full outer join ",
	" inner join ",
	" cross join "
};

//-------------------------------------------------------------------

const dodoString constructor::additionalRequestStatements[] = {
	" where ",
	" having ",
	" group by ",
	" order by ",
	" limit ",
	" offset ",
	" as ",
};

//-------------------------------------------------------------------

const dodoString constructor::statements[] = {
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
						  short            type)
{
	fieldTypes[collectedData.dbInfo.db + statements[STATEMENT_COLON] + table][field] = type;
}

//-------------------------------------------------------------------

void
constructor::additionalCollect(unsigned int     typeTocheck,
							   const dodoString &collectedString)
{
	if (collectedData.additional == STATE_NONE)
		return ;

	if (isSetFlag(collectedData.additional, 1 << typeTocheck)) {
		request.append(additionalRequestStatements[typeTocheck - 1]);
		request.append(collectedString);
	}
}

//-------------------------------------------------------------------

void
constructor::functionCollect()
{
	request = statements[STATEMENT_SELECT];
	request.append(collectedData.table);
	request.append(statements[STATEMENT_LEFTBRACKET]);
	request.append(tools::misc::join(collectedData.fields, statements[STATEMENT_COMA]));
	request.append(statements[STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
constructor::procedureCollect()
{
	request = statements[STATEMENT_CALL];
	request.append(collectedData.table);
	request.append(statements[STATEMENT_LEFTBRACKET]);
	request.append(tools::misc::join(collectedData.fields, statements[STATEMENT_COMA]));
	request.append(statements[STATEMENT_RIGHTBRACKET]);
}

//-------------------------------------------------------------------

void
constructor::selectCollect()
{
	if (collectedData.table.size() > 0) {
		request = statements[STATEMENT_SELECT];
		request.append(tools::misc::join(collectedData.fields, statements[STATEMENT_COMA]));
		request.append(statements[STATEMENT_FROM]);
		request.append(collectedData.table);
	} else {
		request = statements[STATEMENT_SELECT];
		request.append(tools::misc::join(collectedData.fields, statements[STATEMENT_COMA]));
	}
}

//-------------------------------------------------------------------

void
constructor::insertCollect()
{
	request = statements[STATEMENT_INSERT];
	request.append(statements[STATEMENT_INTO]);
	request.append(collectedData.table);
	if (collectedData.fields.size() != 0) {
		request.append(statements[STATEMENT_LEFTBRACKET]);
		request.append(tools::misc::join(collectedData.fields, statements[STATEMENT_COMA]));
		request.append(statements[STATEMENT_RIGHTBRACKET]);
	}
	request.append(statements[STATEMENT_VALUES]);

	dodoArray<dodoStringArray>::iterator k(collectedData.values.begin()), l(collectedData.values.end());
	if (k != l) {
		dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.dbInfo.db + statements[STATEMENT_COLON] + collectedData.table);
		if (types != fieldTypes.end()) {
			dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
			dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

			dodoStringArray::iterator t;

			--l;
			for (; k != l; ++k) {
				request.append(statements[STATEMENT_LEFTBRACKET]);

				t = collectedData.fields.begin();

				dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
				for (; i != j; ++i, ++t) {
					type = types->second.find(*t);
					if (type != typesEnd) {
						if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
							request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
						else
							request.append(*i + statements[STATEMENT_COMA]);
					} else
						request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
				}
				type = types->second.find(*t);
				if (type != typesEnd) {
					if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
						request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
					else
						request.append(*i);
				} else
					request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

				request.append(statements[STATEMENT_RIGHTBRACKETCOMA]);
			}
			request.append(statements[STATEMENT_LEFTBRACKET]);

			t = collectedData.fields.begin();

			dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
			for (; i != j; ++i, ++t) {
				type = types->second.find(*t);
				if (type != typesEnd) {
					if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
						request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
					else
						request.append(*i + statements[STATEMENT_COMA]);
				} else
					request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHECOMA]);
			}
			type = types->second.find(*t);
			if (type != typesEnd) {
				if (type->second == FIELD_TEXT || type->second == FIELD_BINARY)
					request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);
				else
					request.append(*i);
			} else
				request.append(statements[STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[STATEMENT_APOSTROPHE]);

			request.append(statements[STATEMENT_RIGHTBRACKET]);
		} else {
			--l;
			for (; k != l; ++k) {
				request.append(statements[STATEMENT_LEFTBRACKET]);
				request.append(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
				request.append(statements[STATEMENT_RIGHTBRACKETCOMA]);
			}
			request.append(statements[STATEMENT_LEFTBRACKET]);
			request.append(joinFields(*k, statements[STATEMENT_COMA], statements[STATEMENT_APOSTROPHE]));
			request.append(statements[STATEMENT_RIGHTBRACKET]);
		}
	}
}

//-------------------------------------------------------------------

void
constructor::updateCollect()
{
	request = statements[STATEMENT_UPDATE];
	request.append(collectedData.table);
	request.append(statements[STATEMENT_SET]);

	dodoArray<dodoStringArray>::iterator v = collectedData.values.begin();
	if (v != collectedData.values.end()) {
		dodoStringArray::const_iterator i(collectedData.fields.begin()), j(v->begin());
		if (i != j) {
			unsigned int fn(collectedData.fields.size()), fv(v->size());

			unsigned int o(fn <= fv ? fn : fv);

			dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.dbInfo.db + statements[STATEMENT_COLON] + collectedData.table);
			if (types != fieldTypes.end()) {
				dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
				dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

				--o;
				for (unsigned int k(0); k < o; ++i, ++k, ++j) {
					request.append(*i);

					type = types->second.find(*i);
					if (type != typesEnd) {
						if (type->second == FIELD_TEXT || type->second == FIELD_BINARY) {
							request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
							request.append(escapeFields(*j));
							request.append(statements[STATEMENT_APOSTROPHECOMA]);
						} else {
							request.append(statements[STATEMENT_EQUAL]);
							request.append(*j);
							request.append(statements[STATEMENT_COMA]);
						}
					} else {
						request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
						request.append(escapeFields(*j));
						request.append(statements[STATEMENT_APOSTROPHECOMA]);
					}
				}
				request.append(*i);

				type = types->second.find(*i);
				if (type != typesEnd) {
					if (type->second == FIELD_TEXT || type->second == FIELD_BINARY) {
						request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
						request.append(escapeFields(*j));
						request.append(statements[STATEMENT_APOSTROPHE]);
					} else {
						request.append(statements[STATEMENT_EQUAL]);
						request.append(*j);
					}
				} else {
					request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
					request.append(escapeFields(*j));
					request.append(statements[STATEMENT_APOSTROPHE]);
				}
			} else {
				--o;
				for (unsigned int k(0); k < o; ++i, ++k, ++j) {
					request.append(*i);
					request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
					request.append(escapeFields(*j));
					request.append(statements[STATEMENT_APOSTROPHECOMA]);
				}
				request.append(*i);
				request.append(statements[STATEMENT_EQUALAPOSTROPHE]);
				request.append(escapeFields(*j));
				request.append(statements[STATEMENT_APOSTROPHE]);
			}
		}
	}
}

//-------------------------------------------------------------------

void
constructor::delCollect()
{
	request = statements[STATEMENT_DELETE];
	request.append(statements[STATEMENT_FROM]);
	request.append(collectedData.table);
}

//-------------------------------------------------------------------

void
constructor::subCollect()
{
	request = tools::misc::join(collectedData.subQueries, subrequestStatements[collectedData.type - 1]);
}

//-------------------------------------------------------------------

void
constructor::joinCollect()
{
	dodoStringArray::iterator i = collectedData.joinTables.begin(), j = collectedData.joinTables.end();
	dodoStringArray::iterator o = collectedData.joinConds.begin(), p = collectedData.joinConds.end();
	dodoArray<int>::iterator m = collectedData.joinTypes.begin(), n = collectedData.joinTypes.end();
	for (; i != j; ++i, ++o, ++m) {
		if (*m >= 0 && *m < JOIN_ENUMSIZE)
			request.append(joinStatements[*m]);
		else
			throw exception::basic(exception::MODULE_DATABASESQLCONSTRUCTOR, SQLCONSTRUCTOREX_JOINCOLLECT, exception::ERRNO_LIBDODO, SQLCONSTRUCTOREX_UNKNOWNJOINTYPE, DATABASESQLCONSTRUCTOREX_UNKNOWNJOIN_STR, __LINE__, __FILE__);

		request.append(*i);

		if (o->size() > 0) {
			request.append(" on ");
			request.append(*o);
		}
	}
}

//-------------------------------------------------------------------

dodoString
constructor::construct()
{
	bool additionalActions = true;
	bool selectAction = false;

	switch (collectedData.type) {
		case REQUEST_SELECT:

			selectCollect();
			selectAction = true;

			break;

		case REQUEST_INSERT:

			insertCollect();
			additionalActions = false;

			break;

		case REQUEST_UPDATE:

			updateCollect();

			break;

		case REQUEST_DELETE:

			delCollect();

			break;

		case SUBREQUEST_UNION:
		case SUBREQUEST_UNION_ALL:
		case SUBREQUEST_MINUS:
		case SUBREQUEST_INTERSECT:

			subCollect();
			additionalActions = false;

			break;

		case REQUEST_FUNCTION:

			functionCollect();
			selectAction = true;

			break;

		case REQUEST_PROCEDURE:

			procedureCollect();

			break;

		default:

			additionalActions = false;
	}

	if (additionalActions) {
		if (selectAction && isSetFlag(collectedData.additional, 1 << ADDITIONAL_REQUEST_JOIN))
			joinCollect();
		additionalCollect(ADDITIONAL_REQUEST_AS, collectedData.where);
		additionalCollect(ADDITIONAL_REQUEST_WHERE, collectedData.where);
		if (selectAction) {
			additionalCollect(ADDITIONAL_REQUEST_GROUPBY, collectedData.group);
			additionalCollect(ADDITIONAL_REQUEST_HAVING, collectedData.having);
		}
		additionalCollect(ADDITIONAL_REQUEST_ORDERBY, collectedData.order);
		additionalCollect(ADDITIONAL_REQUEST_LIMIT, collectedData.limit);
		additionalCollect(ADDITIONAL_REQUEST_OFFSET, collectedData.offset);
	}

	return request;
}

//-------------------------------------------------------------------

dodoString
constructor::escapeFields(const dodoString &data)
{
	dodoString temp;

	unsigned long size = data.size();

	for (unsigned long i = 0; i < size; ++i) {
		if (data[i] == '\\')
			temp.append("\\\\");
		else {
			if (data[i] == '\'')
				temp.append("\\'");
			else
				temp.append(1, data[i]);
		}
	}

	return temp;
}

//-------------------------------------------------------------------

dodoString
constructor::joinFields(const dodoStringArray &fields,
						const dodoString      &separator,
						const dodoString      &frame,
						int                   limit)
{
	int k(0);

	dodoString temp, fs(frame + separator);
	dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
	if (i != j) {
		--j;
		for (; i != j; ++i) {
			if (limit != -1) {
				if (k > limit)
					return temp;
				++k;
			}
			temp.append(frame);
			temp.append(escapeFields(*i));
			temp.append(fs);
		}
		temp.append(frame);
		temp.append(escapeFields(*i));
		temp.append(frame);
	}

	return temp;
}

//-------------------------------------------------------------------
