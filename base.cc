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

#include "base.h"

using namespace dodo;


//-------------------------------------------------------------------

__sqlStorage::__sqlStorage(std::vector<stringArr> a_rows, stringArr a_fields) : rows(a_rows), 
																				fields(a_fields)
{
}

//-------------------------------------------------------------------

__collectedData::__collectedData(
					std::string &a_pre_where,
					stringArr &a_pre_fieldsNames,
					stringArr &a_pre_fieldsVal,
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

base *base::getSelf()
{
	return this;
}

//-------------------------------------------------------------------

base::base() :show(false),

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

base::~base()
{	
}

//-------------------------------------------------------------------

void
base::debug(__sqlStorage &result) const
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
base::select(std::string a_table,
			    const stringArr &a_fieldsNames,
			    const std::string a_where) const 
{
	qType = SELECT;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	if (strlen(a_where.c_str()) != 0)
	{
		tools::addF(qShift,1<<WHERE);
		pre_where = a_where;
	}
	
	show = true;
}

//-------------------------------------------------------------------

void 
base::insert(std::string a_table, 
				const std::map<std::string, std::string> &a_fields)
{
	qType = INSERT;
	
	pre_table = a_table;
	
	std::map<std::string, std::string>::const_iterator i(a_fields.begin()),j(a_fields.end());
	
	pre_fieldsNames.clear();
	pre_fieldsVal.clear();
	
	for (;i!=j;++i)
	{
		pre_fieldsNames.push_back((*i).first);
		pre_fieldsVal.push_back((*i).second);				
	}
	
	show = false;
}

//-------------------------------------------------------------------

void 
base::insert(std::string a_table,
			    const stringArr &a_fieldsVal, 
			    const stringArr &a_fieldsNames)
{
	qType = INSERT;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	pre_fieldsVal = a_fieldsVal;
	
	show = false;	
}

//-------------------------------------------------------------------

void 
base::insertSelect(std::string a_tableTo, 
					std::string a_tableFrom, 
					const stringArr &a_fieldsNamesTo,
					const stringArr &a_fieldsNamesFrom,
					std::string a_where)
{
	qType = INSERT_SELECT;
	
	pre_tableTo = a_tableTo;
	pre_table = a_tableFrom;
	pre_fieldsNames = a_fieldsNamesTo;
	pre_fieldsVal = a_fieldsNamesFrom;
	
	if (strlen(a_where.c_str()) != 0)
	{
		tools::addF(qShift,1<<WHERE);
		pre_where = a_where;
	}
	
	show = false;	
}

//-------------------------------------------------------------------

void 
base::update(std::string a_table, 
				const std::map<std::string, std::string> &a_fields, 
				const std::string a_where)
{
	qType = UPDATE;
	
	pre_table = a_table;
	
	std::map<std::string, std::string>::const_iterator i(a_fields.begin()),j(a_fields.end());
	
	pre_fieldsNames.clear();
	pre_fieldsVal.clear();
	
	for (;i!=j;++i)
	{
		pre_fieldsNames.push_back((*i).first);
		pre_fieldsVal.push_back((*i).second);				
	}
	if (strlen(a_where.c_str()) != 0)
	{
		tools::addF(qShift,1<<WHERE);
		pre_where = a_where;
	}	
	
	show = false;	
}

//-------------------------------------------------------------------

void
base::update(std::string a_table,
			   const stringArr &a_fieldsVal, 
			   const stringArr &a_fieldsNames, 
			   std::string a_where)
{
	qType = UPDATE;
	
	pre_table = a_table;
	pre_fieldsNames = a_fieldsNames;
	pre_fieldsVal = a_fieldsVal;	

	if (strlen(a_where.c_str()) != 0)
	{
		tools::addF(qShift,1<<WHERE);
		pre_where = a_where;
	}	
	
	show = false;
}

//-------------------------------------------------------------------

void
base::del(std::string a_table, 
			std::string a_where)
{
	qType = DELETE;

	pre_table = a_table;

	if (a_where.size() != 0)
	{
		tools::addF(qShift,1<<WHERE);
		pre_where = a_where;
	}
	
	show = false;	
}

//-------------------------------------------------------------------

void
base::subquery(const stringArr &sub, 
				int type)
{
	qType = type;
	pre_subQ = sub;
}

//-------------------------------------------------------------------

void 
base::use(std::string db)
{
	qType = USE;
	sqlInfo.db = db;
	show = false;
}

//-------------------------------------------------------------------

void 
base::where(std::string where) const
{
	pre_where = where;

	tools::addF(qShift,1<<WHERE);
}

//-------------------------------------------------------------------

void 
base::limit(unsigned int a_number) const
{
	tools::addF(qShift,1<<LIMIT);
	
	char temp[SIZEOFNUM];
	sprintf(temp,"%d",a_number);
	pre_limNumber = temp;
}
//-------------------------------------------------------------------

void 
base::offset(unsigned int a_number) const
{
	tools::addF(qShift,1<<OFFSET);
	
	char temp[SIZEOFNUM];
	sprintf(temp,"%d",a_number);	
	pre_limOffset = temp;
}

//-------------------------------------------------------------------

void 
base::order(std::string order) const
{
	pre_order = order;

	tools::addF(qShift,1<<ORDERBY);
}

//-------------------------------------------------------------------

void 
base::group(std::string group) const
{	
	pre_group = group;

	tools::addF(qShift,1<<GROUPBY);
}

//-------------------------------------------------------------------

void 
base::having(std::string having) const
{
	pre_having = having;

	tools::addF(qShift,1<<HAVING);
}


//-------------------------------------------------------------------

void 
base::unwhere() const
{
	tools::removeF(qShift,1<<WHERE);	
}

//-------------------------------------------------------------------

void 
base::unlimit() const
{
	tools::removeF(qShift,1<<LIMIT);
}

//-------------------------------------------------------------------

void 
base::unoffset() const
{
	tools::removeF(qShift,1<<OFFSET);
}

//-------------------------------------------------------------------

void 
base::unorder() const
{
	tools::removeF(qShift,1<<ORDERBY);
}

//-------------------------------------------------------------------

void 
base::ungroup() const
{	
	tools::removeF(qShift,1<<GROUPBY);
}

//-------------------------------------------------------------------

void 
base::unhaving() const
{
	tools::removeF(qShift,1<<HAVING);
}


//-------------------------------------------------------------------

void 
base::setAddInsSt(unsigned int statement)
{
	/*switch (statement)
	{
		break;
		default:
			break;
		
	}*/
	tools::addF(qInsShift,1<<statement);
}

//-------------------------------------------------------------------

void 
base::setAddUpSt(unsigned int statement)
{
	/*switch (statement)
	{
		default:
			break;
		
	}*/
	tools::addF(qUpShift,1<<statement);	
}

//-------------------------------------------------------------------

void 
base::setAddSelSt(unsigned int statement)
{
	switch (statement)
	{
		case SELECT_DISTINCT:
		case SELECT_ALL:
			tools::removeF(qSelShift,1<<SELECT_ALL);
			tools::removeF(qSelShift,1<<SELECT_DISTINCT);
			break;
		default:
			break;
		
	}
	tools::addF(qSelShift,1<<statement);	
}

//-------------------------------------------------------------------

void 
base::setAddDelSt(unsigned int statement)
{
	/*switch (statement)
	{
		default:
			break;
		
	}*/
	tools::addF(qDelShift,1<<statement);
}

//-------------------------------------------------------------------

void 
base::unsetAddInsSt(unsigned int statement)
{
	tools::removeF(qInsShift,1<<statement);
}

//-------------------------------------------------------------------

void 
base::unsetAddUpSt(unsigned int statement)
{
	tools::removeF(qUpShift,1<<statement);	
}

//-------------------------------------------------------------------

void 
base::unsetAddSelSt(unsigned int statement)
{
	tools::removeF(qSelShift,1<<statement);	
}

//-------------------------------------------------------------------

void 
base::unsetAddDelSt(unsigned int statement)
{
	tools::removeF(qDelShift,1<<statement);	
}

//-------------------------------------------------------------------

void 
base::cleanCollect() const
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
base::setSqlInfo(std::string db,
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
base::getQType() const
{
	return qType;
}

//-------------------------------------------------------------------

__collectedData 
base::getCollectedData()
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
base::store()
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
base::restore()
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
