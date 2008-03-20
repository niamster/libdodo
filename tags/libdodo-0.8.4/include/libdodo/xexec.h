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
	 * @enum xexecObjectTypeEnum defines objects that access xexec
	 */
	enum xexecObjectTypeEnum
	{
		XEXEC_OBJECT_DBMYSQL,
		XEXEC_OBJECT_DBSQLITE,
		XEXEC_OBJECT_IOSTD,
		XEXEC_OBJECT_IODISK,
		XEXEC_OBJECT_IOSOCKET,
		XEXEC_OBJECT_IOCGIFAST,
		XEXEC_OBJECT_IOSOCKETEXCHANGE,
		XEXEC_OBJECT_DBPOSTGRESQL,
		XEXEC_OBJECT_IMAGE,
	};

	/**
	 * @typedef inExec defines function that will be called as hook
	 * @param first is pointer class object that uses hook
	 * @param second is type of object that called hook
	 * @param third is data needed for hook
	 */
	typedef void (*inExec)(void *, short, void *);

	/**
	 * @struct __execItem defines xexec node
	 */
	struct __execItem
	{
		inExec func;    ///< function to execute
		short type;     ///< type of object[see xexecObjTypeEnum]
		void *data;     ///< function data
		void *obj;      ///< pointer to object that uses hook
		bool enabled;   ///< if true hook is enabled
		int position;   ///< object identificator

#ifdef DL_EXT
		void *handle;    ///< handle to library
#endif
	};

	/**
	 * @struct __execItemList defines collection of hooks
	 */
	struct __execItemList
	{
		dodoList<__execItem> exec;  ///< hooks
		bool execDisabled;          ///< if true hooks are disabled
	};

	/**
	 * @enum xexecOperTypeEnum defines default operation type
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
	 * @struct __xexecExMod is returned from initXexecModule in the library
	 */
	struct __xexecMod
	{
		char name[64];              ///< name of the library
		char discription[256];      ///< discription of the library
		char hook[64];              ///< name of function in module that will be a hook
		short execType;             ///< type of hook[see xexecModuleActionTypeEnum]
	};

	/**
	 * @typedef initXexecModule defines type of init function for library
	 */
	typedef __xexecMod (*initXexecModule)(void *);

	/**
	 * @typedef deinitXexecModule defines type of deinit function for library
	 */
	typedef void (*deinitXexecModule)();

	/**
	 * @struct __xexecCounts defines identificator of pre or/and post exec
	 * @note -1 if not set
	 */
	struct __xexecCounts
	{
		/**
		 * constructor
		 */
		__xexecCounts();

		int pre;        ///< identificator of preExec
		int post;       ///< identificator of postExec
	};

#endif

	/**
	 * example of exec function that performs xexec
	 *
	 *	void
	 *	derivedClass::exec()
	 *	{
	 *		performXExec(preExec);
	 *		///< execute routine
	 *		performXExec(postExec);
	 *	}
	 */

	/**
	 * @class xexec provides pre/post exec actions for defrived classes
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
			 * @return postExec identificator
			 * @param func defines function that will be called
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 */
			virtual int _addPostExec(inExec func, void *obj, short type, void *data);

			/**
			 * set function that will be executed before the main action call
			 * @return preExec identificator
			 * @param func defines function that will be called
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 */
			virtual int _addPreExec(inExec func, void *obj, short type, void *data);

#ifdef DL_EXT

			/**
			 * set function that will be executed after the main action call
			 * @return postExec identificator
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int _addPostExec(const dodoString &path, void *obj, short type, void *data, void *toInit = NULL);   ///< if applied modules more than XEXEC_MAXMODULES, will return -1[see directives.h]

			/**
			 * set function that will be executed before the main action call
			 * @return preExec identificator
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int _addPreExec(const dodoString &path, void *obj, short type, void *data, void *toInit = NULL);   ///< if applied modules more than XEXEC_MAXMODULES, will return -1[see directives.h]

			/**
			 * set function that will be executed before/after the main action call
			 * @return pre/postExec identificators
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 * @note type of hook[pre/post] is defined in module
			 */
			virtual __xexecCounts _addExec(const dodoString &path, void *obj, short type, void *data, void *toInit = NULL);   ///< if applied modules more than XEXEC_MAXMODULES, will return -1[see directives.h]

