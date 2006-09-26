/***************************************************************************
 *            dbSqlBase.h
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
 
#ifndef _DBSQLBASE_H_
#define _DBSQLBASE_H_

#include <directives.h>

#include <dbSqlBaseEx.h>
#include <types.h>
#include <dbBase.h>

namespace dodo
{
	
	/**
	 * @class to provide wide abilities for sql manipulations
	 * be carefull!!! all data become escaped and fremed with <'>. Fields' names are not framed with <`> to prevent "`count(*)`" or smth else
	 * when u are usin' select wo table_name field names are not escaped and framed!!
	 * If u want to prevent data framin' set dbSqlBase.preventFraming = true; but remember u have to use <'> using compare with strings in 'where' statement
	 */
	class dbSqlBase : public dbBase
	{
		public:
							
			/*
			 * constructor
			 */
			dbSqlBase();	
			
			/**
			 * destructor
			 */
			virtual ~dbSqlBase();
			
			/**
			 * @return constructed query from collected data;
			 * it doesn't clean collected data
			 */
			virtual std::string queryCollect() const;
			
			/**
			 * @returns string in 'exist()'
			 * @param statement is sql statement that you want to cover with 'exist()'
			 */	
			static std::string exists(const std::string &statement);
			
			/**
			 * @returns string in 'not exist()'
			 * @param statement is sql statement that you want to cover with 'not exist()'
			 */	
			static std::string noexists(const std::string &statement);
		
			bool preventFraming;///< to frame or not with `'` fields values in insert and update; false by default

			mutable std::vector<stringArr> rows;///< to store rows
			mutable stringArr fields;///< to store fields
			mutable dodoStringMapArr rowsFields;///< to store arrau of hashes 'field'=>'row'
			
			mutable std::string request;///< ready sql statement
						
		protected:		
		
			/**
			 * @return escaped string
			 * @param data is string to escape
			 */
			inline static std::string escapeFields(const std::string &data);
			
			/**
			 * constructs from collected data to SELECT sql statement
			 */
			virtual void selectCollect() const;
			
			/**
			 * constructs from collected data to INSERT sql statement
			 */
			virtual void insertCollect() const;
			
			/**
			 * constructs from collected data to INSERT ... SELECT sql statement
			 */
			virtual void insertSelectCollect() const;
			
			/**
			 * constructs from collected data to UPDATE sql statement
			 */
			virtual void updateCollect() const;     
			
			/**
			 * constructs from collected data to DELETE sql statement
			 */
			virtual void delCollect() const;
			
			/**
			 * constructs from collected data to USE sql statement
			 */
			virtual void useCollect() const;
			
			/**
			 * constructs from collected data to UNION, MINUS, INTERSECT sql statement
			 */
			virtual void subCollect() const;
			
			/**
			 * constructs from collected data to TRUNCATE sql statement
			 */
			virtual void truncateCollect() const;
			
			/**
			 * constructs from collected data to DROP DATABASE sql statement
			 */
			virtual void delBaseCollect() const;
			
			/**
			 * constructs from collected data to DROP TABLE sql statement
			 */
			virtual void delTableCollect() const;
			
			/**
			 * constructs from collected data to ALTER ... DROP sql statement
			 */
			virtual void delFieldCollect() const;
			
			/**
			 * constructs from collected data to RENAME sql statement
			 * FIXME: implement it!
			 */
			virtual void renameBaseCollect() const;
			
			/**
			 * constructs from collected data to RENAME sql statement
			 */
			virtual void renameTableCollect() const;
			
			/**
			 * constructs from collected data to RENAME sql statement
			 * FIXME: implement it!
			 */
			virtual void renameFieldCollect() const;
			
			/**
			 * constructs from collected data to CREATE DATABASE sql statement
			 */
			virtual void createBaseCollect() const;
			
			/**
			 * constructs from collected data to CREATE DATABASE sql statement
			 */
			virtual void createTableCollect() const;
			
			/**
			 * constructs from collected data to ALTER TABLE ... ADD sql statement
			 */
			virtual void createFieldCollect() const;
			
			/**
			 * constructs from collected data to sql adaptive field info for proper fields representation
			 */
			virtual std::string fieldCollect(__fieldInfo &row) const;
			
			/**
			 * adds to the end of request additional data collection for query if check passed
			 * @param qTypeTocheck indicates what type of additional info to check
			 * @collectedString is string that holds additional statement
			 */
			virtual void additionalCollect(unsigned int qTypeTocheck, const std::string &collectedString) const;
			
			/**
			 * @return string constructed from collected additional statements
			 * @param sqlAddEnumArr is array of statements[position in sqlAddArr]
			 * @param sqlAddArr is array of statements
			 * @param qTypeShift is value that indicates what values were set
			 */
			virtual std::string insideAddCollect(const unsigned int sqlAddEnumArr[], const __statements sqlAddArr[], int qTypeShift) const;
			
			/**
			 * @return string constructed from collected additional statements(DB-dependent)
			 * @param statement is array of statements
			 * @param qTypeShift is value that indicates what values were set
			 */
			virtual std::string insideAddCollect(const stringArr &statements, int qTypeShift) const;
		
			/**
			 * @return string from fields' names and 'em values
			 * @param fieldsVal is array of fields
			 * @param fieldsNames is array of names
			 * @param frame is value with what values will be framed
			 */
			virtual std::string fieldsValName(const stringArr &fieldsVal, const stringArr &fieldsNames, const std::string &frame="'") const;
			
			mutable std::string auto_increment;///< AUTO_INCREMENT syntax. that's funny, but some understans AUTO_INCREMENT, others AUTOINCREMENT =); by default is AUTO_INCREMENT; have to redefine in derived class if differs;
		
		private:
			
			/**
			 * @note share vars
			 */
			mutable std::string temp;///< to store temporary data
			mutable std::string temp_;///< to store temporary data
			mutable unsigned int temp_bit;///< to store temporary data
			
			mutable bool additionalActions;///< indicates whether to make additional actions [where, limit]
			mutable bool selectAction;///< indicates type of request
			
			mutable stringArr::const_iterator i;///< iterator for array of strings
			mutable stringArr::const_iterator j;///< iterator for array of strings

			mutable stringArr fieldsVPart;///< temporary places request's parts
			
	};

};

#endif
