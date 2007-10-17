/***************************************************************************
 *            dbSqlite.cc
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

#include <dbSqlite.h>

#ifdef SQLITE_EXT
	
	#include <dbSqliteEx.h>
	#include <tools.h>
	
	using namespace dodo;

	
	dbSqlite::dbSqlite() : rowsNum(-1),
							fieldsNum(-1)
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
			sqlite3_close(lite);
		}
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::connect() const
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
				throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE_CONNECT,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
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
	dbSqlite::disconnect() const
	{
		if (connected)
		{
			#ifndef DBSQLITE_WO_XEXEC
				operType = DBSQLITE_OPER_DISCONNECT;
				performXExec(preExec);
			#endif
			
			if (sqlite3_close(lite) != SQLITE_OK)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE_DISCONNECT,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
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
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::_exec(const std::string &query, 
					bool result) const
	{	
		if (query.size()==0)
			queryCollect();			
		else
		{
			request = query;
			show = result;
		}

		callBackData.data = (dbSqlite *)this;
		callBackData.first = true;
		
		rowsNum = 0;
		fieldsNum = 0;
		fields.clear();
		rows.clear();

		if (sqlite3_exec(lite,request.c_str(),sqlite_callback,(void *)&callBackData, NULL)!=SQLITE_OK)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE__EXEC,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	std::vector<stringArr>
	dbSqlite::fetchRow() const
	{
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_FETCHROW;
			performXExec(preExec);
		#endif		
			
		if (!show)
			return __stringarrayvector__;

		#ifndef DBSQLITE_WO_XEXEC
			performXExec(postExec);
		#endif
				
		return rows;
	}
	
	//-------------------------------------------------------------------
	
	stringArr
	dbSqlite::fetchField() const
	{	
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_FETCHFIELD;
			performXExec(preExec);
		#endif
					
		if (!show)
			return __stringarray__;

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
		if (!show)
			return rowsNum;
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbSqlite::fieldsCount() const
	{
		if (!show)
			return fieldsNum;
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	
	unsigned int
	dbSqlite::affectedRowsCount()
	{
		if (show)
			return rowsNum;
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::exec(const std::string &query, 
					bool result) const
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
							void *data) const
		{
			return _addPostExec(func, (void *)this, XEXECOBJ_DBSQLITE, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		dbSqlite::addPreExec(inExec func, 
							void *data) const
		{
			return _addPreExec(func, (void *)this, XEXECOBJ_DBSQLITE, data);
		}
		
		//-------------------------------------------------------------------
		
		#ifdef DL_EXT
		
			int 
			dbSqlite::addPostExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addPostExec(module, (void *)this, XEXECOBJ_DBSQLITE, data, toInit);
			}
			
			//-------------------------------------------------------------------
			
			int 
			dbSqlite::addPreExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addPreExec(module, (void *)this, XEXECOBJ_DBSQLITE, data, toInit);
			}
		
			//-------------------------------------------------------------------
			
			xexecCounts 
			dbSqlite::addExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addExec(module, (void *)this, XEXECOBJ_DBSQLITE, data, toInit);
			}
		
		#endif
	
		//-------------------------------------------------------------------
			
	#endif
	
	//-------------------------------------------------------------------	
	
	int 
	dbSqlite::sqlite_callback(void *data, 
							int argc, 
							char **argv, 
							char **azColName)
	{
		__sqliteCallbackData *liteData = (__sqliteCallbackData *)data;
		
		liteData->data->rowPart.clear();
		liteData->data->rowPart.reserve(argc);
		
		++liteData->data->rowsNum;
		
		for (register int i(0);i<argc;++i)
		{
			liteData->data->rowPart.push_back(argv[i]!=NULL?argv[i]:"NULL");
			if (liteData->first)
				liteData->data->fields.push_back(azColName[i]);
		}
		
		liteData->data->rows.push_back(liteData->data->rowPart);
		
		if (liteData->first)
		{
			liteData->data->fieldsNum = argc;
			liteData->first = false;
		}
		
		return 0;
	}
	
	//-------------------------------------------------------------------
	
	dodoStringMapArr 
	dbSqlite::fetchAssoc() const
	{
		if (!show)
			return __dodostringmap__;
		
		j = fields.end();
		k = rows.begin();
		l = rows.end();
		
		rowFieldsPart.clear();
		
		for (;k!=l;++k)
		{
			for (i=fields.begin(), o=k->begin();i!=j;++i,++o)
				rowFieldsPart.realArr[*i] = *o;
				
			rowsFields.push_back(rowFieldsPart);
		}
		
		return rowsFields;
	}

	//-------------------------------------------------------------------
	
	
#endif
