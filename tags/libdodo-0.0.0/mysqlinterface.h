/***************************************************************************
 *            mysqlinterface.h
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

#ifndef _MYSQLINTERFACE_H_
#define _MYSQLINTERFACE_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN
	#include <winsock.h>
#else
	#include <sys/socket.h>
#endif

#include <mysql.h>
#include <string.h>
#include <stdio.h>

#include "derectives.h"
#include "mysqlinterfaceEx.h"
#include "sqlBase.h"

namespace dodo
{
	enum mysqlErrStEnum
	{
		CONN = 0,
		QUERY,
		STORE_RES
	};
	/**
	* Error statements in mySQL
	*/
	static __sqlStatements mysqlErrStArr[] = /// format : '`function` : Error `errNum` (`errStr`)' 
	{
		{"mysql_real_connect():\nError %s (%s)\n",32},//CONN
		{"mysql_query():\nError %s (%s)\n",25},//QUERY
		{"mysql_store_result():\nError %s (%s)\n",32}//STORE_RES
	};	
	/**
	*	class to work with mysql through c++
	*	usage: see test.cc
	*/
	class mysqlpp : virtual public sqlBase
	{
		friend class mysqlppEx;///class of exception
		public:
	
			mysqlpp();
			mysqlpp(mysqlpp *a_mypp);
			virtual ~mysqlpp();	
		
			#ifndef NO_EX
				void connect();
			#else				
				bool connect();
			#endif
		
			virtual void disconnect();
		
			virtual int affectedRowsCount();
		
			virtual char ***fetchRow();// last is null
			virtual char **fetchField();// last is null 
			virtual __sqlStorage *fetch();
			/**
			 * sets delayed insert
			*/
			//virtual void delayedIns(bool set = true) const;///	
			/**
			 * executes request
			*/
			bool exec() const;

		private:

			mutable bool connected;//connected or not
			mutable bool empty;//for detectin' whether mysqlResult is empty or not
	
			mutable MYSQL *mysql;
			mutable MYSQL_RES *mysqlRes;
			mutable MYSQL_ROW mysqlRow;
			mutable MYSQL_FIELD *mysqlFields;
	};
};
#endif
