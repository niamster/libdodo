/***************************************************************************
 *            xexec.h
 *
 *  Wed Aug 24 00:32:16 2005
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
 
#ifndef _XEXEC_H_
#define _XEXEC_H_

#include <directives.h>

#include <string>
#include <vector>

#ifdef DL_EXT

	#include <dlfcn.h>

#endif	

#include <types.h>
#include <xexecEx.h>

namespace dodo
{
	
	/**
	 * @enum xexecObjTypeEnum describes object that passes to module
	 */
	enum xexecObjTypeEnum
	{
		XEXECOBJ_DBMYSQL,
		XEXECOBJ_DBSQLITE,
		XEXECOBJ_FLUSHSTD,
		XEXECOBJ_FLUSHDISK,
		XEXECOBJ_FLUSHSOCKET,
		XEXECOBJ_FLUSHSOCKETEXCHANGE,
	};
	
	/**
	 * @typedef that describes function that will be called as hook
	 * @param first is pointer class object that uses hook; 
	 * @param second is data needed for hook
	 */
	typedef void (*inExec)(void *, xexecObjTypeEnum, void *);

	/**
	 * @struct __execItem describes a node for Xexec
	 */
	struct __execItem
	{
		inExec func;///< function to execute
		xexecObjTypeEnum type;///< type of object that passes to hook
		void *data;///< data passed to func
		void *obj;///< pointer to object that uses hook
		bool enabled;///< disable or enable hook
		int position;///< position in list[not really in list, but that is returned to user]
	};

	/**
	 * @struct __execItemList holds vector of hooks
	 */
	struct __execItemList
	{
		std::vector<__execItem> exec;///< vector of hooks
		bool execDisabled;///< enable or disable list
	};
	
	/**
	 * @enum xexecOperTypeEnum describes the very default value of operation type
	 */
	enum xexecOperTypeEnum
	{
		XEXEC_NONE,
	};

	#ifdef DL_EXT
	
		/**
		 * @enum execTypeEnum defines what type of exec[pre/post] will be used for module
		 */
		enum execTypeEnum
		{
			XEXECMODULE_PRE,
			XEXECMODULE_POST,
			XEXECMODULE_BOTH,
		};
		/**
		 * @struct xexecExMod must be returned from initXexecModule in the module
		 */
		struct xexecMod
		{
			char name[20];///< name of module
			char discription[40];///< discription of module
			char hook[20];///< name of function in module that will be a hook
			execTypeEnum execType;///< type of execType to use; it is skipped if you define module in your program
		};
		
		/**
		 * @typedef describes function in module that must return info for the hook
		 */
		typedef xexecMod (*initXexecModule)();

		/**
		 * @typedef describes function in module that will be called during module unloading
		 */
		typedef void (*deinitXexecModule)();
		
		/**
		 * @struct xexecCounts describes what position of pre or[and] post exec was set from module;
		 * @note if not set = -1
		 */
		struct xexecCounts
		{
			/**
			 * constructor
			 */
			xexecCounts();
			
			int pre;///< position of preExec
			int post;///< position of postExec
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
	 *		///execute 
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
			 * @param data is pointer to data that will pass to hook
			 * @attention data is not copied!!!
			 */
			virtual int _addPostExec(inExec func, void *obj, xexecObjTypeEnum type, void *data) const;
			
			/**
			 * set function that will be executed before  the main action call
			 * @return number in list where function is set
			 * @param func is reference to functions that will be called as a hook
			 * @param obj is pouinter to object that is uses this hook
			 * @param data is pointer to data that will pass to hook
			 * @attention data is not copied!!!
			 */
			virtual int _addPreExec(inExec func, void *obj, xexecObjTypeEnum type, void *data) const;
			
			#ifdef DL_EXT
						
				/**
				 * set function from module that will be executed after  the main action call
				 * @return number in list where function is set
				 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
				 * @param obj is pouinter to object that is uses this hook
				 * @param data is pointer to data that will pass to hook
				 * @attention data is not copied!!!
				 */
				virtual int _addPostExec(const std::string &module, void *obj, xexecObjTypeEnum type, void *data) const;///if applied modules more than XEXEC_MAXMODULES, will return -1; see directives.h
			
				/**
				 * set function from module that will be executed before  the main action call
				 * @return number in list where function is set
				 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
				 * @param obj is pouinter to object that is uses this hook
				 * @param data is pointer to data that will pass to hook
				 * @attention data is not copied!!!
				 */
				virtual int _addPreExec(const std::string &module, void *obj, xexecObjTypeEnum type, void *data) const;///if applied modules more than XEXEC_MAXMODULES, will return -1; see directives.h
	
				/**
				 * set function from module that will be executed before/after the main action call
				 * the type of hook[pre/post] is defined in module
				 * @return number in list where function is set
				 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
				 * @param obj is pouinter to object that is uses this hook
				 * @param data is pointer to data that will pass to hook
				 * @attention data is not copied!!!
				 */
				virtual xexecCounts _addExec(const std::string &module, void *obj, xexecObjTypeEnum type, void *data) const;///if applied modules more than XEXEC_MAXMODULES, will return -1; see directives.h
			
			#endif
			
			/**
			 * deletes hook from list
			 * @param position is position in list of hooks[returned from addPostExec]
			 */
			virtual void delPostExec(int position) const;
						
			/**
			 * deletes hook from list
			 * @param position is position in list of hooks[returned from addPreExec]
			 */
			virtual void delPreExec(int position) const;
						
			/**
			 * replaces hook from list with another
			 * @param position is position in list of hooks[returned from addPostExec]
			 * @param func is reference to functions that will be called as a hook
			 * @param data is pointer to data that will pass to hook
			 */
			virtual bool replacePostExec(int position, inExec func, void *data) const;
						
			/**
			 * replaces hook from list
			 * @param position is position in list of hooks[returned from addPreExec]
			 * @param func is reference to functions that will be called as a hook
			 * @param data is pointer to data that will pass to hook
			 */
			virtual bool replacePreExec(int position, inExec func, void *data) const;
						
			/**
			 * disable hook
			 * @param position is position in list of hooks[returned from addPostExec]
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disablePostExec(int position) const;
						
			/**
			 * disable hook
			 * @param position is position in list of hooks[returned from addPreExec]
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disablePreExec(int position) const;
						
			/**
			 * enable hook
			 * @param position is position in list of hooks[returned from addPostExec]
			 */			
			virtual void enablePostExec(int position) const;
						
			/**
			 * enable hook
			 * @param position is position in list of hooks[returned from addPreExec]
			 */
			virtual void enablePreExec(int position) const;		
							
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
			 * disable all PostExec and PreExec hooks
			 * @note usefull when u don't want to make cyclic hook using for examle exec of main action in hook from the class that called that hook
			 */
			virtual void disableAll() const;
						
			/**
			 * enable all PostExec and PreExec hooks
			 */
			virtual void enableAll() const;
			
			/**
			 * enable\disable other hooks when executing hook
			 * prevent recursive hooks if u for example call exec in hook, if safeHooks disabled, all hooks set to this class will be called
			 */
			mutable bool safeHooks;
			
			mutable int operType;///< operation type set by main action; can be used in hook to determine type of action
			
			#ifdef DL_EXT
			
				/**
				 * @return info about module
				 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
				 */
				static xexecMod getModuleInfo(const std::string &module);
			
			#endif
			
		protected:
		
			/**
			 * @return true if found
			 * @param list describes list where to search
			 * @param position is position in list of hooks
			 * @param iter is iterator that is specified with position
			 */
			bool getXexec(std::vector<__execItem> &list, int position, std::vector<__execItem>::iterator &iter) const;
		
			/**
			 * set function that will be executed before  the main action call
			 * @return number in list where function is set
			 * @param list describes list where hook will be set
			 * @param func is reference to functions that will be called as a hook
			 * @param obj is pouinter to object that is uses this hook
			 * @param data is pointer to data that will pass to hook
			 * @attention data is not copied!!!
			 */			
			inline virtual int addXExec(std::vector<__execItem> &list, inExec func, void *obj, xexecObjTypeEnum type, void *data) const;
									
			/**
			 * deletes hook from list
			 * @param list describes list where is set hook
			 * @param position is position in list of hooks
			 */
			inline virtual void delXExec(std::vector<__execItem> &list, int position) const;
						
			/**
			 * replaces hook from list
			 * @param list describes list where is set hook
			 * @param position is position in list of hooks
			 * @param func is reference to functions that will be called as a hook
			 * @param data is pointer to data that will pass to hook
			 */			
			inline virtual bool replaceXExec(std::vector<__execItem> &list, int position, inExec func, void *data) const;
			
			/**
			 * set state(enable/disable) for XExec
			 * @param list describes list where to get hooks
			 * @param position is position in list where to set state
			 * @param stat indicates what state to set
			 */
			virtual void setStatXExec(std::vector<__execItem> &list, int position, bool stat) const;
			
			#ifdef DL_EXT
			
				/**
				 * set function from  module that will be executed before  the main action call
				 * @return number in list where function is set
				 * @param list describes list where hook will be set
				 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
				 * @param obj is pouinter to object that is uses this hook
				 * @param data is pointer to data that will pass to hook
				 * @attention data is not copied!!!
				 */
				inline virtual int addXExecModule(std::vector<__execItem> &list, void *obj, xexecObjTypeEnum type, const std::string &module, void *data) const;
			
			#endif
			
			/**
			 * perform enabled hooks from the list
			 * @param list describes list where to get hooks
			 */
			virtual void performXExec(__execItemList &list) const;
			
			mutable __execItemList preExec;///< list of hooks executed before exec
			mutable __execItemList postExec;///< list of hooks executed after exec
			
			mutable int execs;
			
			mutable std::vector<__execItem>::iterator i;///< iterator for list
			mutable std::vector<__execItem>::iterator j;///< iterator for list
			mutable std::vector<__execItem>::iterator k;///< iterator for list
			
			mutable __execItem temp;///< temp storage for hook
			
			#ifdef DL_EXT
			
				mutable void *handles[XEXEC_MAXMODULES];///< handles to modules
				mutable int handlesOpened[XEXEC_MAXMODULES];///< map of active modules
				
			#endif	
	};

};

#endif
