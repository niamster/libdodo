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

#include <libdodo/directives.h>

#include <libdodo/tools.h>
#include <libdodo/dbSqlBaseEx.h>
#include <libdodo/types.h>
#include <libdodo/dbBase.h>

namespace dodo
{

	/**
	 * @class dbSqlBase provide construction of SQL request
	 * @note all data become escaped and framed with '. Names of fields are not framed with ` to prevent cases as `count(*)`, etc.
	 * If you want to prevent data framing define preventFraming dbSqlBase class propertie as true but remember
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
			 * @return constructed query from collected data
			 * @note it doesn't clean collected data
			 */
			virtual dodoString queryCollect();

			/**
			 * @return string in `exist()` statement
			 * @param statement defines an SQL statement that will be covered with `exist()` statement
			 */
			static dodoString exists(const dodoString &statement);

			/**
			 * @return string in `exist()` statement
			 * @param statement defines an SQL statement that will be covered with `exist()` statement
			 */
			static dodoString notexists(const dodoString &statement);

			bool preventFraming;    ///< if true values of fields will be framed with ' in `insert` and `update` statements[false by default]

			bool preventEscaping;   ///< if true values of fields {\,'} will be escaped in `insert` and `update` statements[false by default]

			bool autoFraming;       ///< if true try to detect automaticaly whether to frame or not; autoFraming is omited if preventFraming is true; framed values are only escaped if preventEscaping is false[true by default]

			dodoString request;     ///< SQL statement

			/**
			 * @return escaped string
			 * @param data defines a string to escape
			 */
			static dodoString escapeFields(const dodoString &data);

			/**
			 * @return unescaped string
			 * @param data defines a string to unescape
			 */
			static dodoString unescapeFields(const dodoString &data);

		protected:

			/**
			 * construct `SELECT function` statement
			 */
			virtual void callFunctionCollect();

			/**
			 * construct `select procedure` statement
			 */
			virtual void callProcedureCollect();

			/**
			 * construct `select` statement
			 */
			virtual void selectCollect();

			/**
			 * construct `join` statement
			 */
			virtual void joinCollect();

			/**
			 * construct `insert` statement
			 */
			virtual void insertCollect();

			/**
			 * construct `insert ... select` statement
			 */
			virtual void insertSelectCollect();

			/**
			 * construct `update` statement
			 */
			virtual void updateCollect();

			/**
			 * construct `delete` statement
			 */
			virtual void delCollect();

			/**
			 * construct `union`, `minus`, `intersect` statements
			 */
			virtual void subCollect();

			/**
			 * construct `truncate` statement
			 */
			virtual void truncateCollect();

			/**
			 * construct `drop database` statement
			 */
			virtual void delDbCollect();

			/**
			 * construct `drop table` statement
			 */
			virtual void delTableCollect();

			/**
			 * construct `alter ... drop` statement
			 */
			virtual void delFieldCollect();

			/**
			 * construct `rename database` statement
			 */
			virtual void renameDbCollect();

			/**
			 * construct `alter table` statement
			 */
			virtual void renameTableCollect();

			/**
			 * construct `alter table` statement
			 */
			virtual void renameFieldCollect();

			/**
			 * construct `create database` statement
			 */
			virtual void createDbCollect();

			/**
			 * construct `create table` statement
			 */
			virtual void createTableCollect();

			/**
			 * construct `create index` statement
			 */
			virtual void createIndexCollect();

			/**
			 * construct `drop index` statement
			 */
			virtual void deleteIndexCollect();

			/**
			 * construct `alter table ... add` statement
			 */
			virtual void createFieldCollect();

			/**
			 * construct adaptive field info for statements
			 */
			virtual dodoString fieldCollect(const __fieldInfo &row);

			/**
			 * add additional statements for query
			 * @param qTypeToCheck defines type of additional info to check
			 * @param collectedString defines string that defines additional statement
			 */
			virtual void additionalCollect(unsigned int qTypeToCheck, const dodoString &collectedString);

			/**
			 * @return string constructed from additional statements
			 * @param sqlAddEnumArr defines statements[see sqlAddEnumArr]
			 * @param sqlAddArr defines statements[see sqlAddArr]
			 * @param qTypeShift defines additional statements that has been set
			 */
			virtual dodoString insideAddCollect(const unsigned int sqlAddEnumArr[], const dodoString sqlAddArr[], int qTypeShift);

			/**
			 * @return string constructed from additional statements(DB-dependent)
			 * @param statements defines statements
			 * @param qTypeShift defines additional statements that has been set
			 */
			virtual dodoString insideAddCollect(const dodoStringArray &statements, int qTypeShift);

			/**
			 * @return SQL adaptive statements of `field name`=`value` tuples separated with coma
			 * @param values defines fields
			 * @param fields defines names
			 * @param frame defines frame of values
			 */
			virtual dodoString valuesName(const dodoStringArray &values, const dodoStringArray &fields, const dodoString &frame = "'");

			/**
			 * @return SQL compliant data type
			 * @param type defines data type
			 */
			virtual dodoString sqlDataType(int type);

			/**
			 * @return 1 data-type must have range, 0 if may have, -1 if mustn't have;
			 * @param type defines data type
			 */
			virtual int chkRange(int type);

			/**
			 * @return SQL adaptive statements of references to fields
			 * @param type defines type of reference
			 */
			virtual dodoString stringReference(int type);

			dodoMap<dodoString, dodoStringArray, stringTools::iequal> framingFields;   ///< hash of 'db:table' => `array of fields to frame`

		private:

			static const dodoString sqlAddSelArr[3];  ///< additional `select` statement
			static const dodoString sqlAddDelArr[2];  ///< additional `delete` statement
			static const dodoString sqlAddUpArr[2];   ///< additional `update` statement
			static const dodoString sqlAddInsArr[2];  ///< additional `insert` statement

			static const dodoString sqlAddArr[8];     ///< additional statements(where, limit, ...)

			static const dodoString sqlQStArr[5];     ///< statements for complex queries(union, ...)

			static const unsigned int addSelEnumArr[2]; ///< positions in sqlAddSelArr additional `select` statement
			static const unsigned int addDelEnumArr[1]; ///< positions in sqlAddDelArr additional `delete` statement
			static const unsigned int addUpEnumArr[1];  ///< positions in sqlAddUpArr additional `update` statement
			static const unsigned int addInsEnumArr[1]; ///< positions in sqlAddInsArr additional `insert` statement

	};

};

#endif
