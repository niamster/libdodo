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
	
	namespace dodo
	{
		
		/**
		 * @enum dbBerkeleyOperTypeEnum describes type of operation for hook
		 */
		enum dbBerkeleyOperTypeEnum
		{
			DBBERKELEY_OPER_EXEC,
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
				 * @param db is a path to database
				 */
				virtual void setSqlInfo(const std::string &db);
				
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
				
				DB *bdb;
		};
	};
#endif

#endif
