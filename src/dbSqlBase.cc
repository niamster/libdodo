/***************************************************************************
 *            dbSqlBase.cc
 *
 *  Mon Jul 18 19:30:55 2005
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
 *  aint with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <dbSqlBase.h>

#include <dbSqlBaseEx.h>
#include <tools.h>

using namespace dodo;

/**
 * arrays of positions of "statement" for complete realization. In sql wersion - see dbSqlBase;
 */	
static unsigned int addInsEnumArr[1] = 
{
	DBREQUEST_INSERT_IGNORE,
};

//-------------------------------------------------------------------

static unsigned int addUpEnumArr[1] = 
{
	DBREQUEST_UPDATE_IGNORE,
};

//-------------------------------------------------------------------

static unsigned int addDelEnumArr[1] = 
{
	DBREQUEST_DELETE_IGNORE,
};

//-------------------------------------------------------------------

static unsigned int addSelEnumArr[2] = 
{
	DBREQUEST_SELECT_DISTINCT,
	DBREQUEST_SELECT_ALL
};	

//-------------------------------------------------------------------

///////////////////////////////////
//	These statements need additional statement
///////////////////////////////////		
/**
* do not edit it please.
* dbSqlBase use them in such way, as they are
*/
static const __statements sqlQStArr[4] = 
{
	{" union "},
	{" union all "},
	{" minus "},
	{" intersect "}
};
static 	__statements sqlAddArr[7] = 
{
	{""},
	{" where "},
	{" having "},
	{" group by "},			
	{" order by "},		
	{" limit "},
	{" offset "},
};
///////////////////////////////////
//	These statements don't need additional statement
///////////////////////////////////
static 	__statements sqlAddInsArr[2] = 
{
	{""},
	{" ignore "},
};
static 	__statements sqlAddUpArr[2] = 
{
	{""},
	{" ignore "},
};
static 	__statements sqlAddDelArr[2] = 
{
	{""},
	{" ignore "},
};	
static 	__statements sqlAddSelArr[3] = 
{
	{""},
	{" distinct "},
	{" all "},
};

//-------------------------------------------------------------------

dbSqlBase::dbSqlBase() : preventFraming(false)
{	
	auto_increment = " auto_increment ";
}

//-------------------------------------------------------------------

dbSqlBase::~dbSqlBase()
{	
}

//-------------------------------------------------------------------

std::string
dbSqlBase::fieldsValName(const stringArr &fieldsVal, 
					const stringArr &fieldsNames,
					const std::string &frame) const
{
	temp.clear();
	
	register unsigned int fn(fieldsNames.size()),fv(fieldsVal.size());
	
	register unsigned int o(fn<=fv?fn:fv);
	
	i = fieldsNames.begin();
	j = fieldsVal.begin();
		
	for (register unsigned int k(0);k<o-1;++i,++k,++j)
	{
		temp.append(*i);	
		temp.append("=");	
		temp.append(frame);	
		temp.append(escapeFields(*j));	
		temp.append(frame);	
		temp.append(",");	
		
	}
	temp.append(*i);	
	temp.append("=");	
	temp.append(frame);	
	temp.append(escapeFields(*j));	
	temp.append(frame);	
	
	return temp;
}

//-------------------------------------------------------------------

std::string 
dbSqlBase::exists(const std::string &statement)
{
	return std::string("exists("+statement+')');
}

//-------------------------------------------------------------------

std::string 
dbSqlBase::noexists(const std::string &statement)
{
	return std::string("not exists("+statement+')');
}

//-------------------------------------------------------------------

void 
dbSqlBase::additionalCollect(unsigned int qTypeTocheck, 
						const std::string &collectedString) const
{
	if (qShift == DB_EMPTY)
		return ;
		
	register int tempQTypeTocheck = 1<<qTypeTocheck;
	if ((tempQTypeTocheck & qShift)==tempQTypeTocheck)
	{
		request.append(sqlAddArr[qTypeTocheck].str);
		request.append(collectedString);
	}
}

//-------------------------------------------------------------------

