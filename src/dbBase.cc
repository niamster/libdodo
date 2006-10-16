/***************************************************************************
 *            base.cc
 *
 *  Thu Aug 25 22:03:07 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <dbBase.h>

using namespace dodo;

__dbInfo::__dbInfo(const std::string &a_db, 
					const std::string &a_host, 
					const std::string &a_user, 
					const std::string &a_password, 
					const std::string &a_path, 
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
	for (dodoArray<__fieldInfo>::const_iterator i=from.fields.begin();i!=j;++i)
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

__dbStorage::__dbStorage(dodoArray<stringArr> a_rows, stringArr a_fields) : rows(a_rows), 
																				fields(a_fields)
{
}

//-------------------------------------------------------------------

__collectedData::__collectedData(
					std::string &a_pre_where,
					stringArr &a_pre_fieldsNames,
					dodoArray<stringArr> &a_pre_fieldsVal,
					std::string &a_pre_table,
					std::string &a_pre_tableTo,
					std::string &a_pre_order,
					std::string &a_pre_having,
					std::string &a_pre_group,
					std::string &a_pre_limNumber,
					std::string &a_pre_limOffset,
					stringArr &a_pre_subQ,
					int &a_qType,
					int &a_qShift,
					int &a_qSelShift,
					int &a_qInsShift,
					int &a_qUpShift,
					int &a_qDelShift		
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
dbBase::select(const std::string &a_table,
			    const stringArr &a_fieldsNames,
			    const std::string &a_where) const 
{
	qType = DBREQUEST_SELECT;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	
	if (a_where.size() != 0)
	{
		addF(qShift,1<<DBADDREQUEST_WHERE);
		pre_where = a_where;
	}
	
	show = true;
}

//-------------------------------------------------------------------

void 
dbBase::insert(const std::string &a_table, 
				const assocArr &a_fields)
{
	qType = DBREQUEST_INSERT;
	
	pre_table = a_table;
	
	i = a_fields.begin();
	j = a_fields.end();
	
	pre_fieldsNames.clear();
	pre_fieldsVal.clear();
	
	temp.clear();
	
	for (;i!=j;++i)
	{
		pre_fieldsNames.push_back(i->first);
		temp.push_back(i->second);			
	}
	pre_fieldsVal.push_back(temp);
	
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::insert(const std::string &a_table, 
				const dodoArray<assocArr> &a_fields)
{
	qType = DBREQUEST_INSERT;
	
	pre_table = a_table;
	
	v = a_fields.begin();
	b = a_fields.end();
	
	pre_fieldsNames.clear();
	pre_fieldsVal.clear();
	
	j = v->end();
	for (i=v->begin();i!=j;++i)
		pre_fieldsNames.push_back(i->first);

	temp.clear();
	
	for (;v!=b;++v)
	{
		j = v->end();
		temp.clear();
		
		for (i=v->begin();i!=j;++i)
			temp.push_back(i->second);	
			
		pre_fieldsVal.push_back(temp);	
	}

	show = false;	
}

//-------------------------------------------------------------------

void 
dbBase::insert(const std::string &a_table,
			    const stringArr &a_fieldsVal, 
			    const stringArr &a_fieldsNames)
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
dbBase::insert(const std::string &a_table, 
				const dodoArray<stringArr> &a_fieldsVal, 
				const stringArr &a_fieldsNames)
{
	qType = DBREQUEST_INSERT;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	
	k = a_fieldsVal.begin();
	l = a_fieldsVal.end();;
	
	for(;k!=l;++k)
		pre_fieldsVal.push_back(*k);
	
	show = false;	
	
}

//-------------------------------------------------------------------

void 
dbBase::insertSelect(const std::string &a_tableTo, 
					const std::string &a_tableFrom, 
					const stringArr &a_fieldsNamesTo,
					const stringArr &a_fieldsNamesFrom,
					const std::string &a_where)
{
	qType = DBREQUEST_INSERT_SELECT;
	
	pre_tableTo = a_tableTo;
	pre_table = a_tableFrom;
	pre_fieldsNames = a_fieldsNamesTo;
	
	pre_fieldsVal.clear();
	
	pre_fieldsVal.push_back(a_fieldsNamesFrom);
	
	if (a_where.size() != 0)
	{
		addF(qShift,1<<DBADDREQUEST_WHERE);
		pre_where = a_where;
	}
	
	show = false;	
}

//-------------------------------------------------------------------
void 
dbBase::update(const std::string &a_table, 
				const assocArr &a_fields, 
				const std::string &a_where)
{
	qType = DBREQUEST_UPDATE;
        
	pre_table = a_table;
	
	i = a_fields.begin();
	j = a_fields.end();
	
	pre_fieldsNames.clear();
	pre_fieldsVal.clear();
	
	temp.clear();
	
	for (;i!=j;++i)
	{
		pre_fieldsNames.push_back((*i).first);
		temp.push_back((*i).second);				
	}
	pre_fieldsVal.push_back(temp);
	
	if (a_where.size() != 0)
	{
		addF(qShift,1<<DBADDREQUEST_WHERE);
		pre_where = a_where;
	}	
	
	show = false;	
}

//-------------------------------------------------------------------

void
dbBase::update(const std::string &a_table,
			   const stringArr &a_fieldsVal, 
			   const stringArr &a_fieldsNames, 
			   const std::string &a_where)
{
	qType = DBREQUEST_UPDATE;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	
	pre_fieldsVal.clear();
	
	pre_fieldsVal.push_back(a_fieldsVal);	

	if (a_where.size() != 0)
	{
		addF(qShift,1<<DBADDREQUEST_WHERE);
		pre_where = a_where;
	}	
	
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::del(const std::string &a_table, 
			const std::string &a_where)
{
	qType = DBREQUEST_DELETE;

	pre_table = a_table;

	if (a_where.size() != 0)
	{
		addF(qShift,1<<DBADDREQUEST_WHERE);
		pre_where = a_where;
	}
	
	show = false;	
}

//-------------------------------------------------------------------

void
dbBase::subquery(const stringArr &sub, 
				int type) const
{
	qType = type;
	pre_subQ = sub;
}

//-------------------------------------------------------------------

void 
dbBase::use(const std::string &db) const
{
	qType = DBREQUEST_USE;
	dbInfo.db = db;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::truncate(const std::string &table)
{
	qType = DBREQUEST_TRUNCATE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::renameDb(const std::string &db,
			const std::string &to_db)
{
	qType = DBREQUEST_RENAME_DB;
	pre_order = db;
	pre_having = to_db;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::renameTable(const std::string &table,
				const std::string &to_table)
{
	qType = DBREQUEST_RENAME_TABLE;
	pre_table = table;
	pre_having = to_table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::renameField(const std::string &field,
				const std::string &to_field, 
				const std::string &table)
{
	qType = DBREQUEST_RENAME_FIELD;
	pre_tableTo = field;
	pre_having = to_field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::deleteDb(const std::string &db)
{
	qType = DBREQUEST_DELETE_DB;
	pre_order = db;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::deleteTable(const std::string &table)
{
	qType = DBREQUEST_DELETE_TABLE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::deleteField(const std::string &field, 
				const std::string &table)
{
	qType = DBREQUEST_DELETE_FIELD;
	pre_tableTo = field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::createDb(const std::string &db,
			const std::string &charset)
{
	qType = DBREQUEST_CREATE_DB;
	pre_order = db;
	pre_having = charset;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::createTable(__tableInfo &tableInfo)
{
	qType = DBREQUEST_CREATE_TABLE;
	pre_tableInfo = tableInfo;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::createField(__fieldInfo &row, 
				const std::string &table)
{
	qType = DBREQUEST_CREATE_FIELD;
	pre_fieldInfo = row;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::where(const std::string &where) const
{
	pre_where = where;

	addF(qShift,1<<DBADDREQUEST_WHERE);
}

//-------------------------------------------------------------------

void 
dbBase::limit(unsigned int a_number) const
{
	addF(qShift,1<<DBADDREQUEST_LIMIT);

	pre_limNumber = tools::lToString(a_number);
}
//-------------------------------------------------------------------

void 
dbBase::offset(unsigned int a_number) const
{
	addF(qShift,1<<DBADDREQUEST_OFFSET);
	
	pre_limOffset = tools::lToString(a_number);
}

//-------------------------------------------------------------------

void 
dbBase::order(const std::string &order) const
{
	pre_order = order;

	addF(qShift,1<<DBADDREQUEST_ORDERBY);
}

//-------------------------------------------------------------------

void 
dbBase::group(const std::string &group) const
{	
	pre_group = group;

	addF(qShift,1<<DBADDREQUEST_GROUPBY);
}

//-------------------------------------------------------------------

void 
dbBase::having(const std::string &having) const
{
	pre_having = having;

	addF(qShift,1<<DBADDREQUEST_HAVING);
}


//-------------------------------------------------------------------

void 
dbBase::unwhere() const
{
	removeF(qShift,1<<DBADDREQUEST_WHERE);	
}

//-------------------------------------------------------------------

void 
dbBase::unlimit() const
{
	removeF(qShift,1<<DBADDREQUEST_LIMIT);
}

//-------------------------------------------------------------------

void 
dbBase::unoffset() const
{
	removeF(qShift,1<<DBADDREQUEST_OFFSET);
}

//-------------------------------------------------------------------

void 
dbBase::unorder() const
{
	removeF(qShift,1<<DBADDREQUEST_ORDERBY);
}

//-------------------------------------------------------------------

void 
dbBase::ungroup() const
{	
	removeF(qShift,1<<DBADDREQUEST_GROUPBY);
}

//-------------------------------------------------------------------

void 
dbBase::unhaving() const
{
	removeF(qShift,1<<DBADDREQUEST_HAVING);
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
	addF(qInsShift,1<<statement);
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
	addF(qUpShift,1<<statement);	
}

//-------------------------------------------------------------------

void 
dbBase::setAddSelSt(unsigned int statement)
{
	switch (statement)
	{
		case DBREQUEST_SELECT_DISTINCT:
		case DBREQUEST_SELECT_ALL:
		
			removeF(qSelShift,1<<DBREQUEST_SELECT_ALL);
			removeF(qSelShift,1<<DBREQUEST_SELECT_DISTINCT);
			
			break;
			
		default:
		
			break;
		
	}
	addF(qSelShift,1<<statement);	
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
	addF(qDelShift,1<<statement);
}

//-------------------------------------------------------------------

void 
dbBase::unsetAddInsSt(unsigned int statement)
{
	removeF(qInsShift,1<<statement);
}

//-------------------------------------------------------------------

void 
dbBase::unsetAddUpSt(unsigned int statement)
{
	removeF(qUpShift,1<<statement);	
}

//-------------------------------------------------------------------

void 
dbBase::unsetAddSelSt(unsigned int statement)
{
	removeF(qSelShift,1<<statement);	
}

//-------------------------------------------------------------------

void 
dbBase::unsetAddDelSt(unsigned int statement)
{
	removeF(qDelShift,1<<statement);	
}

//-------------------------------------------------------------------

void 
dbBase::cleanCollect() const
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
dbBase::setDbInfo(const std::string &db,
	const std::string &host,
	unsigned int port, 
	const std::string &user, 
	const std::string &password,
	const std::string &path) const
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
dbBase::setDbInfo(const __dbInfo &info) const
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
	qDelShift	
	);
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
