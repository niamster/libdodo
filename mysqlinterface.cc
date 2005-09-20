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
 *  aint with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef MYSQL_EXT

	#include "mysqlinterface.h"
	
	using namespace dodo;
	
	dodoBase *
	mysqlpp::getSelf()
	{
		return dynamic_cast<dodoBase *>(this);
	}
	
	//-------------------------------------------------------------------
	
	
	mysqlpp::mysqlpp() : empty(true),
						connected(false)
	{
		mysql = mysql_init(NULL);
		addSQL();
	}
	
	//-------------------------------------------------------------------
	
	mysqlpp::mysqlpp(mysqlpp *a_mypp) :	empty(true),
										connected(false)
	{
		mysql = mysql_init(NULL);
		addSQL();
		
		setSqlInfo(a_mypp->sqlInfo.db,
		a_mypp->sqlInfo.host,
		a_mypp->sqlInfo.port,
		a_mypp->sqlInfo.user,
		a_mypp->sqlInfo.password,
		a_mypp->sqlInfo.socket);
	}
	
	//-------------------------------------------------------------------
	
	mysqlpp::~mysqlpp()
	{
		if (!empty)
			mysql_free_result(mysqlRes);
		disconnect();
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::addSQL()
	{
		sqlDbDepAddInsArr.push_back(" delayed ");
		sqlDbDepAddInsArr.push_back(" low_priority ");
		sqlDbDepAddInsArr.push_back(" high_priority ");
		
		sqlDbDepAddSelArr.push_back(" straight_join ");
		sqlDbDepAddSelArr.push_back(" sql_small_result ");
		sqlDbDepAddSelArr.push_back(" sql_big_result ");
		
		sqlDbDepAddUpArr.push_back(" low_priority ");
	
		sqlDbDepAddDelArr.push_back(" low_priority ");
		sqlDbDepAddDelArr.push_back(" quick ");	
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::setMyAddInsSt(unsigned int statement)
	{
		/*switch (statement)
		{
			break;
			default:
				break;
			
		}*/
		addF(qDbDepInsShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::setMyAddUpSt(unsigned int statement)
	{
		/*switch (statement)
		{
			default:
				break;
			
		}*/
		addF(qDbDepUpShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::setMyAddSelSt(unsigned int statement)
	{
		switch (statement)
		{
			case SELECT_DISTINCT:
			case SELECT_ALL:
				removeF(qSelShift,2<<(SELECT_ALL-1));
				removeF(qSelShift,2<<(SELECT_DISTINCT-1));
				break;
			default:
				break;
			
		}
		addF(qDbDepSelShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::setMyAddDelSt(unsigned int statement)
	{
		/*switch (statement)
		{
			default:
				break;
			
		}*/
		addF(qDbDepDelShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::unsetMyAddInsSt(unsigned int statement)
	{
		removeF(qDbDepInsShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::unsetMyAddUpSt(unsigned int statement)
	{
		removeF(qDbDepUpShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::unsetMyAddSelSt(unsigned int statement)
	{
		removeF(qDbDepSelShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::unsetMyAddDelSt(unsigned int statement)
	{
		removeF(qDbDepDelShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void
	#else
		bool
	#endif 
	mysqlpp::connect(unsigned long type) const
	{
		if (connected)
			disconnect();
		if (!mysql_real_connect(mysql,
			sqlInfo.host.size()==0?NULL:sqlInfo.host.c_str(),
			sqlInfo.user.size()==0?NULL:sqlInfo.user.c_str(),
			sqlInfo.password.size()==0?NULL:sqlInfo.password.c_str(),
			sqlInfo.db.size()==0?NULL:sqlInfo.db.c_str(),
			sqlInfo.port,
			sqlInfo.socket.size()==0?NULL:sqlInfo.socket.c_str(),
			type))
		{
			connected = false;
			err = "Query was: " + request + "\n";
			err.append("mysql_real_connect(): ");
			err.append(mysql_error(mysql));
			#ifndef NO_EX
				throw mysqlppEx(MYSQL_CONNECT,(mysqlpp *)this,__LINE__,__FILE__);
			#else
				return false;
			#endif
		}
		connected = true;
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	void 
	mysqlpp::disconnect() const
	{
		if (connected)
		{
	     	mysql_close(mysql);
		 	connected = false;
		}
	}
	
	//-------------------------------------------------------------------
	
	bool 
	mysqlpp::_exec() const
	{	
		queryCollect();
		
		if (mysql_real_query(mysql,request.c_str(),request.size()) != 0)
		{
			err = "Query was: " + request + "\n";
			err.append("mysql_real_query(): ");
			err.append(mysql_error(mysql));
			#ifndef NO_EX
				throw mysqlppEx(MYSQL_QUERY,(mysqlpp *)this,__LINE__,__FILE__);
			#else
				return false;
			#endif
		}
		
		if (!show)
			return true;
		
		if (!empty)
			mysql_free_result(mysqlRes);
		empty = false;
		
		mysqlRes = mysql_store_result(mysql);
		if (mysqlRes == NULL)
		{
			err = "Query was: " + request + "\n";
			err.append("mysql_store_result(): ");
			err.append(mysql_error(mysql));
			#ifndef NO_EX
				throw mysqlppEx(MYSQL_STORE,(mysqlpp *)this,__LINE__,__FILE__);
			#else
				return false;
			#endif
		}
		return true;
	}
	
	//-------------------------------------------------------------------
	
	std::vector<stringArr>
	mysqlpp::fetchRow()
	{	
		register unsigned  numFields = mysql_num_fields(mysqlRes);	
		
		std::vector<stringArr> rows;
		rows.reserve(mysql_num_rows(mysqlRes));
		
		stringArr row;
		row.reserve(numFields);
		
		std::string rowPart;
		
		register unsigned long *length, j;
		while ((mysqlRow = mysql_fetch_row(mysqlRes)) != NULL)
		{		
			length = mysql_fetch_lengths(mysqlRes);
			row.clear();
			for (j=0;j<numFields;j++)
			{
				rowPart.assign(mysqlRow[j]?mysqlRow[j]:"NULL",mysqlRow[j]?length[j]:4);
				row.push_back(rowPart);
			}
			rows.push_back(row);
		}
	
		return rows;
	}
	
	//-------------------------------------------------------------------
	
	stringArr
	mysqlpp::fetchField()
	{	
		unsigned int numFields = mysql_num_fields(mysqlRes);	
		mysqlFields = mysql_fetch_fields(mysqlRes);
		
		stringArr fields;
		fields.reserve(numFields);
		
		for (register unsigned int i(0);i<numFields;i++)
			fields.push_back(mysqlFields[i].name);
		
		return fields;
	}
	
	//-------------------------------------------------------------------
	
	__sqlStorage 
	mysqlpp::fetch()
	{
		return __sqlStorage(fetchRow(), fetchField());
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	mysqlpp::rowsCount()
	{
		return mysql_num_rows(mysqlRes);
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	mysqlpp::fieldsCount()
	{
		return mysql_num_fields(mysqlRes);
	}
	
	//-------------------------------------------------------------------
	
	unsigned int
	mysqlpp::affectedRowsCount()
	{
		return mysql_affected_rows(mysql);
	}
	
	//-------------------------------------------------------------------
	
	bool 
	mysqlpp::exec() const
	{
		performXExec(preExec);
		bool result = _exec(); 
		performXExec(postExec);
		cleanCollect();
		return result;
	}
	
	//-------------------------------------------------------------------
	
	std::string
	mysqlpp::getErr() const
	{
		return err;
	}
	
	//-------------------------------------------------------------------
	
	int 
	mysqlpp::myAddPostExec(inExec func, 
						void *data) const
	{
		xexec::addPostExec(func, (dodoBase *)this, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	mysqlpp::myAddPreExec(inExec func, 
						void *data) const
	{
		addPreExec(func, (dodoBase *)this, data);
	}
	
	//-------------------------------------------------------------------
	
#endif
