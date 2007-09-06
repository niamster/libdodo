/***************************************************************************
 *            base.cc
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

#include <dbBase.h>

using namespace dodo;

__dbInfo::__dbInfo(const dodoString&a_db,
				   const dodoString&a_host,
				   const dodoString&a_user,
				   const dodoString&a_password,
				   const dodoString&a_path,
				   int a_port) : db(a_db),
	host(a_host),
	user(a_user),
	password(a_password),
	path(a_path),
	port(a_port)
{
}

//-------------------------------------------------------------------

__dbInfo::__dbInfo()
{
}

//-------------------------------------------------------------------

__tableInfo::__tableInfo()
{
	ifNotExists = false;
}

//-------------------------------------------------------------------

const __tableInfo&
__tableInfo::operator=(const __tableInfo&from)
{
	name = from.name;

	fields.clear();

	dodoArray<__fieldInfo>::const_iterator j = from.fields.end();
	for (dodoArray<__fieldInfo>::const_iterator i = from.fields.begin(); i != j; ++i)
		fields.push_back(*i);

	primKeys = from.primKeys;
	uniq = from.uniq;
	ifNotExists = from.ifNotExists;

	return from;
}

//-------------------------------------------------------------------

__fieldInfo::__fieldInfo()
{
	type = -1;
	length = -1;
	flag = 0;
	onDelete = -1;
	onUpdate = -1;
}

//-------------------------------------------------------------------

const __fieldInfo&
__fieldInfo::operator=(const __fieldInfo&from)
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

__dbStorage::__dbStorage(dodoArray<dodoStringArr> a_rows, dodoStringArr a_fields) : rows(a_rows),
	fields(a_fields)
{
}

//-------------------------------------------------------------------

__collectedData::__collectedData(
	dodoString&a_pre_where,
	dodoStringArr&a_pre_fieldsNames,
	dodoArray<dodoStringArr>&a_pre_fieldsVal,
	dodoString&a_pre_table,
	dodoString&a_pre_tableTo,
	dodoString&a_pre_order,
	dodoString&a_pre_having,
	dodoString&a_pre_group,
	dodoString&a_pre_limNumber,
	dodoString&a_pre_limOffset,
	dodoStringArr&a_pre_subQ,
	int&a_qType,
	int&a_qShift,
	int&a_qSelShift,
	int&a_qInsShift,
	int&a_qUpShift,
	int&a_qDelShift
	) :
	pre_where(a_pre_where),
	pre_fieldsNames(a_pre_fieldsNames),
	pre_fieldsVal(a_pre_fieldsVal),
	pre_table(a_pre_table),
	pre_tableTo(a_pre_tableTo),
	pre_order(a_pre_order),
	pre_having(a_pre_having),
	pre_group(a_pre_group),
	pre_limNumber(a_pre_limNumber),
	pre_limOffset(a_pre_limOffset),
	pre_subQ(a_pre_subQ),
	qType(a_qType),
	qShift(a_qShift),
	qSelShift(a_qSelShift),
	qInsShift(a_qInsShift),
	qUpShift(a_qUpShift),
	qDelShift(a_qDelShift)
{
}

//-------------------------------------------------------------------

dbBase::dbBase() : show(false),
	qType(-1),
	qShift(DB_EMPTY),
	qSelShift(DB_EMPTY),
	qInsShift(DB_EMPTY),
	qUpShift(DB_EMPTY),
	qDelShift(DB_EMPTY),
	qDbDepSelShift(DB_EMPTY),
	qDbDepInsShift(DB_EMPTY),
	qDbDepUpShift(DB_EMPTY),
	qDbDepDelShift(DB_EMPTY)
{
}

//-------------------------------------------------------------------

dbBase::~dbBase()
{
}

//-------------------------------------------------------------------

void
dbBase::select(const dodoString&a_table,
			   const dodoStringArr&a_fieldsNames,
			   const dodoString&a_where)
{
	qType = DBREQUEST_SELECT;

	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = true;
}

//-------------------------------------------------------------------

void
dbBase::insert(const dodoString&a_table,
			   const dodoStringMap&a_fields)
{
	qType = DBREQUEST_INSERT;

	pre_table = a_table;

	pre_fieldsNames.clear();
	pre_fieldsVal.clear();

	dodoStringArr temp;

	dodoStringMap::const_iterator i = (a_fields.begin()), j(a_fields.end());
	for (; i != j; ++i)
	{
		pre_fieldsNames.push_back(i->first);
		temp.push_back(i->second);
	}
	pre_fieldsVal.push_back(temp);

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::insert(const dodoString&a_table,
			   const dodoArray<dodoStringMap>&a_fields)
{
	qType = DBREQUEST_INSERT;

	pre_table = a_table;

	dodoArray<dodoStringMap>::const_iterator v(a_fields.begin()), b(a_fields.end());

	pre_fieldsNames.clear();
	pre_fieldsVal.clear();

	dodoStringMap::const_iterator i(v->begin()), j(v->end());
	for (; i != j; ++i)
		pre_fieldsNames.push_back(i->first);

	dodoStringArr temp;

	for (; v != b; ++v)
	{
		j = v->end();
		temp.clear();

		for (i = v->begin(); i != j; ++i)
			temp.push_back(i->second);

		pre_fieldsVal.push_back(temp);
	}

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::insert(const dodoString&a_table,
			   const dodoStringArr&a_fieldsVal,
			   const dodoStringArr&a_fieldsNames)
{
	qType = DBREQUEST_INSERT;

	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;

	pre_fieldsVal.clear();

	pre_fieldsVal.push_back(a_fieldsVal);

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::insert(const dodoString&a_table,
			   const dodoArray<dodoStringArr>&a_fieldsVal,
			   const dodoStringArr&a_fieldsNames)
{
	qType = DBREQUEST_INSERT;

	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;

	dodoArray<dodoStringArr>::const_iterator k(a_fieldsVal.begin()), l(a_fieldsVal.end());
	for (; k != l; ++k)
		pre_fieldsVal.push_back(*k);

	show = false;

}

//-------------------------------------------------------------------

void
dbBase::insertSelect(const dodoString&a_tableTo,
					 const dodoString&a_tableFrom,
					 const dodoStringArr&a_fieldsNamesTo,
					 const dodoStringArr&a_fieldsNamesFrom,
					 const dodoString&a_where)
{
	qType = DBREQUEST_INSERT_SELECT;

	pre_tableTo = a_tableTo;
	pre_table = a_tableFrom;
	pre_fieldsNames = a_fieldsNamesTo;

	pre_fieldsVal.clear();

	pre_fieldsVal.push_back(a_fieldsNamesFrom);

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------
void
dbBase::update(const dodoString&a_table,
			   const dodoStringMap&a_fields,
			   const dodoString&a_where)
{
	qType = DBREQUEST_UPDATE;

	pre_table = a_table;

	dodoStringMap::const_iterator i(a_fields.begin()), j(a_fields.end());

	pre_fieldsNames.clear();
	pre_fieldsVal.clear();

	dodoStringArr temp;

	for (; i != j; ++i)
	{
		pre_fieldsNames.push_back((*i).first);
		temp.push_back((*i).second);
	}
	pre_fieldsVal.push_back(temp);

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::update(const dodoString&a_table,
			   const dodoStringArr&a_fieldsVal,
			   const dodoStringArr&a_fieldsNames,
			   const dodoString&a_where)
{
	qType = DBREQUEST_UPDATE;

	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;

	pre_fieldsVal.clear();

	pre_fieldsVal.push_back(a_fieldsVal);

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::del(const dodoString&a_table,
			const dodoString&a_where)
{
	qType = DBREQUEST_DELETE;

	pre_table = a_table;

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::subquery(const dodoStringArr&sub,
				 int type)
{
	qType = type;
	pre_subQ = sub;
}

//-------------------------------------------------------------------

void
dbBase::truncate(const dodoString&table)
{
	qType = DBREQUEST_TRUNCATE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::renameDb(const dodoString&db,
				 const dodoString&to_db)
{
	qType = DBREQUEST_RENAME_DB;
	pre_order = db;
	pre_having = to_db;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::renameTable(const dodoString&table,
					const dodoString&to_table)
{
	qType = DBREQUEST_RENAME_TABLE;
	pre_table = table;
	pre_having = to_table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createIndex(const dodoString&table,
					const dodoString&field,
					const dodoString&name)
{
	qType = DBREQUEST_CREATE_INDEX;
	pre_table = table;
	pre_fieldsNames.push_back(field);
	pre_having = name;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createIndex(const dodoString&table,
					const dodoStringArr&fields,
					const dodoString&name)
{
	qType = DBREQUEST_CREATE_INDEX;
	pre_table = table;
	pre_fieldsNames = fields;
	pre_having = name;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::deleteIndex(const dodoString&table,
					const dodoString&field)
{
	qType = DBREQUEST_DELETE_INDEX;
	pre_table = table;
	pre_having = field;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::renameField(const dodoString&field,
					const dodoString&to_field,
					const dodoString&table)
{
	qType = DBREQUEST_RENAME_FIELD;
	pre_tableTo = field;
	pre_having = to_field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::deleteDb(const dodoString&db)
{
	qType = DBREQUEST_DELETE_DB;
	pre_order = db;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::deleteTable(const dodoString&table)
{
	qType = DBREQUEST_DELETE_TABLE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::deleteField(const dodoString&field,
					const dodoString&table)
{
	qType = DBREQUEST_DELETE_FIELD;
	pre_tableTo = field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createDb(const dodoString&db,
				 const dodoString&charset)
{
	qType = DBREQUEST_CREATE_DB;
	pre_order = db;
	pre_having = charset;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createTable(__tableInfo&tableInfo)
{
	qType = DBREQUEST_CREATE_TABLE;
	pre_tableInfo = tableInfo;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createField(__fieldInfo&row,
					const dodoString&table)
{
	qType = DBREQUEST_CREATE_FIELD;
	pre_fieldInfo = row;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::where(const dodoString&where)
{
	pre_where = where;

	addFlag(qShift, 1 << DBADDREQUEST_WHERE);
}

//-------------------------------------------------------------------

void
dbBase::limit(unsigned int a_number)
{
	addFlag(qShift, 1 << DBADDREQUEST_LIMIT);

	pre_limNumber = dodoString::lToString(a_number);
}
//-------------------------------------------------------------------

void
dbBase::offset(unsigned int a_number)
{
	addFlag(qShift, 1 << DBADDREQUEST_OFFSET);

	pre_limOffset = dodoString::lToString(a_number);
}

//-------------------------------------------------------------------

void
dbBase::order(const dodoString&order)
{
	pre_order = order;

	addFlag(qShift, 1 << DBADDREQUEST_ORDERBY);
}

//-------------------------------------------------------------------

void
dbBase::group(const dodoString&group)
{
	pre_group = group;

	addFlag(qShift, 1 << DBADDREQUEST_GROUPBY);
}

//-------------------------------------------------------------------

void
dbBase::having(const dodoString&having)
{
	pre_having = having;

	addFlag(qShift, 1 << DBADDREQUEST_HAVING);
}


//-------------------------------------------------------------------

void
dbBase::unwhere()
{
	removeFlag(qShift, 1 << DBADDREQUEST_WHERE);
}

//-------------------------------------------------------------------

void
dbBase::unlimit()
{
	removeFlag(qShift, 1 << DBADDREQUEST_LIMIT);
}

//-------------------------------------------------------------------

void
dbBase::unoffset()
{
	removeFlag(qShift, 1 << DBADDREQUEST_OFFSET);
}

//-------------------------------------------------------------------

void
dbBase::unorder()
{
	removeFlag(qShift, 1 << DBADDREQUEST_ORDERBY);
}

//-------------------------------------------------------------------

void
dbBase::ungroup()
{
	removeFlag(qShift, 1 << DBADDREQUEST_GROUPBY);
}

//-------------------------------------------------------------------

void
dbBase::unhaving()
{
	removeFlag(qShift, 1 << DBADDREQUEST_HAVING);
}


//-------------------------------------------------------------------

void
dbBase::setAddInsSt(unsigned int statement)
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
dbBase::setAddUpSt(unsigned int statement)
{
	/*switch (statement)
	   {
	   	default:

	   		break;

	   }*/
	addFlag(qUpShift, 1 << statement);
}

