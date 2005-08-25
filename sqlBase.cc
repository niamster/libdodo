/***************************************************************************
 *            sqlBase.cc
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
 
#include "sqlBase.h"

using namespace dodo;

///////////////////////////////////
//	These statements need additional statement
///////////////////////////////////		
/**
* do not edit it please.
* sqlBase use them in such way, as they are
*/
static const __sqlStatements sqlQStArr[] = 
{
	{" union "},
	{" union all "},
	{" minus "},
	{" intersect "}
};
static 	__sqlStatements sqlAddArr[] = 
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
static 	__sqlStatements sqlAddInsArr[2] = 
{
	{""},
	{" ignore "},
};
static 	__sqlStatements sqlAddUpArr[2] = 
{
	{""},
	{" ignore "},
};
static 	__sqlStatements sqlAddDelArr[2] = 
{
	{""},
	{" ignore "},
};	
static 	__sqlStatements sqlAddSelArr[3] = 
{
	{""},
	{" distinct "},
	{" all "},
};

//-------------------------------------------------------------------

sqlBase::sqlBase()
{	
	request.reserve(100);
}

//-------------------------------------------------------------------

sqlBase *sqlBase::getSelf()
{
	return this;
}

//-------------------------------------------------------------------

sqlBase::~sqlBase()
{	
}

//-------------------------------------------------------------------

std::string
sqlBase::fieldsValName(const stringArr &fieldsVal, 
					const stringArr &fieldsNames)
{
	std::string temp;
	
	register unsigned int fn(fieldsNames.size()),fv(fieldsVal.size());
	
	register unsigned int o(fn<=fv?fn:fv);
	
	stringArr::const_iterator i(fieldsNames.begin()), k(fieldsVal.begin());

	for (register unsigned int j(0);j<o-1;++i,++k,++j)
		temp.append('`' + escapeFieldsNames(*i) + "`='" + escapeFields(*k) + "',");	
	temp.append('`' + escapeFields(*i) + "`='" + escapeFieldsNames(*k) + '\'');
	
	return temp;
}

//-------------------------------------------------------------------

std::string 
sqlBase::exists(std::string statement)
{
	return std::string("exists("+statement+')');
}

//-------------------------------------------------------------------

std::string 
sqlBase::noexists(std::string statement)
{
	return std::string("not exists("+statement+')');
}

//-------------------------------------------------------------------

void 
sqlBase::additionalCollect(unsigned int qTypeTocheck, 
						std::string collectedString) const
{
	if (qShift == EMPTY)
		return ;
	register int tempQTypeTocheck = 1<<qTypeTocheck;
	if ((tempQTypeTocheck & qShift)==tempQTypeTocheck)
		request.append(sqlAddArr[qTypeTocheck].str+collectedString);
}

//-------------------------------------------------------------------

std::string 
sqlBase::insideAddCollect(unsigned int sqlAddEnumArr[],
						__sqlStatements sqlAddArr[],
						int qTypeShift) const
{
	if (qTypeShift == EMPTY)
		return "";
	std::string temp;
	register unsigned int arrLen = sizeof(sqlAddArr)/sizeof(std::string);
	register unsigned int temp_bit(0);
	for (register unsigned int i=0;i<=arrLen;++i)
	{
		temp_bit = 1<<sqlAddEnumArr[i];
		if ((temp_bit & qTypeShift)==temp_bit)
			temp.append(sqlAddArr[sqlAddEnumArr[i]].str);
	}
	return temp;
}

//-------------------------------------------------------------------

std::string 
sqlBase::insideAddCollect(std::list<std::string> &statements, 
						int qTypeShift) const
{
	if (qTypeShift == EMPTY)
		return "";
	std::string temp;
	register unsigned int temp_bit(0), k(0);
	register std::list<std::string>::const_iterator i(statements.begin()),j(statements.end());
	for (;i!=j;++i,++k)
	{
		temp_bit = 1<<k;
		if ((temp_bit & qTypeShift)==temp_bit)
			temp.append(*i);
	}
	return temp;
	
}

//-------------------------------------------------------------------

void 
sqlBase::selectCollect() const 
{
	std::string temp = insideAddCollect(sqlAddSelEnumArr,sqlAddSelArr,qSelShift);
	temp.append(insideAddCollect(sqlDbDepAddSelArr,qDbDepSelShift));
	
	if (pre_table.size()>0)
	{
		temp.append(tools::implode(pre_fieldsNames,&dodo::sqlBase::escapeFieldsNames,",","`"));
		pchar t_request = new char[temp.size()+pre_table.size()+18];	
		if (t_request == 0)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASE_MEMORY_OVER, (sqlBase *)this,__LINE__,__FILE__);	
		#else
			;
		#endif		

		sprintf(t_request,"select %s from `%s`",temp.c_str(),pre_table.c_str());		
		
		request = t_request;
	
		delete [] t_request;
	}
	else
	{
		temp.append(tools::implode(pre_fieldsNames,&dodo::sqlBase::escapeFieldsNames,",","'"));
		request = "select ";
		request.append(temp);
	}
}

//-------------------------------------------------------------------

