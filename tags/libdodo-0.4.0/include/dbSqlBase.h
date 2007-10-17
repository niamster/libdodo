/***************************************************************************
 *            dbSqlBase.h
 *
 *  Mon Jul 18 19:13:33 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _DBSQLBASE_H_
#define _DBSQLBASE_H_

#include <directives.h>

#include <tools.h>
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
			virtual dodoString queryCollect();
			
			/**
			 * @returns string in 'exist()'
			 * @param statement is sql statement that you want to cover with 'exist()'
			 */	
			static dodoString exists(const dodoString &statement);
			
			/**
			 * @returns string in 'not exist()'
			 * @param statement is sql statement that you want to cover with 'not exist()'
			 */	
			static dodoString noexists(const dodoString &statement);
		
			bool preventFraming;///< to frame or not with `'` fields values in insert and update; false by default
			
			bool preventEscaping;///< to escape{\,'} or not fields' values in insert and update; false by default
			
			bool autoFraming;///< detects automatic whether to frame or not; true by default; if preventFraming is set - autoFraming is disabled; if preventEscaping is false - escaping only framed values
			
			dodoString request;///< ready sql statement with `'` fields values in insert and update; true by default

			/**
			 * @return escaped string
			 * @param data is string to escape
			 */
			static dodoString escapeFields(const dodoString &data);
		
			/**
			 * @return unescaped string
			 * @param data is string to unescape
			 */
			static dodoString unescapeFields(const dodoString &data);			
						
		protected:		
			
			/**
			 * constructs from collected data to SELECT sql statement
			 */
			virtual void selectCollect();
			
			/**
			 * constructs from collected data to INSERT sql statement
			 */
			virtual void insertCollect();
			
			/**
			 * constructs from collected data to INSERT ... SELECT sql statement
			 */
			virtual void insertSelectCollect();
			
			/**
			 * constructs from collected data to UPDATE sql statement
			 */
			virtual void updateCollect();     
			
			/**
			 * constructs from collected data to DELETE sql statement
			 */
			virtual void delCollect();
			
			/**
			 * constructs from collected data to UNION, MINUS, INTERSECT sql statement
			 */
			virtual void subCollect();
			
			/**
			 * constructs from collected data to TRUNCATE sql statement
			 */
			virtual void truncateCollect();
			
			/**
			 * constructs from collected data to DROP DATABASE sql statement
			 */
			virtual void delBaseCollect();
			
			/**
			 * constructs from collected data to DROP TABLE sql statement
			 */
			virtual void delTableCollect();
			
			/**
			 * constructs from collected data to ALTER ... DROP sql statement
			 */
			virtual void delFieldCollect();
			
			/**
			 * constructs from collected data to RENAME sql statement
			 * FIXME: implement it!
			 */
			virtual void renameBaseCollect();
			
			/**
			 * constructs from collected data to RENAME sql statement
			 */
			virtual void renameTableCollect();
			
			/**
			 * constructs from collected data to RENAME sql statement
			 * FIXME: implement it!
			 */
			virtual void renameFieldCollect();
			
			/**
			 * constructs from collected data to CREATE DATABASE sql statement
			 */
			virtual void createBaseCollect();
			
			/**
			 * constructs from collected data to CREATE TABLE sql statement
			 */
			virtual void createTableCollect();
			
			/**
			 * constructs from collected data to CREATE INDEX sql statement
			 */
			virtual void createIndexCollect();
			
			/**
			 * constructs from collected data to DROP INDEX sql statement
			 */
			virtual void deleteIndexCollect();
			
			/**
			 * constructs from collected data to ALTER TABLE ... ADD sql statement
			 */
			virtual void createFieldCollect();
			
			/**
			 * constructs from collected data to sql adaptive field info for proper fields representation
			 */
			virtual dodoString fieldCollect(__fieldInfo &row);
			
			/**
			 * adds to the end of request additional data collection for query if check passed
			 * @param qTypeTocheck indicates what type of additional info to check
			 * @collectedString is string that holds additional statement
			 */
			virtual void additionalCollect(unsigned int qTypeTocheck, const dodoString &collectedString);
			
			/**
			 * @return string constructed from collected additional statements
			 * @param sqlAddEnumArr is array of statements[position in sqlAddArr]
			 * @param sqlAddArr is array of statements
			 * @param qTypeShift is value that indicates what values were set
			 */
			virtual dodoString insideAddCollect(const unsigned int sqlAddEnumArr[], const __statements sqlAddArr[], int qTypeShift);
			
			/**
			 * @return string constructed from collected additional statements(DB-dependent)
			 * @param statement is array of statements
			 * @param qTypeShift is value that indicates what values were set
			 */
			virtual dodoString insideAddCollect(const dodoStringArr &statements, int qTypeShift);
		
			/**
			 * @return string from fields' names and 'em values
			 * @param fieldsVal is array of fields
			 * @param fieldsNames is array of names
			 * @param frame is value with what values will be framed
			 */
			virtual dodoString fieldsValName(const dodoStringArr &fieldsVal, const dodoStringArr &fieldsNames, const dodoString &frame="'");
	
			/**
			 * @return sql compliant data type
			 * @param type indicates the data type
			 */
			virtual dodoString stringType(int type);
			
			/**
			 * @return : if type must have range = 1; if may have = 0; if mustn't have = -1;
			 * @param type indicates the data type
			 */
			virtual int chkRange(int type);
			
			/**
			 * @return sql compliant references statements
			 * @param type indicates the type of reference
			 */
			virtual dodoString stringReference(int type);
						
			dodoString auto_increment;///< AUTO_INCREMENT syntax. that's funny, but some understans AUTO_INCREMENT, others AUTOINCREMENT =); by default is AUTO_INCREMENT; have to redefine in derived class if differs;
			dodoString blob;///< bytea for postgres, blob for others 
			dodoString tinyblob;///< bytea for postgres, blob for others 
			dodoString mediumblob;///< bytea for postgres, blob for others 
			dodoString longblob;///< bytea for postgres, blob for others 
		
			__dodoMap<dodoStringArr> framingFields;///< hash of 'db:table' => `array of fields to frame`
		
		private:
			
			static const __statements sqlAddSelArr[3];///< `select` additional statement
			static const __statements sqlAddDelArr[2];///< `delete` additional statement
			static const __statements sqlAddUpArr[2];///< `update` additional statement
			static const __statements sqlAddInsArr[2];///< `insert` additional statement
			
			static const __statements sqlAddArr[7];///< additional sql statements for queries(where, limit, ...)
			
			static const __statements sqlQStArr[4];///< sql statements for complex queries(union, ...)
			
			static const unsigned int addSelEnumArr[2];///< array of positions in sqlAddSelArr `select` additional statement
			static const unsigned int addDelEnumArr[1];///< array of positions in sqlAddDelArr `delete` additional statement
			static const unsigned int addUpEnumArr[1];///< array of positions in sqlAddUpArr `update` additional statement
			static const unsigned int addInsEnumArr[1];///< array of positions in sqlAddInsArr `insert` additional statement
			
	};

};

#endif
