/***************************************************************************
 *            xexec.h
 *
 *  Wed Aug 24 00:32:16 2005
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

#ifndef _XEXEC_H_
#define _XEXEC_H_

#include <libdodo/directives.h>

#ifdef DL_EXT

	#include <dlfcn.h>

#endif

#include <libdodo/xexecEx.h>
#include <libdodo/types.h>

namespace dodo
{

	/**
	 * @enum xexecObjectEnum describes object that passes to module
	 */
	enum xexecObjTypeEnum
	{
		XEXEC_OBJECT_DBMYSQL,
		XEXEC_OBJECT_DBSQLITE,
		XEXEC_OBJECT_IOSTD,
		XEXEC_OBJECT_IODISK,
		XEXEC_OBJECT_IOSOCKET,
		XEXEC_OBJECT_IOSOCKETEXCHANGE,
		XEXEC_OBJECT_DBPOSTGRESQL,
	};

	/**
	 * @typedef that describes function that will be called as hook
	 * @param first is pointer class object that uses hook
	 * @param second is type of object that called hook
	 * @param third is data needed for hook
	 */
	typedef void (*inExec)(void *, short, void *);

	/**
	 * @struct __execItem describes a node for Xexec
	 */
	struct __execItem
	{
		inExec func;    ///< function to execute
		short type;     ///< type of object that passes to hook
		void *data;     ///< data passed to func
		void *obj;      ///< pointer to object that uses hook
		bool enabled;   ///< disable or enable hook
		int position;   ///< identificator of object

		#ifdef DL_EXT
		void *handle;    ///< handle to module
		#endif
	};

	/**
	 * @struct __execItemList holds vector of hooks
	 */
	struct __execItemList
	{
		std::list<__execItem> exec; ///< vector of hooks
		bool execDisabled;          ///< enable or disable list
	};

	/**
	 * @enum xexecOperTypeEnum describes the very default value of operation type
	 */
	enum xexecOperTypeEnum
	{
		XEXEC_OPERTYPE_NONE,
	};

	#ifdef DL_EXT

	/**
	 * @enum xexecModuleActionTypeEnum defines what type of exec[pre/post] will be used for module
	 */
	enum xexecModuleActionTypeEnum
	{
		XEXEC_MODULEACTIONTYPE_PRE,
		XEXEC_MODULEACTIONTYPE_POST,
		XEXEC_MODULEACTIONTYPE_BOTH,
	};

	/**
	 * @struct __xexecExMod must be returned from initXexecModule in the module
	 */
	struct __xexecMod
	{
		char name[64];              ///< name of module
		char discription[256];      ///< discription of module
		char hook[64];              ///< name of function in module that will be a hook
		short execType;             ///< type of execType to use; it is skipped if you define module in your program[see execTypeEnum]
	};

	/**
	 * @typedef describes function in module that must return info for the hook
	 */
	typedef __xexecMod (*initXexecModule)(void *);

	/**
	 * @typedef describes function in module that will be called during module unloading
	 */
	typedef void (*deinitXexecModule)();

	/**
	 * @struct __xexecCounts describes what position of pre or[and] post exec was set from module;
	 * @note if not set=-1
	 */
	struct __xexecCounts
	{
		/**
		 * constructor
		 */
		__xexecCounts();

		int pre;        ///< position of preExec
		int post;       ///< position of postExec
	};

	#endif

	/**
	 * example of exec function that performs xexec
	 * !!!you must overload it if you want to you same
	 * this simply does preExec and postExec
	 *
	 *	bool
	 *	derivedClass::exec() const
	 *	{
	 *		performXExec(preExec);
	 *		///< execute
	 *		performXExec(postExec);
	 *	}
	 *
	 */

	/**
	 * @class xexec provides wide abilities exec manipulations
	 * you can derive from this class and use pre/post manipulations before main actions
	 */
	class xexec
	{
		public:

			/*
			 * constructor
			 */
			xexec();

			/**
			 * destructor
			 */
			virtual ~xexec();

			/**
			 * set function that will be executed after the main action call
			 * @return number in list where function is set
			 * @param func is reference to functions that will be called as a hook
			 * @param obj is pouinter to object that is uses this hook
			 * @param type is describes what object called hook[see xexecObjTypeEnum]
			 * @param data is pointer to data that will pass to hook
			 * @attention data is not copied!!!
			 */
			virtual int _addPostExec(inExec func, void *obj, short type, void *data);

			/**
			 * set function that will be executed before  the main action call
			 * @return number in list where function is set
			 * @param func is reference to functions that will be called as a hook
			 * @param obj is pouinter to object that is uses this hook
			 * @param type is describes what object called hook[see xexecObjTypeEnum]
			 * @param data is pointer to data that will pass to hook
			 * @attention data is not copied!!!
			 */
			virtual int _addPreExec(inExec func, void *obj, short type, void *data);

			#ifdef DL_EXT

			/**
			 * set function from module that will be executed after  the main action call
			 * @return number in list where function is set
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param obj is pouinter to object that is uses this hook
			 * @param type is describes what object called hook[see xexecObjTypeEnum]
			 * @param data is pointer to data that will pass to hook
			 * @param toInit indicates data that will path to initialize function
			 * @attention data is not copied!!!
			 */
			virtual int _addPostExec(const dodoString &module, void *obj, short type, void *data, void *toInit = NULL);   ///< if applied modules more than XEXEC_MAXMODULES, will return -1[see directives.h]

			/**
			 * set function from module that will be executed before  the main action call
			 * @return number in list where function is set
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param obj is pouinter to object that is uses this hook
			 * @param type is describes what object called hook[see xexecObjTypeEnum]
			 * @param data is pointer to data that will pass to hook
			 * @param toInit indicates data that will path to initialize function
			 * @attention data is not copied!!!
			 */
			virtual int _addPreExec(const dodoString &module, void *obj, short type, void *data, void *toInit = NULL);   ///< if applied modules more than XEXEC_MAXMODULES, will return -1[see directives.h]

			/**
			 * set function from module that will be executed before/after the main action call
			 * the type of hook[pre/post] is defined in module
			 * @return number in list where function is set
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param obj is pouinter to object that is uses this hook
			 * @param type is describes what object called hook[see xexecObjTypeEnum]
			 * @param data is pointer to data that will pass to hook
			 * @param toInit indicates data that will path to initialize function
			 * @attention data is not copied!!!
			 */
			virtual __xexecCounts _addExec(const dodoString &module, void *obj, short type, void *data, void *toInit = NULL);   ///< if applied modules more than XEXEC_MAXMODULES, will return -1[see directives.h]

			#endif

			/**
			 * deletes hook from list
			 * @param position is position in list of hooks[returned from addPostExec]
			 */
			virtual void delPostExec(int position);

			/**
			 * deletes hook from list
			 * @param position is position in list of hooks[returned from addPreExec]
			 */
			virtual void delPreExec(int position);

			/**
			 * replaces hook from list with another
			 * @return false if nothing to replace
			 * @param position is position in list of hooks[returned from addPostExec]
			 * @param func is reference to functions that will be called as a hook
			 * @param data is pointer to data that will pass to hook
			 */
			virtual bool replacePostExec(int position, inExec func, void *data);

			/**
			 * replaces hook from list
			 * @return false if nothing to replace
			 * @param position is position in list of hooks[returned from addPreExec]
			 * @param func is reference to functions that will be called as a hook
			 * @param data is pointer to data that will pass to hook
			 */
			virtual bool replacePreExec(int position, inExec func, void *data);

			/**
			 * disable hook
			 * @param position is position in list of hooks[returned from addPostExec]
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disablePostExec(int position);

			/**
			 * disable hook
			 * @param position is position in list of hooks[returned from addPreExec]
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disablePreExec(int position);

			/**
			 * enable hook
			 * @param position is position in list of hooks[returned from addPostExec]
			 */
			virtual void enablePostExec(int position);

			/**
			 * enable hook
			 * @param position is position in list of hooks[returned from addPreExec]
			 */
			virtual void enablePreExec(int position);

			/**
			 * enable all PreExec hooks
			 */
			virtual void enableAllPreExec();

			/**
			 * enable all PostExec hooks
			 */
			virtual void enableAllPostExec();

			/**
			 * disable all PreExec hooks
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disableAllPreExec();

			/**
			 * disable all PostExec hooks
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disableAllPostExec();

			/**
			 * disable all PostExec and PreExec hooks
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disableAll();

			/**
			 * enable all PostExec and PreExec hooks
			 */
			virtual void enableAll();

			/**
			 * enable\disable other hooks when executing hook
			 * prevent recursive hooks if u for example call exec in hook, if safeHooks disabled, all hooks set to this class will be called
			 */
			bool safeHooks;

			#ifdef DL_EXT

			/**
			 * @return info about module
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param toInit indicates data that will path to initialize function
			 */
			static __xexecMod getModuleInfo(const dodoString &module, void *toInit = NULL);

			#endif
			
			/**
			 * permits to collect data for xexec
			 * @param enable defines whether collect or not
			 */
			virtual void enableDataCollecting(bool enable);
			
			/**
			 * @return true if it's permitted to collect data for xexec
			 */
			virtual bool dataCollectingEnabled();

		protected:

			/**
			 * enable all PreExec hooks
			 */
			virtual void enableAllPreExec() const;

			/**
			 * enable all PostExec hooks
			 */
			virtual void enableAllPostExec() const;

			/**
			 * disable all PreExec hooks
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disableAllPreExec() const;

			/**
			 * disable all PostExec hooks
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disableAllPostExec() const;

			/**
			 * @return true if found
			 * @param list describes list where to search
			 * @param position is position in list of hooks
			 * @param iter is iterator that is specified with position
			 */
			virtual bool getXexec(std::list<__execItem> &list, int position);

			/**
			 * set function that will be executed before  the main action call
			 * @return number in list where function is set
			 * @param list describes list where hook will be set
			 * @param func is reference to functions that will be called as a hook
			 * @param obj is pouinter to object that is uses this hook
			 * @param type is describes what object called hook[see xexecObjTypeEnum]
			 * @param data is pointer to data that will pass to hook
			 * @attention data is not copied!!!
			 */
			virtual int addXExec(std::list<__execItem> &list, inExec func, void *obj, short type, void *data);

			/**
			 * deletes hook from list
			 * @param list describes list where is set hook
			 * @param position is position in list of hooks
			 */
			virtual void delXExec(std::list<__execItem> &list, int position);

			/**
			 * replaces hook from list
			 * @param list describes list where is set hook
			 * @param position is position in list of hooks
			 * @param func is reference to functions that will be called as a hook
			 * @param data is pointer to data that will pass to hook
			 */
			virtual bool replaceXExec(std::list<__execItem> &list, int position, inExec func, void *data);

			/**
			 * set state(enable/disable) for XExec
			 * @param list describes list where to get hooks
			 * @param position is position in list where to set state
			 * @param stat indicates what state to set
			 */
			virtual void setStatXExec(std::list<__execItem> &list, int position, bool stat);

			#ifdef DL_EXT

			/**
			 * set function from  module that will be executed before  the main action call
			 * @return number in list where function is set
			 * @param list describes list where hook will be set
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param obj is pouinter to object that is uses this hook
			 * @param type is describes what object called hook[see xexecObjTypeEnum]
			 * @param data is pointer to data that will pass to hook
			 * @param toInit indicates data that will path to initialize function
			 * @attention data is not copied!!!
			 */
			virtual int addXExecModule(std::list<__execItem> &list, void *obj, short type, const dodoString &module, void *data, void *toInit = NULL);

			#endif

			/**
			 * perform enabled hooks from the list
			 * @param list describes list where to get hooks
			 */
			virtual void performXExec(__execItemList &list) const;

			mutable __execItemList preExec;             ///< list of hooks executed before exec
			mutable __execItemList postExec;            ///< list of hooks executed after exec

			int execs;                                  ///< execs counter

			std::list<__execItem>::iterator current;    ///< iterator for list[for matched]
			
			mutable bool collectData;///< to collect data for xexec[true by default]

			mutable int operType; ///< operation type set by main action; can be used in hook to determine type of action
	};

};

#endif