//-------------------------------------------------------------------

void
dbBase::setAddSelSt(unsigned int statement)
{
	switch (statement)
	{
		case DBREQUEST_SELECT_DISTINCT:
		case DBREQUEST_SELECT_ALL:

			removeFlag(qSelShift, 1 << DBREQUEST_SELECT_ALL);
			removeFlag(qSelShift, 1 << DBREQUEST_SELECT_DISTINCT);

			break;

		default:

			break;

	}
	addFlag(qSelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
dbBase::setAddDelSt(unsigned int statement)
{
	/*switch (statement)
	   {
	   	default:

	   		break;

	   }*/
	addFlag(qDelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
dbBase::unsetAddInsSt(unsigned int statement)
{
	removeFlag(qInsShift, 1 << statement);
}

//-------------------------------------------------------------------

void
dbBase::unsetAddUpSt(unsigned int statement)
{
	removeFlag(qUpShift, 1 << statement);
}

//-------------------------------------------------------------------

void
dbBase::unsetAddSelSt(unsigned int statement)
{
	removeFlag(qSelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
dbBase::unsetAddDelSt(unsigned int statement)
{
	removeFlag(qDelShift, 1 << statement);
}

//-------------------------------------------------------------------

void
dbBase::cleanCollect()
{
	qType = -1;

	qShift = DB_EMPTY;

	qSelShift = DB_EMPTY;
	qInsShift = DB_EMPTY;
	qUpShift = DB_EMPTY;
	qDelShift = DB_EMPTY;

	qDbDepSelShift = DB_EMPTY;
	qDbDepInsShift = DB_EMPTY;
	qDbDepUpShift = DB_EMPTY;
	qDbDepDelShift = DB_EMPTY;
}

//-------------------------------------------------------------------

void
dbBase::setDbInfo(const dodoString&db,
				  const dodoString&host,
				  unsigned int port,
				  const dodoString&user,
				  const dodoString&password,
				  const dodoString&path)
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
dbBase::setDbInfo(const __dbInfo&info)
{
	dbInfo.port = info.port;
	dbInfo.db = info.db;
	dbInfo.host = info.host;
	dbInfo.user = info.user;
	dbInfo.password = info.password;
	dbInfo.path = info.path;
}

//-------------------------------------------------------------------

int
dbBase::getQType() const
{
	return qType;
}

//-------------------------------------------------------------------

__collectedData
dbBase::collectedData()
{
	return __collectedData(
			   pre_where,
			   pre_fieldsNames,
			   pre_fieldsVal,
			   pre_table,
			   pre_tableTo,
			   pre_order,
			   pre_having,
			   pre_group,
			   pre_limNumber,
			   pre_limOffset,
			   pre_subQ,
			   qType,
			   qShift,
			   qSelShift,
			   qInsShift,
			   qUpShift,
			   qDelShift);
}

//-------------------------------------------------------------------

void
dbBase::initTableInfo(__tableInfo&table)
{
	table.name.clear();

	table.primKeys.clear();
	table.uniq.clear();

	table.fields.clear();
}

//-------------------------------------------------------------------

void
dbBase::initFieldInfo(__fieldInfo&field)
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
