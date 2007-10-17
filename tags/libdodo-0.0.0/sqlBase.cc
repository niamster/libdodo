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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include "sqlBase.h"

using namespace dodo;

void dummyHook(sqlBase *base, void *data)
{
}

//-------------------------------------------------------------------

sqlBase::sqlBase() : numRows(0), 
					numFields(0), 
					show(false), 
					affectedRows(0),
					pre_limNumber(0),
					stored(false),

					requestLength(0),

					qShift(EMPTY),
					qSelShift(EMPTY),
					qInsShift(EMPTY),
					qUpShift(EMPTY),
					qDelShift(EMPTY),

					safeHooks(true),
					qType(-1),
					ready(false)
{
	myAlloc();
}

//-------------------------------------------------------------------

sqlBase::~sqlBase()
{	
	myFree();
}

//-------------------------------------------------------------------

ppchar
sqlBase::copy2(const char **array) const
{	
	if (array == NULL)
		return NULL;
	int i(0), num(1);
	while(array[++i] != NULL)
		num++;
	ppchar array_ = (ppchar)malloc((num+1)*size_of_pointer);
	if (array_ == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	for (i=0;i<num;i++)
		array_[i] = strdup(array[i]);
	array_[i] = (pchar)malloc(0);
	array_[i] = NULL;

	return array_;
}

//-------------------------------------------------------------------

pppchar
sqlBase::copy3(const char ***array) const
{
	if (array == NULL)
		return NULL;
	int i(0), num(1);
	while(array[++i] != NULL)
		num++;
	pppchar array_ = (pppchar)malloc((num+1)*size_of_pointer);
	if (array_ == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	for (i=0;i<num;i++)
		array_[i] = copy2(array[i]);
	array_[i] = (ppchar)malloc(0);
	if (array_[i] == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	array_[i] = NULL;
	return array_;
}

//-------------------------------------------------------------------

char **
sqlBase::strcpy2(char **dst_array, 
				const char **src_array) const
{
	if (src_array == NULL)
	{
		dst_array = NULL;
		return dst_array;	
	}
	int i(0), num(1);
	while(src_array[++i] != NULL)
		num++;
	dst_array = (ppchar)realloc(dst_array, (num+1)*size_of_pointer);
	if (dst_array == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	for (i=0;i<num;i++)
	{
		dst_array[i] = (pchar)malloc((strlen(src_array[i])+1)*size_of_char);
		if (dst_array[i] == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif	
		strcpy(dst_array[i],src_array[i]);
	}
	dst_array[i] = (pchar)malloc(0);
	if (dst_array[i] == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	dst_array[i] = NULL;
	
	return dst_array;	
}

//-------------------------------------------------------------------

char ***
sqlBase::strcpy3(char ***dst_array, 
				const char ***src_array) const
{
	if (src_array == NULL)
	{
		dst_array = NULL;
		return dst_array;	
	}
	int i(0), num(1);
	while(src_array[++i] != NULL)
		num++;
	dst_array = (pppchar)realloc(dst_array, (num+1)*size_of_pointer);
	if (dst_array == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	for (i=0;i<num;i++)
		strcpy2(dst_array[i],src_array[i]);
	dst_array[i] = (ppchar)malloc(0);
	if (dst_array[i] == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	dst_array[i] = NULL;

	return dst_array;	
}

//-------------------------------------------------------------------

void 
sqlBase::free2(char **array) const
{
	if (array == NULL)
		return;
	int i(0);
	while(array[++i] != NULL)
		free(array[i]);
	free(array[i]);
}

//-------------------------------------------------------------------

void
sqlBase::free3(char ***array) const
{
	if (array == NULL)
		return;
	int i(0);
	while(array[++i] != NULL)
	{
		free2(array[i]);
		free(array[i]);
	}
	free2(array[i]);
	free(array[i]);
}

//-------------------------------------------------------------------

char *
sqlBase::implode(const char **fields, 
				const char *separator,
				const char *frame) const
{		
	if (fields == NULL)
		return NULL;
	
	unsigned short frame_len(strlen(frame)), separator_len(strlen(separator));
	unsigned long allLen(0);
	int i(0);
	if (fields[1] != NULL)
		while(fields[i] != NULL)
			allLen += strlen(fields[i++]);
	else
		allLen += strlen(fields[i]);
    pchar fieldsPart = (pchar)malloc((allLen+(separator_len+frame_len*2)*i)*size_of_char);	
	if (fieldsPart == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	strcpy(fieldsPart,"");	
	pchar temp = (pchar)malloc(size_of_char);
	if (temp == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	
	i = 0;
	while(fields[i+1] != NULL)
	{
		temp = (pchar)malloc((strlen(fields[i])+frame_len*2+1)*size_of_char);
		if (temp == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif	
		sprintf(temp,"%s%s%s",frame,fields[i++],frame);
        strcat(fieldsPart,strcat(temp,separator));
	}
	temp = (pchar)malloc((strlen(fields[i])+frame_len*2+1)*size_of_char);
	if (temp == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	sprintf(temp,"%s%s%s",frame,fields[i],frame);	
	strcat(fieldsPart,temp);
	
	free(temp);
	
	return fieldsPart;
}

//-------------------------------------------------------------------

pchar 
sqlBase::fieldsValName(const char **fieldsVal, 
					const char **fieldsNames) const
{
	int i(0);
	unsigned long allLen(0);
	while(fieldsNames[i] != NULL)
		allLen += strlen(fieldsNames[i]) + strlen(fieldsVal[i++]) + 6;
	
	pchar setPart = (pchar)malloc((allLen+1)*size_of_char);
	if (setPart == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	strcpy(setPart,"");
	pchar temp = (pchar)malloc(0);
	
	i = 0;
	while(fieldsNames[i+1] != NULL)
	{
		temp = (pchar)realloc(temp,(strlen(fieldsNames[i]) + strlen(fieldsVal[i]) + 7)*size_of_char);
		if (temp == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif	
		sprintf(temp,"`%s`='%s',",fieldsNames[i],fieldsVal[i]);
		i++;
		strcat(setPart,temp);
	}
	temp = (pchar)realloc(temp,(strlen(fieldsNames[i]) + strlen(fieldsVal[i]) + 6)*size_of_char);
	if (temp == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	sprintf(temp,"`%s`='%s'",fieldsNames[i],fieldsVal[i]);
	strcat(setPart,temp);
	
	free(temp);		
	
	return setPart;
}

//-------------------------------------------------------------------

void
sqlBase::debug(__sqlStorage &result) const
{
	unsigned int i(0),j(0);
	printf("\nRequest:%s\n",request);
	printf("\n--Result--\nrows=%d\tfields=%d\n",result.numRows,result.numFields);
	for (;j<result.numFields;j++)
		printf("%s\t",result.fields[j]);
	for (;i<result.numRows;i++)
	{
		printf("\n");
		for (j=0;j<result.numFields;j++)
			printf("%s\t",result.rows[i][j]);
	}
	printf("\n");
}

//-------------------------------------------------------------------

void 
sqlBase::select(const char *a_table,
			    const char **a_fieldsNames,
			    const char *a_where) const 
{
	qType = SELECT;
	
	free2(pre_fieldsNames);
	free(pre_fieldsNames);	
	pre_fieldsNames = copy2(a_fieldsNames);
	
	free(pre_table);
	pre_table = strdup(a_table);
	if (pre_table == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	
	
	if (a_where != NULL)
	{
		addF(qShift,WHERE);
		
		free(pre_where);	
		pre_where = strdup(a_where);
		if (pre_where == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif	
	}
	
	ready = true;
}

//-------------------------------------------------------------------

void 
sqlBase::insert(const char *a_table,
			    const char **a_fieldsVal, 
			    const char **a_fieldsNames)
{
	qType = INSERT;
	
	free2(pre_fieldsNames);
	free(pre_fieldsNames);
	pre_fieldsNames = copy2(a_fieldsNames);
	
	free2(pre_fieldsVal);
	free(pre_fieldsVal);
	pre_fieldsVal = copy2(a_fieldsVal);
	
	free(pre_table);
	pre_table = strdup(a_table);
	if (pre_table == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif	

	ready = true;
}

//-------------------------------------------------------------------

void
sqlBase::update(const char *a_table,
			   const char **a_fieldsVal, 
			   const char **a_fieldsNames, 
			   const char *a_where)
{
	qType = UPDATE;
	
	free2(pre_fieldsNames);
	free(pre_fieldsNames);
	pre_fieldsNames = copy2(a_fieldsNames);
	
	free2(pre_fieldsVal);
	free(pre_fieldsVal);
	pre_fieldsVal = copy2(a_fieldsVal);
	
	free(pre_table);
	pre_table = strdup(a_table);
	if (pre_table == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	
	if (a_where != NULL)
	{
		addF(qShift,WHERE);
		
		free(pre_where);	
		pre_where = strdup(a_where);
		if (pre_where == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif	
	}
	
	ready = true;
}

//-------------------------------------------------------------------

void
sqlBase::del(const char *a_table, 
			const char *a_where)
{
	qType = DELETE;
	
	free(pre_table);
	pre_table = strdup(a_table);
	if (pre_table == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	
	if (a_where != NULL)
	{
		addF(qShift,WHERE);
		
		free(pre_where);	
		pre_where = strdup(a_where);
		if (pre_where == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif	
	}
	
	ready = true;
}

//-------------------------------------------------------------------

void 
sqlBase::limit(int a_number) const
{
	addF(qShift,LIMIT);
	pre_limNumber = a_number;
}

//-------------------------------------------------------------------

void 
sqlBase::order(char *order) const
{
	free(pre_order);
	pre_order = strdup(order);
	if (pre_order == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	addF(qShift,ORDERBY);
}

//-------------------------------------------------------------------

void 
sqlBase::group(char *group) const
{
	free(pre_group);
	pre_group = strdup(group);
	if (pre_group == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	addF(qShift,GROUPBY);
}

//-------------------------------------------------------------------

void 
sqlBase::having(char *having) const
{
	free(pre_having);
	pre_having = strdup(having);
	if (pre_having == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	addF(qShift,HAVING);
}

//-------------------------------------------------------------------

void 
sqlBase::setAddInsSt(unsigned short statement)
{
	switch (statement)
	{
		case INSERT_HIGH_PRIORITY:
		case INSERT_LOW_PRIORITY:
		case INSERT_DELAYED:
			removeF(qInsShift,INSERT_LOW_PRIORITY);
			removeF(qInsShift,INSERT_HIGH_PRIORITY);
			removeF(qInsShift,INSERT_DELAYED);
			break;
		default:
			break;
		
	}
	addF(qInsShift,statement);
}

//-------------------------------------------------------------------

void 
sqlBase::setAddUpSt(unsigned short statement)
{
	/*switch (statement)
	{
		default:
			break;
		
	}*/
	addF(qUpShift,statement);	
}

//-------------------------------------------------------------------

void 
sqlBase::setAddSelSt(unsigned short statement)
{
	switch (statement)
	{
		case SELECT_DISTINCT:
		case SELECT_ALL:
			removeF(qInsShift,SELECT_ALL);
			removeF(qInsShift,SELECT_DISTINCT);
			break;
		default:
			break;
		
	}
	addF(qSelShift,statement);	
}

//-------------------------------------------------------------------

void 
sqlBase::setAddDelSt(unsigned short statement)
{
	/*switch (statement)
	{
		default:
			break;
		
	}*/
	addF(qDelShift,statement);
}

//-------------------------------------------------------------------

void 
sqlBase::unsetAddInsSt(unsigned short statement)
{
	removeF(qInsShift,statement);
}

//-------------------------------------------------------------------

void 
sqlBase::unsetAddUpSt(unsigned short statement)
{
	removeF(qUpShift,statement);	
}

//-------------------------------------------------------------------

void 
sqlBase::unsetAddSelSt(unsigned short statement)
{
	removeF(qSelShift,statement);	
}

//-------------------------------------------------------------------

void 
sqlBase::unsetAddDelSt(unsigned short statement)
{
	removeF(qDelShift,statement);	
}

//-------------------------------------------------------------------

void 
sqlBase::cleanCollect() const
{
	qType = -1;
	
	qShift = EMPTY;
	
	qSelShift = EMPTY;			
	qInsShift = EMPTY;
	qUpShift = EMPTY;
	qDelShift = EMPTY;
}

//-------------------------------------------------------------------

void 
sqlBase::additionalCollect(unsigned long qTypeTocheck, 
						pchar collectedString) const
{
	if (qTypeTocheck == EMPTY)
		return ;
	int tempQTypeTocheck = TOBIT(qTypeTocheck);
	if ((tempQTypeTocheck & TOBIT(qShift))==tempQTypeTocheck)
	{
		unsigned long _len = strlen(collectedString)+sqlAddArr[qTypeTocheck].len;
		pchar temp = (pchar)malloc((_len+1)*size_of_char);
		if (temp == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif	
		sprintf(temp,sqlAddArr[qTypeTocheck].str,collectedString);
		
		requestLength += _len;
		request = (pchar)realloc(request,(requestLength+1)*size_of_char);	
		if (request == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif		
		strcat(request,temp);
		
		free(temp);
	}
}
//-------------------------------------------------------------------

pchar 
sqlBase::insideAddCollect(unsigned short sqlAddEnumArr[],
						__sqlStatements sqlAddArr[],
						unsigned short arrLen,
						short qTypeShift) const
{
	pchar temp = (pchar)malloc(size_of_char);
	strcpy(temp,"");
	if (qTypeShift == EMPTY)
		return temp;
	for (unsigned short i=0;i<arrLen;i++)
		if ((TOBIT(sqlAddEnumArr[i]) & TOBIT(qTypeShift))==TOBIT(sqlAddEnumArr[i]))
		{
			temp = (pchar)realloc(temp,(sqlAddArr[sqlAddEnumArr[i]].len+1)*size_of_char);
			strcat(temp,sqlAddArr[sqlAddEnumArr[i]].str);
		}	
	return temp;
}

//-------------------------------------------------------------------

void 
sqlBase::selectCollect() const 
{
	pchar temp = insideAddCollect(sqlAddSelEnumArr,sqlAddSelArr,sqlAddSelArrLen,qSelShift);
	
    pchar fieldsPart = implode((const char**)pre_fieldsNames,",","`");
	
	temp = (pchar)realloc(temp,(strlen(temp)+strlen(fieldsPart)+1)*size_of_char);
	if (temp == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif

	strcat(temp,fieldsPart);
	free(fieldsPart);
		
	requestLength = sqlQStArr[SELECT].len+strlen(temp)+strlen(pre_table);
	request = (pchar)realloc(request,(requestLength+1)*size_of_char);	
	if (request == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	sprintf(request,sqlQStArr[SELECT].str,temp,pre_table);

	show = true;
	free(temp);
}

//-------------------------------------------------------------------

void 
sqlBase::insertCollect() const
{
	pchar fieldsPart = implode((const char**)pre_fieldsVal,",","'");

	pchar temp = insideAddCollect(sqlAddInsEnumArr,sqlAddInsArr,sqlAddInsArrLen,qInsShift);
	
	pchar tempFNP = (pchar)malloc((strlen(pre_table)+3)*size_of_char);
	if (tempFNP == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif

	sprintf(tempFNP,"`%s`",pre_table);
	
	if (pre_fieldsNames != NULL)
	{
		pchar fieldsNamesPart = implode((const char**)pre_fieldsNames,",","`");
		
		pchar tempFieldsNamesPart = (pchar)malloc((strlen(fieldsNamesPart)+5)*size_of_char);
		if (tempFieldsNamesPart == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif

		sprintf(tempFieldsNamesPart," (%s) ",fieldsNamesPart);
		free(fieldsNamesPart);
		
		tempFNP = (pchar)realloc(tempFNP,(strlen(tempFieldsNamesPart)+strlen(tempFNP)+1)*size_of_char);
		if (tempFNP  == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif

		strcat(tempFNP,tempFieldsNamesPart);
		
		free(tempFieldsNamesPart);
	}
	
	requestLength = sqlQStArr[INSERT].len+strlen(temp)+strlen(tempFNP)+strlen(fieldsPart);
	request = (pchar)realloc(request,(requestLength+1)*size_of_char);
	
	sprintf(request,sqlQStArr[INSERT].str,temp,tempFNP,fieldsPart);
	
	show = false;
	
	free(temp);
	free(tempFNP);
	free(fieldsPart);
}

//-------------------------------------------------------------------

void
sqlBase::updateCollect() const
{
	pchar setPart = fieldsValName((const char**)pre_fieldsVal, (const char**)pre_fieldsNames);
	
	pchar temp = insideAddCollect(sqlAddUpEnumArr,sqlAddUpArr,sqlAddUpArrLen,qUpShift);
	
	pchar tempFNP = (pchar)malloc((strlen(pre_table)+5)*size_of_char);
	if (tempFNP == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif

	sprintf(tempFNP," `%s` ",pre_table);

	temp = (pchar)realloc(temp,strlen(tempFNP)+strlen(temp)+1);
	strcat(temp,tempFNP);
	free(tempFNP);
	
	requestLength = strlen(temp)+strlen(setPart)+sqlQStArr[UPDATE].len;
	request = (pchar)realloc(request,(requestLength+1)*size_of_char);	
	if (request == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	
	sprintf(request,sqlQStArr[UPDATE].str,temp,setPart);
	
	free(temp);
	free(setPart);
	
	show = false;	
}

//-------------------------------------------------------------------

void
sqlBase::delCollect() const
{
	pchar temp = insideAddCollect(sqlAddDelEnumArr,sqlAddDelArr,sqlAddDelArrLen,qDelShift);
	
	requestLength = sqlQStArr[DELETE].len+strlen(pre_table)+strlen(temp);
		
	request = (pchar)realloc(request,(requestLength+1)*size_of_char);
	if (request == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	sprintf(request,sqlQStArr[DELETE].str,temp,pre_table);
	show = false;	
	
	free(temp);
}

//-------------------------------------------------------------------

void 
sqlBase::limitCollect() const
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%d",pre_limNumber);
	additionalCollect(LIMIT,temp);
}

//-------------------------------------------------------------------

void 
sqlBase::queryCollect() const
{
	bool additionalActions = true, select = false;
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
		default:
			additionalActions = false;
	}
	if (additionalActions)
	{
		additionalCollect(WHERE,pre_where);
		if (select)
		{
			additionalCollect(GROUPBY,pre_group);
			additionalCollect(HAVING,pre_having);
		}
		additionalCollect(ORDERBY,pre_order);
		limitCollect();		
	}
}

//-------------------------------------------------------------------

void 
sqlBase::setSqlInfo(const char *db,
	const char *host,
	int port, 
	const char *user, 
	const char *password,
	const char *socket) const
{
	sqlInfo.port = port;
	if (db == NULL)
		sqlInfo.db = NULL;
	else
	{
		sqlInfo.db = (pchar)realloc(sqlInfo.db,size_of_char*(strlen(db)+1));
		if (sqlInfo.db == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif
		strcpy(sqlInfo.db,db);
	}
	
	if (host == NULL)
		sqlInfo.host = NULL;
	else
	{
		sqlInfo.host = (pchar)realloc(sqlInfo.host,size_of_char*(strlen(host)+1));
		if (sqlInfo.host  == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif
		strcpy(sqlInfo.host,host);
	}	
	if (user == NULL)
		sqlInfo.user = NULL;
	else
	{
		sqlInfo.user = (pchar)realloc(sqlInfo.user,size_of_char*(strlen(user)+1));
		if (sqlInfo.user == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif
		strcpy(sqlInfo.user,user);
	}	
	if (password == NULL)
		sqlInfo.password = NULL;
	else
	{
		sqlInfo.password = (pchar)realloc(sqlInfo.password,size_of_char*(strlen(password)+1));
		if (sqlInfo.password == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif
		strcpy(sqlInfo.password,password);
	}	
	if (socket == NULL)
		sqlInfo.socket = NULL;
	else
	{
		sqlInfo.socket = (pchar)realloc(sqlInfo.socket,size_of_char*(strlen(socket)+1));
		if (sqlInfo.socket == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif		
		strcpy(sqlInfo.socket,socket);
	}
}

//-------------------------------------------------------------------

void
sqlBase::freeSqlInfo()
{
	free(sqlInfo.db);
	free(sqlInfo.host);
	free(sqlInfo.user);
	free(sqlInfo.password);	
	free(sqlInfo.socket);	
}

//-------------------------------------------------------------------

void
sqlBase::allocateSqlInfo()
{
	sqlInfo.db = (pchar)malloc(0);
	if (sqlInfo.db == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif	
	sqlInfo.host = (pchar)malloc(0);
	if (sqlInfo.host == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif	
	sqlInfo.user = (pchar)malloc(0);
	if (sqlInfo.user == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif	
	sqlInfo.password = (pchar)malloc(0);
	if (sqlInfo.password == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif		
	sqlInfo.socket = (pchar)malloc(0);
	if (sqlInfo.socket == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif	
}

//-------------------------------------------------------------------

void 
sqlBase::myFree()
{
	free2(pre_fieldsNames);
	free(pre_fieldsNames);
	free2(pre_fieldsVal);
	free(pre_fieldsVal);
	free(pre_where);
	free(pre_table);	
	free(pre_order);
	free(pre_group);	
	free(pre_having);
	
	free(request);
	free(err);

	free3(rows);
	free2(fields);
	free(rows);
	free(fields);

	freeXExec(preExec);
	freeXExec(postExec);	
	
	freeBackupedData();
}

//-------------------------------------------------------------------

void 
sqlBase::myAlloc()
{
	request = (pchar)malloc(size_of_char);
	if (request == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	strcpy(request,"");
	
	pre_table = (pchar)malloc(size_of_char);
	if (pre_table == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	strcpy(pre_table,"");

	pre_group = (pchar)malloc(size_of_char);
	if (pre_group == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	strcpy(pre_group,"");
	
	pre_having = (pchar)malloc(size_of_char);
	if (pre_having == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	strcpy(pre_having,"");	
	
	pre_order = (pchar)malloc(size_of_char);
	if (pre_order == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	strcpy(pre_order,"");
	
	pre_where = (pchar)malloc(size_of_char);
	if (pre_where == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif	
	strcpy(pre_where,"");
	
	pre_fieldsNames = (ppchar)malloc(0);
	if (pre_fieldsNames == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	pre_fieldsNames = NULL;

	pre_fieldsVal = (ppchar)malloc(0);
	if (pre_fieldsVal == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	pre_fieldsVal = NULL;
	
	err = (pchar)malloc(0);
	if (err == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif 
	err = NULL;
	
	fields = (ppchar)malloc(0);
	if (fields == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	fields = NULL;
	
	rows = (pppchar)malloc(0);
	if (rows == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	rows = NULL;
		
	initXExec(preExec);
	initXExec(postExec);
	
	allocateSqlInfo();
}

//-------------------------------------------------------------------

void 
sqlBase::initXExec(__execList &list)
{
	list.item = (__execItem *)malloc(0);
	if (list.item == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, this);	
	#else
		;
	#endif
	list.item = NULL;
	
	list.number = 0;
}

//-------------------------------------------------------------------

void 
sqlBase::freeXExec(__execList &list)
{
	free(list.item);
}

//-------------------------------------------------------------------

__collectedData *
sqlBase::getCollectedData()
{
	__collectedData *data = (__collectedData *)malloc(sizeof(__collectedData));
	if (data == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		return NULL;
	#endif	
	data->pre_where = pre_where;
	data->pre_fieldsNames = pre_fieldsNames;
	data->pre_fieldsVal = pre_fieldsVal;
	data->pre_table = pre_table;
	data->pre_order = pre_order;
	data->pre_group = pre_group;
	data->pre_having = pre_having;
	data->pre_limNumber = &pre_limNumber;
	
	data->fields = fields;	
	data->rows = rows;
	
	data->qSelShift = &qSelShift;	
	data->qInsShift = &qInsShift;	
	data->qUpShift = &qUpShift;	
	data->qDelShift = &qDelShift;		
	
	
	return data;
}

//-------------------------------------------------------------------

void 
sqlBase::freeBackupedData() const
{
	if (stored)
	{
		free(backup.pre_where);
		
		free2(backup.pre_fieldsNames);
		free(backup.pre_fieldsNames);
		
		free2(backup.pre_fieldsVal);
		free(backup.pre_fieldsVal);
		
		free(backup.pre_table);
		free(backup.pre_order);		
		free(backup.pre_having);
		free(backup.pre_group);
		free(backup.pre_limNumber);
		
		free2(backup.fields);
		free(backup.fields);
		
		free3(backup.rows);
		free(backup.rows);
		
		free(backup.qType);
		
		free(backup.qShift);
		
		free(backup.qSelShift);
		free(backup.qInsShift);
		free(backup.qUpShift);
		free(backup.qDelShift);	
	}
}

//-------------------------------------------------------------------

void 
sqlBase::storeCollected() const
{
	freeBackupedData();
	
	stored = true;
	backup.pre_where = strdup(pre_where);
	backup.pre_fieldsNames = copy2((const char **)pre_fieldsNames);
	backup.pre_fieldsVal = copy2((const char **)pre_fieldsVal);
	backup.pre_table = strdup(pre_table);
	backup.pre_order = strdup(pre_order);
	backup.pre_group = strdup(pre_group);
	backup.pre_having = strdup(pre_having);
	
	backup.pre_limNumber = (unsigned long *)malloc(sizeof(unsigned long));
	if (backup.pre_limNumber == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	*backup.pre_limNumber = pre_limNumber;
	
	backup.fields = copy2((const char **)fields);	
	backup.rows = copy3((const char ***)rows);

	backup.qType = (short *)malloc(sizeof(short));
	if (backup.qType == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	*backup.qType = qType;	
	
	backup.qShift = (short *)malloc(sizeof(short));
	if (backup.qShift == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	*backup.qShift = qShift;
	
	backup.qSelShift = (short *)malloc(sizeof(short));
	if (backup.qSelShift == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	*backup.qSelShift = qSelShift;	
	
	backup.qInsShift = (short *)malloc(sizeof(short));
	if (backup.qInsShift == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	*backup.qInsShift = qInsShift;	
	
	backup.qUpShift = (short *)malloc(sizeof(short));
	if (backup.qUpShift == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	*backup.qUpShift = qUpShift;	
	
	backup.qDelShift = (short *)malloc(sizeof(short));
	if (backup.qDelShift == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		;
	#endif
	*backup.qDelShift = qDelShift;	
}

//-------------------------------------------------------------------

void 
sqlBase::restoreCollected() const
{
	if (stored)
	{
		pre_group = (pchar)realloc(pre_group,size_of_char*(strlen(backup.pre_group)+1));
		if (pre_group == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif
		strcpy(pre_group,backup.pre_group);
		
		pre_having = (pchar)realloc(pre_having,size_of_char*(strlen(backup.pre_having)+1));
		if (pre_having == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif
		strcpy(pre_having,backup.pre_having);
		
		pre_where = (pchar)realloc(pre_where,size_of_char*(strlen(backup.pre_where)+1));
		if (pre_where == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif
		strcpy(pre_where,backup.pre_where);
		
		pre_table = (pchar)realloc(pre_table,size_of_char*(strlen(backup.pre_table)+1));
		if (pre_table == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif
		strcpy(pre_table,backup.pre_table);
		
		pre_order = (pchar)realloc(pre_order,size_of_char*(strlen(backup.pre_order)+1));
		if (pre_order == NULL)
		#ifndef NO_EX
			throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
		#else
			;
		#endif
		strcpy(pre_order,backup.pre_order);
		
		free2(pre_fieldsNames);
		strcpy2(pre_fieldsNames,(const char **)backup.pre_fieldsNames);
		free2(pre_fieldsVal);
		strcpy2(pre_fieldsVal,(const char **)backup.pre_fieldsVal);
		
		pre_limNumber = *backup.pre_limNumber;	
		
		free2(fields);
		strcpy2(fields,(const char **)backup.fields);		
		free3(rows);
		strcpy3(rows,(const char ***)backup.rows);		
		
		qShift = *backup.qShift;
		
		qType = *backup.qType;
		
		qSelShift = *backup.qSelShift;
		qInsShift = *backup.qInsShift;
		qUpShift = *backup.qUpShift;
		qDelShift = *backup.qDelShift;	
		
		freeBackupedData();
		stored = false;
	}
}

//-------------------------------------------------------------------

int 
sqlBase::addXExec(__execList &list, 
		inExec func, 
		void *data, 
		bool restore) const
{
	list.number++;
	
	list.item = (__execItem *)realloc(list.item,sizeof(__execItem)*list.number);
	if (list.item == NULL)
	#ifndef NO_EX
		throw sqlBaseEx(SQLBASEEXR_MEMORY_OVER, (sqlBase *)this);	
	#else
		return -1;
	#endif

	list.item[list.number-1].data = data;
	list.item[list.number-1].func = func;
	list.item[list.number-1].present = true;
	list.item[list.number-1].enabled = true;
	list.item[list.number-1].restore = restore;
	return list.number;	
}

//-------------------------------------------------------------------

void 
sqlBase::setStatXtExec(__execList &list, 
		unsigned short position,
		bool stat) const
{
	position--;
	if (position <= list.number)
		list.item[position].enabled = stat;	
}

//-------------------------------------------------------------------

void 
sqlBase::delXtExec(__execList &list, 
		unsigned short position) const
{
	position--;
	if (position <= list.number)
	{
		list.item[position].func = dummyHook;
		list.item[position].data = NULL;
		list.item[position].present = false;
	}
}

//-------------------------------------------------------------------

int 
sqlBase::addPreExec(inExec func, 
				void *data, 
				bool restore) const
{
	return addXExec(preExec,func,data,restore);
}

//-------------------------------------------------------------------

void 
sqlBase::disablePreExec(unsigned short position) const
{
	setStatXtExec(preExec,position,false);
}

//-------------------------------------------------------------------

void 
sqlBase::delPreExec(unsigned short position) const
{
	delXtExec(preExec,position);
}

//-------------------------------------------------------------------

void 
sqlBase::enablePreExec(unsigned short position) const
{
	setStatXtExec(preExec,position,true);
}

//-------------------------------------------------------------------

int 
sqlBase::addPostExec(inExec func, 
				void *data, 
				bool restore) const
{
	addXExec(postExec,func,data,restore);
}

//-------------------------------------------------------------------

void 
sqlBase::disablePostExec(unsigned short position) const
{
	setStatXtExec(postExec,position,false);
}

//-------------------------------------------------------------------

void 
sqlBase::delPostExec(unsigned short position) const
{
	delXtExec(postExec,position);
}

//-------------------------------------------------------------------

void 
sqlBase::enablePostExec(unsigned short position) const
{
	setStatXtExec(postExec,position,true);
}

//-------------------------------------------------------------------

void 
sqlBase::setStatAllXtExec(__execList &list,
				bool stat) const
{
	for (unsigned short i=0;i<list.number;i++)
		list.item[i].enabled = stat;
}

//-------------------------------------------------------------------

void 
sqlBase::enableAllPreExec() const
{
	setStatAllXtExec(preExec,true);
}

//-------------------------------------------------------------------

void
sqlBase::enableAllPostExec() const
{
	setStatAllXtExec(postExec,true);
}

//-------------------------------------------------------------------

void 
sqlBase::disableAllPreExec() const
{
	setStatAllXtExec(preExec,false);	
}

//-------------------------------------------------------------------

void 
sqlBase::disableAllPostExec() const
{
	setStatAllXtExec(postExec,false);
}

//-------------------------------------------------------------------

void 
sqlBase::performXExec(__execList &list) const
{
	for (unsigned short i=0;i<list.number;i++)
		if (list.item[i].present && list.item[i].enabled)
		{
			if (list.item[i].restore)
				storeCollected();
			if (safeHooks)
			{
					disableAllPostExec();
					disableAllPreExec();
			}
			list.item[i].func((sqlBase *)this,list.item[i].data);
			if (list.item[i].restore)
				restoreCollected();
			if (safeHooks)
			{
				enableAllPostExec();
				enableAllPreExec();
			}
		}
}

//-------------------------------------------------------------------

char *
sqlBase::getErr() const
{
	return err;
}

//-------------------------------------------------------------------

void 
sqlBase::freeSqlStorage(__sqlStorage *storage) const
{
	free3(storage->rows);
	free(storage->rows);
	free2(storage->fields);
	free(storage->fields);
	free(storage);
}

//-------------------------------------------------------------------

void 
sqlBase::setSafeHooks(bool state) const
{
	safeHooks = state;
}

//-------------------------------------------------------------------

short 
sqlBase::getQType() const
{
	return qType;
}

//-------------------------------------------------------------------

void 
sqlBase::addF(short &flag, 
			unsigned short statement) const
{
	flag |= statement;
}

//-------------------------------------------------------------------

void 
sqlBase::removeF(short &flag, 
			unsigned short statement) const
{
	flag &= ~statement-1;
}

//-------------------------------------------------------------------