std::string 
dbSqlBase::insideAddCollect(const unsigned int sqlAddEnumArr[],
						const __statements sqlAddArr[],
						int qTypeShift) const
{
	if (qTypeShift == DB_EMPTY)
		return __string__;
		
	temp_.clear();
	
	register unsigned int arrLen = sizeof(sqlAddArr)/sizeof(char *);
	temp_bit = 0;
	
	for (register unsigned int i=0;i<arrLen;++i)
	{
		temp_bit = 1<<sqlAddEnumArr[i];
		if ((temp_bit & qTypeShift) == temp_bit)
			temp_.append(sqlAddArr[sqlAddEnumArr[i]].str);
	}
	
	return temp_;
}

//-------------------------------------------------------------------

std::string 
dbSqlBase::insideAddCollect(const stringArr &statements, 
						int qTypeShift) const
{
	if (qTypeShift == DB_EMPTY)
		return __string__;
		
	temp_.clear();

	register unsigned int k(1);
	temp_bit = 0;
	
	i = statements.begin();
	j = statements.end();
	
	for (;i!=j;++i,++k)
	{
		temp_bit = 1<<k;
		if ((temp_bit & qTypeShift)==temp_bit)
			temp_.append(*i);
	}
	
	return temp_;
	
}

//-------------------------------------------------------------------

void 
dbSqlBase::selectCollect() const 
{
	temp = insideAddCollect(addSelEnumArr,sqlAddSelArr,qSelShift);
	temp.append(insideAddCollect(sqlDbDepAddSelArr,qDbDepSelShift));
	
	if (pre_table.size()>0)
	{
		temp.append(tools::implode(pre_fieldsNames,","));

		request = "select ";
		request.append(temp);
		request.append(" from ");
		request.append(pre_table);
	}
	else
	{
		temp.append(tools::implode(pre_fieldsNames,","));
		request = "select ";
		request.append(temp);
	}
}

//-------------------------------------------------------------------

void 
dbSqlBase::insertCollect() const
{
	fieldsVPart.clear();

	k = pre_fieldsVal.begin();
	l = pre_fieldsVal.end();

	char frame[] = "'";
	if (preventFraming)
		frame[0] = ' ';
		
	for (;k!=l;++k)
		fieldsVPart.push_back(tools::implode(*k,&dodo::dbSqlBase::escapeFields,",",frame));
	
	i = fieldsVPart.begin();
	j = fieldsVPart.end()-1;
	
	std::string fieldsPart;
	for (;i!=j;++i)
	{
		fieldsPart.append("(");	
		fieldsPart.append(*i);	
		fieldsPart.append("),");	
	}
	fieldsPart.append("(");	
	fieldsPart.append(*i);	
	fieldsPart.append(")");	
	
	temp = insideAddCollect(addInsEnumArr,sqlAddInsArr,qInsShift);
	temp.append(insideAddCollect(sqlDbDepAddInsArr,qDbDepInsShift));
	
	temp_ = pre_table;
	
	if (pre_fieldsNames.size() != 0)
	{
		temp_.append(" (");
		temp_.append(tools::implode(pre_fieldsNames,","));
		temp_.append(") ");
	}
	
	request = "insert ";
	request.append(temp);
	request.append(" into ");
	request.append(temp_);
	request.append(" values ");
	request.append(fieldsPart);
}

//-------------------------------------------------------------------

