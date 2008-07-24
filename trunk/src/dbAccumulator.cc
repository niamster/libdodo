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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/dbAccumulator.h>

using namespace dodo::db;

#ifndef DB_WO_XEXEC

__xexecDbAccumulatorCollectedData::__xexecDbAccumulatorCollectedData(int &a_operType,
																	 void *a_executor) : operType(a_operType),
																						 executor(a_executor)
{
}

#endif

//-------------------------------------------------------------------

accumulator::accumulator() : show(false)
#ifndef DB_WO_XEXEC

							 ,
							 collectedData(operType,
										   (void *) this)

#endif
{
#ifndef DB_WO_XEXEC

	execObjectData = (void *)&collectedData;

#endif

	collectedData.qType = -1;

	collectedData.qShift = ACCUMULATOR_NONE;
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
	collectedData.qType = ACCUMULATOR_REQUEST_CALL_FUNCTION;

	collectedData.table = name;
	collectedData.fields = arguments;

	if (as.size() != 0)
	{
		addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_AS);
		collectedData.where = as;
	}

	show = true;
}

//-------------------------------------------------------------------

void
accumulator::callProcedure(const dodoString &name,
						   const dodoStringArray &arguments)
{
	collectedData.qType = ACCUMULATOR_REQUEST_CALL_PROCEDURE;

	collectedData.table = name;
	collectedData.fields = arguments;

	show = true;
}

//-------------------------------------------------------------------

void
accumulator::select(const dodoString &a_table,
					const dodoStringArray &a_fields,
					const dodoString &a_where)
{
	collectedData.qType = ACCUMULATOR_REQUEST_SELECT;

	collectedData.table = a_table;
	collectedData.fields = a_fields;

	if (a_where.size() != 0)
	{
		addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		collectedData.where = a_where;
	}

	show = true;
}

//-------------------------------------------------------------------

void
accumulator::selectAll(const dodoString &a_table,
					   const dodoString &a_where)
{
	collectedData.qType = ACCUMULATOR_REQUEST_SELECT;

	collectedData.table = a_table;
	collectedData.fields.clear();
	collectedData.fields.push_back("*");

	if (a_where.size() != 0)
	{
		addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		collectedData.where = a_where;
	}

	show = true;
}

//-------------------------------------------------------------------

