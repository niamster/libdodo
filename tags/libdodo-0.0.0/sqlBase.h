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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "derectives.h"
#include "types.h"
#include "sqlBaseEx.h"

namespace dodo
{

	class sqlBase;
	
	struct __sqlStatements 
	{
		char *str;
		unsigned int len;
	};
	
	/**
	* some statements can't be togather in one query. They are marcked specially
	* for more security before applying one, remove other;
	* example: 
	* in select statement : [ALL | DISTINCT]
	* so to set ALL u should remove from flag DISTINCTROW
	* ----
	* qSelShift &= ~DISTINCT-1;
	* qSelShift |= ALL;
	* ----
	* or to use functions removeF and addF
	*/

#define TOBIT(argument) (int)pow(2,(argument))
	
///////////////////////////////////
//	These statements need additional statement
///////////////////////////////////		
	enum sqlQStEnum
	{
		SELECT = 0,
		INSERT,
		UPDATE,
		DELETE,
	};
	/**
	* do not edit it please.
	* sqlBase use them in such way, as they are
	*/
	static const __sqlStatements sqlQStArr[] = 
		{
			{"select %s from `%s`",15},//SELECT
			{"insert %s into %s values (%s)",23},//INSERT
			{"update %s set %s",12},//UPDATE
			{"delete %s from `%s`",15},//DELETE
		};
///////////////////////////////////
	enum sqlEmpty
	{
		EMPTY = 0
	};
	
///////////////////////////////////
//	These statements need additional statement
///////////////////////////////////	
	enum sqlAddEnum
	{
		WHERE = 1,
		HAVING = 2,
		GROUPBY = 3,
		ORDERBY = 4,
		LIMIT = 5,
	};
	static 	__sqlStatements sqlAddArr[] = 
	{
		{"",0},
		{" where %s ",8},
		{" having %s ",9},
		{" group by %s ",11},			
		{" order by %s ",11},		
		{" limit %s ",8},
	};
	
///////////////////////////////////
//	These statements don't need additional statement
///////////////////////////////////
	enum sqlAddInsEnum
	{
		INSERT_IGNORE = 1,
		INSERT_DELAYED,///dup1
		INSERT_HIGH_PRIORITY,///dup1
		INSERT_LOW_PRIORITY///dup1
	};
	static unsigned short sqlAddInsEnumArr[] = 
	{
		INSERT_IGNORE,
		INSERT_DELAYED,
		INSERT_HIGH_PRIORITY,
		INSERT_LOW_PRIORITY
	};
	static 	__sqlStatements sqlAddInsArr[] = 
	{
		{"",0},
		{" ignore ",8},
		{" delayed ",9},
		{" high_priority ",15},
		{" low_priority ",14},
	};
	static unsigned short sqlAddInsArrLen = 5;
///////////////////////////////////
//	These statements don't need additional statement
///////////////////////////////////
	enum sqlAddUpEnum
	{
		UPDATE_IGNORE = 1,
		UPDATE_LOW_PRIORITY
	};
	static unsigned short sqlAddUpEnumArr[] = 
	{
		UPDATE_IGNORE,
		UPDATE_LOW_PRIORITY
	};
	static 	__sqlStatements sqlAddUpArr[] = 
	{
		{"",0},
		{" ignore ",8},
		{" low_priority ",14},
	};
	static unsigned short sqlAddUpArrLen = 3;
///////////////////////////////////
//	These statements don't need additional statement
///////////////////////////////////
	enum sqlAddDelEnum
	{
		DELETE_IGNORE = 1,
		DELETE_LOW_PRIORITY
	};
	static unsigned short sqlAddDelEnumArr[] = 
	{
		DELETE_IGNORE,
		DELETE_LOW_PRIORITY
	};
	static 	__sqlStatements sqlAddDelArr[] = 
	{
		{"",0},
		{" ignore ",8},
		{" low_priority ",14},
	};
	static unsigned short sqlAddDelArrLen = 3;		
///////////////////////////////////
//	These statements don't need additional statement
///////////////////////////////////
	enum sqlAddSelEnum
	{
		SELECT_DISTINCT = 1,///dup1
		SELECT_ALL///dup1
	};
	static unsigned short sqlAddSelEnumArr[] = 
	{
		SELECT_DISTINCT,
		SELECT_ALL
	};
	static 	__sqlStatements sqlAddSelArr[] = 
	{
		{"",0},
		{" distinct ",10},
		{" all ",5},
	};
	static unsigned short sqlAddSelArrLen = 3;
///////////////////////////////////	
	
	/**
	* data that could be retrieved from result
	*/
	struct __sqlStorage
	{
		char ***rows;
		char **fields;
		unsigned long numRows; 
		unsigned long numFields;
	};	
	
	/**
	* data to connect to server
	*/
	struct __sqlInfo
	{
		char *db;
		char *host;
		char *user;
		char *password;
		char *socket;
		int port;
	};
	
	typedef void (*inExec)(sqlBase *, void *);
	
