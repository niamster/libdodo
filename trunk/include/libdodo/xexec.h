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

namespace dodo {
	/**
	 * @enum xexecObjectTypeEnum defines objects that access xexec
	 */
	enum xexecObjectTypeEnum {
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
	 * @typedef hook
	 * @brief defines function that will be called as hook
	 * @param odata defines object data
	 * @param object defines type of object that called hook[see xexecObjectTypeEnum]
	 * @param udata defines user data
	 */
	typedef void (*hook)(__xexecCollectedData__ *odata, short object, void *udata);

	/**
	 * @enum xexecOperTypeEnum defines default operation type
	 */
	enum xexecOperTypeEnum {
		XEXEC_OPERTYPE_NONE,
	};

	/**
	 * @enum xexecModuleActionTypeEnum defines what type of exec[pre/post] will be used for module
	 */
	enum xexecActionEnum {
		XEXEC_ACTION_PREEXEC = 2,
		XEXEC_ACTION_POSTEXEC = 4,
	};

#ifdef DL_EXT
	/**
	 * @struct __xexecModule__
	 * @brief is returned from xexecModuleInit in the library
	 */
	struct __xexecModule__ {
		char  name[64];         ///< name of the library
		char  discription[256]; ///< discription of the library
		char  hook[64];         ///< name of function in the library that will be used as hook
		short type;				///< type of hook[see xexecActionEnum][could me grouped with OR]
	};

	/**
	 * @typedef xexecModuleInit
	 * @brief defines type of init function for library
	 * @param data defines user data
	 */
	typedef __xexecModule__ (*xexecModuleInit)(void *data);

	/**
	 * @typedef xexecModuleDeinit
	 * @brief defines type of deinit function for library
	 */
	typedef void (*xexecModuleDeinit)();
#endif

	class xexec;

	/**
	 * @class __xexecCollectedData__
	 * @brief defines data that could be retrieved from the object where xexec action raised
	 */
	class __xexecCollectedData__ {
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
	 *      ///< execute routine
	 *		performXExec(postExec);
	 *	}
	 */

	/**
	 * @class xexec
	 * @brief provides pre/post exec actions for defrived classes
	 */
	class xexec {
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
		 * set function that will be executed before/after the main action call
		 * @return exec identificator
		 * @param type defines type of hook[see xexecActionEnum]
		 * @param func defines function that will be called
		 * @param data defines hook data
		 */
		int addXExec(short type,
						hook  func,
						void  *data);

#ifdef DL_EXT
		/**
		 * set function that will be executed before and/or after the main action call
		 * @param path defines path to the library[if not in ldconfig db] or library name
		 * @param data defines hook data
		 * @param initData defines data that will be passed to the init function
		 * @param preExecId defines pre-exec identificator if hook was set or -1
		 * @param postExecId defines post-exec identificator if hook was set or -1
		 * @note type of hook is defined in module
		 */
		void addXExec(const dodoString &path,
						 void             *data,
						 int			  &preExecId,
						 int			  &postExecId,
						 void             *initData = NULL);
#endif

		/**
		 * delete hook
		 * @param id defines exec identificator
		 */
		void delXExec(int id);

		/**
		 * enable/disable other hooks during hook execution
		 * @note this prevents recursive hooks calls
		 * true by default
		 */
		bool safeXExecs;

		mutable bool                    disabledXExecs;	  ///< if true hooks are disabled

#ifdef DL_EXT
		/**
		 * @return information about module
		 * @param path defines path to the library[if not in ldconfig db] or library name
		 * @param initData defines data that will be passed to the init function
		 */
		static __xexecModule__ getModuleInfo(const dodoString &path,
										  void             *initData = NULL);
#endif

	  protected:

		/**
		 * perform preExec hooks
		 */
		void performPreExec() const;

		/**
		 * perform preExec hooks
		 */
		void performPostExec() const;

		mutable int operType;                           ///< operation type set by main action

	  private:

		/**
		 * @struct __xexecItem__
		 * @brief defines xexec node
		 */
		struct __xexecItem__ {
			hook func;                                ///< function to execute
			void   *data;                               ///< user data
			int    id;                            ///< object identificator
#ifdef DL_EXT
			void   *handle;                             ///< handle to library
#endif
		};

		/**
		 * @return true if found
		 * @param list defines list of hooks
		 * @param id defines XExec identificator
		 */
		bool getXexec(dodoList<__xexecItem__> &list,
							  int                       id);

		/**
		 * set hook function
		 * @return postExec identificator
		 * @param list defines list of hooks
		 * @param func defines function that will be called
		 * @param data defines hook data
		 */
		int addXExec(dodoList<__xexecItem__> &list,
							 hook                    func,
							 void                      *data);

		/**
		 * perform enabled hooks
		 * @param list defines list of hooks
		 */
		void performXExec(dodoList<__xexecItem__> &list) const;

		mutable dodoList<__xexecItem__> preExec;      ///< preExec hooks
		mutable dodoList<__xexecItem__> postExec;     ///< postExec hooks

		int execs;                                      ///< hook counter

		dodoList<__xexecItem__>::iterator current;    ///< iterator for list[for matched with getXexec method]

		short execObject;                               ///< type of object[see xexecObjectTypeEnum]
		__xexecCollectedData__ *execObjectData;         ///< object data
	};
};

#endif
