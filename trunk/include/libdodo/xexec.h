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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _XEXEC_H_
#define _XEXEC_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo
{
	/**
	 * @enum xexecObjectTypeEnum defines objects that access xexec
	 */
	enum xexecObjectTypeEnum
	{
		XEXEC_OBJECT_XEXEC,
		XEXEC_OBJECT_DATABASEMYSQL,
		XEXEC_OBJECT_DATABASESQLITE,
		XEXEC_OBJECT_DATABASEPOSTGRESQL,
		XEXEC_OBJECT_IOSTDIO,
		XEXEC_OBJECT_IOFILEREGULAR,
		XEXEC_OBJECT_IOFILETEMP,
		XEXEC_OBJECT_IOFILEFIFO,
		XEXEC_OBJECT_IOPIPE,
		XEXEC_OBJECT_IOMEMORY,
		XEXEC_OBJECT_IONETWORKCLIENT,
		XEXEC_OBJECT_IONETWORKSERVER,
		XEXEC_OBJECT_IONETWORKEXCHANGE,
		XEXEC_OBJECT_IONETWORKSSLCLIENT,
		XEXEC_OBJECT_IONETWORKSSLSERVER,
		XEXEC_OBJECT_IONETWORKSSLEXCHANGE,
		XEXEC_OBJECT_CGIFASTEXCHANGE,
		XEXEC_OBJECT_CGIBASICEXCHANGE,
		XEXEC_OBJECT_GRAPHICSIMAGE,
	};

	class __xexecCollectedData__;

	/**
	 * @typedef inExec
	 * @brief defines function that will be called as hook
	 * @param odata defines object data
	 * @param object defines type of object that called hook[see xexecObjectTypeEnum]
	 * @param udata defines user data
	 */
	typedef void (*inExec)(__xexecCollectedData__ *odata, short object, void *udata);

	struct __xexecItem__;
	struct __xexecItemList__;

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
	 * @struct __xexecExMod__
	 * @brief is returned from initXexecModule in the library
	 */
	struct __xexecMod__
	{
		char  name[64];         ///< name of the library
		char  discription[256]; ///< discription of the library
		char  hook[64];         ///< name of function in module that will be a hook
		short execType;         ///< type of hook[see xexecModuleActionTypeEnum]
	};

	/**
	 * @typedef initXexecModule
	 * @brief defines type of init function for library
	 * @param data defines user data
	 */
	typedef __xexecMod__ (*initXexecModule)(void *data);

	/**
	 * @typedef deinitXexecModule
	 * @brief defines type of deinit function for library
	 */
	typedef void (*deinitXexecModule)();

	/**
	 * @struct __xexecCounts__
	 * @brief defines identificator of pre or/and post exec
	 * @note -1 if not set
	 */
	struct __xexecCounts__
	{
		/**
		 * constructor
		 */
		__xexecCounts__();

		int pre;                ///< identificator of preExec
		int post;               ///< identificator of postExec
	};
#endif

	class xexec;

	/**
	 * @class __xexecCollectedData__
	 * @brief defines data that could be retrieved from the object where xexec action raised
	 */
	class __xexecCollectedData__
	{
	  public:

		/**
		 * constructor
		 * @param executor defines class that executed hook
		 * @param execObject defines type of object that executed a hook[see xexecObjectTypeEnum]
		 */
		__xexecCollectedData__(xexec *executor,
							 short execObject);

		/**
		 * @param execObject defines type of object that executed a hook[see xexecObjectTypeEnum]
		 */
		void setExecObject(short execObject);

		int &operType;      ///< xexec operation

		xexec *executor;    ///< class that executed hook
	};

	/**
	 * example of exec function that performs xexec
	 *
	 *	void
	 *	derivedClass::exec()
	 *	{
	 *		performXExec(preExec);
	 * ///< execute routine
	 *		performXExec(postExec);
	 *	}
	 */

	/**
	 * @class xexec
	 * @brief provides pre/post exec actions for defrived classes
	 */
	class xexec
	{
		friend class __xexecCollectedData__;

	  private:

		/*
		 * copy constructor
		 * @note to prevent copying
		 */
		xexec(const xexec &exec);

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
		 * @param data defines hook data
		 */
		virtual int addPostExec(inExec func,
								void   *data);

		/**
		 * set function that will be executed before the main action call
		 * @return preExec identificator
		 * @param func defines function that will be called
		 * @param data defines hook data
		 */
		virtual int addPreExec(inExec func,
							   void   *data);

#ifdef DL_EXT
		/**
		 * set function that will be executed after the main action call
		 * @return postExec identificator
		 * @param path defines path to the library[if not in ldconfig db] or library name
		 * @param data defines hook data
		 * @param toInit defines data that will be passed to the init function
		 * if applied modules more than XEXEC_MAXMODULES, will return -1[see directives.h]
		 */
		virtual int addPostExec(const dodoString &path,
								void             *data,
								void             *toInit = NULL);

		/**
		 * set function that will be executed before the main action call
		 * @return preExec identificator
		 * @param path defines path to the library[if not in ldconfig db] or library name
		 * @param data defines hook data
		 * @param toInit defines data that will be passed to the init function
		 * if applied modules more than XEXEC_MAXMODULES, will return -1[see directives.h]
		 */
		virtual int addPreExec(const dodoString &path,
							   void             *data,
							   void             *toInit = NULL);

		/**
		 * set function that will be executed before/after the main action call
		 * @return pre/postExec identificators
		 * @param path defines path to the library[if not in ldconfig db] or library name
		 * @param data defines hook data
		 * @param toInit defines data that will be passed to the init function
		 * @note type of hook[pre/post] is defined in module
		 * if applied modules more than XEXEC_MAXMODULES, will return -1[see directives.h]
		 */
		virtual __xexecCounts__ addExec(const dodoString &path,
									  void             *data,
									  void             *toInit = NULL);
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
		virtual void replacePostExec(int    position,
									 inExec func,
									 void   *data);

		/**
		 * replace hook with another one
		 * @param position defines preExec identificator
		 * @param func defines function that will be called
		 * @param data defines hook data
		 */
		virtual void replacePreExec(int    position,
									inExec func,
									void   *data);

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
		 * true by default
		 */
		bool safeHooks;

#ifdef DL_EXT
		/**
		 * @return information about module
		 * @param path defines path to the library[if not in ldconfig db] or library name
		 * @param toInit defines data that will be passed to the init function
		 */
		static __xexecMod__ getModuleInfo(const dodoString &path,
										void             *toInit = NULL);
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
		virtual bool getXexec(dodoList<__xexecItem__ *> &list,
							  int                   position);

		/**
		 * set hook function
		 * @return postExec identificator
		 * @param list defines list of hooks
		 * @param func defines function that will be called
		 * @param data defines hook data
		 */
		virtual int addXExec(dodoList<__xexecItem__ *> &list,
							 inExec                func,
							 void                  *data);

		/**
		 * delete hook from list
		 * @param list defines list of hooks
		 * @param position defines XExec identificator
		 */
		virtual void delXExec(dodoList<__xexecItem__ *> &list,
							  int                   position);

		/**
		 * replace hook with another one
		 * @param list defines list of hooks
		 * @param position defines postExec identificator
		 * @param func defines function that will be called
		 * @param data defines hook data
		 */
		virtual void replaceXExec(dodoList<__xexecItem__ *> &list,
								  int                   position,
								  inExec                func,
								  void                  *data);

		/**
		 * set state(enable/disable) for XExec
		 * @param list defines list of hooks
		 * @param position defines postExec identificator
		 * @param stat defines hook enabled state
		 */
		virtual void setStatXExec(dodoList<__xexecItem__ *> &list,
								  int                   position,
								  bool                  stat);

#ifdef DL_EXT
		/**
		 * set function that will be executed after the main action call
		 * @return XExec identificator
		 * @param list defines list of hooks
		 * @param path defines path to the library[if not in ldconfig db] or library name
		 * @param data defines hook data
		 * @param toInit defines data that will be passed to the init function
		 */
		virtual int addXExecModule(dodoList<__xexecItem__ *> &list,
								   const dodoString      &path,
								   void                  *data,
								   void                  *toInit = NULL);
#endif

		/**
		 * perform enabled hooks
		 * @param list defines list of hooks
		 */
		virtual void performXExec(__xexecItemList__ *list) const;

		mutable __xexecItemList__ *preExec;            ///< preExec hooks
		mutable __xexecItemList__ *postExec;           ///< postExec hooks

		int execs;                                  ///< hook counter

		dodoList<__xexecItem__ *>::iterator current;    ///< iterator for list[for matched with getXexec method]

		mutable int operType;                       ///< operation type set by main action

		short execObject;                           ///< type of object[see xexecObjectTypeEnum]
		__xexecCollectedData__ *execObjectData;       ///< object data
	};
};

#endif