void 
dbSqlBase::insertSelectCollect() const
{
	
	std::string fieldsPartTo = tools::implode(pre_fieldsNames,",");
	std::string fieldsPartFrom = tools::implode(pre_fieldsVal.front(),",");
	
	temp = insideAddCollect(addInsEnumArr,sqlAddInsArr,qInsShift);
	temp.append(insideAddCollect(sqlDbDepAddInsArr,qDbDepInsShift));
	
	std::string tempS = insideAddCollect(addSelEnumArr,sqlAddSelArr,qSelShift);
	tempS.append(insideAddCollect(sqlDbDepAddSelArr,qDbDepSelShift));
	
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
dbSqlBase::updateCollect() const
{
	char frame[] = "'";
	if (preventFraming)
		frame[0] = ' ';
		
	std::string setPart = fieldsValName(pre_fieldsVal.front(), pre_fieldsNames,frame);
	
	insideAddCollect(addUpEnumArr,sqlAddUpArr,qUpShift);
	temp.assign(insideAddCollect(sqlDbDepAddUpArr,qDbDepUpShift));

	temp.append(pre_table);

	request = "update ";
	request.append(temp);
	request.append(" set ");
	request.append(setPart);
}

//-------------------------------------------------------------------

void
dbSqlBase::delCollect() const
{
	temp = insideAddCollect(addDelEnumArr,sqlAddDelArr,qDelShift);
	temp.append(insideAddCollect(sqlDbDepAddDelArr,qDbDepDelShift));

	request = "delete ";
	request.append(temp);
	request.append("from ");
	request.append(pre_table);
}

//-------------------------------------------------------------------

void 
dbSqlBase::useCollect() const
{
	request = "use " + dbInfo.db;
}

//-------------------------------------------------------------------

void 
dbSqlBase::subCollect() const
{
	request = tools::implode(pre_subQ,sqlQStArr[qType].str);
}

//-------------------------------------------------------------------

void 
dbSqlBase::truncateCollect() const
{
	request = "truncate " + pre_table;
}

//-------------------------------------------------------------------

void 
dbSqlBase::delBaseCollect() const
{
	request = "drop database " + pre_order;
}

//-------------------------------------------------------------------

void 
dbSqlBase::delTableCollect() const
{
	request = "drop table " + pre_table;
}

//-------------------------------------------------------------------

void 
dbSqlBase::delFieldCollect() const
{
	request = "alter " + pre_order + " drop " + pre_table;
}
//-------------------------------------------------------------------

void 
dbSqlBase::renameBaseCollect() const
{
	request = __string__;
}

//-------------------------------------------------------------------

void 
dbSqlBase::renameTableCollect() const
{
	request = "alter table " + pre_table + " rename to " + pre_having;
}

//-------------------------------------------------------------------

void 
dbSqlBase::renameFieldCollect() const
{
	request = __string__;
}

//-------------------------------------------------------------------

void 
dbSqlBase::createBaseCollect() const
{
	request = "create database " + pre_order;
	if (pre_having.size() != 0)
		request.append(" character set " + pre_having);
}
//-------------------------------------------------------------------

void 
dbSqlBase::createTableCollect() const
{
	request = "create table "; 
	
	request.append(pre_tableInfo.ifNotExists?"if not exists ":__string__);
	
	request.append(pre_tableInfo.name + "(");
	
	{
		std::vector<__fieldInfo>::iterator i(pre_tableInfo.fields.begin()), j(pre_tableInfo.fields.end()-1);
		for (;i!=j;++i)
			request.append(fieldCollect(*i) + ",");
		request.append(fieldCollect(*i));
	}
	
	request.append(!pre_tableInfo.primKeys.empty()?(", primary key " + tools::implode(pre_tableInfo.primKeys,",")):__string__);
	request.append(!pre_tableInfo.keys.empty()?(", key " + tools::implode(pre_tableInfo.keys,",")):__string__);
	request.append(!pre_tableInfo.uniq.empty()?(", unique key " + tools::implode(pre_tableInfo.uniq,",")):__string__);
	request.append(pre_tableInfo.avgRowLen>0?(", avg_row_length = " + tools::lToString(pre_tableInfo.avgRowLen)):__string__);
	request.append(pre_tableInfo.autoIncr>0?(", auto_increment = " + tools::lToString(pre_tableInfo.autoIncr)):__string__);
	request.append(!pre_tableInfo.comment.empty()?(", comment = " + pre_tableInfo.comment):__string__);	
	request.append(!pre_tableInfo.charset.empty()?(", character set = " + pre_tableInfo.charset):__string__);	
	
	request.append(")");
}

//-------------------------------------------------------------------

void 
dbSqlBase::createFieldCollect() const
{
	request = "alter table " + pre_table + " add " + fieldCollect(pre_fieldInfo);
}

//-------------------------------------------------------------------

std::string
dbSqlBase::queryCollect() const
{	
	additionalActions = true;
	selectAction = false;
	
	switch (qType)
	{
		case DBREQUEST_SELECT:
			selectCollect();
			selectAction = true;
			break;
			
		case DBREQUEST_INSERT:
			insertCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_UPDATE:
			updateCollect();
			break;
			
		case DBREQUEST_DELETE:
			delCollect();
			break;
			
		case DBREQUEST_USE:
			useCollect();
			break;
			
		case DBREQUEST_INSERT_SELECT:
			insertSelectCollect();
			selectAction = true;
			break;
			
		case DBREQUEST_UNION:
		case DBREQUEST_UNION_ALL:
		case DBREQUEST_MINUS:
		case DBREQUEST_INTERSECT:
			subCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_TRUNCATE:
			truncateCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_RENAME_DB:
			renameBaseCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_RENAME_TABLE:
			renameTableCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_RENAME_FIELD:
			renameFieldCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_DELETE_DB:
			delBaseCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_DELETE_TABLE:
			delTableCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_DELETE_FIELD:
			delFieldCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_CREATE_DB:
			createBaseCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_CREATE_TABLE:
			createTableCollect();
			additionalActions = false;
			break;
			
		case DBREQUEST_CREATE_FIELD:
			createFieldCollect();
			additionalActions = false;
			break;
			
		default:
			additionalActions = false;
	}
	#ifndef FAST
		if (request.size()==0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBSQLBASE,DBSQLBASE_QUERYCOLLECT,ERR_LIBDODO,DBSQLBASE_EMPTY_REQUEST,DBSQLBASE_EMPTY_REQUEST_STR,__LINE__,__FILE__);	
			#else
				return __string__;
			#endif	
	#endif
	if (additionalActions)
	{
		additionalCollect(DBADDREQUEST_WHERE,pre_where);
		if (selectAction)
		{
			additionalCollect(DBADDREQUEST_GROUPBY,pre_group);
			additionalCollect(DBADDREQUEST_HAVING,pre_having);
		}
		additionalCollect(DBADDREQUEST_ORDERBY,pre_order);
		additionalCollect(DBADDREQUEST_LIMIT,pre_limNumber);
		additionalCollect(DBADDREQUEST_OFFSET,pre_limOffset);
	}
	
	return request;
}

//-------------------------------------------------------------------

inline std::string
dbSqlBase::escapeFields(const std::string &a_data)
{ 
	std::string temp(a_data);
	
	tools::replace("\\","\\\\",temp);
	tools::replace("'","\\'",temp);
	
	return temp;
}

//-------------------------------------------------------------------

std::string 
dbSqlBase::fieldCollect(__fieldInfo &row) const
{
	register int type = row.type, flag = row.flag;
	std::string resRow(row.name + " " + stringType(type));
	
	resRow.append(!row.set_enum.empty()?(" (" + tools::implode(row.set_enum,escapeFields,",") + ")"):__string__);
	resRow.append((chkRange(type)>0 && row.length>0)?(" ("+ tools::lToString(row.length) +") "):__string__);
	resRow.append((row.charset.size()>0)?(" character set " + row.charset):" ");
	resRow.append(((FIELDPROP_NULL&flag)==FIELDPROP_NULL)?" null ":" not null ");
	resRow.append((row.defaultVal.size()>0)?("default '" + row.defaultVal + "' "):__string__);
	resRow.append(((FIELDPROP_KEY&flag)==FIELDPROP_KEY)?" primary key ":"");
	resRow.append(((FIELDPROP_AUTO_INCREMENT&flag)==FIELDPROP_AUTO_INCREMENT)?auto_increment:__string__);
	resRow.append((row.comment.size()>0)?(" comment '" + row.comment + "' "):__string__);
	
	if (row.refTable.size()>0)
	{
		resRow.append(" references " + row.refTable);
		resRow.append(!row.refFields.empty()?("(" + tools::implode(row.set_enum,",") +" )"):__string__);
		resRow.append((row.onDelete>=0)?(" on delete " + stringReference(row.onDelete)):__string__);
		resRow.append((row.onUpdate>=0)?(" on update " + stringReference(row.onUpdate)):__string__);
	}
	
	return resRow;
}

//-------------------------------------------------------------------
