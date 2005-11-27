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

	
	dbBerkeley::dbBerkeley()
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
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbBerkeley::setSqlInfo(const std::string &db)
	{
		dbInfo.path = db;
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
	
#endif
