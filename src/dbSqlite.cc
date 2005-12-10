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
	
	using namespace dodo;

	
	dbSqlite::dbSqlite() : connected(false),
						empty(true)
	{
		addSQL();
	}
	
	//-------------------------------------------------------------------
	
	dbSqlite::dbSqlite(dbSqlite &a_pp)
	{
	}
	
	//-------------------------------------------------------------------
	
	dbSqlite::~dbSqlite()
	{
		if (!empty)
			;
			
		disconnect();
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbSqlite::addSQL()
	{
		/*sqlDbDepAddInsArr.push_back(" delayed ");
		
		sqlDbDepAddSelArr.push_back(" sql_big_result ");
		
		sqlDbDepAddUpArr.push_back(" low_priority ");
	
		sqlDbDepAddDelArr.push_back(" quick ");	*/
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbSqlite::setLiteAddInsSt(sqliteAddInsEnum statement)
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
	dbSqlite::setLiteAddUpSt(sqliteAddUpEnum statement)
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
	dbSqlite::setLiteAddSelSt(sqliteAddSelEnum statement)
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
	dbSqlite::setLiteAddDelSt(sqliteAddDelEnum statement)
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
	dbSqlite::unsetLiteAddInsSt(sqliteAddInsEnum statement)
	{
		removeF(qDbDepInsShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbSqlite::unsetLiteAddUpSt(sqliteAddUpEnum statement)
	{
		removeF(qDbDepUpShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbSqlite::unsetLiteAddSelSt(sqliteAddSelEnum statement)
	{
		removeF(qDbDepSelShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbSqlite::unsetLiteAddDelSt(sqliteAddDelEnum statement)
	{
		removeF(qDbDepDelShift,1<<statement);	
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
	dbSqlite::_exec() const
	{	
		queryCollect();
		
		
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	std::vector<stringArr>
	dbSqlite::fetchRow()
	{
			
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_FETCHROW;
			performXExec(preExec);
		#endif		
			
		if (empty || !show)
			return std::vector<stringArr>();
			
		rows.clear();
		rows.reserve(0);

		#ifndef DBSQLITE_WO_XEXEC
			performXExec(postExec);
		#endif
				
		return rows;
	}
	
	//-------------------------------------------------------------------
	
	stringArr
	dbSqlite::fetchField()
	{	
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_FETCHFIELD;
			performXExec(preExec);
		#endif
					
		if (empty || !show)
			return stringArr();
			
		fields.clear();
		fields.reserve(0);

		#ifndef DBSQLITE_WO_XEXEC
			performXExec(postExec);
		#endif			
					
		return fields;
	}
	
	//-------------------------------------------------------------------
	
	__sqlStorage 
	dbSqlite::fetch()
	{
		return __sqlStorage(fetchRow(), fetchField());
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbSqlite::rowsCount()
	{
		if (empty || !show)
			return 0;
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbSqlite::fieldsCount()
	{
		if (empty || !show)
			return 0;
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	
	unsigned int
	dbSqlite::affectedRowsCount()
	{
		if (empty || show)
			return 0;
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::exec() const
	{
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_EXEC;
			performXExec(preExec);
		#endif
		
		#ifdef NO_EX
			bool result = 
		#endif	
			_exec(); 
		
		#ifndef DBSQLITE_WO_XEXEC		
			performXExec(postExec);
		#endif
		
		cleanCollect();
		
		#ifdef NO_EX
			return result;
		#endif
	}
	
	//-------------------------------------------------------------------
	
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
							void *data) const
		{
			return _addPostExec(module, (void *)this, XEXECOBJ_DBSQLITE, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		dbSqlite::addPreExec(const std::string &module, 
							void *data) const
		{
			return _addPreExec(module, (void *)this, XEXECOBJ_DBSQLITE, data);
		}
	
		//-------------------------------------------------------------------
		
		int 
		dbSqlite::addExec(const std::string &module, 
							void *data) const
		{
			return _addExec(module, (void *)this, XEXECOBJ_DBSQLITE, data);
		}
	
	#endif
	
	//-------------------------------------------------------------------
	
#endif