#endif

			/**
			 * delete hook
			 * @param position defines postExec identificator
			 */
			virtual void delPostExec(int position);

			/**
			 * delete hook
			 * @param position defines preExec identificator
			 */
			virtual void delPreExec(int position);

			/**
			 * replace hook with another one
			 * @param position defines postExec identificator
			 * @param func defines function that will be called
			 * @param data defines hook data
			 */
			virtual void replacePostExec(int position, inExec func, void *data);

			/**
			 * replace hook with another one
			 * @param position defines preExec identificator
			 * @param func defines function that will be called
			 * @param data defines hook data
			 */
			virtual void replacePreExec(int position, inExec func, void *data);

			/**
			 * disable hook
			 * @param position defines postExec identificator
			 * @note usefull to avoid hook calling during hook execution
			 */
			virtual void disablePostExec(int position);

			/**
			 * disable hook
			 * @param position defines preExec identificator
			 * @note usefull to avoid hook calling during hook execution
			 */
			virtual void disablePreExec(int position);

			/**
			 * enable hook
			 * @param position defines postExec identificator
			 */
			virtual void enablePostExec(int position);

			/**
			 * enable hook
			 * @param position defines preExec identificator
			 */
			virtual void enablePreExec(int position);

			/**
			 * enable all preExec hooks
			 */
			virtual void enableAllPreExec();

			/**
			 * enable all postExec hooks
			 */
			virtual void enableAllPostExec();

			/**
			 * disable all preExec hooks
			 * @note useful to avoid hooks calling during hook execution
			 */
			virtual void disableAllPreExec();

			/**
			 * disable all postExec hooks
			 * @note useful to avoid hooks calling during hook execution
			 */
			virtual void disableAllPostExec();

			/**
			 * disable all postExec and preExec hooks
			 * @note useful to avoid hooks calling during hook execution
			 */
			virtual void disableAll();

			/**
			 * enable all postExec and preExec hooks
			 */
			virtual void enableAll();

			/**
			 * enable/disable other hooks during hook execution
			 * @note this prevents recursive hooks calls
			 */
			bool safeHooks;

#ifdef DL_EXT

			/**
			 * @return information about module
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param toInit defines data that will be passed to the init function
			 */
			static __xexecMod getModuleInfo(const dodoString &path, void *toInit = NULL);

#endif

		protected:

			/**
			 * enable all preExec hooks
			 */
			virtual void enableAllPreExec() const;

			/**
			 * enable all postExec hooks
			 */
			virtual void enableAllPostExec() const;

			/**
			 * disable all preExec hooks
			 * @note useful to avoid hooks calling during hook execution
			 */
			virtual void disableAllPreExec() const;

			/**
			 * disable all postExec hooks
			 * @note useful to avoid hooks calling during hook execution
			 */
			virtual void disableAllPostExec() const;

			/**
			 * @return true if found
			 * @param list defines list of hooks
			 * @param position defines XExec identificator
			 */
			virtual bool getXexec(dodoList<__execItem> &list, int position);

			/**
			 * set hook function
			 * @return postExec identificator
			 * @param list defines list of hooks
			 * @param func defines function that will be called
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 */
			virtual int addXExec(dodoList<__execItem> &list, inExec func, void *obj, short type, void *data);

			/**
			 * delete hook from list
			 * @param list defines list of hooks
			 * @param position defines XExec identificator
			 */
			virtual void delXExec(dodoList<__execItem> &list, int position);

			/**
			 * replace hook with another one
			 * @param list defines list of hooks
			 * @param position defines postExec identificator
			 * @param func defines function that will be called
			 * @param data defines hook data
			 */
			virtual void replaceXExec(dodoList<__execItem> &list, int position, inExec func, void *data);

			/**
			 * set state(enable/disable) for XExec
			 * @param list defines list of hooks
			 * @param position defines postExec identificator
			 * @param stat defines hook enabled state
			 */
			virtual void setStatXExec(dodoList<__execItem> &list, int position, bool stat);

#ifdef DL_EXT

			/**
			 * set function that will be executed after the main action call
			 * @return XExec identificator
			 * @param list defines list of hooks
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param obj defines object that called the hook
			 * @param type defines hook type[see xexecObjTypeEnum]
			 * @param data defines hook data
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addXExecModule(dodoList<__execItem> &list, const dodoString &path, void *obj, short type, void *data, void *toInit = NULL);

#endif

			/**
			 * perform enabled hooks
			 * @param list defines list of hooks
			 */
			virtual void performXExec(__execItemList &list) const;

			mutable __execItemList preExec;             ///< preExec hooks
			mutable __execItemList postExec;            ///< postExec hooks

			int execs;                                  ///< hook counter

			dodoList<__execItem>::iterator current;     ///< iterator for list[for matched with getXexec method]

			mutable bool collectData;                   ///< if true to collect data for xexec[true by default]

			mutable int operType;                       ///< operation type set by main action
	};

};

#endif
