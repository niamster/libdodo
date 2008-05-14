/***************************************************************************
 *            dbAccumulator.cc
 *
 *  Thu Aug 25 22:03:07 2005
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

#include <libdodo/dbAccumulator.h>

using namespace dodo::db;

__connectorTable::__connectorTable()
{
	ifNotExists = false;
}

//-------------------------------------------------------------------

const __connectorTable &
__connectorTable::operator=(const __connectorTable &from)
{
	name = from.name;

	fields.clear();

	dodoArray<__connectorField>::const_iterator j = from.fields.end();
	for (dodoArray<__connectorField>::const_iterator i = from.fields.begin(); i != j; ++i)
		fields.push_back(*i);

	primKeys = from.primKeys;
	uniq = from.uniq;
	ifNotExists = from.ifNotExists;

	return from;
}

//-------------------------------------------------------------------

__connectorField::__connectorField()
{
	type = -1;
	length = -1;
	flag = 0;
	onDelete = -1;
	onUpdate = -1;
}

//-------------------------------------------------------------------

const __connectorField &
__connectorField::operator=(const __connectorField &from)
{
	name = from.name;
	type = from.type;
	length = from.length;
	flag = from.flag;

	refTable = from.refTable;
	onDelete = from.onDelete;
	onUpdate = from.onUpdate;
	refFields = from.refFields;

	defaultVal = from.defaultVal;
	set_enum = from.set_enum;

	charset = from.charset;

	return from;
}

//-------------------------------------------------------------------

__xexexDbAccumulatorCollectedData::__xexexDbAccumulatorCollectedData(
	dodoString &a_pre_where,
	dodoStringArray &a_pre_fields,
	dodoArray<dodoStringArray> &a_pre_values,
	dodoString &a_pre_table,
	dodoString &a_pre_tableTo,
	dodoString &a_pre_order,
	dodoString &a_pre_having,
	dodoString &a_pre_group,
	dodoString &a_pre_limit,
	dodoString &a_pre_offset,
	dodoStringArray &a_pre_subQueries,
	dodoStringArray &a_pre_joinTables,
	dodoStringArray &a_pre_joinConds,
	dodoArray<int> &a_pre_joinTypes,
	int &a_qType,
	int &a_qShift,
	int &a_qSelShift,
	int &a_qInsShift,
	int &a_qUpShift,
	int &a_qDelShift,
	int &a_operType,
	void *a_executor) : pre_where(a_pre_where),
						pre_fields(a_pre_fields),
						pre_values(a_pre_values),
						pre_table(a_pre_table),
						pre_tableTo(a_pre_tableTo),
						pre_order(a_pre_order),
						pre_having(a_pre_having),
						pre_group(a_pre_group),
						pre_limit(a_pre_limit),
						pre_offset(a_pre_offset),
						pre_subQueries(a_pre_subQueries),
						pre_joinTables(a_pre_joinTables),
						pre_joinConds(a_pre_joinConds),
						pre_joinTypes(a_pre_joinTypes),
						qType(a_qType),
						qShift(a_qShift),
						qSelShift(a_qSelShift),
						qInsShift(a_qInsShift),
						qUpShift(a_qUpShift),
						qDelShift(a_qDelShift),
						operType(a_operType),
						executor(a_executor)
{
}

//-------------------------------------------------------------------

accumulator::accumulator() : show(false),
				   qDbDepSelShift(ACCUMULATOR_NONE),
				   qDbDepInsShift(ACCUMULATOR_NONE),
				   qDbDepUpShift(ACCUMULATOR_NONE),
				   qDbDepDelShift(ACCUMULATOR_NONE),
				   qType(-1),
				   qShift(ACCUMULATOR_NONE),
				   qSelShift(ACCUMULATOR_NONE),
				   qInsShift(ACCUMULATOR_NONE),
				   qUpShift(ACCUMULATOR_NONE),
				   qDelShift(ACCUMULATOR_NONE),
				   collectedData(pre_where,
								 pre_fields,
								 pre_values,
								 pre_table,
								 pre_tableTo,
								 pre_order,
								 pre_having,
								 pre_group,
								 pre_limit,
								 pre_offset,
								 pre_subQueries,
								 pre_joinTables,
								 pre_joinConds,
								 pre_joinTypes,
								 qType,
								 qShift,
								 qSelShift,
								 qInsShift,
								 qUpShift,
								 qDelShift,
								 operType,
								 (void *) this)
{
}

//-------------------------------------------------------------------

accumulator::~accumulator()
{
}

//-------------------------------------------------------------------

void
accumulator::callFunction(const dodoString &name,
					 const dodoStringArray &arguments,
					 const dodoString &as)
{
	qType = ACCUMULATOR_REQUEST_CALL_FUNCTION;

	pre_table = name;
	pre_fields = arguments;

	if (as.size() != 0)
	{
		addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_AS);
		pre_where = as;
	}

	show = true;
}

//-------------------------------------------------------------------

void
accumulator::callProcedure(const dodoString &name,
					 const dodoStringArray &arguments)
{
	qType = ACCUMULATOR_REQUEST_CALL_PROCEDURE;

	pre_table = name;
	pre_fields = arguments;

	show = true;
}

//-------------------------------------------------------------------

void
accumulator::select(const dodoString &a_table,
			   const dodoStringArray &a_fields,
			   const dodoString &a_where)
{
	qType = ACCUMULATOR_REQUEST_SELECT;

	pre_table = a_table;
	pre_fields = a_fields;

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = true;
}

//-------------------------------------------------------------------

void
accumulator::selectAll(const dodoString &a_table,
			   const dodoString &a_where)
{
	qType = ACCUMULATOR_REQUEST_SELECT;

	pre_table = a_table;
	pre_fields.push_back("*");

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = true;
}

//-------------------------------------------------------------------

void
accumulator::insert(const dodoString &a_table,
			   const dodoStringMap &a_fields)
{
	qType = ACCUMULATOR_REQUEST_INSERT;

	pre_table = a_table;

	pre_fields.clear();
	pre_values.clear();

	dodoStringArray temp;

	dodoStringMap::const_iterator i = (a_fields.begin()), j(a_fields.end());
	for (; i != j; ++i)
	{
		pre_fields.push_back(i->first);
		temp.push_back(i->second);
	}
	pre_values.push_back(temp);

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::insert(const dodoString &a_table,
			   const dodoArray<dodoStringMap> &a_fields)
{
	qType = ACCUMULATOR_REQUEST_INSERT;

	pre_table = a_table;

	dodoArray<dodoStringMap>::const_iterator v(a_fields.begin()), b(a_fields.end());

	pre_fields.clear();
	pre_values.clear();

	dodoStringMap::const_iterator i(v->begin()), j(v->end());
	for (; i != j; ++i)
		pre_fields.push_back(i->first);

	dodoStringArray temp;

	for (; v != b; ++v)
	{
		j = v->end();
		temp.clear();

		for (i = v->begin(); i != j; ++i)
			temp.push_back(i->second);

		pre_values.push_back(temp);
	}

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::insert(const dodoString &a_table,
			   const dodoStringArray &a_values,
			   const dodoStringArray &a_fields)
{
	qType = ACCUMULATOR_REQUEST_INSERT;

	pre_table = a_table;
	pre_fields = a_fields;

	pre_values.clear();

	pre_values.push_back(a_values);

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::insert(const dodoString &a_table,
			   const dodoArray<dodoStringArray> &a_values,
			   const dodoStringArray &a_fields)
{
	qType = ACCUMULATOR_REQUEST_INSERT;

	pre_table = a_table;
	pre_fields = a_fields;

	dodoArray<dodoStringArray>::const_iterator k(a_values.begin()), l(a_values.end());
	for (; k != l; ++k)
		pre_values.push_back(*k);

	show = false;

}

//-------------------------------------------------------------------

void
accumulator::insertSelect(const dodoString &a_tableTo,
					 const dodoString &a_tableFrom,
					 const dodoStringArray &a_fieldsTo,
					 const dodoStringArray &a_fieldsFrom,
					 const dodoString &a_where)
{
	qType = ACCUMULATOR_REQUEST_INSERT_SELECT;

	pre_tableTo = a_tableTo;
	pre_table = a_tableFrom;
	pre_fields = a_fieldsTo;

	pre_values.clear();

	pre_values.push_back(a_fieldsFrom);

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::update(const dodoString &a_table,
			   const dodoStringMap &a_fields,
			   const dodoString &a_where)
{
	qType = ACCUMULATOR_REQUEST_UPDATE;

	pre_table = a_table;

	dodoStringMap::const_iterator i(a_fields.begin()), j(a_fields.end());

	pre_fields.clear();
	pre_values.clear();

	dodoStringArray temp;

	for (; i != j; ++i)
	{
		pre_fields.push_back((*i).first);
		temp.push_back((*i).second);
	}
	pre_values.push_back(temp);

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::update(const dodoString &a_table,
			   const dodoStringArray &a_values,
			   const dodoStringArray &a_fields,
			   const dodoString &a_where)
{
	qType = ACCUMULATOR_REQUEST_UPDATE;

	pre_table = a_table;
	pre_fields = a_fields;

	pre_values.clear();

	pre_values.push_back(a_values);

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::del(const dodoString &a_table,
			const dodoString &a_where)
{
	qType = ACCUMULATOR_REQUEST_DELETE;

	pre_table = a_table;

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::subquery(const dodoStringArray &sub,
				 int type)
{
	qType = type;
	pre_subQueries = sub;
}

//-------------------------------------------------------------------

void
accumulator::truncate(const dodoString &table)
{
	qType = ACCUMULATOR_REQUEST_TRUNCATE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::renameDb(const dodoString &db,
				 const dodoString &to_db)
{
	qType = ACCUMULATOR_REQUEST_RENAME_DB;
	pre_order = db;
	pre_having = to_db;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::renameTable(const dodoString &table,
					const dodoString &to_table)
{
	qType = ACCUMULATOR_REQUEST_RENAME_TABLE;
	pre_table = table;
	pre_having = to_table;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::createIndex(const dodoString &table,
					const dodoString &field,
					const dodoString &name)
{
	qType = ACCUMULATOR_REQUEST_CREATE_INDEX;
	pre_table = table;
	pre_fields.push_back(field);
	pre_having = name;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::createIndex(const dodoString &table,
					const dodoStringArray &fields,
					const dodoString &name)
{
	qType = ACCUMULATOR_REQUEST_CREATE_INDEX;
	pre_table = table;
	pre_fields = fields;
	pre_having = name;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::deleteIndex(const dodoString &table,
					const dodoString &field)
{
	qType = ACCUMULATOR_REQUEST_DELETE_INDEX;
	pre_table = table;
	pre_having = field;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::renameField(const dodoString &field,
					const dodoString &to_field,
					const dodoString &table)
{
	qType = ACCUMULATOR_REQUEST_RENAME_FIELD;
	pre_tableTo = field;
	pre_having = to_field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::deleteDb(const dodoString &db)
{
	qType = ACCUMULATOR_REQUEST_DELETE_DB;
	pre_order = db;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::deleteTable(const dodoString &table)
{
	qType = ACCUMULATOR_REQUEST_DELETE_TABLE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::deleteField(const dodoString &field,
					const dodoString &table)
{
	qType = ACCUMULATOR_REQUEST_DELETE_FIELD;
	pre_tableTo = field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::createDb(const dodoString &db,
				 const dodoString &charset)
{
	qType = ACCUMULATOR_REQUEST_CREATE_DB;
	pre_order = db;
	pre_having = charset;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::createTable(const __connectorTable &tableInfo)
{
	qType = ACCUMULATOR_REQUEST_CREATE_TABLE;
	pre_tableInfo = tableInfo;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::createField(const __connectorField &row,
					const dodoString &table)
{
	qType = ACCUMULATOR_REQUEST_CREATE_FIELD;
	pre_fieldInfo = row;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
accumulator::where(const dodoString &where)
{
	pre_where = where;

	addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
}

//-------------------------------------------------------------------

void
accumulator::limit(unsigned int a_number)
{
	addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_LIMIT);

	pre_limit = tools::string::lToString(a_number);
}
//-------------------------------------------------------------------

void
accumulator::offset(unsigned int a_number)
{
	addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_OFFSET);

	pre_offset = tools::string::lToString(a_number);
}

//-------------------------------------------------------------------

void
accumulator::order(const dodoString &order)
{
	pre_order = order;

	addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_ORDERBY);
}

//-------------------------------------------------------------------

void
accumulator::group(const dodoString &group)
{
	pre_group = group;

	addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_GROUPBY);
}

//-------------------------------------------------------------------

void
accumulator::having(const dodoString &having)
{
	pre_having = having;

	addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_HAVING);
}

//-------------------------------------------------------------------

void
accumulator::join(const dodoString &table,
		int type,
		const dodoString &condition)
{
	pre_joinTables.push_back(table);
	pre_joinConds.push_back(condition);
	pre_joinTypes.push_back(type);

	addFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_JOIN);
}

//-------------------------------------------------------------------

void
accumulator::unwhere()
{
	removeFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
}

//-------------------------------------------------------------------

void
accumulator::unlimit()
{
	removeFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_LIMIT);
}

//-------------------------------------------------------------------

void
accumulator::unoffset()
{
	removeFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_OFFSET);
}

//-------------------------------------------------------------------

void
accumulator::unorder()
{
	removeFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_ORDERBY);
}

//-------------------------------------------------------------------

void
accumulator::ungroup()
{
	removeFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_GROUPBY);
}

//-------------------------------------------------------------------

void
accumulator::unhaving()
{
	removeFlag(qShift, 1 << ACCUMULATOR_ADDREQUEST_HAVING);
}


//-------------------------------------------------------------------

void
accumulator::setAddInsSt(unsigned int statement)
{
	/*switch (statement)
	   {
	   	default:

	   		break;

	   }*/
	addFlag(qInsShift, 1 << statement);
}

