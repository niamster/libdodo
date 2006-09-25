/***************************************************************************
 *            dbMysql.cc
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

#include <dbMysql.h>
	
#ifdef MYSQL_EXT

	#include <sys/socket.h>
	
	#include <dbMysqlEx.h>
	#include <tools.h>
	
	using namespace dodo;

	__mysqlSSLOptions::__mysqlSSLOptions()
	{
	}

	//-------------------------------------------------------------------
	
	__mysqlSSLOptions::__mysqlSSLOptions(const std::string &a_key, 
						const std::string &a_cert, 
						const std::string &a_ca, 
						const std::string &a_capath, 
						const std::string &a_cipher) : key(a_key),
									cert(a_cert),
									ca(a_ca),
									capath(a_capath),
									cipher(a_cipher)
	{
	}
	
	//-------------------------------------------------------------------

	
	dbMysql::dbMysql() : empty(true),
							type(CLIENT_MULTI_STATEMENTS)
	{	
		connected = false;
		
		addSQL();
	}
	
	//-------------------------------------------------------------------
	
	dbMysql::dbMysql(dbMysql &a_mypp)
	{
	}
	
	//-------------------------------------------------------------------
	
	dbMysql::~dbMysql()
	{
		if (connected)
		{
			if (!empty)
				mysql_free_result(mysqlRes);
			
	     	mysql_close(mysql);
		}
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::addSQL()
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
	dbMysql::setMyAddInsSt(short statement)
	{
		removeF(qDbDepInsShift,1<<DBREQUEST_INSERT_DELAYED);
		removeF(qDbDepInsShift,1<<DBREQUEST_INSERT_LOW_PRIORITY);
		removeF(qDbDepInsShift,1<<DBREQUEST_INSERT_HIGH_PRIORITY);
		
		addF(qDbDepInsShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::setMyAddUpSt(short statement)
	{
		addF(qDbDepUpShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::setMyAddSelSt(short statement)
	{
		addF(qDbDepSelShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::setMyAddDelSt(short statement)
	{
		addF(qDbDepDelShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::unsetMyAddInsSt(short statement)
	{
		removeF(qDbDepInsShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::unsetMyAddUpSt(short statement)
	{
		removeF(qDbDepUpShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::unsetMyAddSelSt(short statement)
	{
		removeF(qDbDepSelShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::unsetMyAddDelSt(short statement)
	{
		removeF(qDbDepDelShift,1<<statement);	
	}

	//-------------------------------------------------------------------

	void
	dbMysql::connectSettings(unsigned long a_type,
					const __mysqlSSLOptions &options) const
	{
		type = a_type;
		
		mysql_ssl_set(mysql,
		options.key.size()==0?NULL:options.key.c_str(),
		options.cert.size()==0?NULL:options.cert.c_str(),
		options.ca.size()==0?NULL:options.ca.c_str(),
		options.capath.size()==0?NULL:options.capath.c_str(),
		options.cipher.size()==0?NULL:options.cipher.c_str());		
	}	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbMysql::connect() const
	{
		disconnect();
			
		#ifndef DBMYSQL_WO_XEXEC
			operType = DBMYSQL_OPER_CONNECT;
			performXExec(preExec);
		#endif
		
		mysql = mysql_init(NULL);
		
		if (!mysql_real_connect(mysql,
			dbInfo.host.size()==0?NULL:dbInfo.host.c_str(),
			dbInfo.user.size()==0?NULL:dbInfo.user.c_str(),
			dbInfo.password.size()==0?NULL:dbInfo.password.c_str(),
			dbInfo.db.size()==0?NULL:dbInfo.db.c_str(),
			dbInfo.port,
			dbInfo.path.size()==0?NULL:dbInfo.path.c_str(),
			type))
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBMYSQL,DBMYSQL_CONNECT,ERR_MYSQL,mysql_errno(mysql),mysql_error(mysql),__LINE__,__FILE__);
				#else
					return false;
				#endif
		
		#ifndef DBMYSQL_WO_XEXEC
			performXExec(postExec);
		#endif
		
		connected = true;
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::disconnect() const
	{
		if (connected)
		{
			#ifndef DBMYSQL_WO_XEXEC
				operType = DBMYSQL_OPER_DISCONNECT;
				performXExec(preExec);
			#endif
		
			if (!empty)
			{
				empty = true;
				mysql_free_result(mysqlRes);
			}
			
	     	mysql_close(mysql);

			#ifndef DBMYSQL_WO_XEXEC
				performXExec(postExec);
			#endif
	     	
		 	connected = false;
		}
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbMysql::_exec(const std::string &query, 
					bool result) const
	{	
		if (query.size()==0)
			queryCollect();			
		else
		{
			request = query;
			show = result;
		}

		if (mysql_real_query(mysql,request.c_str(),request.size()) != 0)
		{
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBMYSQL,DBMYSQL_CONNECT,ERR_MYSQL,mysql_errno(mysql),mysql_error(mysql),__LINE__,__FILE__);
			#else
				return false;
			#endif
		}
		
		if (!show)
			#ifndef NO_EX
				return ;
			#else
				return true;
			#endif
		
		if (!empty)
		{
			mysql_free_result(mysqlRes);
			empty = true;
		}
		
		mysqlRes = mysql_store_result(mysql);
		if (mysqlRes == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBMYSQL,DBMYSQL_CONNECT,ERR_MYSQL,mysql_errno(mysql),mysql_error(mysql),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		empty = false;
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	std::vector<stringArr>
	dbMysql::fetchRow() const
	{
		#ifndef DBMYSQL_WO_XEXEC
			operType = DBMYSQL_OPER_FETCHROW;
			performXExec(preExec);
		#endif		
			
		if (empty || !show)
			return __stringarrayvector__;
		
		numFields = mysql_num_fields(mysqlRes);	
		
		rows.clear();
		rows.reserve(mysql_num_rows(mysqlRes));
		
		register unsigned long *length, j;
		
		while ((mysqlRow = mysql_fetch_row(mysqlRes)) != NULL)
		{		
			length = mysql_fetch_lengths(mysqlRes);
			
			rowsPart.clear();
			rowsPart.reserve(numFields);
			
			for (j=0;j<numFields;j++)
			{
				rowPart.assign((mysqlRow[j]!=NULL)?mysqlRow[j]:"NULL",mysqlRow[j]?length[j]:4);
				rowsPart.push_back(rowPart);
			}
			
			rows.push_back(rowsPart);
		}

		#ifndef DBMYSQL_WO_XEXEC
			performXExec(postExec);
		#endif
				
		return rows;
	}
	
	//-------------------------------------------------------------------
	
	stringArr
	dbMysql::fetchField() const
	{	
		#ifndef DBMYSQL_WO_XEXEC
			operType = DBMYSQL_OPER_FETCHFIELD;
			performXExec(preExec);
		#endif
					
		if (empty || !show)
			return __stringarray__;
			
		numFields = mysql_num_fields(mysqlRes);	
		mysqlFields = mysql_fetch_fields(mysqlRes);
		
		fields.clear();
		fields.reserve(numFields);
		
		for (register unsigned int i(0);i<numFields;i++)
			fields.push_back(mysqlFields[i].name);

		#ifndef DBMYSQL_WO_XEXEC
			performXExec(postExec);
		#endif
					
		return fields;
	}
	
	//-------------------------------------------------------------------
	
	__dbStorage 
	dbMysql::fetch() const
	{
		return __dbStorage(fetchRow(), fetchField());
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbMysql::rowsCount() const
	{
		if (empty || !show)
			return 0;
		else	
			return mysql_num_rows(mysqlRes);
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbMysql::fieldsCount() const
	{
		if (empty || !show)
			return 0;
		else	
			return mysql_num_fields(mysqlRes);
	}
	
	//-------------------------------------------------------------------
	
	unsigned int
	dbMysql::affectedRowsCount()
	{
		if (empty || show)
			return 0;
		else	
			return mysql_affected_rows(mysql);
	}
	
	//-------------------------------------------------------------------
	

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbMysql::exec(const std::string &query, 
				bool result) const
	{
		#ifndef DBMYSQL_WO_XEXEC
			operType = DBMYSQL_OPER_EXEC;
			performXExec(preExec);
		#endif
		
		#ifdef NO_EX
			bool _result = 
		#endif	
			_exec(query, result); 
		
		#ifndef DBMYSQL_WO_XEXEC		
			performXExec(postExec);
		#endif
		
		cleanCollect();
		
		#ifdef NO_EX
			return _result;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	#ifndef DBMYSQL_WO_XEXEC
	
		int 
		dbMysql::addPostExec(inExec func, 
							void *data) const
		{
			return _addPostExec(func, (void *)this, XEXECOBJ_DBMYSQL, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		dbMysql::addPreExec(inExec func, 
							void *data) const
		{
			return _addPreExec(func, (void *)this, XEXECOBJ_DBMYSQL, data);
		}
		
		//-------------------------------------------------------------------
		
		#ifdef DL_EXT
		
			int 
			dbMysql::addPostExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addPostExec(module, (void *)this, XEXECOBJ_DBMYSQL, data, toInit);
			}
			
			//-------------------------------------------------------------------
			
			int 
			dbMysql::addPreExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addPreExec(module, (void *)this, XEXECOBJ_DBMYSQL, data, toInit);
			}
		
			//-------------------------------------------------------------------
			
			xexecCounts 
			dbMysql::addExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addExec(module, (void *)this, XEXECOBJ_DBMYSQL, data, toInit);
			}
		
		#endif
	
		//-------------------------------------------------------------------
			
	#endif
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::setCharset(const std::string &charset)
	{
		mysql_options(mysql,MYSQL_READ_DEFAULT_FILE,charset.c_str());
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::setConnectTimeout(unsigned int time)
	{
		mysql_options(mysql,MYSQL_OPT_CONNECT_TIMEOUT,(char *)&time);
	}

	//-------------------------------------------------------------------
	
	std::string 
	dbMysql::getCharset() const
	{
		return mysql_character_set_name(mysql);
	}
	
	//-------------------------------------------------------------------
	
	dodoStringMapArr 
	dbMysql::fetchAssoc() const
	{
		if (empty || !show)
			return __dodostringmap__;
		
		numFields = mysql_num_fields(mysqlRes);	
		mysqlFields = mysql_fetch_fields(mysqlRes);

		rowsFields.reserve(mysql_num_rows(mysqlRes));
		
		rowFieldsPart.clear();

		register unsigned long *length, j;
		
		while ((mysqlRow = mysql_fetch_row(mysqlRes)) != NULL)
		{		
			length = mysql_fetch_lengths(mysqlRes);
			
			for (j=0;j<numFields;j++)
			{
				rowPart.assign((mysqlRow[j]!=NULL)?mysqlRow[j]:"NULL",mysqlRow[j]?length[j]:4);			
				rowFieldsPart.realArr[mysqlFields[j].name] = rowPart;
			}
			
			rowsFields.push_back(rowFieldsPart);
		}
		
		return rowsFields;		
	}

	//-------------------------------------------------------------------
	
#endif
