/***************************************************************************
 *            dbInterface.h
 *
 *  Sun Jan  15 19:45:19 2005
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

#ifndef _DBINTERFACE_H_
#define _DBINTERFACE_H_

#include <directives.h>

#include <dbSqlBase.h>

namespace dodo
{

	/**
 	 * @class dbInterface is an interface to mysql db through sql-,database- independent interfaces; 
 	 * you should derive from it if you want to write 'plugin' for your database
	 */
	class dbInterface : public dbSqlBase
	{
		private:
			
			/**
			 * constructor
			 * to prevent from copying
			 */			
			dbInterface(dbInterface &a_pp);
		
		public:
		
			dbInterface();
		
			/**
			 * destructor
			 */
			virtual ~dbInterface();
		
			/**
			 * connect to database
			 */	
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							connect() const;
			
			/**
			 * disconnect from database
			 */
			virtual void disconnect() const;
			
			/**
			 * @return amount of affected rows(update,delete...)
			 */
			virtual unsigned int affectedRowsCount();
			
			/**
			 * @return amount of rows got from request(select ...)
			 */
			virtual unsigned int rowsCount() const;				
			
			/**
			 * @return amount of fields got from request(select ...)
			 */
			virtual unsigned int fieldsCount() const;
			
			/**
			 * @return array of rows got from request
			 */
			virtual std::vector<stringArr> fetchRow() const;
			
			/**
			 * @return array of fields got from request
			 */
			virtual stringArr fetchField() const;
			
			/**
			 * @return structure that holds array of rows and array of fields got from request
			 */
			virtual __dbStorage fetch() const;
			
			/**
			 * @return array that holds assoc array['fiels'=>'value'] got from request
			 */
			virtual dodoStringMapArr fetchAssoc() const;

			/**
			 * executes collected request
			 * @param query contains query for DB. You may pass it if you don't use methods like select, update of libdodo
			 * @param result describes whether request returns result[show, select...] or not[delete, update]
			 */				
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							exec(const std::string &query = __string__, bool result = false) const;
									
		protected:
		
			mutable bool connected;///< connected or not
		
	};
	
};

#endif