	/**
	* a node for Xexec
	*/
	struct __execItem
	{
		inExec func;///function to execute
		void *data;///data to parce to func
		bool present;///if false = deleted
		bool enabled;///disable or enable
		bool restore;///to restore or not data after execution
	};
	
	/**
	* array of nodes for Xexec
	*/
	struct __execList
	{
		__execItem *item;
		unsigned short number;
	};
	
	/**
	* data that could be retrieved from class(to modyficate)
	*/
	struct __collectedData
	{
		pchar pre_where;	
		ppchar pre_fieldsNames;
		ppchar pre_fieldsVal;
		pchar pre_table;
		pchar pre_order;
		pchar pre_having;
		pchar pre_group;
		unsigned long *pre_limNumber;
		
		pppchar rows;
		ppchar fields;	
		
		short *qType;
		
		short *qShift;
		
		short *qSelShift;
		short *qInsShift;
		short *qUpShift;
		short *qDelShift;		
	};
	
	/**
	* class to provide wide abilities for sql manipulations
	*/
	class sqlBase
	{
		friend class sqlBaseEx;///class of exception
		public:
			/*
			* constructors and destructors
			*/
			sqlBase();	
			virtual ~sqlBase();

			///next functions just modify internal data, but don't make a query. that's usefull to make preExec or postExec
			/**
			* sets info for mysql database
			*/
			void setSqlInfo(const char *db, const char *host, const int port, const char *user, const char *password, const char *socket = NULL) const;
			/**
			 * table - (char*)
			 * fields - array of (char*), last is null
			 * where - array of (char*), last is null
		   */
			virtual void select(const char *table, const char **fieldsNames, const char *where = NULL) const;
			/**
			 * table - (char*)
			 * fieldsVal - array of (char*), last is null
			 * fieldsNames - array of (char*), last is null
		   */
			virtual void insert(const char *table, const char **fieldsVal, const char **fieldsNames = NULL);
			/**
			 * table - (char*)
			 * fieldsVal - array of (char*), last is null
			 * fieldsNames - array of (char*), last is null
			 * where - array of (char*), last is null
		   */
			virtual void update(const char *table, const char **fieldsVal, const char **fieldsNames, const char *where = NULL);
			/**
			 * table - (char*)
			 * where - array of (char*), last is null
			*/       
			virtual void del(const char *table, const char *where = NULL);
			/**
			 * sets limit for request
			*/
			virtual void limit(int number) const;
			/**
			 * sets order for request
			*/
			virtual void order(char *order) const;
			/**
			 * sets group for request
			*/
			virtual void group(char *order) const;
			/**
			 * sets having for request
			*/
			virtual void having(char *order) const;
			/**
			* functions to set(unset) additional parameters for standart qTypes
			*/
			virtual void setAddInsSt(unsigned short statement);
			virtual void setAddUpSt(unsigned short statement);
			virtual void setAddSelSt(unsigned short statement);
			virtual void setAddDelSt(unsigned short statement);
			
