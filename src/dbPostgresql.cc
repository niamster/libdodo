/***************************************************************************
 *            dbPostgresql.cc
 *
 *  Fri Jan  13 19:25:19 2006
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <dbPostgresql.h>

#ifdef POSTGRESQL_EXT
	
	using namespace dodo;
	
	dbPostgresql::dbPostgresql() : empty(true)
	{
		blob = "bytea";
		tinyblob = "bytea";
		mediumblob = "bytea";
		longblob = "bytea";
	}
	
	//-------------------------------------------------------------------
	
	dbPostgresql::dbPostgresql(dbPostgresql &a_mypp)
	{
	}
	
	//-------------------------------------------------------------------
	
	dbPostgresql::~dbPostgresql()
	{
		if (connected)
		{
			if (!empty)
				PQclear(pgResult);
			
	     	PQfinish(conn);
		}
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbPostgresql::connect()
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
						
		register int status = PQstatus(conn);
		
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
	dbPostgresql::disconnect()
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

	void 
	dbPostgresql::setBLOBValues(const dodoStringArr &values)
	{
		blobs = values;
	}
	
	//-------------------------------------------------------------------
		
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbPostgresql::_exec(const std::string &query, 
						bool result)
	{	
		register bool blobHint;
		register int status;
		
		if (query.size() == 0)
		{
		
			if (autoFraming)
			{
				if (qType == DBREQUEST_INSERT || qType == DBREQUEST_UPDATE)
				{
					std::string temp = dbInfo.db + ":" + pre_table;
					
					if (!framingFields.isset(temp))
					{
						request = "select column_name, data_type from information_schema.columns where table_name='" + pre_table + "'";
					
						if (!empty)
						{
							PQclear(pgResult);
							empty = true;
						}
							
						pgResult = PQexecParams(conn,request.c_str(),0,NULL,NULL,NULL,NULL,1);
						if(pgResult == NULL)
							#ifndef NO_EX
								throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL__EXEC,ERR_MYSQL,PGRES_FATAL_ERROR,PQerrorMessage(conn),__LINE__,__FILE__,request);
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
									throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL__EXEC,ERR_MYSQL,status,PQerrorMessage(conn),__LINE__,__FILE__);
								#else
									return false;
								#endif
						}
						
						empty = false;

						register int rowsNum = PQntuples(pgResult);
						char *fieldType;
						
						dodoStringArr rowsPart;
				
						for (register int i(0);i<rowsNum;++i)
						{
							fieldType = PQgetvalue(pgResult,i,1);
							
							if (strcasestr(fieldType,"char") != NULL || 
								strcasestr(fieldType,"date") != NULL || 
								strcasestr(fieldType,"bytea") != NULL || 
								strcasestr(fieldType,"array") != NULL || 
								strcasestr(fieldType,"text") != NULL || 
								strcasestr(fieldType,"cidr") != NULL || 
								strcasestr(fieldType,"macaddrcd ") != NULL || 
								strcasestr(fieldType,"inet") != NULL)
									rowsPart.push_back(PQgetvalue(pgResult,i,0));
						}
					
						if (!empty)
						{
							PQclear(pgResult);
							empty = true;
						}							
							
						framingFields.insert(temp, rowsPart);												
					}
				}
			}
			
			queryCollect();
			
			blobHint = false;
		}			
		else
		{
			if (strstr(query.c_str(),"dodo:hint:db:blob") != NULL)
			{
				queryCollect();
				
				if (!show)
					blobHint = true;
			}
			else
			{
				request = query;
				show = result;
				
				blobHint = false;
			}
		}

		if (!empty)
		{
			PQclear(pgResult);
			empty = true;
		}
		
		if (blobHint)
		{
			switch (qType)
			{
				case DBREQUEST_UPDATE:
				case DBREQUEST_INSERT:
				
					{
						register long size = blobs.size();
						
						register char **values = new char*[size];
						register int *lengths = new int[size];
						register int *formats = new int[size];
						
						dodoStringArr::iterator i(blobs.begin()), j(blobs.end());
						for (register int o=0;i!=j;++i,++o)
						{
							values[o] = (char *)i->c_str();
							lengths[o] = i->size();
							formats[o] = 1;
						}

						pgResult = PQexecParams(conn,request.c_str(),size,NULL,values,lengths,formats,0);
						if(pgResult == NULL)
							#ifndef NO_EX
							{
								delete [] values;
								delete [] lengths;
								delete [] formats;
								
								throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL__EXEC,ERR_MYSQL,PGRES_FATAL_ERROR,PQerrorMessage(conn),__LINE__,__FILE__);
							}
							#else
							{
								delete [] values;
								delete [] lengths;
								delete [] formats;
								
								return false;
							}
							#endif
					
						delete [] values;
						delete [] lengths;
						delete [] formats;	
					}	
										
					break;
					
				default:
					
					#ifndef NO_EX
						throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL__EXEC,ERR_LIBDODO,DBPOSTGRESQL_WRONG_HINT_USAGE,DBPOSTGRESQL_WRONG_HINT_USAGE_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
					
			}			
		}
		else
		{
			pgResult = PQexecParams(conn,request.c_str(),0,NULL,NULL,NULL,NULL,1);
			if(pgResult == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL__EXEC,ERR_MYSQL,PGRES_FATAL_ERROR,PQerrorMessage(conn),__LINE__,__FILE__,request);
				#else
					return false;
				#endif
		}
		
		status = PQresultStatus(pgResult);

		switch (status)
		{
			case PGRES_EMPTY_QUERY:
			case PGRES_BAD_RESPONSE:
			case PGRES_NONFATAL_ERROR:
			case PGRES_FATAL_ERROR:
			
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL__EXEC,ERR_MYSQL,status,PQerrorMessage(conn),__LINE__,__FILE__);
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
	
	dodoArray<dodoStringArr>
	dbPostgresql::fetchRow() const
	{
			
		#ifndef DBPOSTGRESQL_WO_XEXEC
			operType = DBPOSTGRESQL_OPER_FETCHROW;
			performXExec(preExec);
		#endif		
			
		if (empty || !show)
			return __stringarrayvector__;
		
		register int rowsNum = PQntuples(pgResult);
		register int fieldsNum = PQnfields(pgResult);

		dodoArray<dodoStringArr> rows;
		
		#ifndef USE_DEQUE
			rows.reserve(rowsNum);
		#endif
		
		register int j;
		
		dodoStringArr rowsPart;
		std::string rowPart;

		for (register int i(0);i<rowsNum;++i)
		{
			rowsPart.clear();
			
			#ifndef USE_DEQUE
				rowsPart.reserve(fieldsNum);
			#endif
			
			for (j=0;j<fieldsNum;++j)
			{
				if (PQgetisnull(pgResult,i,j) == 1)
					rowPart.assign("NULL");
				else
				{
					if (preventEscaping)
						rowPart.assign(PQgetvalue(pgResult,i,j),PQgetlength(pgResult,i,j));
					else
						rowPart.assign(unescapeFields(std::string(PQgetvalue(pgResult,i,j),PQgetlength(pgResult,i,j))));
				}

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
	
	dodoStringArr
	dbPostgresql::fetchField() const
	{	
		#ifndef DBPOSTGRESQL_WO_XEXEC
			operType = DBPOSTGRESQL_OPER_FETCHFIELD;
			performXExec(preExec);
		#endif
					
		if (empty || !show)
			return __stringarray__;

		register int fieldsNum = PQnfields(pgResult);
		
		dodoStringArr fields;
		
		#ifndef USE_DEQUE
			fields.reserve(fieldsNum);
		#endif
		
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
	dbPostgresql::affectedRowsCount() const
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
	dbPostgresql::exec(const std::string &query, 
						bool result)
	{
		#ifndef DBPOSTGRESQL_WO_XEXEC
			operType = DBPOSTGRESQL_OPER_EXEC;
			performXExec(preExec);
		#endif
		
		#ifdef NO_EX
			bool _result = 
		#endif	
			_exec(query, result); 
		
		#ifndef DBPOSTGRESQL_WO_XEXEC		
			performXExec(postExec);
		#endif
		
		cleanCollect();
		
		#ifdef NO_EX
			return _result;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	#ifndef DBPOSTGRESQL_WO_XEXEC
	
		int 
		dbPostgresql::addPostExec(inExec func, 
							void *data)
		{
			return _addPostExec(func, (void *)this, XEXECOBJ_DBPOSTGRESQL, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		dbPostgresql::addPreExec(inExec func, 
							void *data)
		{
			return _addPreExec(func, (void *)this, XEXECOBJ_DBPOSTGRESQL, data);
		}
		
		//-------------------------------------------------------------------
		
		#ifdef DL_EXT
		
			int 
			dbPostgresql::addPostExec(const std::string &module, 
								void *data,
								void *toInit)
			{
				return _addPostExec(module, (void *)this, XEXECOBJ_DBPOSTGRESQL, data, toInit);
			}
			
			//-------------------------------------------------------------------
			
			int 
			dbPostgresql::addPreExec(const std::string &module, 
								void *data,
								void *toInit)
			{
				return _addPreExec(module, (void *)this, XEXECOBJ_DBPOSTGRESQL, data, toInit);
			}
		
			//-------------------------------------------------------------------
			
			xexecCounts 
			dbPostgresql::addExec(const std::string &module, 
								void *data,
								void *toInit)
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
			return __dodostringmap__;
		
		register int rowsNum = PQntuples(pgResult);
		register int fieldsNum = PQnfields(pgResult);
		
		dodoStringMapArr rowsFields;
		
		#ifndef USE_DEQUE
			rowsFields.reserve(rowsNum);
		#endif
		
		dodoStringMap rowFieldsPart;
		std::string rowPart;
		
		register int j;

		for (register int i(0);i<rowsNum;++i)
		{
			rowFieldsPart.clear();
			
			for (j=0;j<fieldsNum;++j)
			{
				if (PQgetisnull(pgResult,i,j)==1)
					rowPart.assign("NULL");
				else
				{
					if (preventEscaping)
						rowPart.assign(PQgetvalue(pgResult,i,j),PQgetlength(pgResult,i,j));
					else
						rowPart.assign(unescapeFields(std::string(PQgetvalue(pgResult,i,j),PQgetlength(pgResult,i,j))));					
				}

				rowFieldsPart.realArr[PQfname(pgResult,i)] = rowPart;
			}
			
			rowsFields.push_back(rowFieldsPart);
		}
		
		return rowsFields;
	}

	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif 
	dbPostgresql::setCharset(const std::string &charset)
	{
		register int status = PQsetClientEncoding(conn, charset.c_str());
		if (status == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBPOSTGRESQL,DBPOSTGRESQL_SETCHARSET,ERR_MYSQL,status,PQerrorMessage(conn),__LINE__,__FILE__);
			#else
				return false;
			#endif

		#ifdef NO_EX
			return true;
		#endif					
	}
	
	//-------------------------------------------------------------------
	
	int 
	dbPostgresql::getCharset() const
	{
		return PQclientEncoding(conn);
	}

	//-------------------------------------------------------------------
		
#endif
