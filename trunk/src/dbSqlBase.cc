/***************************************************************************
 *            dbSqlBase.cc
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

#include <libdodo/dbSqlBase.h>

using namespace dodo;

const unsigned int dbSqlBase::addInsEnumArr[] =
{
	DBBASE_ADDREQUEST_INSERT_IGNORE,
};

//-------------------------------------------------------------------

const unsigned int dbSqlBase::addUpEnumArr[] =
{
	DBBASE_ADDREQUEST_UPDATE_IGNORE,
};

//-------------------------------------------------------------------

const unsigned int dbSqlBase::addDelEnumArr[] =
{
	DBBASE_ADDREQUEST_DELETE_IGNORE,
};

//-------------------------------------------------------------------

const unsigned int dbSqlBase::addSelEnumArr[] =
{
	DBBASE_ADDREQUEST_SELECT_DISTINCT,
	DBBASE_ADDREQUEST_SELECT_ALL
};

//-------------------------------------------------------------------

const dodoString dbSqlBase::sqlQStArr[] =
{
	""             ,
	" union "      ,
	" union all "  ,
	" minus "      ,
	" intersect "  
};

//-------------------------------------------------------------------

const dodoString dbSqlBase::sqlAddArr[] =
{
	""           ,
	" where "    ,
	" having "   ,
	" group by " ,
	" order by " ,
	" limit "    ,
	" offset "   ,
	" as "       ,
};

//-------------------------------------------------------------------

const dodoString dbSqlBase::sqlAddInsArr[] =
{
	""         ,
	" ignore " ,
};

//-------------------------------------------------------------------

const dodoString dbSqlBase::sqlAddUpArr[] =
{
	""         ,
	" ignore " ,
};

//-------------------------------------------------------------------

const dodoString dbSqlBase::sqlAddDelArr[] =
{
	""         ,
	" ignore " ,
};

//-------------------------------------------------------------------

const dodoString dbSqlBase::sqlAddSelArr[] =
{
	""           ,
	" distinct " ,
	" all "      ,
};

//-------------------------------------------------------------------

dbSqlBase::dbSqlBase() : preventFraming(false),
						 preventEscaping(false),
						 autoFraming(true)
{
}

//-------------------------------------------------------------------

dbSqlBase::~dbSqlBase()
{
}

//-------------------------------------------------------------------

dodoString
dbSqlBase::valuesName(const dodoStringArray &values,
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
			temp.append("=");
			temp.append(frame);
			temp.append(preventEscaping ? *j : escapeFields(*j));
			temp.append(frame);
			temp.append(",");
		}
		temp.append(*i);
		temp.append("=");
		temp.append(frame);
		temp.append(preventEscaping ? *j : escapeFields(*j));
		temp.append(frame);
	}

	return temp;
}

//-------------------------------------------------------------------

dodoString
dbSqlBase::exists(const dodoString &statement)
{
	return dodoString("exists(" + statement + ')');
}

//-------------------------------------------------------------------

dodoString
dbSqlBase::notexists(const dodoString &statement)
{
	return dodoString("not exists(" + statement + ')');
}

//-------------------------------------------------------------------

void
dbSqlBase::additionalCollect(unsigned int qTypeTocheck,
							 const dodoString &collectedString)
{
	if (qShift == DB_EMPTY)
		return ;

	int tempQTypeTocheck = 1 << qTypeTocheck;
	if (isSetFlag(qShift, tempQTypeTocheck))
	{
		request.append(sqlAddArr[qTypeTocheck]);
		request.append(collectedString);
	}
}

//-------------------------------------------------------------------

dodoString
dbSqlBase::insideAddCollect(const unsigned int sqlAddEnumArr[],
							const dodoString sqlAddArr[],
							int qTypeShift)
{
	if (qTypeShift == DB_EMPTY)
		return __dodostring__;

	dodoString temp;

	unsigned int arrLen = sizeof(sqlAddArr) / sizeof(char *);

	for (unsigned int i = 0; i < arrLen; ++i)
	{
		if (isSetFlag(qTypeShift, 1 << sqlAddEnumArr[i]))
			temp.append(sqlAddArr[sqlAddEnumArr[i]]);
	}

	return temp;
}

//-------------------------------------------------------------------

dodoString
dbSqlBase::insideAddCollect(const dodoStringArray &statements,
							int qTypeShift)
{
	if (qTypeShift == DB_EMPTY)
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
dbSqlBase::callFunctionCollect()
{
	request = "select ";
	request.append(pre_table);
	request.append("(");

	char frame[] = "'";
	if (preventFraming)
		frame[0] = ' ';

	if (preventEscaping)
		request.append(tools::implode(pre_fields, ",", frame));
	else
		request.append(tools::implode(pre_fields, escapeFields, ",", frame));

	request.append(")");
}

//-------------------------------------------------------------------

void
dbSqlBase::callProcedureCollect()
{
	request = "call ";
	request.append(pre_table);
	request.append("(");

	char frame[] = "'";
	if (preventFraming)
		frame[0] = ' ';

	if (preventEscaping)
		request.append(tools::implode(pre_fields, ",", frame));
	else
		request.append(tools::implode(pre_fields, escapeFields, ",", frame));

	request.append(")");
}

//-------------------------------------------------------------------

void
dbSqlBase::selectCollect()
{
	dodoString temp = insideAddCollect(addSelEnumArr, sqlAddSelArr, qSelShift);
	temp.append(insideAddCollect(sqlDbDepAddSelArr, qDbDepSelShift));

	if (pre_table.size() > 0)
	{
		temp.append(tools::implode(pre_fields, ","));

		request = "select ";
		request.append(temp);
		request.append(" from ");
		request.append(pre_table);
	}
	else
	{
		temp.append(tools::implode(pre_fields, ","));
		request = "select ";
		request.append(temp);
	}
}

//-------------------------------------------------------------------

void
dbSqlBase::insertCollect()
{
	dodoStringArray fieldsVPart;

	dodoArray<dodoStringArray>::iterator k(pre_values.begin()), l(pre_values.end());

	dodoMap<dodoString, dodoStringArray>::iterator y = framingFields.find(dbInfo.db + ":" + pre_table);

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
				if (tools::isInArray(y->second, *t, true))
				{
					if (preventEscaping)
						temp.append("'" + *i + "',");
					else
						temp.append("'" + escapeFields(*i) + "',");
				}
				else
					temp.append(*i + ",");
			}
			if (tools::isInArray(y->second, *t, true))
			{
				if (preventEscaping)
					temp.append("'" + *i + "'");
				else
					temp.append("'" + escapeFields(*i) + "'");
			}
			else
				temp.append(*i);

			fieldsVPart.push_back(temp);
		}
	}
	else
	{
		char frame[] = "'";
		if (preventFraming)
			frame[0] = ' ';

		for (; k != l; ++k)
		{
			if (preventEscaping)
				fieldsVPart.push_back(tools::implode(*k, ",", frame));
			else
				fieldsVPart.push_back(tools::implode(*k, escapeFields, ",", frame));
		}
	}

	dodoString fieldsPart;

	dodoStringArray::iterator i(fieldsVPart.begin()), j(fieldsVPart.end());
	if (i != j)
	{
		--j;
		for (; i != j; ++i)
		{
			fieldsPart.append("(");
			fieldsPart.append(*i);
			fieldsPart.append("),");
		}
		fieldsPart.append("(");
		fieldsPart.append(*i);
		fieldsPart.append(")");
	}

	dodoString temp = insideAddCollect(addInsEnumArr, sqlAddInsArr, qInsShift);
	temp.append(insideAddCollect(sqlDbDepAddInsArr, qDbDepInsShift));

	dodoString temp1 = pre_table;

	if (pre_fields.size() != 0)
	{
		temp1.append(" (");
		temp1.append(tools::implode(pre_fields, ","));
		temp1.append(") ");
	}

	request = "insert ";
	request.append(temp);
	request.append(" into ");
	request.append(temp1);
	request.append(" values ");
	request.append(fieldsPart);
}

//-------------------------------------------------------------------

void
dbSqlBase::insertSelectCollect()
{

	dodoString fieldsPartTo = tools::implode(pre_fields, ",");
	dodoString fieldsPartFrom = tools::implode(pre_values.front(), ",");

	dodoString temp = insideAddCollect(addInsEnumArr, sqlAddInsArr, qInsShift);
	temp.append(insideAddCollect(sqlDbDepAddInsArr, qDbDepInsShift));

	dodoString tempS = insideAddCollect(addSelEnumArr, sqlAddSelArr, qSelShift);
	tempS.append(insideAddCollect(sqlDbDepAddSelArr, qDbDepSelShift));

	tempS.append(fieldsPartFrom);

	request = "insert ";
	request.append(temp);
	request.append(" into ");
	request.append(pre_tableTo);
	request.append("(");
	request.append(fieldsPartTo);
	request.append(") select ");
	request.append(tempS);
	request.append(" from ");
	request.append(pre_table);
}

//-------------------------------------------------------------------

void
dbSqlBase::updateCollect()
{
	dodoString setPart;

	dodoMap<dodoString, dodoStringArray>::iterator y = framingFields.find(dbInfo.db + ":" + pre_table);

	if (autoFraming && !preventFraming && y != framingFields.end() && pre_fields.size() != 0)
	{
		unsigned int fn(pre_fields.size()), fv(pre_values.front().size());
		unsigned int o(fn <= fv ? fn : fv);

		dodoStringArray::iterator i(pre_fields.begin()), j(pre_values.front().begin());
		for (unsigned int k(0); k < o - 1; ++i, ++j, ++k)
		{
			if (tools::isInArray(y->second, *i, true))
			{
				setPart.append(*i);
				setPart.append("='");
				setPart.append(preventEscaping ? *j : escapeFields(*j));
				setPart.append("',");
			}
			else
			{
				setPart.append(*i);
				setPart.append("=");
				setPart.append(preventEscaping ? *j : escapeFields(*j));
				setPart.append(",");
			}
		}
		if (tools::isInArray(y->second, *i, true))
		{
			setPart.append(*i);
			setPart.append("='");
			setPart.append(preventEscaping ? *j : escapeFields(*j));
			setPart.append("'");
		}
		else
		{
			setPart.append(*i);
			setPart.append("=");
			setPart.append(preventEscaping ? *j : escapeFields(*j));
		}
	}
	else
	{
		char frame[] = "'";
		if (preventFraming)
			frame[0] = ' ';

		setPart = valuesName(pre_values.front(), pre_fields, frame);
	}

	insideAddCollect(addUpEnumArr, sqlAddUpArr, qUpShift);
	dodoString temp = insideAddCollect(sqlDbDepAddUpArr, qDbDepUpShift);

	temp.append(pre_table);

	request = "update ";
	request.append(temp);
	request.append(" set ");
	request.append(setPart);
}

//-------------------------------------------------------------------

void
dbSqlBase::delCollect()
{
	dodoString temp = insideAddCollect(addDelEnumArr, sqlAddDelArr, qDelShift);
	temp.append(insideAddCollect(sqlDbDepAddDelArr, qDbDepDelShift));

	request = "delete ";
	request.append(temp);
	request.append("from ");
	request.append(pre_table);
}

//-------------------------------------------------------------------

void
dbSqlBase::subCollect()
{
	request = tools::implode(pre_subQueries, sqlQStArr[qType]);
}

//-------------------------------------------------------------------

void
dbSqlBase::truncateCollect()
{
	request = "truncate " + pre_table;
}

//-------------------------------------------------------------------

void
dbSqlBase::delDbCollect()
{
	request = "drop database " + pre_order;
}

//-------------------------------------------------------------------

void
dbSqlBase::delTableCollect()
{
	request = "drop table " + pre_table;
}

//-------------------------------------------------------------------

void
dbSqlBase::delFieldCollect()
{
	request = "alter " + pre_order + " drop " + pre_table;
}

//-------------------------------------------------------------------

void
dbSqlBase::renameDbCollect()
{
	request = __dodostring__;
}

//-------------------------------------------------------------------

void
dbSqlBase::renameTableCollect()
{
	request = "alter table " + pre_table + " rename to " + pre_having;
}

//-------------------------------------------------------------------

void
dbSqlBase::renameFieldCollect()
{
	request = __dodostring__;
}

//-------------------------------------------------------------------

void
dbSqlBase::createDbCollect()
{
	request = "create database " + pre_order;
	if (pre_having.size() != 0)
		request.append(" character set " + pre_having);
}

//-------------------------------------------------------------------

void
dbSqlBase::createIndexCollect()
{
	request = "create index " + pre_having + " on " + pre_table + " (" + tools::implode(pre_fields, ",") + ")";
}

//-------------------------------------------------------------------

void
dbSqlBase::deleteIndexCollect()
{
	request = "drop index " + pre_having + " on " + pre_table;
}

//-------------------------------------------------------------------

void
dbSqlBase::createTableCollect()
{
	request = "create table ";

	request.append(pre_tableInfo.ifNotExists ? "if not exists " : __dodostring__);

	request.append(pre_tableInfo.name + "(");

	{
		dodoArray<__fieldInfo>::iterator i(pre_tableInfo.fields.begin()), j(pre_tableInfo.fields.end());
		if (i != j)
		{
			--j;
			for (; i != j; ++i)
				request.append(fieldCollect(*i) + ",");
			request.append(fieldCollect(*i));
		}
	}

	request.append(!pre_tableInfo.primKeys.empty() ? ", primary key (" + tools::implode(pre_tableInfo.primKeys, ",") + ") " : __dodostring__);
	request.append(!pre_tableInfo.uniq.empty() ? ", unique " + tools::implode(pre_tableInfo.uniq, ",") : __dodostring__);

	request.append(")");
}

//-------------------------------------------------------------------

void
dbSqlBase::createFieldCollect()
{
	request = "alter table " + pre_table + " add " + fieldCollect(pre_fieldInfo);
}

//-------------------------------------------------------------------

void 
dbSqlBase::joinCollect()
{
	dodoStringArray::iterator i = pre_joinTables.begin(), j = pre_joinTables.end();
	dodoStringArray::iterator o = pre_joinConds.begin(), p = pre_joinConds.end();
	dodoArray<int>::iterator m = pre_joinTypes.begin(), n = pre_joinTypes.end();
	for (;i!=j;++i, ++o, ++m)
	{
		switch (*m)
		{
			case DB_JOINTYPE_JOIN:
				
				request.append(" join ");
				
				break;
			
			case DB_JOINTYPE_LEFTOUTER:
				
				request.append(" left outer join ");
				
				break;
				
			case DB_JOINTYPE_RIGHTOUTER:

				request.append(" right outer join ");
				
				break;
				
			case DB_JOINTYPE_FULLOUTER:

				request.append(" full outer join ");
				
				break;
				
			case DB_JOINTYPE_INNER:

				request.append(" inner join ");
				
				break;
				
			case DB_JOINTYPE_CROSS:

				request.append(" cross join ");
				
				break;
				
			default:
				
				throw baseEx(ERRMODULE_DBSQLBASE, DBSQLBASEEX_JOINCOLLECT, ERR_LIBDODO, DBSQLBASEEX_UNKNOWNJOINTYPE, DBSQLBASEEX_UNKNOWNJOINTYPE_STR, __LINE__, __FILE__);
		}
		
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
dbSqlBase::queryCollect()
{
	bool additionalActions = true;
	bool selectAction = false;

	switch (qType)
	{
		case DBBASE_REQUEST_SELECT:

			selectCollect();
			selectAction = true;
			
			joinCollect();

			break;

		case DBBASE_REQUEST_INSERT:

			insertCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_UPDATE:

			updateCollect();

			break;

		case DBBASE_REQUEST_DELETE:

			delCollect();

			break;

		case DBBASE_REQUEST_INSERT_SELECT:

			insertSelectCollect();
			selectAction = true;

			break;

		case DBBASE_REQUEST_UNION:
		case DBBASE_REQUEST_UNION_ALL:
		case DBBASE_REQUEST_MINUS:
		case DBBASE_REQUEST_INTERSECT:

			subCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_CALL_FUNCTION:

			callFunctionCollect();
			selectAction = true;

			break;

		case DBBASE_REQUEST_CALL_PROCEDURE:

			callProcedureCollect();

			break;

		case DBBASE_REQUEST_TRUNCATE:

			truncateCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_RENAME_DB:

			renameDbCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_CREATE_INDEX:

			createIndexCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_DELETE_INDEX:

			deleteIndexCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_RENAME_TABLE:

			renameTableCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_RENAME_FIELD:

			renameFieldCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_DELETE_DB:

			delDbCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_DELETE_TABLE:

			delTableCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_DELETE_FIELD:

			delFieldCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_CREATE_DB:

			createDbCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_CREATE_TABLE:

			createTableCollect();
			additionalActions = false;

			break;

		case DBBASE_REQUEST_CREATE_FIELD:

			createFieldCollect();
			additionalActions = false;

			break;

		default:

			additionalActions = false;
	}

#ifndef FAST

	if (request.size() == 0)
		throw baseEx(ERRMODULE_DBSQLBASE, DBSQLBASEEX_QUERYCOLLECT, ERR_LIBDODO, DBSQLBASEEX_EMPTYREQUEST, DBSQLBASEEX_EMPTYREQUEST_STR, __LINE__, __FILE__);

#endif

	if (additionalActions)
	{
		additionalCollect(DBBASE_ADDREQUEST_AS, pre_where);
		additionalCollect(DBBASE_ADDREQUEST_WHERE, pre_where);
		if (selectAction)
		{
			additionalCollect(DBBASE_ADDREQUEST_GROUPBY, pre_group);
			additionalCollect(DBBASE_ADDREQUEST_HAVING, pre_having);
		}
		additionalCollect(DBBASE_ADDREQUEST_ORDERBY, pre_order);
		additionalCollect(DBBASE_ADDREQUEST_LIMIT, pre_limit);
		additionalCollect(DBBASE_ADDREQUEST_OFFSET, pre_offset);
	}

	return request;
}

//-------------------------------------------------------------------

dodoString
dbSqlBase::unescapeFields(const dodoString &data)
{
	dodoString temp = data;

	toolsString::replace("\\'", "'", temp);
	toolsString::replace("\\\\", "\\", temp);

	return temp;
}

//-------------------------------------------------------------------

dodoString
dbSqlBase::escapeFields(const dodoString &data)
{
	dodoString temp = data;

	toolsString::replace("\\", "\\\\", temp);
	toolsString::replace("'", "\\'", temp);

	return temp;
}

//-------------------------------------------------------------------

dodoString
dbSqlBase::fieldCollect(const __fieldInfo &row)
{
	int type = row.type, flag = row.flag;
	dodoString resRow(row.name + " " + sqlDataType(type));

	if (preventEscaping)
		resRow.append(!row.set_enum.empty() ? " (" + tools::implode(row.set_enum, ",") + ")" : __dodostring__);
	else
		resRow.append(!row.set_enum.empty() ? " (" + tools::implode(row.set_enum, escapeFields, ",") + ")" : __dodostring__);
	resRow.append((chkRange(type) > 0 && row.length > 0) ? " (" + toolsString::lToString(row.length) + ") " : __dodostring__);
	resRow.append(row.charset.size() > 0 ? " collate " + row.charset : " ");
	resRow.append(isSetFlag(flag, DBBASE_FIELDFLAG_NULL) ? " null " : " not null ");
	resRow.append(row.defaultVal.size() > 0 ? "default '" + row.defaultVal + "' " : __dodostring__);
	resRow.append(isSetFlag(flag, DBBASE_FIELDFLAG_AUTO_INCREMENT) ? " primary key auto_increment" : __dodostring__);

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

dodoString
dbSqlBase::sqlDataType(int type)
{
	switch (type)
	{
		case DBBASE_FIELDTYPE_INT:
		case DBBASE_FIELDTYPE_INTEGER:

			return dodoString("INTEGER");

		case DBBASE_FIELDTYPE_DATE:

			return dodoString("DATE");

		case DBBASE_FIELDTYPE_VARCHAR:

			return dodoString("VARCHAR");

		case DBBASE_FIELDTYPE_TIMESTAMP:

			return dodoString("TIMESTAMP");

		case DBBASE_FIELDTYPE_TIME:

			return dodoString("TIME");

		case DBBASE_FIELDTYPE_TINYINT:

			return dodoString("TINYINT");

		case DBBASE_FIELDTYPE_SMALLINT:

			return dodoString("SMALLINT");

		case DBBASE_FIELDTYPE_MEDIUMINT:

			return dodoString("MEDIUMINT");

		case DBBASE_FIELDTYPE_BIGINT:

			return dodoString("BIGINT");

		case DBBASE_FIELDTYPE_FLOAT:

			return dodoString("FLOAT");

		case DBBASE_FIELDTYPE_REAL:
		case DBBASE_FIELDTYPE_DOUBLE:

			return dodoString("REAL");

		case DBBASE_FIELDTYPE_DECIMAL:

			return dodoString("DECIMAL");

		case DBBASE_FIELDTYPE_CHAR:

			return dodoString("CHAR");

		case DBBASE_FIELDTYPE_TINYBLOB:

			return dodoString("TINYBLOB");

		case DBBASE_FIELDTYPE_BLOB:

			return dodoString("BLOB");

		case DBBASE_FIELDTYPE_MEDIUMBLOB:

			return dodoString("MEDIUMBLOB");

		case DBBASE_FIELDTYPE_LONGBLOB:

			return dodoString("LONGBLOB");

		case DBBASE_FIELDTYPE_TINYTEXT:

			return dodoString("TINYTEXT");

		case DBBASE_FIELDTYPE_TEXT:

			return dodoString("TEXT");

		case DBBASE_FIELDTYPE_MEDIUMTEXT:

			return dodoString("MEDIUMTEXT");

		case DBBASE_FIELDTYPE_LONGTEXT:

			return dodoString("LONGTEXT");

		case DBBASE_FIELDTYPE_ENUM:

			return dodoString("ENUM");

		case DBBASE_FIELDTYPE_SET:

			return dodoString("SET");

		default:

			return __dodostring__;
	}
}

//-------------------------------------------------------------------

int
dbSqlBase::chkRange(int type)
{
	switch (type)
	{
		case DBBASE_FIELDTYPE_DATE:
		case DBBASE_FIELDTYPE_TIME:
		case DBBASE_FIELDTYPE_TINYBLOB:
		case DBBASE_FIELDTYPE_BLOB:
		case DBBASE_FIELDTYPE_MEDIUMBLOB:
		case DBBASE_FIELDTYPE_LONGBLOB:
		case DBBASE_FIELDTYPE_TINYTEXT:
		case DBBASE_FIELDTYPE_TEXT:
		case DBBASE_FIELDTYPE_MEDIUMTEXT:
		case DBBASE_FIELDTYPE_LONGTEXT:
		case DBBASE_FIELDTYPE_ENUM:
		case DBBASE_FIELDTYPE_SET:

			return -1;

		case DBBASE_FIELDTYPE_INTEGER:
		case DBBASE_FIELDTYPE_INT:
		case DBBASE_FIELDTYPE_TINYINT:
		case DBBASE_FIELDTYPE_SMALLINT:
		case DBBASE_FIELDTYPE_MEDIUMINT:
		case DBBASE_FIELDTYPE_BIGINT:
		case DBBASE_FIELDTYPE_FLOAT:
		case DBBASE_FIELDTYPE_REAL:
		case DBBASE_FIELDTYPE_DOUBLE:
		case DBBASE_FIELDTYPE_TIMESTAMP:

			return 0;

		case DBBASE_FIELDTYPE_VARCHAR:
		case DBBASE_FIELDTYPE_CHAR:
		case DBBASE_FIELDTYPE_DECIMAL:

			return 1;

		default:

			return -1;
	}
}

//-------------------------------------------------------------------

dodoString
dbSqlBase::stringReference(int type)
{
	switch (type)
	{
		case DBBASE_REFERENCE_RESTRICT:

			return dodoString("restrict");

		case DBBASE_REFERENCE_CASCADE:

			return dodoString("cascade");

		case DBBASE_REFERENCE_SET_NULL:

			return dodoString("set null");

		case DBBASE_REFERENCE_NO_ACTION:

			return dodoString("no action");

		case DBBASE_REFERENCE_SET_DEFAULT:

			return dodoString("set default");

		default:

			return __dodostring__;
	}
}

//-------------------------------------------------------------------

