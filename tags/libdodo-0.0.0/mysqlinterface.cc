/***************************************************************************
 *            mysqlinterface.cc
 *
 *  Thu Apr  30 13:45:19 2005
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


#include "mysqlinterface.h"

using namespace dodo;

mysqlpp::mysqlpp() : empty(true),
					connected(false) 
{
	mysql = mysql_init(NULL);
}

mysqlpp::mysqlpp(mysqlpp *a_mypp) :	empty(true),
									connected(false)
{
	mysql = mysql_init(NULL);
	
	setSqlInfo(a_mypp->sqlInfo.db,
	a_mypp->sqlInfo.host,
	a_mypp->sqlInfo.port,
	a_mypp->sqlInfo.user,
	a_mypp->sqlInfo.password,
	a_mypp->sqlInfo.socket);
}

mysqlpp::~mysqlpp()
{
	if (!empty)
		mysql_free_result(mysqlRes);
	disconnect();
}

#ifndef NO_EX
	void
#else
	bool
#endif 
mysqlpp::connect()
{
	if (connected)
		disconnect();
	if (!mysql_real_connect(mysql,
		sqlInfo.host,
		sqlInfo.user,
		sqlInfo.password,
		sqlInfo.db,
		sqlInfo.port,
		sqlInfo.socket,
		CLIENT_MULTI_RESULTS))
	{
		connected = false;
		pchar mysqlE = (pchar)mysql_error(mysql);
		char temp[SIZEOFNUM];
		sprintf(temp,"%d",mysql_errno(mysql));
		err = (pchar)realloc(err,(strlen(mysqlE) + strlen(temp) + mysqlErrStArr[CONN].len+1)*size_of_char);
		if (err == NULL)
		#ifndef NO_EX
			throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER,this);
		#else
			return false;
		#endif
		sprintf(err,mysqlErrStArr[CONN].str, temp, mysqlE);
		#ifndef NO_EX
			throw mysqlppEx(MYSQL_CONNECT,this);
		#else
			return false;
		#endif
	}
	connected = true;
	#ifdef NO_EX
		return true;
	#endif
}

void 
mysqlpp::disconnect()
{
	if (connected)
	{
     	mysql_close(mysql);
	 	connected = false;
	}
}

bool 
mysqlpp::exec() const
{
	if (!connected)
		return false;
	
	free3(rows);
	rows = (pppchar)realloc(rows,0);
	rows = NULL;
	
	free2(fields);
	fields = (ppchar)realloc(fields,0);
	fields = NULL;	
	
	numRows = 0;
	numFields = 0;
	affectedRows = 0;
	
	performXExec(preExec);
	
	if (!ready)
		return false;	
	
	queryCollect();
	
	if (ready)
		if (mysql_query(mysql,request) != 0)
		{
			pchar mysqlE = (pchar)mysql_error(mysql);
			char temp[SIZEOFNUM];
			sprintf(temp,"%d",mysql_errno(mysql));
			err = (pchar)realloc(err,(strlen(temp)+strlen(mysqlE)+mysqlErrStArr[QUERY].len+1)*size_of_char);
			if (err == NULL)
			#ifndef NO_EX
				throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER,(mysqlpp *)this);
			#else
				return false;
			#endif
			sprintf(err,mysqlErrStArr[QUERY].str, temp, mysqlE);
			#ifndef NO_EX
				throw mysqlppEx(MYSQL_QUERY,(mysqlpp *)this);
			#else
				return false;
			#endif
		}
	
	performXExec(postExec);

	cleanCollect();
		
	if (!ready)
		return false;
	
	ready = false;
	
	if (!show)
		return true;
	
	if (!empty)
		mysql_free_result(mysqlRes);
	empty = false;
	
	mysqlRes = mysql_store_result(mysql);
	if (mysqlRes == NULL)
	{
		pchar mysqlE = (pchar)mysql_error(mysql);
		char temp[SIZEOFNUM];
		sprintf(temp,"%d",mysql_errno(mysql));
		err = (pchar)realloc(err,(strlen(temp)+strlen(mysqlE)+mysqlErrStArr[STORE_RES].len+1)*size_of_char);
		if (err == NULL)
		#ifndef NO_EX
			throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER,(mysqlpp *)this);
		#else
			return false;
		#endif		
		sprintf(err,mysqlErrStArr[STORE_RES].str, temp, mysqlE);
		#ifndef NO_EX
			throw mysqlppEx(MYSQL_STORE,(mysqlpp *)this);
		#else
			return false;
		#endif
	}
	return true;
}

char ***
mysqlpp::fetchRow()
{	
	numRows = mysql_num_rows(mysqlRes);
	numFields = mysql_num_fields(mysqlRes);	
	if (numRows == 0)
		return NULL;
	if (rows != NULL)
		return copy3((const char***)rows);
	if (mysqlRes == NULL)
		return NULL;
	unsigned long i(0),j(0);
	pppchar result = (pppchar)malloc((numRows+1)*size_of_pointer);
	if (result == NULL)
	#ifndef NO_EX
		throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER, this);	
	#else
		return NULL;
	#endif
	
	unsigned long *length, length2;
	while ((mysqlRow = mysql_fetch_row(mysqlRes)) != NULL)
	{		
		result[i] = (ppchar)malloc((numFields+1)*size_of_pointer);
		if (result[i] == NULL)
		#ifndef NO_EX
			throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER, this);	
		#else
			return NULL;
		#endif
		if (mysqlRow[j])
				length = mysql_fetch_lengths(mysqlRes);
		for (j=0;j<numFields;j++)
		{
			length2 = mysqlRow[j]?length[j]:4;
			result[i][j] = (pchar)malloc((length2+1)*size_of_char);	
			if (result[i][j] == NULL)
			#ifndef NO_EX
				throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER, this);	
			#else
				return NULL;
			#endif
			memcpy(result[i][j],mysqlRow[j]?mysqlRow[j]:"NULL",length2);
			result[i][j][length2] = '\0';
		}
		result[i][j] = (pchar)malloc(0);
		if (result[i][j] == NULL)
		#ifndef NO_EX
			throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER, this);	
		#else
			return NULL;
		#endif		
		result[i][j] = NULL;
		i++;
	}
	result[i] = (ppchar)malloc(0);
	if (result[i] == NULL)
	#ifndef NO_EX
		throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER, this);	
	#else
		return NULL;
	#endif
	result[i] = NULL;
	free3(rows);
	free(rows);
	rows = copy3((const char***)result);
	return result;
}

char **
mysqlpp::fetchField()
{	
	numFields = mysql_num_fields(mysqlRes);	
	if (numFields == 0)
		return NULL;
	if (fields != NULL)
		return copy2((const char**)fields);
	if (mysqlRes == NULL)
		return NULL;	
	unsigned long i(0);
	ppchar result = (ppchar)malloc((numFields+1)*size_of_pointer);
	if (result == NULL)
	#ifndef NO_EX
		throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER, this);	
	#else
		return NULL;
	#endif
	mysqlFields = mysql_fetch_fields(mysqlRes);
	for (;i<numFields;i++)
	{
		result[i] = strdup(mysqlFields[i].name);
		if (result[i] == NULL)
		#ifndef NO_EX
			throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER, this);	
		#else
			return NULL;
		#endif		
	}
	result[i] = (pchar)malloc(0);
	if (result[i] == NULL)
	#ifndef NO_EX
		throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER, this);	
	#else
		return NULL;
	#endif	
	result[i] = NULL;
	free2(fields);
	free(fields);
	fields = copy2((const char**)result);
	return result;
}

__sqlStorage *
mysqlpp::fetch()
{
	__sqlStorage *result = (__sqlStorage *)malloc(sizeof(__sqlStorage));
	if (result == NULL)
	#ifndef NO_EX
		throw mysqlppEx(MYSQLPPEXR_MEMORY_OVER, this);	
	#else
		return NULL;
	#endif		
	result->rows = fetchRow();
	result->fields = fetchField();
	result->numFields = numFields;
	result->numRows = numRows;
	return result;
}
int
mysqlpp::affectedRowsCount()
{
	affectedRows = mysql_affected_rows(mysql);
	return affectedRows;
}
