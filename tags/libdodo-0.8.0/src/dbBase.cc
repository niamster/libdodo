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

#include <libdodo/dbBase.h>

using namespace dodo;

__dbInfo::__dbInfo(const dodoString &a_db,
				   const dodoString &a_host,
				   const dodoString &a_user,
				   const dodoString &a_password,
				   const dodoString &a_path,
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

const __tableInfo &
__tableInfo::operator=(const __tableInfo &from)
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

const __fieldInfo &
__fieldInfo::operator=(const __fieldInfo &from)
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

__dbStorage::__dbStorage(dodoArray<dodoStringArray> a_rows, dodoStringArray a_fields) : rows(a_rows),
																						fields(a_fields)
{
}

//-------------------------------------------------------------------

dbBase::dbBase() : show(false),
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
dbBase::select(const dodoString &a_table,
			   const dodoStringArray &a_fieldsNames,
			   const dodoString &a_where)
{
	qType = DBBASE_REQUEST_SELECT;

	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBBASE_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = true;
}

//-------------------------------------------------------------------

void
dbBase::insert(const dodoString &a_table,
			   const dodoStringMap &a_fields)
{
	qType = DBBASE_REQUEST_INSERT;

	pre_table = a_table;

	pre_fieldsNames.clear();
	pre_fieldsVal.clear();

	dodoStringArray temp;

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
dbBase::insert(const dodoString &a_table,
			   const dodoArray<dodoStringMap> &a_fields)
{
	qType = DBBASE_REQUEST_INSERT;

	pre_table = a_table;

	dodoArray<dodoStringMap>::const_iterator v(a_fields.begin()), b(a_fields.end());

	pre_fieldsNames.clear();
	pre_fieldsVal.clear();

	dodoStringMap::const_iterator i(v->begin()), j(v->end());
	for (; i != j; ++i)
		pre_fieldsNames.push_back(i->first);

	dodoStringArray temp;

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
dbBase::insert(const dodoString &a_table,
			   const dodoStringArray &a_fieldsVal,
			   const dodoStringArray &a_fieldsNames)
{
	qType = DBBASE_REQUEST_INSERT;

	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;

	pre_fieldsVal.clear();

	pre_fieldsVal.push_back(a_fieldsVal);

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::insert(const dodoString &a_table,
			   const dodoArray<dodoStringArray> &a_fieldsVal,
			   const dodoStringArray &a_fieldsNames)
{
	qType = DBBASE_REQUEST_INSERT;

	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;

	dodoArray<dodoStringArray>::const_iterator k(a_fieldsVal.begin()), l(a_fieldsVal.end());
	for (; k != l; ++k)
		pre_fieldsVal.push_back(*k);

	show = false;

}

//-------------------------------------------------------------------

void
dbBase::insertSelect(const dodoString &a_tableTo,
					 const dodoString &a_tableFrom,
					 const dodoStringArray &a_fieldsNamesTo,
					 const dodoStringArray &a_fieldsNamesFrom,
					 const dodoString &a_where)
{
	qType = DBBASE_REQUEST_INSERT_SELECT;

	pre_tableTo = a_tableTo;
	pre_table = a_tableFrom;
	pre_fieldsNames = a_fieldsNamesTo;

	pre_fieldsVal.clear();

	pre_fieldsVal.push_back(a_fieldsNamesFrom);

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBBASE_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------
void
dbBase::update(const dodoString &a_table,
			   const dodoStringMap &a_fields,
			   const dodoString &a_where)
{
	qType = DBBASE_REQUEST_UPDATE;

	pre_table = a_table;

	dodoStringMap::const_iterator i(a_fields.begin()), j(a_fields.end());

	pre_fieldsNames.clear();
	pre_fieldsVal.clear();

	dodoStringArray temp;

	for (; i != j; ++i)
	{
		pre_fieldsNames.push_back((*i).first);
		temp.push_back((*i).second);
	}
	pre_fieldsVal.push_back(temp);

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBBASE_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::update(const dodoString &a_table,
			   const dodoStringArray &a_fieldsVal,
			   const dodoStringArray &a_fieldsNames,
			   const dodoString &a_where)
{
	qType = DBBASE_REQUEST_UPDATE;

	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;

	pre_fieldsVal.clear();

	pre_fieldsVal.push_back(a_fieldsVal);

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBBASE_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::del(const dodoString &a_table,
			const dodoString &a_where)
{
	qType = DBBASE_REQUEST_DELETE;

	pre_table = a_table;

	if (a_where.size() != 0)
	{
		addFlag(qShift, 1 << DBBASE_ADDREQUEST_WHERE);
		pre_where = a_where;
	}

	show = false;
}

//-------------------------------------------------------------------

void
dbBase::subquery(const dodoStringArray &sub,
				 int type)
{
	qType = type;
	pre_subQ = sub;
}

//-------------------------------------------------------------------

void
dbBase::truncate(const dodoString &table)
{
	qType = DBBASE_REQUEST_TRUNCATE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::renameDb(const dodoString &db,
				 const dodoString &to_db)
{
	qType = DBBASE_REQUEST_RENAME_DB;
	pre_order = db;
	pre_having = to_db;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::renameTable(const dodoString &table,
					const dodoString &to_table)
{
	qType = DBBASE_REQUEST_RENAME_TABLE;
	pre_table = table;
	pre_having = to_table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createIndex(const dodoString &table,
					const dodoString &field,
					const dodoString &name)
{
	qType = DBBASE_REQUEST_CREATE_INDEX;
	pre_table = table;
	pre_fieldsNames.push_back(field);
	pre_having = name;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createIndex(const dodoString &table,
					const dodoStringArray &fields,
					const dodoString &name)
{
	qType = DBBASE_REQUEST_CREATE_INDEX;
	pre_table = table;
	pre_fieldsNames = fields;
	pre_having = name;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::deleteIndex(const dodoString &table,
					const dodoString &field)
{
	qType = DBBASE_REQUEST_DELETE_INDEX;
	pre_table = table;
	pre_having = field;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::renameField(const dodoString &field,
					const dodoString &to_field,
					const dodoString &table)
{
	qType = DBBASE_REQUEST_RENAME_FIELD;
	pre_tableTo = field;
	pre_having = to_field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::deleteDb(const dodoString &db)
{
	qType = DBBASE_REQUEST_DELETE_DB;
	pre_order = db;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::deleteTable(const dodoString &table)
{
	qType = DBBASE_REQUEST_DELETE_TABLE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::deleteField(const dodoString &field,
					const dodoString &table)
{
	qType = DBBASE_REQUEST_DELETE_FIELD;
	pre_tableTo = field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createDb(const dodoString &db,
				 const dodoString &charset)
{
	qType = DBBASE_REQUEST_CREATE_DB;
	pre_order = db;
	pre_having = charset;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createTable(__tableInfo &tableInfo)
{
	qType = DBBASE_REQUEST_CREATE_TABLE;
	pre_tableInfo = tableInfo;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::createField(__fieldInfo &row,
					const dodoString &table)
{
	qType = DBBASE_REQUEST_CREATE_FIELD;
	pre_fieldInfo = row;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::where(const dodoString &where)
{
	pre_where = where;

	addFlag(qShift, 1 << DBBASE_ADDREQUEST_WHERE);
}

//-------------------------------------------------------------------

void
dbBase::limit(unsigned int a_number)
{
	addFlag(qShift, 1 << DBBASE_ADDREQUEST_LIMIT);

	pre_limNumber = stringTools::lToString(a_number);
}
//-------------------------------------------------------------------

void
dbBase::offset(unsigned int a_number)
{
	addFlag(qShift, 1 << DBBASE_ADDREQUEST_OFFSET);

	pre_limOffset = stringTools::lToString(a_number);
}

//-------------------------------------------------------------------

void
dbBase::order(const dodoString &order)
{
	pre_order = order;

	addFlag(qShift, 1 << DBBASE_ADDREQUEST_ORDERBY);
}

//-------------------------------------------------------------------

void
dbBase::group(const dodoString &group)
{
	pre_group = group;

	addFlag(qShift, 1 << DBBASE_ADDREQUEST_GROUPBY);
}

//-------------------------------------------------------------------

void
dbBase::having(const dodoString &having)
{
	pre_having = having;

	addFlag(qShift, 1 << DBBASE_ADDREQUEST_HAVING);
}

//-------------------------------------------------------------------

void
dbBase::unwhere()
{
	removeFlag(qShift, 1 << DBBASE_ADDREQUEST_WHERE);
}

//-------------------------------------------------------------------

void
dbBase::unlimit()
{
	removeFlag(qShift, 1 << DBBASE_ADDREQUEST_LIMIT);
}

//-------------------------------------------------------------------

void
dbBase::unoffset()
{
	removeFlag(qShift, 1 << DBBASE_ADDREQUEST_OFFSET);
}

//-------------------------------------------------------------------

void
dbBase::unorder()
{
	removeFlag(qShift, 1 << DBBASE_ADDREQUEST_ORDERBY);
}

//-------------------------------------------------------------------

void
dbBase::ungroup()
{
	removeFlag(qShift, 1 << DBBASE_ADDREQUEST_GROUPBY);
}

//-------------------------------------------------------------------

void
dbBase::unhaving()
{
	removeFlag(qShift, 1 << DBBASE_ADDREQUEST_HAVING);
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

	   }*/
	addFlag(qUpShift, 1 << statement);
}

//-------------------------------------------------------------------

void
dbBase::setAddSelSt(unsigned int statement)
{
	switch (statement)
	{
		case DBBASE_REQUEST_SELECT_DISTINCT:
		case DBBASE_REQUEST_SELECT_ALL:

			removeFlag(qSelShift, 1 << DBBASE_REQUEST_SELECT_ALL);
			removeFlag(qSelShift, 1 << DBBASE_REQUEST_SELECT_DISTINCT);

			break;

			/*default:*/

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
dbBase::cleanCollected()
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
dbBase::setDbInfo(const dodoString &db,
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
dbBase::setDbInfo(const __dbInfo &info)
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
dbBase::initTableInfo(__tableInfo &table)
{
	table.name.clear();

	table.primKeys.clear();
	table.uniq.clear();

	table.fields.clear();
}

//-------------------------------------------------------------------

void
dbBase::initFieldInfo(__fieldInfo &field)
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
