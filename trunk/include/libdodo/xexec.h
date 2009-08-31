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
	  private:

		/*
		 * copy constructor
		 * @note to prevent copying
		 */
		xexec(const xexec &);

	  public:

#ifdef DL_EXT
		/**
		 * @struct __module__
		 * @brief is returned from moduleInit in the library
		 */
		struct __module__ {
			char  name[64];         ///< name of the library
			char  discription[256]; ///< discription of the library
			char  hook[64];         ///< name of function in the library that will be used as hook
			short type;				///< type of hook, @see xexec::actionEnum, could me grouped with OR
		};

		/**
		 * @typedef moduleInit
		 * @brief defines type of init function for library
		 * @param data defines user data
		 * @note name in the library must be initXexecModule
		 */
		typedef __module__ (*moduleInit)(void *data);

		/**
		 * @typedef moduleDeinit
		 * @brief defines type of deinit function for library
		 * @note name in the library must be deinitXexecModule
		 */
		typedef void (*moduleDeinit)();
#endif

		/**
		 * @enum objectEnum defines objects that access xexec
		 */
		enum objectEnum {
			OBJECT_XEXEC,
			OBJECT_DATABASEMYSQL,
			OBJECT_DATABASESQLITE,
			OBJECT_DATABASEPOSTGRESQL,
			OBJECT_IOSTDIO,
			OBJECT_IOFILEREGULAR,
			OBJECT_IOFILETEMP,
			OBJECT_IOFILEFIFO,
			OBJECT_IOPIPE,
			OBJECT_IOMEMORY,
			OBJECT_IONETWORKCLIENT,
			OBJECT_IONETWORKSERVER,
			OBJECT_IONETWORKEXCHANGE,
			OBJECT_IONETWORKSSLCLIENT,
			OBJECT_IONETWORKSSLSERVER,
			OBJECT_IONETWORKSSLEXCHANGE,
			OBJECT_CGIFASTEXCHANGE,
			OBJECT_CGIBASICEXCHANGE,
			OBJECT_GRAPHICSIMAGE,
		};

		/**
		 * @enum operationEnum defines default operation type
		 */
		enum operationEnum {
			OPERATION_NONE,
		};

		/**
		 * @enum actionEnum defines what type of exec[pre/post] will be used for module
		 */
		enum actionEnum {
			ACTION_PREEXEC = 2,
			ACTION_POSTEXEC = 4,
		};

		/**
		 * @class __collected_data__
		 * @brief defines data that could be retrieved from the object where xexec action raised
		 */
		class __collected_data__ {
		  public:

			/**
			 * constructor
			 * @param executor defines class that executed hook
			 * @param execObject defines type of object that executed a hook, @see xexec::objectEnum
			 */
			__collected_data__(xexec *executor,
								   short execObject);

			/**
			 * @param execObject defines type of object that executed a hook, @see xexec::objectEnum
			 */
			void setExecObject(short execObject);

			int &operType;      ///< xexec operation

			xexec *executor;    ///< class that executed hook
		};

		/**
		 * @typedef hook
		 * @brief defines function that will be called as hook
		 * @param odata defines object data
		 * @param object defines type of object that called hook, @see xexec::objectEnum
		 * @param udata defines user data
		 */
		typedef void (*hook)(__collected_data__ *odata, short object, void *udata);

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
		 * @param type defines type of hook, @see xexec::actionEnum
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
		void removeXExec(int id);

		/**
		 * enable/disable other hooks during hook execution
		 * @note this prevents recursive hooks calls
		 * true by default
		 */
		bool safeXExecs;

		mutable bool                    disableXExecs;	  ///< if true hooks are disabled

#ifdef DL_EXT
		/**
		 * @return information about module
		 * @param path defines path to the library[if not in ldconfig db] or library name
		 * @param initData defines data that will be passed to the init function
		 */
		static __module__ module(const dodoString &path,
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
		 * @struct __item__
		 * @brief defines xexec node
		 */
		struct __item__ {
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
		bool getXexec(dodoList<__item__> &list,
							  int                       id);

		/**
		 * set hook function
		 * @return postExec identificator
		 * @param list defines list of hooks
		 * @param func defines function that will be called
		 * @param data defines hook data
		 */
		int addXExec(dodoList<__item__> &list,
							 hook                    func,
							 void                      *data);

		/**
		 * perform enabled hooks
		 * @param list defines list of hooks
		 */
		void performXExec(dodoList<__item__> &list) const;

		mutable dodoList<__item__> preExec;      ///< preExec hooks
		mutable dodoList<__item__> postExec;     ///< postExec hooks

		int execs;                                      ///< hook counter

		dodoList<__item__>::iterator current;    ///< iterator for list[for matched with getXexec method]

		short execObject;                               ///< type of object, @see xexec::objectEnum
		__collected_data__ *execObjectData;         ///< object data
	};
};
#endif