//-------------------------------------------------------------------

void
accumulator::setAddUpSt(unsigned int statement)
{
	/*switch (statement)
	   {
	   	default:

	   }*/
	addFlag(qUpShift, 1 << statement);
}

//-------------------------------------------------------------------

void
accumulator::setAddSelSt(unsigned int statement)
{
	switch (statement)
	{
		case ACCUMULATOR_ADDREQUEST_SELECT_DISTINCT:
		case ACCUMULATOR_ADDREQUEST_SELECT_ALL:

			removeFlag(qSelShift, 1 << ACCUMULATOR_ADDREQUEST_SELECT_ALL);
			removeFlag(qSelShift, 1 << ACCUMULATOR_ADDREQUEST_SELECT_DISTINCT);

			break;

			/*default:*/

	}
	addFlag(qSelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
accumulator::setAddDelSt(unsigned int statement)
{
	/*switch (statement)
	   {
	   	default:

	   }*/
	addFlag(qDelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
accumulator::unsetAddInsSt(unsigned int statement)
{
	removeFlag(qInsShift, 1 << statement);
}

//-------------------------------------------------------------------

void
accumulator::unsetAddUpSt(unsigned int statement)
{
	removeFlag(qUpShift, 1 << statement);
}

//-------------------------------------------------------------------

void
accumulator::unsetAddSelSt(unsigned int statement)
{
	removeFlag(qSelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
accumulator::unsetAddDelSt(unsigned int statement)
{
	removeFlag(qDelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
accumulator::cleanCollected()
{
	qType = -1;

	qShift = ACCUMULATOR_NONE;

	qSelShift = ACCUMULATOR_NONE;
	qInsShift = ACCUMULATOR_NONE;
	qUpShift = ACCUMULATOR_NONE;
	qDelShift = ACCUMULATOR_NONE;

	qDbDepSelShift = ACCUMULATOR_NONE;
	qDbDepInsShift = ACCUMULATOR_NONE;
	qDbDepUpShift = ACCUMULATOR_NONE;
	qDbDepDelShift = ACCUMULATOR_NONE;
	
	pre_where.clear();
	pre_fields.clear();
	pre_values.clear();
	pre_table.clear();
	pre_tableTo.clear();
	pre_order.clear();
	pre_having.clear();
	pre_group.clear();
	pre_limit.clear();
	pre_offset.clear();
	pre_subQueries.clear();
	pre_joinTables.clear();
	pre_joinConds.clear();
	pre_joinTypes.clear();
}

//-------------------------------------------------------------------

void
accumulator::setDbInfo(const dodoString &db,
				  const dodoString &host,
				  unsigned int port,
				  const dodoString &user,
				  const dodoString &password,
				  const dodoString &path)
{
	dbInfo.port = port;
	dbInfo.db = db;
	dbInfo.host = host;
	dbInfo.user = user;
	dbInfo.password = password;
	dbInfo.path = path;
}

//-------------------------------------------------------------------

void
accumulator::setDbInfo(const __connectorInfo &info)
{
	dbInfo.port = info.port;
	dbInfo.db = info.db;
	dbInfo.host = info.host;
	dbInfo.user = info.user;
	dbInfo.password = info.password;
	dbInfo.path = info.path;
}

//-------------------------------------------------------------------

void
accumulator::initTableInfo(__connectorTable &table)
{
	table.name.clear();

	table.primKeys.clear();
	table.uniq.clear();

	table.fields.clear();
}

//-------------------------------------------------------------------

void
accumulator::initFieldInfo(__connectorField &field)
{
	field.type = -1;
	field.length = -1;
	field.flag = 0;
	field.onDelete = -1;
	field.onUpdate = -1;

	field.name.clear();

	field.charset.clear();
	field.refTable.clear();
	field.refFields.clear();
	field.set_enum.clear();
	field.defaultVal.clear();
}

//-------------------------------------------------------------------

