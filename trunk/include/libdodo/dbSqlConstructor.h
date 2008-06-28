/***************************************************************************
 *            dbSqlConstructor.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DBSQLCONSTRUCTOR_H_
#define _DBSQLCONSTRUCTOR_H_

#include <libdodo/directives.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/dbSqlConstructorEx.h>
#include <libdodo/types.h>
#include <libdodo/dbAccumulator.h>

namespace dodo
{
	namespace db
	{
		/**
		 * @class sqlConstructor provide construction of SQL request
		 * @note all data become escaped and framed with '. Names of fields are not framed with ` to prevent cases as `count(*)`, etc.
		 * If you want to prevent data framing define preventFraming sqlConstructor class propertie as true but remember
		 */
		class sqlConstructor : public accumulator
		{
			public:

				/*
				 * constructor
				 */
				sqlConstructor();

				/**
				 * destructor
				 */
				virtual ~sqlConstructor();

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

				bool preventFraming;                                                                                    ///< if true values of fields will be framed with ' in `insert` and `update` statements[false by default]

				bool preventEscaping;                                                                                   ///< if true values of fields {\,'} will be escaped in `insert` and `update` statements[false by default]

#ifdef ENABLE_SQL_AUTOFRAMING

				bool autoFraming;                                                                                       ///< if true try to detect automaticaly whether to frame or not; autoFraming is omited if preventFraming is true; framed values are only escaped if preventEscaping is false[true by default]

#endif

				dodoMap<dodoString, dodoStringArray, dodoMapICaseStringCompare> framingFields;                          ///< hash of 'db:table' => `array of fields to frame`

				dodoString request;                                                                                     ///< SQL statement

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
				virtual dodoString fieldCollect(const __connectorField &row);

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
				 * @param arrSize defines size of sqlAddEnumArr and sqlAddArr[they should have same sizes]
				 * @param qTypeShift defines additional statements that has been set
				 */
				virtual dodoString insideAddCollect(const unsigned int sqlAddEnumArr[], const dodoString sqlAddArr[], int arrSize, int qTypeShift);

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

			private:

				static const dodoString sqlAddSelArr[ACCUMULATOR_ADDREQUESTSELECTSTATEMENTS];                           ///< additional `select` statement
				static const dodoString sqlAddDelArr[ACCUMULATOR_ADDREQUESTDELETESTATEMENTS];                           ///< additional `delete` statement
				static const dodoString sqlAddUpArr[ACCUMULATOR_ADDREQUESTUPDATESTATEMENTS];                            ///< additional `update` statement
				static const dodoString sqlAddInsArr[ACCUMULATOR_ADDREQUESTINSERTSTATEMENTS];                           ///< additional `insert` statement
				static const unsigned int addSelEnumArr[ACCUMULATOR_ADDREQUESTSELECTSTATEMENTS];                        ///< positions in sqlAddSelArr additional `select` statement
				static const unsigned int addDelEnumArr[ACCUMULATOR_ADDREQUESTDELETESTATEMENTS];                        ///< positions in sqlAddDelArr additional `delete` statement
				static const unsigned int addUpEnumArr[ACCUMULATOR_ADDREQUESTUPDATESTATEMENTS];                         ///< positions in sqlAddUpArr additional `update` statement
				static const unsigned int addInsEnumArr[ACCUMULATOR_ADDREQUESTINSERTSTATEMENTS];                        ///< positions in sqlAddInsArr additional `insert` statement

				static const dodoString sqlAddArr[ACCUMULATOR_ADDREQUESTSTATEMENTS];                                    ///< additional statements(`where`, `limit`, ...)

				static const dodoString sqlQStArr[CONNECTOR_SUBREQUESTSTATEMENTS];                                      ///< statements for complex queries(`union`, ...)

				static const dodoString sqlJoinArr[CONNECTOR_JOINTYPESTSTATEMENTS];                              ///< statements for `join` queries


#define SQLCONSTRUCTOR_STATEMENTS 20

				/**
				 * @enum sqlConstructorStatementEnum defines sqlConstructor statements
				 * @note defines positions of string representation in 'statements' class property
				 */
				enum sqlConstructorStatementEnum
				{
					SQLCONSTRUCTOR_STATEMENT_EQUAL,
					SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE,
					SQLCONSTRUCTOR_STATEMENT_APOSTROPHE,
					SQLCONSTRUCTOR_STATEMENT_COMA,
					SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET,
					SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET,
					SQLCONSTRUCTOR_STATEMENT_SELECT,
					SQLCONSTRUCTOR_STATEMENT_CALL,
					SQLCONSTRUCTOR_STATEMENT_FROM,
					SQLCONSTRUCTOR_STATEMENT_COLON,
					SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA,
					SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKETCOMA,
					SQLCONSTRUCTOR_STATEMENT_INSERT,
					SQLCONSTRUCTOR_STATEMENT_INTO,
					SQLCONSTRUCTOR_STATEMENT_VALUES,
					SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKETSELECT,
					SQLCONSTRUCTOR_STATEMENT_UPDATE,
					SQLCONSTRUCTOR_STATEMENT_SET,
					SQLCONSTRUCTOR_STATEMENT_DELETE,
				};

				static const dodoString statements[SQLCONSTRUCTOR_STATEMENTS];                 ///< sqlConstructor statements
		};
	};
};

#endif
