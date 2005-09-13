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

#include "dbBase.h"

using namespace dodo;

//-------------------------------------------------------------------

__tableInfo::__tableInfo()
{
	autoIncr = -1;
	avgRowLen = -1;
}

//-------------------------------------------------------------------

const __tableInfo &
__tableInfo::operator=(__tableInfo &from)
{
	name = from.name;
	
	std::vector<__fieldInfo>::iterator j = from.fields.end();
	for (std::vector<__fieldInfo>::iterator i=from.fields.begin();i!=j;++i)
		fields.push_back(*i);
	
	keys = from.keys;
	primKeys = from.primKeys;
	indexes = from.indexes;
	uniq = from.uniq;
	
	autoIncr = from.autoIncr;
	avgRowLen = from.avgRowLen;
	
	comment = from.comment;
	charset = from.charset;
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
__fieldInfo::operator=(__fieldInfo &from)
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

	comment = from.comment;
	charset = from.charset;
}

//-------------------------------------------------------------------

__sqlStorage::__sqlStorage(std::vector<stringArr> a_rows, stringArr a_fields) : rows(a_rows), 
																				fields(a_fields)
{
}

//-------------------------------------------------------------------

__collectedData::__collectedData(
					std::string &a_pre_where,
					stringArr &a_pre_fieldsNames,
					std::vector<stringArr> &a_pre_fieldsVal,
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

dbBase *dbBase::getSelf()
{
	return this;
}

//-------------------------------------------------------------------

dbBase::dbBase() :show(false),

					qShift(EMPTY),

					qSelShift(EMPTY),
					qInsShift(EMPTY),
					qUpShift(EMPTY),
					qDelShift(EMPTY),

					qType(-1),

					qDbDepSelShift(EMPTY),
					qDbDepInsShift(EMPTY),
					qDbDepUpShift(EMPTY),
					qDbDepDelShift(EMPTY)
{
	
	sqlDbDepAddSelArr.push_back("");
	sqlDbDepAddInsArr.push_back("");
	sqlDbDepAddUpArr.push_back("");
	sqlDbDepAddDelArr.push_back("");	
	
	pre_group.reserve(15);
	backup.pre_group.reserve(15);
	
	pre_limNumber.reserve(15);
	backup.pre_limNumber.reserve(15);
	
	pre_limOffset.reserve(15);
	backup.pre_limOffset.reserve(15);	
	
	pre_having.reserve(15);
	backup.pre_having.reserve(15);
	
	pre_where.reserve(30);
	backup.pre_where.reserve(30);
	
	pre_table.reserve(10);
	backup.pre_table.reserve(10);
	
	pre_tableTo.reserve(10);
	backup.pre_tableTo.reserve(10);
	
	pre_order.reserve(15);
	backup.pre_order.reserve(15);
	
	pre_fieldsNames.reserve(10);
	backup.pre_fieldsNames.reserve(10);
	
	pre_fieldsVal.reserve(10);
	backup.pre_fieldsVal.reserve(10);
	
	pre_subQ.reserve(100);
	backup.pre_subQ.reserve(100);
	
}

//-------------------------------------------------------------------

dbBase::~dbBase()
{	
}

//-------------------------------------------------------------------

void
dbBase::debug(__sqlStorage &result) const
{
	unsigned int i(0),j(0);
	std::cout << "--Result--" << std::endl << "rows=" << result.rows.size() << "\tfields=" << result.fields.size() << std::endl;
	for (;j<result.fields.size();++j)
		std::cout << result.fields[j] << '\t';
	std::cout << std::endl;
	for (;i<result.rows.size();++i)
	{
		std::cout << std::endl;
		for (j=0;j<result.fields.size();++j)
			std::cout << result.rows[i][j] << '\t';
	}
	std::cout << std::endl;
}

//-------------------------------------------------------------------

void 
dbBase::select(std::string a_table,
			    const stringArr &a_fieldsNames,
			    const std::string a_where) const 
{
	qType = SELECT;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	if (strlen(a_where.c_str()) != 0)
	{
		addF(qShift,1<<WHERE);
		pre_where = a_where;
	}
	
	show = true;
}

//-------------------------------------------------------------------

void 
dbBase::insert(std::string a_table, 
				const assocArr &a_fields)
{
	qType = INSERT;
	
	pre_table = a_table;
	
	assocArr::const_iterator i(a_fields.begin()),j(a_fields.end());
	
	pre_fieldsNames.clear();
	pre_fieldsVal.clear();
	
	stringArr temp;
	
	for (;i!=j;++i)
	{
		pre_fieldsNames.push_back((*i).first);
		temp.push_back((*i).second);				
	}
	pre_fieldsVal.push_back(temp);
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::insert(std::string a_table, 
				const std::vector<assocArr> &a_fields)
{
	qType = INSERT;
	
	pre_table = a_table;
	
	std::vector<assocArr>::const_iterator i(a_fields.begin()),j(a_fields.end());
	assocArr::const_iterator v,b;

	
	pre_fieldsNames.clear();
	pre_fieldsVal.clear();

	b = i->end();
	for (v=i->begin();v!=b;++v)	
		pre_fieldsNames.push_back((*v).first);

	stringArr temp;
	
	for (;i!=j;++i)
	{
		b = i->end();
		temp.clear();
		for (v=i->begin();v!=b;++v)
			temp.push_back((*v).second);
		pre_fieldsVal.push_back(temp);	
	}

	show = false;	
}

//-------------------------------------------------------------------

void 
dbBase::insert(std::string a_table,
			    const stringArr &a_fieldsVal, 
			    const stringArr &a_fieldsNames)
{
	qType = INSERT;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	pre_fieldsVal.push_back(a_fieldsVal);
	
	show = false;	
}

//-------------------------------------------------------------------

void 
dbBase::insert(std::string a_table, 
				const std::vector<stringArr> &a_fieldsVal, 
				const stringArr &a_fieldsNames)
{
	qType = INSERT;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	
	std::vector<stringArr>::const_iterator i(a_fieldsVal.begin()),j(a_fieldsVal.end());
	
	for(;i!=j;++i)
		pre_fieldsVal.push_back(*i);
	
	show = false;	
	
}

//-------------------------------------------------------------------

void 
dbBase::insertSelect(std::string a_tableTo, 
					std::string a_tableFrom, 
					const stringArr &a_fieldsNamesTo,
					const stringArr &a_fieldsNamesFrom,
					std::string a_where)
{
	qType = INSERT_SELECT;
	
	pre_tableTo = a_tableTo;
	pre_table = a_tableFrom;
	pre_fieldsNames = a_fieldsNamesTo;
	pre_fieldsVal.push_back(a_fieldsNamesFrom);
	
	if (strlen(a_where.c_str()) != 0)
	{
		addF(qShift,1<<WHERE);
		pre_where = a_where;
	}
	
	show = false;	
}

//-------------------------------------------------------------------
void 
dbBase::update(std::string a_table, 
				const assocArr &a_fields, 
				const std::string a_where)
{
	qType = UPDATE;
	
	pre_table = a_table;
	
	assocArr::const_iterator i(a_fields.begin()),j(a_fields.end());
	
	pre_fieldsNames.clear();
	pre_fieldsVal.clear();
	
	stringArr temp;
	
	for (;i!=j;++i)
	{
		pre_fieldsNames.push_back((*i).first);
		temp.push_back((*i).second);				
	}
	pre_fieldsVal.push_back(temp);
	
	if (strlen(a_where.c_str()) != 0)
	{
		addF(qShift,1<<WHERE);
		pre_where = a_where;
	}	
	
	show = false;	
}

//-------------------------------------------------------------------

void
dbBase::update(std::string a_table,
			   const stringArr &a_fieldsVal, 
			   const stringArr &a_fieldsNames, 
			   std::string a_where)
{
	qType = UPDATE;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	pre_fieldsVal.push_back(a_fieldsVal);	

	if (strlen(a_where.c_str()) != 0)
	{
		addF(qShift,1<<WHERE);
		pre_where = a_where;
	}	
	
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::del(std::string a_table, 
			std::string a_where)
{
	qType = DELETE;

	pre_table = a_table;

	if (a_where.size() != 0)
	{
		addF(qShift,1<<WHERE);
		pre_where = a_where;
	}
	
	show = false;	
}

//-------------------------------------------------------------------

void
dbBase::subquery(const stringArr &sub, 
				int type)
{
	qType = type;
	pre_subQ = sub;
}

//-------------------------------------------------------------------

void 
dbBase::use(std::string db)
{
	qType = USE;
	sqlInfo.db = db;
	show = false;
}

//-------------------------------------------------------------------

void
dbBase::truncate(std::string table)
{
	qType = TRUNCATE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::renameDb(std::string db,
			std::string to_db)
{
	qType = RENAME_DB;
	pre_order = db;
	pre_having = to_db;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::renameTable(std::string table,
				std::string to_table)
{
	qType = RENAME_TABLE;
	pre_table = table;
	pre_having = to_table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::renameField(std::string field,
				std::string to_field, 
				std::string table)
{
	qType = RENAME_FIELD;
	pre_tableTo = field;
	pre_having = to_field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::deleteDb(std::string db)
{
	qType = DELETE_DB;
	pre_order = db;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::deleteTable(std::string table)
{
	qType = DELETE_TABLE;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::deleteField(std::string field, 
				std::string table)
{
	qType = DELETE_FIELD;
	pre_tableTo = field;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::createDb(std::string db,
			std::string charset)
{
	qType = CREATE_DB;
	pre_order = db;
	pre_having = charset;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::createTable(__tableInfo &tableInfo)
{
	qType = CREATE_TABLE;
	pre_tableInfo = tableInfo;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::createField(__fieldInfo &row, 
				std::string table)
{
	qType = CREATE_FIELD;
	pre_fieldInfo = row;
	pre_table = table;
	show = false;
}

//-------------------------------------------------------------------

void 
dbBase::where(std::string where) const
{
	pre_where = where;

	addF(qShift,1<<WHERE);
}

//-------------------------------------------------------------------

void 
dbBase::limit(unsigned int a_number) const
{
	addF(qShift,1<<LIMIT);

	pre_limNumber = tools::lToString(a_number);
}
//-------------------------------------------------------------------

void 
dbBase::offset(unsigned int a_number) const
{
	addF(qShift,1<<OFFSET);
	
	pre_limOffset = tools::lToString(a_number);
}

//-------------------------------------------------------------------

void 
dbBase::order(std::string order) const
{
	pre_order = order;

	addF(qShift,1<<ORDERBY);
}

//-------------------------------------------------------------------

void 
dbBase::group(std::string group) const
{	
	pre_group = group;

	addF(qShift,1<<GROUPBY);
}

//-------------------------------------------------------------------

void 
dbBase::having(std::string having) const
{
	pre_having = having;

	addF(qShift,1<<HAVING);
}


//-------------------------------------------------------------------

void 
dbBase::unwhere() const
{
	removeF(qShift,1<<WHERE);	
}

//-------------------------------------------------------------------

void 
dbBase::unlimit() const
{
	removeF(qShift,1<<LIMIT);
}

//-------------------------------------------------------------------

void 
dbBase::unoffset() const
{
	removeF(qShift,1<<OFFSET);
}

//-------------------------------------------------------------------

void 
dbBase::unorder() const
{
	removeF(qShift,1<<ORDERBY);
}

//-------------------------------------------------------------------

void 
dbBase::ungroup() const
{	
	removeF(qShift,1<<GROUPBY);
}

//-------------------------------------------------------------------

void 
dbBase::unhaving() const
{
	removeF(qShift,1<<HAVING);
}


//-------------------------------------------------------------------

void 
dbBase::setAddInsSt(unsigned int statement)
{
	/*switch (statement)
	{
		break;
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
		case SELECT_DISTINCT:
		case SELECT_ALL:
			removeF(qSelShift,1<<SELECT_ALL);
			removeF(qSelShift,1<<SELECT_DISTINCT);
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
	
	qShift = EMPTY;
	
	qSelShift = EMPTY;			
	qInsShift = EMPTY;
	qUpShift = EMPTY;
	qDelShift = EMPTY;
	
	qDbDepSelShift = EMPTY;			
	qDbDepInsShift = EMPTY;
	qDbDepUpShift = EMPTY;
	qDbDepDelShift = EMPTY;
}

//-------------------------------------------------------------------

void 
dbBase::setSqlInfo(std::string db,
	std::string host,
	unsigned int port, 
	std::string user, 
	std::string password,
	std::string socket) const
{
	sqlInfo.port = port;
	sqlInfo.db = db;
	sqlInfo.host = host;
	sqlInfo.user = user;
	sqlInfo.password = password;
	sqlInfo.socket = socket;
}

int 
dbBase::getQType() const
{
	return qType;
}

//-------------------------------------------------------------------

__collectedData 
dbBase::getCollectedData()
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
dbBase::store()
{
	backup.pre_where = pre_where;
	backup.pre_fieldsNames = pre_fieldsNames;
	backup.pre_fieldsVal = pre_fieldsVal;
	backup.pre_table = pre_table;
	backup.pre_tableTo = pre_tableTo;
	backup.pre_order = pre_order;
	backup.pre_group = pre_group;
	backup.pre_having = pre_having;
	backup.pre_limNumber = pre_limNumber;
	backup.pre_limOffset = pre_limOffset;
	backup.pre_subQ = pre_subQ;

	backup.qType = qType;	
	backup.qShift = qShift;
	backup.qSelShift = qSelShift;	
	backup.qInsShift = qInsShift;	
	backup.qUpShift = qUpShift;	
	backup.qDelShift = qDelShift;	
}

//-------------------------------------------------------------------

void 
dbBase::restore()
{
	pre_group = backup.pre_group;
	pre_having = backup.pre_having;
	pre_where = backup.pre_where;
	pre_table = backup.pre_table;
	pre_tableTo = backup.pre_tableTo;
	pre_order = backup.pre_order;
	pre_fieldsNames = backup.pre_fieldsNames;
	pre_fieldsVal = backup.pre_fieldsVal;
	pre_limNumber = backup.pre_limNumber;
	pre_limOffset = backup.pre_limOffset;
	pre_subQ = backup.pre_subQ;
	
	qShift = backup.qShift;
	qType = backup.qType;
	qSelShift = backup.qSelShift;
	qInsShift = backup.qInsShift;
	qUpShift = backup.qUpShift;
	qDelShift = backup.qDelShift;	
}

//-------------------------------------------------------------------

inline void 
dbBase::initTableInfo(__tableInfo &table)
{
	table.autoIncr = -1;
	table.avgRowLen = -1;
	
	table.name.clear();
	
	table.charset.clear();
	table.comment.clear();
	table.keys.clear();
	table.primKeys.clear();
	table.indexes.clear();
	table.uniq.clear();
	
	table.fields.clear();
}

//-------------------------------------------------------------------
inline void 
dbBase::initRowInfo(__fieldInfo &row)
{
	row.type = -1;
	row.length = -1;
	row.flag = 0;	
	row.onDelete = -1;
	row.onUpdate = -1;
	
	row.name.clear();
	
	row.charset.clear();
	row.comment.clear();
	row.refTable.clear();
	row.refFields.clear();
	row.set_enum.clear();
	row.defaultVal.clear();
}
	
//-------------------------------------------------------------------

inline std::string
dbBase::stringType(int type) const
{
	switch (type)
	{
		case INT:
			return std::string("INT");
		case DATE:
			return std::string("DATE");
		case VARCHAR:
			return std::string("VARCHAR");
		case TIMESTAMP:
			return std::string("TIMESTAMP");
		case TIME:
			return std::string("TIME");
		case TINYINT:
			return std::string("TINYINT");
		case SMALLINT:
			return std::string("SMALLINT");
		case MEDIUMINT:
			return std::string("MEDIUMINT");
		case BIGINT:
			return std::string("BIGINT");
		case FLOAT:
			return std::string("FLOAT");
		case DOUBLE:
			return std::string("DOUBLE");
		case DECIMAL:
			return std::string("DECIMAL");
		case CHAR:
			return std::string("CHAR");
		case TINYBLOB:
			return std::string("TINYBLOB");
		case BLOB:
			return std::string("BLOB");
		case MEDIUMBLOB:
			return std::string("MEDIUMBLOB");
		case LONGBLOB:
			return std::string("LONGBLOB");
		case ENUM:
			return std::string("ENUM");
		case SET:
			return std::string("SET");
		default :
			return std::string("");
	}
}
	
//-------------------------------------------------------------------

inline int 
dbBase::chkRange(int type) const
{
	switch (type)
	{
		case DATE:
		case TIME:
		case TINYBLOB:
		case BLOB:
		case MEDIUMBLOB:
		case LONGBLOB:
		case ENUM:
		case SET:
			return -1;
		case INT:
		case TINYINT:
		case SMALLINT:
		case MEDIUMINT:
		case BIGINT:
		case FLOAT:
		case DOUBLE:
		case TIMESTAMP:
			return 0;
		case VARCHAR:
		case CHAR:
		case DECIMAL:
			return 1;
		default :
			return -1;
	}	
}

//-------------------------------------------------------------------

inline std::string 
dbBase::stringReference(int type) const
{
	switch (type)
	{
		case RESTRICT:
			return std::string("restrict");
		case CASCADE:
			return std::string("cascade");
		case SET_NULL:
			return std::string("set null");
		case NO_ACTION:
			return std::string("no action");
		case SET_DEFAULT:
			return std::string("set default");
	}
}

//-------------------------------------------------------------------
