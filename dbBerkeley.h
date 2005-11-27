/***************************************************************************
 *            dbBerkeley.h
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

#ifndef _DBBERKELEY_H_
#define _DBBERKELEY_H_

#ifdef BERKELEY_EXT
	
	#include <db.h>
	
	#include "directives.h"
	#include "dbBerkeleyEx.h"
	#include "dbBase.h"
	#include "tools.h"
	#include "xexec.h"
	#include "flushDisk.h"
	
	namespace dodo
	{
		
		/**
		 * @enum dbBerkeleyOperTypeEnum describes type of operation for hook
		 */
		enum dbBerkeleyOperTypeEnum
		{
			DBBERKELEY_OPER_EXEC,
			DBBERKELEY_OPER_CONNECT,
			DBBERKELEY_OPER_DISCONNECT,
		};		
		
		/**
	 	 * @class dbBerkeley is an interface to Berkeley db through database independent interfaces
		 */
		class dbBerkeley : public dbBase, public xexec
		{
			private :
				/**
				 * constructor
				 * to prevent from copying
				 */
				dbBerkeley(dbBerkeley &berk);
			
			public:
					
				/**
				 * @return self, casted to base class - dodoBase; 
				 * usefull to cast from child to parent;
				 * used in hooks
				 */	
				virtual dodoBase * const getSelf();
				
				/**
				 * constructor
				 */
				dbBerkeley();
				
				/**
				 * destructor
				 */
				virtual ~dbBerkeley();	

				/**
				 * executes collected request
				 */				
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								exec() const;

				/**
				 * set function from module that will be executed before/after the main action call
				 * the type of hook[pre/post] is defined in module
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */			
				virtual int addExec(const std::string &module, void *data) const;
				
				/**
				 * adds hook after the operation by callback
			 	 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */			
				virtual int addPostExec(inExec func, void *data) const;
				
				/**
				 * adds hook before the operation by callback
			 	 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPreExec(inExec func, void *data) const;
				
				/**
				 * adds hook after the operation by callback
			 	 * @return number in list where function is set
				 * @param module is a path to module, whrere hook exists
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPostExec(const std::string &module, void *data) const;
				
				/**
				 * adds hook after the operation by callback
			 	 * @return number in list where function is set
				 * @param module is a path to module, whrere hook exists
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPreExec(const std::string &module, void *data) const;
				
				/**
				 * overloads from base class
				 * @param path is a path to database
				 * @param db is database name
				 */
				virtual void setDbInfo(const std::string &path, const std::string &db=__string__);
				
				/**
				 * open database
				 * @param flags indicates how to open database
				 * @note flags:
				 * 		DB_CREATE
				 * 		    Create the database. If the database does not already exist and the DB_CREATE flag is not specified, the DB->open will fail. 
				 * 		DB_DIRTY_READ
				 * 		    Support dirty reads; that is, read operations on the database may request the return of modified but not yet committed data. This flag must be specified on all DB handles used to perform dirty reads or database updates, otherwise requests for dirty reads may not be honored and the read may block. 
				 * 		DB_EXCL
				 * 		    Return an error if the database already exists. The DB_EXCL flag is only meaningful when specified with the DB_CREATE flag. 
				 * 		DB_NOMMAP
				 * 		    Do not map this database into process memory (see the DB_ENV->set_mp_mmapsize method for further information). 
				 * 		DB_RDONLY
				 * 		    Open the database for reading only. Any attempt to modify items in the database will fail, regardless of the actual permissions of any underlying files. 
				 * 		DB_THREAD
				 * 		    Cause the DB handle returned by DB->open to be free-threaded; that is, concurrently usable by multiple threads in the address space. 
				 * 		DB_TRUNCATE
				 * 			The DB_TRUNCATE flag cannot be lock or transaction-protected, and it is an error to specify it in a locking or transaction-protected environment.
				 * 
				 * @param type
				 * @note flags:
				 * 		The type parameter is of type DBTYPE, and must be set to one of DB_BTREE, DB_HASH, DB_QUEUE, DB_RECNO, or DB_UNKNOWN. 
				 * 		If type is DB_UNKNOWN, the database must already exist and DB->open will automatically determine its type.
				 * 
				 * @param mode specifies permissions to newly created database file
				 * @note see flushDisk for more info
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								connect(int flags, int type=DB_UNKNOWN, int mode=OWNER_ALL_ACCESS) const;				
				
				/**
				 * disconnect from database
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif				 
								disconnect() const;
				
			private:
			
				/**
				 * executes request
				 * pure mysql actions
				 * in function without `_` hooks are calling
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								_exec() const;		
				
				mutable DB *bdb;///< handler to database
				mutable bool connected;///< connected or not
		};
	};
#endif

#endif