void
accumulator::insert(const dodoString &a_table,
					const dodoStringMap &a_fields)
{
	collectedData.qType = ACCUMULATOR_REQUEST_INSERT;

	collectedData.table = a_table;

	collectedData.fields.clear();
	collectedData.values.clear();

	dodoStringArray temp;

	dodoStringMap::const_iterator i = (a_fields.begin()), j(a_fields.end());
	for (; i != j; ++i)
	{
		collectedData.fields.push_back(i->first);
		temp.push_back(i->second);
	}
	collectedData.values.push_back(temp);

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::insert(const dodoString &a_table,
					const dodoArray<dodoStringMap> &a_fields)
{
	collectedData.qType = ACCUMULATOR_REQUEST_INSERT;

	collectedData.table = a_table;

	dodoArray<dodoStringMap>::const_iterator v(a_fields.begin()), b(a_fields.end());

	collectedData.fields.clear();
	collectedData.values.clear();

	dodoStringMap::const_iterator i(v->begin()), j(v->end());
	for (; i != j; ++i)
		collectedData.fields.push_back(i->first);

	dodoStringArray temp;

	for (; v != b; ++v)
	{
		j = v->end();
		temp.clear();

		for (i = v->begin(); i != j; ++i)
			temp.push_back(i->second);

		collectedData.values.push_back(temp);
	}

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::insert(const dodoString &a_table,
					const dodoStringArray &a_values,
					const dodoStringArray &a_fields)
{
	collectedData.qType = ACCUMULATOR_REQUEST_INSERT;

	collectedData.table = a_table;
	collectedData.fields = a_fields;

	collectedData.values.clear();

	collectedData.values.push_back(a_values);

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::insert(const dodoString &a_table,
					const dodoArray<dodoStringArray> &a_values,
					const dodoStringArray &a_fields)
{
	collectedData.qType = ACCUMULATOR_REQUEST_INSERT;

	collectedData.table = a_table;
	collectedData.fields = a_fields;

	dodoArray<dodoStringArray>::const_iterator k(a_values.begin()), l(a_values.end());
	for (; k != l; ++k)
		collectedData.values.push_back(*k);

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
	collectedData.qType = ACCUMULATOR_REQUEST_INSERT_SELECT;

	collectedData.tableTo = a_tableTo;
	collectedData.table = a_tableFrom;
	collectedData.fields = a_fieldsTo;

	collectedData.values.clear();

	collectedData.values.push_back(a_fieldsFrom);

	if (a_where.size() != 0)
	{
		addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		collectedData.where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::update(const dodoString &a_table,
					const dodoStringMap &a_fields,
					const dodoString &a_where)
{
	collectedData.qType = ACCUMULATOR_REQUEST_UPDATE;

	collectedData.table = a_table;

	dodoStringMap::const_iterator i(a_fields.begin()), j(a_fields.end());

	collectedData.fields.clear();
	collectedData.values.clear();

	dodoStringArray temp;

	for (; i != j; ++i)
	{
		collectedData.fields.push_back((*i).first);
		temp.push_back((*i).second);
	}
	collectedData.values.push_back(temp);

	if (a_where.size() != 0)
	{
		addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		collectedData.where = a_where;
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
	collectedData.qType = ACCUMULATOR_REQUEST_UPDATE;

	collectedData.table = a_table;
	collectedData.fields = a_fields;

	collectedData.values.clear();

	collectedData.values.push_back(a_values);

	if (a_where.size() != 0)
	{
		addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		collectedData.where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::del(const dodoString &a_table,
				 const dodoString &a_where)
{
	collectedData.qType = ACCUMULATOR_REQUEST_DELETE;

	collectedData.table = a_table;

	if (a_where.size() != 0)
	{
		addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_WHERE);
		collectedData.where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
accumulator::subquery(const dodoStringArray &sub,
					  int type)
{
	collectedData.qType = type;

	collectedData.subQueries = sub;
}

//-------------------------------------------------------------------

void
accumulator::limit(unsigned int a_number)
{
	addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_LIMIT);

	collectedData.limit = tools::string::lToString(a_number);
}
//-------------------------------------------------------------------

void
accumulator::offset(unsigned int a_number)
{
	addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_OFFSET);

	collectedData.offset = tools::string::lToString(a_number);
}

//-------------------------------------------------------------------

void
accumulator::order(const dodoString &order)
{
	collectedData.order = order;

	addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_ORDERBY);
}

//-------------------------------------------------------------------

void
accumulator::group(const dodoString &group)
{
	collectedData.group = group;

	addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_GROUPBY);
}

//-------------------------------------------------------------------

void
accumulator::having(const dodoString &having)
{
	collectedData.having = having;

	addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_HAVING);
}

//-------------------------------------------------------------------

void
accumulator::join(const dodoString &table,
				  int type,
				  const dodoString &condition)
{
	collectedData.joinTables.push_back(table);
	collectedData.joinConds.push_back(condition);
	collectedData.joinTypes.push_back(type);

	addFlag(collectedData.qShift, 1 << ACCUMULATOR_ADDREQUEST_JOIN);
}

//-------------------------------------------------------------------

void
accumulator::cleanCollected()
{
	collectedData.qType = -1;

	collectedData.qShift = ACCUMULATOR_NONE;

	collectedData.where.clear();
	collectedData.fields.clear();
	collectedData.values.clear();
	collectedData.table.clear();
	collectedData.tableTo.clear();
	collectedData.order.clear();
	collectedData.having.clear();
	collectedData.group.clear();
	collectedData.limit.clear();
	collectedData.offset.clear();
	collectedData.subQueries.clear();
	collectedData.joinTables.clear();
	collectedData.joinConds.clear();
	collectedData.joinTypes.clear();
}

//-------------------------------------------------------------------

void
accumulator::setDbInfo(const __connectionInfo &info)
{
	collectedData.dbInfo.port = info.port;
	collectedData.dbInfo.db = info.db;
	collectedData.dbInfo.host = info.host;
	collectedData.dbInfo.user = info.user;
	collectedData.dbInfo.password = info.password;
	collectedData.dbInfo.path = info.path;
}

//-------------------------------------------------------------------

