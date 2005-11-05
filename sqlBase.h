/***************************************************************************
 *            sqlBase.h
 *
 *  Mon Jul 18 19:13:33 2005
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
 
#ifndef _SQLBASE_H_
#define _SQLBASE_H_

#include "directives.h"
#include "types.h"
#include "sqlBaseEx.h"
#include "dodoBase.h"
#include "tools.h"
#include "dbBase.h"

namespace dodo
{
	
	/**
	 * class to provide wide abilities for sql manipulations
	 * be carefull!!! all data become escaped and fremed with <'>. Fields' names are not framed with <`> to prevent "`count(*)`" or smth else
	 * when u are usin' select wo table_name field names are not escaped and framed!!
	 * If u want to prevent data framin' set sqlBase.preventFraming = true; but remember u have to use <'> using compare with strings in 'where' statement
	 */
	class sqlBase : public dbBase
	{
		friend class sqlBaseEx;///class of exception
		public:
			virtual dodoBase *getSelf();
			/*
			 * constructors and destructors
			 */
			sqlBase();	
			virtual ~sqlBase();
			/**
			 * performs query collect from collected data;
			 * it doesn't clean collected data
			 */
			virtual std::string queryCollect() const;///collect data into query
			/**
			 * returns string in exist()
			 */	
			static std::string exists(const std::string &statement);
			static std::string noexists(const std::string &statement);
		
			bool preventFraming;///to frame or not with `'` fields values in insert...
			
		protected:		
		
			inline static std::string escapeFields(const std::string &a_data);
			/**
			 * functions to collect data into query after
			 */		
			virtual void selectCollect() const;
			virtual void insertCollect() const;
			virtual void insertSelectCollect() const;
			virtual void updateCollect() const;     
			virtual void delCollect() const;
			virtual void useCollect() const;
			virtual void subCollect() const;///union(union all), minus, intersect
			virtual void truncateCollect() const;
			virtual void delBaseCollect() const;
			virtual void delTableCollect() const;
			virtual void delFieldCollect() const;
			virtual void renameBaseCollect() const;
			virtual void renameTableCollect() const;
			virtual void renameFieldCollect() const;
			virtual void createBaseCollect() const;
			virtual void createTableCollect() const;
			virtual void createFieldCollect() const;
			inline virtual std::string fieldCollect(__fieldInfo &row) const;
			/**
			 * adds to the end of request additional data collection for query
			 */
			virtual void additionalCollect(unsigned int qTypeTocheck, std::string collectedString) const;
			/**
			 * return string that consists of collected data for request inside
			 */
			virtual std::string insideAddCollect(unsigned int sqlAddEnumArr[], __statements sqlAddArr[], int qTypeShift) const;
			/**
			 * return string that consists of collected data for request inside(DB-dependent)
			 */
			virtual std::string insideAddCollect(std::list<std::string> &statements, int qTypeShift) const;
		
			///creates string from fields' names and 'em values
			static std::string fieldsValName(const stringArr &fieldsVal, const stringArr &fieldsNames, const std::string &frame="'");
			
			mutable std::string request;///ready sql statement
	};
};
#endif /* _SQLBASE_H_ */
