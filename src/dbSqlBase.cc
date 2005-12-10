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

using namespace dodo;
/**
 * arrays of positions of "statement" for complete realization. In sql wersion - see dbSqlBase;
 */	
static unsigned int addInsEnumArr[1] = 
{
	INSERT_IGNORE,
};

//-------------------------------------------------------------------

static unsigned int addUpEnumArr[1] = 
{
	UPDATE_IGNORE,
};

//-------------------------------------------------------------------

static unsigned int addDelEnumArr[1] = 
{
	DELETE_IGNORE,
};

//-------------------------------------------------------------------

static unsigned int addSelEnumArr[2] = 
{
	SELECT_DISTINCT,
	SELECT_ALL
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

dodoBase * const 
dbSqlBase::getSelf()
{
	return dynamic_cast<dodoBase *>(this);
}

//-------------------------------------------------------------------

dbSqlBase::dbSqlBase() : preventFraming(false)
{	
	request.reserve(100);
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
		temp.append(*i + "=" + frame + escapeFields(*j) + frame + ",");	
	temp.append(*i + "=" + frame + escapeFields(*j) + frame);
	
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
	if (qShift == EMPTY)
		return ;
		
	register int tempQTypeTocheck = 1<<qTypeTocheck;
	if ((tempQTypeTocheck & qShift)==tempQTypeTocheck)
		request.append(sqlAddArr[qTypeTocheck].str+collectedString);
}

//-------------------------------------------------------------------

std::string 
dbSqlBase::insideAddCollect(const unsigned int sqlAddEnumArr[],
						const __statements sqlAddArr[],
						int qTypeShift) const
{
	if (qTypeShift == EMPTY)
		return "";
		
	temp_.clear();
	
	register unsigned int arrLen = sizeof(sqlAddArr)/sizeof(char *);
	temp_bit = 0;
	
	for (register unsigned int i=0;i<=arrLen;++i)
	{
		temp_bit = 1<<sqlAddEnumArr[i];
		if ((temp_bit & qTypeShift)==temp_bit)
			temp_.append(sqlAddArr[sqlAddEnumArr[i]].str);
	}
	
	return temp_;
}

//-------------------------------------------------------------------

std::string 
dbSqlBase::insideAddCollect(const stringArr &statements, 
						int qTypeShift) const
{
	if (qTypeShift == EMPTY)
		return "";
		
	temp_.clear();

	register unsigned int k(0);
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
		t_request = new char[temp.size()+pre_table.size()+14];	
		if (t_request == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBSQLBASE,DBSQLBASE_SELECTCOLLECT,ERR_LIBDODO,DBSQLBASE_MEMORY_OVER,DBSQLBASE_MEMORY_OVER_STR,__LINE__,__FILE__);	
			#else
				return ;
			#endif		

		sprintf(t_request,"select %s from %s",temp.c_str(),pre_table.c_str());		
		
		request = t_request;
	
		delete [] t_request;
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
		fieldsPart.append("(" + *i + "),");
	fieldsPart.append("(" + *i + ")");
	
	temp = insideAddCollect(addInsEnumArr,sqlAddInsArr,qInsShift);
	temp.append(insideAddCollect(sqlDbDepAddInsArr,qDbDepInsShift));
	
	temp_ = pre_table;
	
	if (pre_fieldsNames.size() != 0)
		temp_.append(" ("+tools::implode(pre_fieldsNames,",")+") ");
	
	t_request = new char[temp.size()+temp_.size()+fieldsPart.size()+22];
	if (t_request == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_DBSQLBASE,DBSQLBASE_INSERTCOLLECT,ERR_LIBDODO,DBSQLBASE_MEMORY_OVER,DBSQLBASE_MEMORY_OVER_STR,__LINE__,__FILE__);	
		#else
			return ;
		#endif	
	
	sprintf(t_request,"insert %s into %s values %s",temp.c_str(),temp_.c_str(),fieldsPart.c_str());
	
	request = t_request;
	
	delete [] t_request;
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
		
	t_request = new char[temp.size()+pre_tableTo.size()+fieldsPartTo.size()+tempS.size()+pre_table.size()+35];
	if (t_request == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_DBSQLBASE,DBSQLBASE_INSERTSELECTCOLLECT,ERR_LIBDODO,DBSQLBASE_MEMORY_OVER,DBSQLBASE_MEMORY_OVER_STR,__LINE__,__FILE__);	
		#else
			;
		#endif	
	
	sprintf(t_request,"insert %s into %s (%s) select %s from %s",temp.c_str(),pre_tableTo.c_str(),fieldsPartTo.c_str(),tempS.c_str(),pre_table.c_str());
	
	request = t_request;
	
	delete [] t_request;	
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

	t_request = new char[temp.size()+setPart.size()+13];	
	if (t_request == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_DBSQLBASE,DBSQLBASE_UPDATECOLLECT,ERR_LIBDODO,DBSQLBASE_MEMORY_OVER,DBSQLBASE_MEMORY_OVER_STR,__LINE__,__FILE__);	
		#else
			return ;
		#endif
	
	sprintf(t_request,"update %s set %s",temp.c_str(),setPart.c_str());

	request = t_request;
	
	delete [] t_request;
}

//-------------------------------------------------------------------

void
dbSqlBase::delCollect() const
{
	temp = insideAddCollect(addDelEnumArr,sqlAddDelArr,qDelShift);
	temp.append(insideAddCollect(sqlDbDepAddDelArr,qDbDepDelShift));
	
	t_request = new char[pre_table.size()+temp.size()+14];
	if (t_request == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_DBSQLBASE,DBSQLBASE_DELCOLLECT,ERR_LIBDODO,DBSQLBASE_MEMORY_OVER,DBSQLBASE_MEMORY_OVER_STR,__LINE__,__FILE__);	
		#else
			return ;
		#endif
		
	sprintf(t_request,"delete %s from %s",temp.c_str(),pre_table.c_str());
	
	request = t_request;
	
	delete [] t_request;
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
	request = "";
}

//-------------------------------------------------------------------

void 
dbSqlBase::renameTableCollect() const
{
	request = "rename " + pre_table + " to " + pre_having;
}

//-------------------------------------------------------------------

void 
dbSqlBase::renameFieldCollect() const
{
	request = "";
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
	request = "create table " + pre_tableInfo.name + "(";
	{
		std::vector<__fieldInfo>::iterator i(pre_tableInfo.fields.begin()), j(pre_tableInfo.fields.end()-1);
		for (;i!=j;++i)
			request.append(fieldCollect(*i) + ",");
		request.append(fieldCollect(*i));
	}
	
	request.append(!pre_tableInfo.primKeys.empty()?(", primary key" + tools::implode(pre_tableInfo.primKeys,",")):"");
	request.append(!pre_tableInfo.keys.empty()?(", key " + tools::implode(pre_tableInfo.keys,",")):"");
	request.append(!pre_tableInfo.uniq.empty()?(", unique key " + tools::implode(pre_tableInfo.uniq,",")):"");
	request.append(pre_tableInfo.avgRowLen>0?(", avg_row_length = " + tools::lToString(pre_tableInfo.avgRowLen)):"");
	request.append(pre_tableInfo.autoIncr>0?(", auto_increment = " + tools::lToString(pre_tableInfo.autoIncr)):"");
	request.append(!pre_tableInfo.comment.empty()?(", comment = " + pre_tableInfo.comment):"");	
	request.append(!pre_tableInfo.charset.empty()?(", character set = " + pre_tableInfo.charset):"");	
	
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
		case SELECT:
			selectCollect();
			selectAction = true;
			break;
		case INSERT:
			insertCollect();
			additionalActions = false;
			break;
		case UPDATE:
			updateCollect();
			break;
		case DELETE:
			delCollect();
			break;
		case USE:
			useCollect();
			break;
		case INSERT_SELECT:
			insertSelectCollect();
			selectAction = true;
			break;
		case UNION:
		case UNION_ALL:
		case MINUS:
		case INTERSECT:
			subCollect();
			additionalActions = false;
			break;
		case TRUNCATE:
			truncateCollect();
			additionalActions = false;
			break;
		case RENAME_DB:
			renameBaseCollect();
			additionalActions = false;
			break;
		case RENAME_TABLE:
			renameTableCollect();
			additionalActions = false;
			break;
		case RENAME_FIELD:
			renameFieldCollect();
			additionalActions = false;
			break;
		case DELETE_DB:
			delBaseCollect();
			additionalActions = false;
			break;
		case DELETE_TABLE:
			delTableCollect();
			additionalActions = false;
			break;
		case DELETE_FIELD:
			delFieldCollect();
			additionalActions = false;
			break;
		case CREATE_DB:
			createBaseCollect();
			additionalActions = false;
			break;
		case CREATE_TABLE:
			createTableCollect();
			additionalActions = false;
			break;
		case CREATE_FIELD:
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
				return "";
			#endif	
	#endif
	if (additionalActions)
	{
		additionalCollect(WHERE,pre_where);
		if (selectAction)
		{
			additionalCollect(GROUPBY,pre_group);
			additionalCollect(HAVING,pre_having);
		}
		additionalCollect(ORDERBY,pre_order);
		additionalCollect(LIMIT,pre_limNumber);
		additionalCollect(OFFSET,pre_limOffset);
	}
	
	return request;
}

//-------------------------------------------------------------------

inline std::string 
dbSqlBase::escapeFields(const std::string &a_data)
{ 
	std::string temp(a_data);
	
	tools::replace("\\","\\\\",temp);
	tools::replace("\n","\\n",temp);
	tools::replace("'","\\'",temp);
	
	return temp;
}

//-------------------------------------------------------------------

inline std::string 
dbSqlBase::fieldCollect(__fieldInfo &row) const
{
	register int type = row.type, flag = row.flag;
	std::string resRow(row.name + stringType(type));
	
	resRow.append(!row.set_enum.empty()?(" (" + tools::implode(row.set_enum,escapeFields,",") + ")"):"");
	resRow.append((chkRange(type)>0 && row.length>0)?(" ("+ tools::lToString(row.length) +") "):"");
	resRow.append((row.charset.size()>0)?(" character set " + row.charset):" ");
	resRow.append(((_NULL&flag)==_NULL)?" NULL ":" NOT NULL ");
	resRow.append((row.defaultVal.size()>0)?("default '" + row.defaultVal + "' "):"");
	resRow.append(((AUTO_INCREMENT&flag)==AUTO_INCREMENT)?" AUTO_INCREMENT ":"");
	resRow.append(((KEY&flag)==KEY)?" KEY ":"");	
	resRow.append((row.comment.size()>0)?(" comment '" + row.comment + "' "):"");
	
	if (row.refTable.size()>0)
	{
		resRow.append(" references " + row.refTable);
		resRow.append(!row.refFields.empty()?("(" + tools::implode(row.set_enum,",") +" )"):"");
		resRow.append((row.onDelete>=0)?(" on delete " + stringReference(row.onDelete)):"");
		resRow.append((row.onUpdate>=0)?(" on update " + stringReference(row.onUpdate)):"");
	}
	
	return resRow;
}

//-------------------------------------------------------------------
