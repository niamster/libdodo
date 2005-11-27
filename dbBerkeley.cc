/***************************************************************************
 *            dbBerkeley.cc
 *
 *  Sun Nov 27 08:45:19 2005
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

#ifdef BERKELEY_EXT

	#include "dbBerkeley.h"
	
	using namespace dodo;

	dodoBase * const 
	dbBerkeley::getSelf()
	{
		return dynamic_cast<dodoBase *>(this);
	}
		
	//-------------------------------------------------------------------

	
	dbBerkeley::dbBerkeley() : connected(false)
	{
		register int result = db_create(&bdb, NULL, 0);
		if (result!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBBERKELEY,DBMYSQL_CONSTRUCTOR,ERR_BERKELEY,result,db_strerror(result),__LINE__,__FILE__);
			#else
				;
			#endif
	}
	
	//-------------------------------------------------------------------
	
	dbBerkeley::dbBerkeley(dbBerkeley &a_mypp)
	{
	}
	
	//-------------------------------------------------------------------
	
	dbBerkeley::~dbBerkeley()
	{
		disconnect();
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbBerkeley::setDbInfo(const std::string &path,
							const std::string &db)
	{
		dbInfo.db = db;
		dbInfo.path = path;
	}

	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbBerkeley::_exec() const
	{	

	}
		
	//-------------------------------------------------------------------

	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbBerkeley::exec() const
	{
		#ifndef DBBERKELEY_WO_XEXEC
			operType = DBBERKELEY_OPER_EXEC;
			performXExec(preExec);
		#endif
		
		#ifdef NO_EX
			bool result = 
		#endif	
			_exec(); 
		
		#ifndef DBBERKELEY_WO_XEXEC		
			performXExec(postExec);
		#endif
		
		cleanCollect();
		
		#ifdef NO_EX
			return result;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	int 
	dbBerkeley::addPostExec(inExec func, 
						void *data) const
	{
		return _addPostExec(func, (dodoBase *)this, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	dbBerkeley::addPreExec(inExec func, 
						void *data) const
	{
		return _addPreExec(func, (dodoBase *)this, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	dbBerkeley::addPostExec(const std::string &module, 
						void *data) const
	{
		return _addPostExec(module, (dodoBase *)this, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	dbBerkeley::addPreExec(const std::string &module, 
						void *data) const
	{
		return _addPreExec(module, (dodoBase *)this, data);
	}

	//-------------------------------------------------------------------
	
	int 
	dbBerkeley::addExec(const std::string &module, 
						void *data) const
	{
		return _addExec(module, (dodoBase *)this, data);
	}

	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif	
	dbBerkeley::connect(int flags, int type, int mode) const
	{
		if (connected)
			disconnect();
			
		#ifndef DBBERKELEY_WO_XEXEC
			operType = DBBERKELEY_OPER_CONNECT;
			performXExec(preExec);
		#endif
		
		register int result = bdb->open(bdb, 
										NULL, 
										dbInfo.path.c_str(), 
										dbInfo.db.size()>0?dbInfo.db.c_str():NULL, 
										(DBTYPE)type, 
										flags, 
										((DB_CREATE&flags)==DB_CREATE)?flushDisk::getPermission(mode):0);
		if (result!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBBERKELEY,DBMYSQL_CONNECT,ERR_BERKELEY,result,db_strerror(result),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		connected = true;	
			
		#ifndef DBBERKELEY_WO_XEXEC		
			performXExec(postExec);
		#endif			
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbBerkeley::disconnect() const
	{
		if (connected)
		{
			#ifndef DBBERKELEY_WO_XEXEC
				operType = DBBERKELEY_OPER_DISCONNECT;
				performXExec(preExec);
			#endif

			register int result = bdb->close(bdb,0);

			if (result!=0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBBERKELEY,DBMYSQL_DISCONNECT,ERR_BERKELEY,result,db_strerror(result),__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			connected = false;	
				
			#ifndef DBBERKELEY_WO_XEXEC		
				performXExec(postExec);
			#endif			
			
		}
	}
		
	//-------------------------------------------------------------------
	
#endif
