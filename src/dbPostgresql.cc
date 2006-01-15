/***************************************************************************
 *            dbPostgresql.cc
 *
 *  Fri Jan  13 19:25:19 2006
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

#include <dbPostgresql.h>

#ifdef POSTGRESQL_EXT
	
	using namespace dodo;
	
	dbPostgresql::dbPostgresql() : empty(true)
	{
	}
	
	//-------------------------------------------------------------------
	
	dbPostgresql::dbPostgresql(dbPostgresql &a_mypp)
	{
	}
	
	//-------------------------------------------------------------------
	
	dbPostgresql::~dbPostgresql()
	{
		disconnect();
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbPostgresql::connect() const
	{
		if (connected)
			disconnect();
			
		#ifndef DBPOSTGRESQL_WO_XEXEC
			operType = DBPOSTGRESQL_OPER_CONNECT;
			performXExec(preExec);
		#endif
		
		char port[6];
		sprintf(port,"%d",dbInfo.port);
		
		conn = PQsetdbLogin(
						dbInfo.host.size()==0?NULL:dbInfo.host.c_str(),
						port,
						NULL,
						NULL,
						dbInfo.db.size()==0?NULL:dbInfo.db.c_str(),
						dbInfo.user.size()==0?NULL:dbInfo.user.c_str(),
						dbInfo.password.size()==0?NULL:dbInfo.password.c_str());
						
		status = PQstatus(conn);
		
		if (status != CONNECTION_OK)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL_CONNECT,ERR_MYSQL,status,PQerrorMessage(conn),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		#ifndef DBPOSTGRESQL_WO_XEXEC
			performXExec(postExec);
		#endif
		
		connected = true;
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbPostgresql::disconnect() const
	{
		if (connected)
		{
			#ifndef DBPOSTGRESQL_WO_XEXEC
				operType = DBPOSTGRESQL_OPER_DISCONNECT;
				performXExec(preExec);
			#endif
			
			if (!empty)
			{
				PQclear(pgResult);
				empty = true;
			}
			
	     	PQfinish(conn);

			#ifndef DBPOSTGRESQL_WO_XEXEC
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
	dbPostgresql::_exec() const
	{	
		queryCollect();

		if (!empty)
		{
			PQclear(pgResult);
			empty = true;
		}
		
		pgResult = PQexec(conn,request.c_str());
		if(pgResult == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL_CONNECT,ERR_MYSQL,PGRES_FATAL_ERROR,PQerrorMessage(conn),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		
		status = PQresultStatus(pgResult);

		switch (status)
		{
			case PGRES_EMPTY_QUERY:
			case PGRES_BAD_RESPONSE:
			case PGRES_NONFATAL_ERROR:
			case PGRES_FATAL_ERROR:
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL_CONNECT,ERR_MYSQL,status,PQerrorMessage(conn),__LINE__,__FILE__);
				#else
					return false;
				#endif
		}
		
		empty = false;
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	std::vector<stringArr>
	dbPostgresql::fetchRow() const
	{
			
		#ifndef DBPOSTGRESQL_WO_XEXEC
			operType = DBPOSTGRESQL_OPER_FETCHROW;
			performXExec(preExec);
		#endif		
			
		if (empty || !show)
			return std::vector<stringArr>();
		
		rowsNum = PQntuples(pgResult);
		fieldsNum = PQnfields(pgResult);

		rows.clear();
		rows.reserve(rowsNum);
		
		register int j;

		for (register int i(0);i<rowsNum;++i)
		{
			rowsPart.clear();
			rowsPart.reserve(fieldsNum);
			
			for (j=0;j<fieldsNum;++j)
			{
				if (PQgetisnull(pgResult,i,j)==1)
					rowPart.assign("NULL");
				else
					rowPart.assign(PQgetvalue(pgResult,i,j),PQgetlength(pgResult,i,j));

				rowsPart.push_back(rowPart);
			}
			
			rows.push_back(rowsPart);
		}

		#ifndef DBPOSTGRESQL_WO_XEXEC
			performXExec(postExec);
		#endif
				
		return rows;
	}
	
	//-------------------------------------------------------------------
	
	stringArr
	dbPostgresql::fetchField() const
	{	
		#ifndef DBPOSTGRESQL_WO_XEXEC
			operType = DBPOSTGRESQL_OPER_FETCHFIELD;
			performXExec(preExec);
		#endif
					
		if (empty || !show)
			return __stringarray__;

		fieldsNum = PQnfields(pgResult);
		
		fields.clear();
		fields.reserve(fieldsNum);
		
		for (register int i(0);i<fieldsNum;++i)
			fields.push_back(PQfname(pgResult,i));

		#ifndef DBPOSTGRESQL_WO_XEXEC
			performXExec(postExec);
		#endif
					
		return fields;
	}
	
	//-------------------------------------------------------------------
	
	__dbStorage 
	dbPostgresql::fetch() const
	{
		return __dbStorage(fetchRow(), fetchField());
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbPostgresql::rowsCount() const
	{
		if (empty || !show)
			return 0;
		else	
			return PQntuples(pgResult);
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbPostgresql::fieldsCount() const
	{
		if (empty || !show)
			return 0;
		else	
			return PQnfields(pgResult);
	}
	
	//-------------------------------------------------------------------
	
	unsigned int
	dbPostgresql::affectedRowsCount()
	{
		if (empty || show)
			return 0;
		else	
			return atoi(PQcmdTuples(pgResult));
	}
	
	//-------------------------------------------------------------------
	

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbPostgresql::exec() const
	{
		#ifndef DBPOSTGRESQL_WO_XEXEC
			operType = DBPOSTGRESQL_OPER_EXEC;
			performXExec(preExec);
		#endif
		
		#ifdef NO_EX
			bool result = 
		#endif	
			_exec(); 
		
		#ifndef DBPOSTGRESQL_WO_XEXEC		
			performXExec(postExec);
		#endif
		
		cleanCollect();
		
		#ifdef NO_EX
			return result;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	#ifndef DBPOSTGRESQL_WO_XEXEC
	
		int 
		dbPostgresql::addPostExec(inExec func, 
							void *data) const
		{
			return _addPostExec(func, (void *)this, XEXECOBJ_DBPOSTGRESQL, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		dbPostgresql::addPreExec(inExec func, 
							void *data) const
		{
			return _addPreExec(func, (void *)this, XEXECOBJ_DBPOSTGRESQL, data);
		}
		
		//-------------------------------------------------------------------
		
		#ifdef DL_EXT
		
			int 
			dbPostgresql::addPostExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addPostExec(module, (void *)this, XEXECOBJ_DBPOSTGRESQL, data, toInit);
			}
			
			//-------------------------------------------------------------------
			
			int 
			dbPostgresql::addPreExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addPreExec(module, (void *)this, XEXECOBJ_DBPOSTGRESQL, data, toInit);
			}
		
			//-------------------------------------------------------------------
			
			xexecCounts 
			dbPostgresql::addExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addExec(module, (void *)this, XEXECOBJ_DBPOSTGRESQL, data, toInit);
			}
		
		#endif
	
		//-------------------------------------------------------------------
	
	#endif
	
	//-------------------------------------------------------------------
	
	dodoStringMapArr 
	dbPostgresql::fetchAssoc() const
	{
		if (empty || !show)
			return dodoStringMapArr();
		
		rowsNum = PQntuples(pgResult);
		fieldsNum = PQnfields(pgResult);
		
		rowsFields.clear();
		rowsFields.reserve(rowsNum);
		
		register int j;

		for (register int i(0);i<rowsNum;++i)
		{
			rowFieldsPart.clear();
			
			for (j=0;j<fieldsNum;++j)
			{
				if (PQgetisnull(pgResult,i,j)==1)
					rowPart.assign("NULL");
				else
					rowPart.assign(PQgetvalue(pgResult,i,j),PQgetlength(pgResult,i,j));

				rowFieldsPart.realArr[PQfname(pgResult,i)] = rowPart;
			}
			
			rowsFields.push_back(rowFieldsPart);
		}
		
		return rowsFields;
	}

	//-------------------------------------------------------------------
	
#endif