			virtual void unsetAddInsSt(unsigned short statement);
			virtual void unsetAddUpSt(unsigned short statement);
			virtual void unsetAddSelSt(unsigned short statement);
			virtual void unsetAddDelSt(unsigned short statement);			
			/**
			* function that return structure with pointers to internal data(not copy).
			* it is used for postExec and preExec functions. For modyfication.
			*/
			virtual __collectedData *getCollectedData();
			/**
			* functions to set(delete) functions that will be executed before or after the exec call.
			* they return number in list where function is set
			* when u delete function from list, position of other don't change
			* be carefull!! data are not copied!!!
			* if restore param is set to false, data after callback function won't be restored
			*/
			virtual int addPostExec(inExec func, void *data, bool restore = true) const;
			virtual int addPreExec(inExec func, void *data, bool restore = true) const;
			virtual void delPostExec(unsigned short position) const;
			virtual void delPreExec(unsigned short position) const;
			/**
			* enable or disable hooks
			* usefull when u don't want to make cyclic hook using for examle exec in hook from the class that called that hook
			* in that case u have to disable
			*/
			virtual void disablePostExec(unsigned short position) const;
			virtual void disablePreExec(unsigned short position) const;
			virtual void enablePostExec(unsigned short position) const;
			virtual void enablePreExec(unsigned short position) const;
			virtual void enableAllPreExec() const;
			virtual void enableAllPostExec() const;
			virtual void disableAllPreExec() const;
			virtual void disableAllPostExec() const;
			/**
			* shows results from storage
			*/
			virtual void debug(__sqlStorage &result) const;
			///get err statement
			virtual char *getErr() const;
			/**
			* functions for hooking: store collected data into temp struct and restore.
			* be carefull!!! if u call restore without store called nothin' will be made
			* when u call restore backuped data will clear, and next restore call without store call will do nothin'
			*/
			virtual void storeCollected() const;
			virtual void restoreCollected() const;
			/**
			* prevent recursive hooks if u for example call exec in hook, if safeHooks disabled, all hooks set to this class will be called
			*/
			virtual void setSafeHooks(bool state) const;
			/**
			* frees memory from __sqlStorage
			*/
			virtual void freeSqlStorage(__sqlStorage *storage) const;
			/**
			* detect what type of request was(will be) performed
			*/
			virtual short getQType() const;
			/**
			* adds/removes option to flag
			*/
			virtual void addF(short &flag, unsigned short statement) const;
			virtual void removeF(short &flag, unsigned short statement) const;			
		protected:
			/**
			* frees data that was backuped
			*/
			virtual void freeBackupedData() const;
			/**
			* adds/deletes XExec to list
			*/
			virtual int addXExec(__execList &list, inExec func, void *data, bool restore) const;
			virtual void delXtExec(__execList &list, unsigned short position) const;
			/**
			* set state(enable/disable) for XExec(all XExecs) 
			*/
			virtual void setStatXtExec(__execList &list, unsigned short position, bool stat) const;
			virtual void setStatAllXtExec(__execList &list, bool stat) const;
			/**
			* initiates/frees memory
			*/
			virtual void initXExec(__execList &list);
			virtual void freeXExec(__execList &list);
			/**
			* perform enabled functions from the list
			*/
			virtual void performXExec(__execList &list) const;
			/**
			* functions to collect data into query after
			*/		
			virtual void selectCollect() const;
			virtual void insertCollect() const;
			virtual void updateCollect() const;     
			virtual void delCollect() const;
			virtual void limitCollect() const;
			/**
			* adds to the end of request additional data collection for query
			*/
			virtual void additionalCollect(unsigned long qTypeTocheck, pchar collectedString) const;
			/**
			* return string that consists of collected data for request inside
			*/
			virtual pchar insideAddCollect(unsigned short sqlAddEnumArr[], __sqlStatements sqlAddArr[], unsigned short arrLen, short qTypeShift) const;
			/**
			* performs query collect from collected data;
			* frees collected data
			*/
			virtual void queryCollect() const;///collect data into query
			virtual void cleanCollect() const;///clean collected data
			/**
			* collected data
			*/
			mutable pchar pre_where;//string of where statement
			mutable ppchar pre_fieldsNames;//array of string with fields' names
			mutable ppchar pre_fieldsVal;//array of string with fields' values(accordingly to pre_fieldsNames)
			mutable pchar pre_table;//string of table name
			mutable pchar pre_order;//string of order statement
			mutable pchar pre_having;//string of having statement
			mutable pchar pre_group;//string of group statement
			mutable unsigned long pre_limNumber;//number for limit
			/**
			* backuped collected data
			*/
			mutable __collectedData backup;
			mutable bool stored;///if backuped
			/**
			* allocates and frees memmory for internal vars
			*/
			void freeSqlInfo();
			void allocateSqlInfo();
			virtual void myAlloc();	
			virtual void myFree();
			/**
			 *returns copy of 2d array and deepness
			*/
			char **copy2(const char **array) const;
			/**
			* as strcpy, but with 2d array. dst_array must me allocated, but as a pointer, not as array
			*/
			char **strcpy2(char **dst_array, const char **src_array) const;			
			/**
			 * frees 2d array
			*/
			void free2(char **array) const;
			/**
			* as strcpy, but with 3d array. dst_array must me allocated, but as a pointer, not as array
			*/
			char ***strcpy3(char ***dst_array, const char ***src_array) const;				
			/**
			 * returns copy of 3d array and deepness
			*/
			char ***copy3(const char ***array) const;
			/**
			 * frees 3d array
			*/
			void free3(char ***array) const;
			
			///creates string from fields' names and 'em values
			virtual pchar fieldsValName(const char **fieldsVal, const char **fieldsNames) const;
			
			///makes string of fields, separated with separator; frame = symbol, that is used to frame the field
			char *implode(const char **fields, const char *separator, const char *frame = "") const;
	
			mutable unsigned long numRows;///number of rows got from result
			mutable unsigned long numFields;///number of fields got from result
			mutable unsigned long affectedRows;///number of rows were modyfied
			mutable pppchar rows;///rows got from result
			mutable ppchar fields;///fields got from result
			
			mutable pchar request;///ready sql statement
			mutable unsigned long requestLength;///length of request
			
			mutable __execList preExec;///functions executed before exec
			mutable __execList postExec;///functions executed after exec
			
			mutable __sqlInfo sqlInfo;///data to connect to server
			
			mutable bool show;//whether request was with result(show,select)	
			
			mutable bool ready;///if data callected
			
			mutable short qType; ///query type(for exec)

			mutable short qShift;///value to shift query template for specific

			mutable short qSelShift;///value to shift query template for specific			
			mutable short qInsShift;///value to shift query template for specific
			mutable short qUpShift;///value to shift query template for specific
			mutable short qDelShift;///value to shift query template for specific
			
			mutable pchar err;///string that contains err
			mutable bool safeHooks;///enable\disable other hooks when executing hook
	};
};
#endif /* _SQLBASE_H_ */