void 
sqlBase::insertCollect() const
{
	std::string fieldsPart = tools::implode(pre_fieldsVal,&dodo::sqlBase::escapeFields,",","'");
	
	std::string temp = insideAddCollect(sqlAddInsEnumArr,sqlAddInsArr,qInsShift);
	temp.append(insideAddCollect(sqlDbDepAddInsArr,qDbDepInsShift));
	
	std::string tempFNP('`'+pre_table+'`');
	
	if (pre_fieldsNames.size() != 0)
		tempFNP.append(" ("+tools::implode(pre_fieldsNames,&dodo::sqlBase::escapeFieldsNames,",","`")+") ");
	
	pchar t_request = new char[temp.size()+tempFNP.size()+fieldsPart.size()+24];
	if (t_request == 0)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASE_MEMORY_OVER, (sqlBase *)this,__LINE__,__FILE__);	
	#else
		;
	#endif	
	
	sprintf(t_request,"insert %s into %s values (%s)",temp.c_str(),tempFNP.c_str(),fieldsPart.c_str());
	
	request = t_request;
	
	delete [] t_request;
}

//-------------------------------------------------------------------

void 
sqlBase::insertSelectCollect() const
{
	
	std::string fieldsPartTo = tools::implode(pre_fieldsNames,&dodo::sqlBase::escapeFieldsNames,",","`");
	std::string fieldsPartFrom = tools::implode(pre_fieldsVal,&dodo::sqlBase::escapeFieldsNames,",","`");
	
	std::string tempI = insideAddCollect(sqlAddInsEnumArr,sqlAddInsArr,qInsShift);
	tempI.append(insideAddCollect(sqlDbDepAddInsArr,qDbDepInsShift));
	std::string tempS = insideAddCollect(sqlAddSelEnumArr,sqlAddSelArr,qSelShift);
	tempS.append(insideAddCollect(sqlDbDepAddSelArr,qDbDepSelShift));
	
	std::string tempFPT = tempS + fieldsPartFrom;
		
	pchar t_request = new char[tempI.size()+pre_tableTo.size()+fieldsPartTo.size()+tempFPT.size()+pre_table.size()+35];
	if (t_request == 0)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASE_MEMORY_OVER, (sqlBase *)this,__LINE__,__FILE__);	
	#else
		;
	#endif	
	
	sprintf(t_request,"insert %s into `%s` (%s) select %s from `%s`",tempI.c_str(),pre_tableTo.c_str(),fieldsPartTo.c_str(),tempFPT.c_str(),pre_table.c_str());
	
	request = t_request;
	
	delete [] t_request;	
}

//-------------------------------------------------------------------

void
sqlBase::updateCollect() const
{
	std::string setPart = fieldsValName(pre_fieldsVal, pre_fieldsNames);
	
	std::string temp = insideAddCollect(sqlAddUpEnumArr,sqlAddUpArr,qUpShift);
	temp.append(insideAddCollect(sqlDbDepAddUpArr,qDbDepUpShift));
	
	std::string tempFNP(" `"+pre_table+"` ");

	temp.append(tempFNP);

	pchar t_request = new char[temp.size()+setPart.size()+13];	
	if (t_request == 0)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASE_MEMORY_OVER, (sqlBase *)this,__LINE__,__FILE__);	
	#else
		;
	#endif
	
	sprintf(t_request,"update %s set %s",temp.c_str(),setPart.c_str());

	request = t_request;
	
	delete [] t_request;
}

//-------------------------------------------------------------------

void
sqlBase::delCollect() const
{
	std::string temp = insideAddCollect(sqlAddDelEnumArr,sqlAddDelArr,qDelShift);
	temp.append(insideAddCollect(sqlDbDepAddDelArr,qDbDepDelShift));
	
	pchar t_request = new char[pre_table.size()+temp.size()+16];
	if (t_request == 0)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASE_MEMORY_OVER, (sqlBase *)this,__LINE__,__FILE__);	
	#else
		;
	#endif
	sprintf(t_request,"delete %s from `%s`",temp.c_str(),pre_table.c_str());
	
	request = t_request;
	
	delete [] t_request;
}

//-------------------------------------------------------------------

void 
sqlBase::useCollect() const
{
	request = "use " + sqlInfo.db;
}

//-------------------------------------------------------------------

void 
sqlBase::subCollect() const
{
	request = tools::implode(pre_subQ,sqlQStArr[qType].str);
}

//-------------------------------------------------------------------

std::string
sqlBase::queryCollect() const
{	
	register bool additionalActions = true, select = false;
	switch (qType)
	{
		case SELECT:
			selectCollect();
			select = true;
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
			select = true;
			break;
		case UNION:
		case UNION_ALL:
		case MINUS:
		case INTERSECT:
			subCollect();
			additionalActions = false;
			break;
		default:
			additionalActions = false;
	}
	#ifndef FAST
		if (strlen(request.c_str())==0)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASE_EMPTY_REQUEST, (sqlBase *)this,__LINE__,__FILE__);	
		#else
			return "";
		#endif	
	#endif
	if (additionalActions)
	{
		additionalCollect(WHERE,pre_where);
		if (select)
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
sqlBase::escapeFields(const std::string &a_data)
{ 
	std::string temp(a_data);
	tools::replace("\\","\\\\",temp);
	tools::replace("\n","\\n",temp);
	tools::replace("\'","\\'",temp);
	return a_data;
}

//-------------------------------------------------------------------

inline std::string 
sqlBase::escapeFieldsNames(const std::string &a_data)
{ 
	std::string temp(a_data);
	tools::replace("`","``",temp);
	return temp;
}

//-------------------------------------------------------------------
