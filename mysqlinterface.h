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
 *  aint with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _MYSQLINTERFACE_H_
#define _MYSQLINTERFACE_H_

#include <cstring>

#ifdef WIN
	#include <winsock.h>
#else
	#include <sys/socket.h>
#endif

#include <mysql.h>

#include "directives.h"
#include "mysqlinterfaceEx.h"
#include "sqlBase.h"
#include "tools.h"
#include "xexec.h"

namespace dodo
{
	
	enum mysqlAddSelEnum
	{
		SELECT_STRAIGHT_JOIN = 1,
		SELECT_SMALL_RESULT,
		SELECT_BIG_RESULT,
	};
	enum mysqlAddDelEnum
	{
		DELETE_LOW_PRIORITY = 1,
		DELETE_QUICK
	};
	enum mysqlAddUpEnum
	{
		UPDATE_LOW_PRIORITY = 1,
	};
	enum mysqlAddInsEnum
	{
		INSERT_DELAYED = 1,
		INSERT_LOW_PRIORITY,
		INSERT_HIGH_PRIORITY,
	};
	
	/**
	*	class to work with mysql through c++
	*	usage: see test.cc
	*/
	class mysqlpp : virtual public dodoBase, virtual public sqlBase, virtual public xexec
	{
		friend class mysqlppEx;///class of exception
		public:
			virtual mysqlpp *getSelf();
		
			mysqlpp();
			mysqlpp(mysqlpp *a_mypp);
			virtual ~mysqlpp();	
		
			/**
			* type - type of connection - se mySQL documentation for more!
			* 	CLIENT_COMPRESS 	Use compression protocol.
			*	CLIENT_FOUND_ROWS 	Return the number of found (matched) rows, not the number of affected rows.
			*	CLIENT_IGNORE_SPACE 	Allow spaces after function names. Makes all functions names reserved words.
			*	CLIENT_INTERACTIVE 	Allow interactive_timeout seconds (instead of wait_timeout seconds) of inactivity before closing the connection. The client's session wait_timeout variable is set to the value of the session interactive_timeout variable.
			*	CLIENT_LOCAL_FILES 	Enable LOAD DATA LOCAL handling.
			*	CLIENT_MULTI_STATEMENTS 	Tell the server that the client may send multiple statements in a single string (separated by ?;?). If this flag is not set, multiple-statement execution is disabled. New in 4.1.
			*	CLIENT_MULTI_RESULTS 	Tell the server that the client can handle multiple result sets from multiple-statement executions or stored procedures. This is automatically set if CLIENT_MULTI_STATEMENTS is set. New in 4.1.
			*	CLIENT_SSL 	Use SSL (encrypted protocol). This option should not be set by application programs; it is set internally in the client library.
			*/ 
			#ifndef NO_EX
				void connect(unsigned long type=CLIENT_MULTI_STATEMENTS) const;
			#else				
				bool connect() const;
			#endif
		
			virtual void disconnect() const;
		
			virtual unsigned int affectedRowsCount();
			virtual unsigned int rowsCount();
			virtual unsigned int fieldsCount();
		
			virtual std::vector<stringArr> fetchRow();
			virtual stringArr fetchField();
			virtual __sqlStorage fetch();
		
			/**
			* functions to set(unset) additional parameters for standart qTypes
			*/
			virtual void setMyAddInsSt(unsigned int statement);
			virtual void setMyAddUpSt(unsigned int statement);
			virtual void setMyAddSelSt(unsigned int statement);
			virtual void setMyAddDelSt(unsigned int statement);
			
			virtual void unsetMyAddInsSt(unsigned int statement);
			virtual void unsetMyAddUpSt(unsigned int statement);
			virtual void unsetMyAddSelSt(unsigned int statement);
			virtual void unsetMyAddDelSt(unsigned int statement);

			virtual std::string getErr() const;///get err statement
			/**
			* executes request
			*/
			virtual bool exec() const;
			
			virtual int myAddPostExec(inExec func, void *data) const;
			virtual int myAddPreExec(inExec func, void *data) const;
		private:
			/**
			 * executes request
			*/
			bool _exec() const;		
		
			virtual void addSQL();
		
			mutable bool connected;//connected or not
			mutable bool empty;//for detectin' whether mysqlResult is empty or not
			
			mutable std::string err;///string that contains err
		
			mutable MYSQL *mysql;
			mutable MYSQL_RES *mysqlRes;
			mutable MYSQL_ROW mysqlRow;
			mutable MYSQL_FIELD *mysqlFields;
	};
};
#endif
