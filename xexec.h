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

#include <string>
#include <vector>
//#include <functional>
#include <dlfcn.h>

#include "directives.h"
#include "types.h"
#include "dodoBase.h"
#include "xexecEx.h"

namespace dodo
{
	/**
	 * first argument = class object; second - data needed for hook
	 */
	typedef void (*inExec)(dodoBase *, void *);

	/**
	 * a node for Xexec
	 */
	struct __execItem
	{
		inExec func;///function to execute
		void *data;///data to parce to func
		dodoBase *obj;///object
		bool present;///if false = deleted
		bool enabled;///disable or enable
		bool restore;///to restore or not data after execution
	};

	/**
	 * vector of hooks
	 */
	struct __execItemList
	{
		std::vector<__execItem> exec;
		bool execDisabled;
	};
	
	/**
	 * the very default value of operation type!!!
	 */
	enum xexecOperTypeEnum
	{
		XEXEC_NONE,
	};

	/**
	 * this structure initXexecModule function must return!
	 */
	struct xexecExMod
	{
		char *name;///name of module
		char *discription;///discription
		char *hook;///name of function
	};

	typedef xexecExMod (*initXexecModule)();

	/**
	 * class to provide wide abilities exec manipulations
	 * pre, post execs.
	 */
	class xexec
	{
		public:
					
			/*
			 * constructors and destructors
			 */
			xexec();	
			virtual ~xexec();
			/**
			 * functions to set(delete,replace) functions that will be executed before or after the exec call.
			 * they return number in list where function is set
			 * when u delete function from list, position of others don't change
			 * be carefull!! data are not copied!!!
			 * if restore param is set to false, data after callback function won't be restored
			 */
			virtual int _addPostExec(inExec func, dodoBase *obj, void *data) const;
			virtual int _addPreExec(inExec func, dodoBase *obj, void *data) const;
			virtual int _addPostExecModule(dodoBase *obj, std::string module, void *data) const;
			virtual int _addPreExecModule(dodoBase *obj, std::string module, void *data) const;
						
			virtual void delPostExec(unsigned int position) const;
			virtual void delPreExec(unsigned int position) const;
			virtual bool replacePostExec(unsigned int position, inExec func, dodoBase *obj, void *data) const;
			virtual bool replacePreExec(unsigned int position, inExec func, dodoBase *obj, void *data) const;			
			/**
			 * enable or disable hooks
			 * usefull when u don't want to make cyclic hook using for examle exec in hook from the class that called that hook
			 * in that case u have to disable
			 * if u called disable(enable)Post(Pre)Exec and then enable(disable)AllPre(Post)Exec = it won't be enabled(disabled)[it'll keep it's state!!!]
			 */
			virtual void disablePostExec(unsigned int position) const;
			virtual void disablePreExec(unsigned int position) const;
			
			virtual void enablePostExec(unsigned int position) const;
			virtual void enablePreExec(unsigned int position) const;
			
			virtual void enableAllPreExec() const;
			virtual void enableAllPostExec() const;
			virtual void disableAllPreExec() const;
			virtual void disableAllPostExec() const;
			
			virtual void disableAll() const;
			virtual void enableAll() const;
			
			/**
			 * prevent recursive hooks if u for example call exec in hook, if safeHooks disabled, all hooks set to this class will be called
			 */
			 mutable bool safeHooks;///enable\disable other hooks when executing hook
			/**
			 * executes smth. with given function(perform xexec)
			 * !!!you must overload it!
			 *
			 *bool 
			 *	derivedClass::exec() const
			 *	{
			 *		performXExec(preExec);
			 *		///execute 
			 *		performXExec(postExec);
			 *	}
			 *
			 */
			virtual bool exec() const;
			
			mutable int operType;///detect operation type
			
		protected:
			
			/**
			 * adds/deletes/replaces XExec to list
			 */
			inline virtual int addXExec(std::vector<__execItem> &list, inExec func, dodoBase *obj, void *data) const;
			inline virtual void delXExec(std::vector<__execItem> &list, unsigned int position) const;
			inline virtual bool replaceXExec(std::vector<__execItem> &list, unsigned int position, inExec func, dodoBase *obj, void *data) const;
			
			/**
			 * set state(enable/disable) for XExec(all XExecs) 
			 */
			virtual void setStatXExec(std::vector<__execItem> &list, unsigned int position, bool stat) const;
			
			/**
			 * loads from external object
			 */
			inline virtual int addXExecModule(std::vector<__execItem> &list, dodoBase *obj, std::string module, void *data) const;
			
			/**
			 * perform enabled functions from the list
			 */
			virtual void performXExec(__execItemList &list) const;
			
			mutable __execItemList preExec;///functions executed before exec
			mutable __execItemList postExec;///functions executed after exec
			
			mutable void *handles[XEXEC_MAXMODULES];
			mutable int handlesOpened;
	};
};


#endif
