/***************************************************************************
 *            dbSqlite.cc
 *
 *  Thu Apr  30 13:45:19 2005
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

#include <dbSqlite.h>

#ifdef SQLITE_EXT
	
	using namespace dodo;

	
	dbSqlite::dbSqlite() : empty(true)
	{
		auto_increment = " autoincrement ";
	}
	
	//-------------------------------------------------------------------
	
	dbSqlite::dbSqlite(dbSqlite &a_pp)
	{
	}
	
	//-------------------------------------------------------------------
	
	dbSqlite::~dbSqlite()
	{
		if (connected)
		{
			if (!empty)
				sqlite3_finalize(liteStmt);
				
			sqlite3_close(lite);
		}
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::connect()
	{
		if (connected)
			disconnect();
			
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_CONNECT;
			performXExec(preExec);
		#endif
		
		if (sqlite3_open(dbInfo.path.c_str(),&lite) != SQLITE_OK)
		{
			sqlite3_close(lite);
			
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE_CONNECT, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__);
			#else
				return false;
			#endif		
		}
		
		#ifndef DBSQLITE_WO_XEXEC
			performXExec(postExec);
		#endif
		
		connected = true;
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbSqlite::disconnect()
	{
		if (connected)
		{
			#ifndef DBSQLITE_WO_XEXEC
				operType = DBSQLITE_OPER_DISCONNECT;
				performXExec(preExec);
			#endif
		
			if (!empty)
			{
				empty = true;
				sqlite3_finalize(liteStmt);
			}
						
			if (sqlite3_close(lite) != SQLITE_OK)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE_DISCONNECT, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__);
				#else
					return ;
				#endif

			#ifndef DBSQLITE_WO_XEXEC
				performXExec(postExec);
			#endif
	     	
		 	connected = false;
		}
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbSqlite::setBLOBValues(const dodoStringArr &values)
	{
		blobs = values;
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::_exec(const dodoString &query, 
					bool result)
	{
		bool blobHint;
		
		if (query.size() == 0)
		{
			if (autoFraming)
			{
				#ifdef SQLITE_ENABLE_COLUMN_METADATA
				
					if (qType == DBREQUEST_INSERT || qType == DBREQUEST_UPDATE)
					{
						dodoString temp = dbInfo.db + ":" + pre_table;
						
						if (!framingFields.isset(temp))
						{
							request = "select * from " + pre_table + " limit 1";
	
							if (!empty)
							{
								sqlite3_finalize(liteStmt);
								empty = true;
							}		
		
							if (sqlite3_prepare(lite, request.c_str(), request.size(), &liteStmt, NULL) != SQLITE_OK)
								#ifndef NO_EX
									throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__, request);
								#else
									return false;
								#endif	
			
							if (liteStmt == NULL)	
								#ifndef NO_EX
									throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__);
								#else
									return false;
								#endif
								
							empty = false;
				
							unsigned int numFields = sqlite3_column_count(liteStmt);
							
							const char *columnType, *columnName;
							
							dodoStringArr temp1;
							
							for (unsigned int i(0);i<numFields;++i)
							{	
								columnName = sqlite3_column_name(liteStmt, i);
											
								if (sqlite3_table_column_metadata(lite, 
															NULL, 
															pre_table.c_str(),
															columnName,
															&columnType,
															NULL,
															NULL,
															NULL,
															NULL) != SQLITE_OK)
																#ifndef NO_EX
																	throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__, request);
																#else
																	return false;
																#endif
																
								if (strcasestr(columnType,"char") != NULL || 
									strcasestr(columnType,"date") != NULL || 
									strcasestr(columnType,"time") != NULL || 
									strcasestr(columnType,"blob") != NULL || 
									strcasestr(columnType,"text") != NULL || 
									strcasestr(columnType,"enum") != NULL || 
									strcasestr(columnType,"set") != NULL)
										temp1.push_back(columnName);
							}
							
							if (!empty)
							{
								sqlite3_finalize(liteStmt);
								empty = true;
							}
							
							framingFields.insert(temp, temp1);
						}
					}
					
				#else
				
					autoFraming = false;
				
				#endif
				
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
			sqlite3_finalize(liteStmt);
			empty = true;
		}			
			
		if (sqlite3_prepare(lite, request.c_str(), request.size(), &liteStmt, NULL) != SQLITE_OK)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__, request);
			#else
				return false;
			#endif
			
		if (blobHint)
		{
			switch (qType)
			{
				case DBREQUEST_UPDATE:
				case DBREQUEST_INSERT:
				
					{
						dodoStringArr::iterator i(blobs.begin()), j(blobs.end());
						for (int o=1;i!=j;++i,++o)
							if (sqlite3_bind_blob(liteStmt, o, i->c_str(), i->size(), SQLITE_TRANSIENT) != SQLITE_OK)
								#ifndef NO_EX
									throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__);
								#else
									return false;
								#endif
					}	
										
					break;
					
				default:
					
					#ifndef NO_EX
						throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE__EXEC, ERR_LIBDODO, DBSQLITE_WRONG_HINT_USAGE, DBSQLITE_WRONG_HINT_USAGE_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
					
			}
		}	
		
		if (liteStmt == NULL)	
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE__EXEC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		empty = false;	
			
		if (!show)
			if (sqlite3_step(liteStmt) != SQLITE_DONE)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE_FETCHROW, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__);
				#else
					return false;
				#endif	
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	dodoArray<dodoStringArr>
	dbSqlite::fetchRow() const
	{
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_FETCHROW;
			performXExec(preExec);
		#endif		
			
		if (!show)
			return __stringarrayvector__;

		sqlite3_reset(liteStmt);
		
		unsigned int numFields = sqlite3_column_count(liteStmt);
		
		bool iterate = true;
		unsigned int i = 0;
		
		dodoArray<dodoStringArr> rows;
		
		#ifndef USE_DEQUE
			rows.reserve(sqlite3_data_count(liteStmt));
		#endif
		
		dodoStringArr rowsPart;
		dodoString rowPart;
		
		int result;
		
		while (iterate)
		{
			result = sqlite3_step(liteStmt);
			switch (result)
			{
				case SQLITE_BUSY:
				
					continue;
					
				case SQLITE_DONE:
				
					iterate = false;
					
					break;
					
				case SQLITE_ERROR:
				
					#ifndef NO_EX
						throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE_FETCHROW, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__);
					#else
						return __stringarrayvector__;
					#endif
					
				case SQLITE_ROW:	
						
					rowsPart.clear();
					
					#ifndef USE_DEQUE
						rowsPart.reserve(numFields);
					#endif
					
					for (i=0;i<numFields;++i)
						switch (sqlite3_column_type(liteStmt, i))
						{
							case SQLITE_INTEGER:
								
								rowsPart.push_back(tools::lToString(sqlite3_column_int(liteStmt, i)));
								
								break;
								
							case SQLITE_FLOAT:
								
								rowsPart.push_back(tools::dToString(sqlite3_column_double(liteStmt, i)));
								
								break;
								
							case SQLITE_TEXT:
								
								rowPart = (const char *)sqlite3_column_text(liteStmt, i);
								if (preventEscaping)
									rowsPart.push_back(rowPart);
								else
									rowsPart.push_back(unescapeFields(rowPart));
								
								break;
								
							case SQLITE_BLOB:
								
								rowPart.assign((const char *)sqlite3_column_blob(liteStmt, i), sqlite3_column_bytes(liteStmt, i));
								if (preventEscaping)
									rowsPart.push_back(rowPart);
								else
									rowsPart.push_back(unescapeFields(rowPart));
								
								break;
								
							case SQLITE_NULL:
							default:
							
								rowsPart.push_back("NULL");
								
								break;	
						}
						
						rows.push_back(rowsPart);
						
					break;
			}
		}

		#ifndef DBSQLITE_WO_XEXEC
			performXExec(postExec);
		#endif
				
		return rows;
	}
	
	//-------------------------------------------------------------------
	
	dodoStringArr
	dbSqlite::fetchField() const
	{	
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_FETCHFIELD;
			performXExec(preExec);
		#endif
		
		if (!show)
			return __stringarray__;
			
		unsigned int numFields = sqlite3_column_count(liteStmt);
		
		dodoStringArr fields;
		
		#ifndef USE_DEQUE
			fields.reserve(numFields);
		#endif
		
		for (unsigned int i(0);i<numFields;++i)
			fields.push_back(sqlite3_column_name(liteStmt, i));

		#ifndef DBSQLITE_WO_XEXEC
			performXExec(postExec);
		#endif			
					
		return fields;
	}
	
	//-------------------------------------------------------------------
	
	__dbStorage 
	dbSqlite::fetch() const
	{
		return __dbStorage(fetchRow(), fetchField());
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbSqlite::rowsCount() const
	{
		if (show)
		{
			sqlite3_reset(liteStmt);
		
			unsigned int numRows = 0;
			bool iterate = true;
			int result;
			
			while (iterate)
			{
				result = sqlite3_step(liteStmt);
				switch (result)
				{
					case SQLITE_BUSY:
					
						continue;
						
					case SQLITE_DONE:
					
						iterate = false;
						
						break;
						
					case SQLITE_ERROR:
					
						#ifndef NO_EX
							throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE_FETCHROW, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__);
						#else
							return false;
						#endif
						
					case SQLITE_ROW:	
					
						++numRows;
							
						break;
				}
			}	
			
			return numRows;	
		}
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbSqlite::fieldsCount() const
	{
		if (show)
			return sqlite3_column_count(liteStmt);
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	
	unsigned int
	dbSqlite::affectedRowsCount() const
	{
		if (!show)
			return sqlite3_changes(lite);
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::exec(const dodoString &query, 
					bool result)
	{
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_EXEC;
			performXExec(preExec);
		#endif
		
		#ifdef NO_EX
			bool _result = 
		#endif	
			_exec(query, result); 
		
		#ifndef DBSQLITE_WO_XEXEC		
			performXExec(postExec);
		#endif
		
		cleanCollect();
		
		#ifdef NO_EX
			return _result;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	#ifndef DBSQLITE_WO_XEXEC
	
		int 
		dbSqlite::addPostExec(inExec func, 
							void *data)
		{
			return _addPostExec(func, (void *)this, XEXECOBJ_DBSQLITE, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		dbSqlite::addPreExec(inExec func, 
							void *data)
		{
			return _addPreExec(func, (void *)this, XEXECOBJ_DBSQLITE, data);
		}
		
		//-------------------------------------------------------------------
		
		#ifdef DL_EXT
		
			int 
			dbSqlite::addPostExec(const dodoString &module, 
								void *data,
								void *toInit)
			{
				return _addPostExec(module, (void *)this, XEXECOBJ_DBSQLITE, data, toInit);
			}
			
			//-------------------------------------------------------------------
			
			int 
			dbSqlite::addPreExec(const dodoString &module, 
								void *data,
								void *toInit)
			{
				return _addPreExec(module, (void *)this, XEXECOBJ_DBSQLITE, data, toInit);
			}
		
			//-------------------------------------------------------------------
			
			xexecCounts 
			dbSqlite::addExec(const dodoString &module, 
								void *data,
								void *toInit)
			{
				return _addExec(module, (void *)this, XEXECOBJ_DBSQLITE, data, toInit);
			}
	
		#endif
	
		//-------------------------------------------------------------------
			
	#endif
	
	//-------------------------------------------------------------------	
	
	dodoStringMapArr 
	dbSqlite::fetchAssoc() const
	{
		if (!show)
			return __dodostringmap__;
		
		sqlite3_reset(liteStmt);
		
		unsigned int numFields = sqlite3_column_count(liteStmt);
		
		bool iterate = true;
		unsigned int i = 0;

		dodoStringMapArr rowsFields;
		dodoStringMap rowFieldsPart;
		dodoString rowPart;
		
		#ifndef USE_DEQUE
			rowsFields.reserve(sqlite3_data_count(liteStmt));
		#endif
		
		int result;
		
		while (iterate)
		{
			result = sqlite3_step(liteStmt);
			switch (result)
			{
				case SQLITE_BUSY:
				
					continue;
					
				case SQLITE_DONE:
				
					iterate = false;
					
					break;
					
				case SQLITE_ERROR:
				
					#ifndef NO_EX
						throw baseEx(ERRMODULE_DBSQLITE, DBSQLITE_FETCHASSOC, ERR_SQLITE, sqlite3_errcode(lite), sqlite3_errmsg(lite),__LINE__,__FILE__);
					#else
						return __dodostringmap__;
					#endif
					
				case SQLITE_ROW:	
						
					rowFieldsPart.clear();
					
					for (i=0;i<numFields;++i)
						switch (sqlite3_column_type(liteStmt, i))
						{
							case SQLITE_INTEGER:
								
								rowFieldsPart.contents[sqlite3_column_name(liteStmt, i)] = tools::lToString(sqlite3_column_int(liteStmt, i));
								
								break;
								
							case SQLITE_FLOAT:
								
								rowFieldsPart.contents[sqlite3_column_name(liteStmt, i)] = tools::dToString(sqlite3_column_double(liteStmt, i));
								
								break;
								
							case SQLITE_TEXT:
								
								rowPart = (const char *)sqlite3_column_text(liteStmt, i);
								if (preventEscaping)
									rowFieldsPart.contents[sqlite3_column_name(liteStmt, i)] = rowPart;
								else
									rowFieldsPart.contents[sqlite3_column_name(liteStmt, i)] = unescapeFields(rowPart);
								
								break;
								
							case SQLITE_BLOB:
								
								rowPart.assign((const char *)sqlite3_column_blob(liteStmt, i), sqlite3_column_bytes(liteStmt, i));
								if (preventEscaping)
									rowFieldsPart.contents[sqlite3_column_name(liteStmt, i)] = rowPart;
								else
									rowFieldsPart.contents[sqlite3_column_name(liteStmt, i)] = unescapeFields(rowPart);
								
								break;
								
							case SQLITE_NULL:
							default:
							
								rowFieldsPart.contents[sqlite3_column_name(liteStmt, i)] = "NULL";
								
								break;	
						}
						
						rowsFields.push_back(rowFieldsPart);
						
					break;			
			}
		}
		
		return rowsFields;	
	}

	//-------------------------------------------------------------------
	
	
#endif
