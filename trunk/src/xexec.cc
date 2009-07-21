/***************************************************************************
 *            xexec.cc
 *
 *  Wed Aug 24 00:39:10 2005
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

#include <libdodo/directives.h>

#ifdef DL_EXT
#include <dlfcn.h>
#endif

#include <libdodo/xexec.h>
#include <libdodo/xexecEx.h>
#include <libdodo/types.h>

namespace dodo {
};

using namespace dodo;

__xexecCollectedData__::__xexecCollectedData__(xexec *executor,
											   short execObject) : operType(executor->operType),
																   executor(executor)
{
	executor->execObjectData = this;
	executor->execObject = execObject;
}

void
__xexecCollectedData__::setExecObject(short execObject)
{
	executor->execObject = execObject;
}

//-------------------------------------------------------------------

xexec::xexec(const xexec &exec)
{
}

//-------------------------------------------------------------------

xexec::xexec() : safeHooks(true),
				 operType(XEXEC_OPERTYPE_NONE),
				 execs(0),
				 execObject(XEXEC_OBJECT_XEXEC),
				 execObjectData(NULL)
{
	preExec.execDisabled = false;
	postExec.execDisabled = false;
}

//-------------------------------------------------------------------

xexec::~xexec()
{
#ifdef DL_EXT
	deinitXexecModule deinit;

	dodoList<__xexecItem__ *>::iterator i(preExec.exec.begin()), j(preExec.exec.end());
	for (; i != j; ++i) {
		if ((*i)->handle == NULL)
			continue;

		deinit = (deinitXexecModule)dlsym((*i)->handle, "deinitXexecModule");
		if (deinit != NULL)
			deinit();

#ifndef DL_FAST
		dlclose((*i)->handle);
#endif
	}

	i = postExec.exec.begin();
	j = postExec.exec.end();
	for (; i != j; ++i) {
		if ((*i)->handle == NULL)
			continue;

		deinit = (deinitXexecModule)dlsym((*i)->handle, "deinitXexecModule");
		if (deinit != NULL)
			deinit();

#ifndef DL_FAST
		dlclose((*i)->handle);
#endif

		delete *i;
	}
#endif
}

//-------------------------------------------------------------------

int
xexec::addXExec(dodoList<__xexecItem__ *> &list,
				inExec                    func,
				void                      *data)
{
	__xexecItem__ *e = new __xexecItem__;

	e->data = data;
	e->func = func;
	e->position = ++execs;
	e->enabled = true;

#ifdef DL_EXT
	e->handle = NULL;
#endif

	list.push_back(e);

	return e->position;
}

//-------------------------------------------------------------------

void
xexec::setStatXExec(dodoList<__xexecItem__ *> &list,
					int                       position,
					bool                      stat)
{
	if (getXexec(list, position))
		(*current)->enabled = stat;
}

//-------------------------------------------------------------------

void
xexec::delXExec(dodoList<__xexecItem__ *> &list,
				int                       position)
{
	if (getXexec(list, position)) {
#ifdef DL_EXT
		if ((*current)->handle != NULL) {
			deinitXexecModule deinit;

			deinit = (deinitXexecModule)dlsym((*current)->handle, "deinitXexecModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose((*current)->handle) != 0)
				throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_DELXEXEC, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif
		}
#endif

		delete *current;

		list.erase(current);
	}
}

//-------------------------------------------------------------------

int
xexec::addPreExec(inExec func,
				  void   *data)
{
	return addXExec(preExec.exec, func, data);
}

//-------------------------------------------------------------------

void
xexec::disablePreExec(int position)
{
	setStatXExec(preExec.exec, position, false);
}

//-------------------------------------------------------------------

void
xexec::delPreExec(int position)
{
	delXExec(preExec.exec, position);
}

//-------------------------------------------------------------------

void
xexec::enablePreExec(int position)
{
	setStatXExec(preExec.exec, position, true);
}

//-------------------------------------------------------------------

int
xexec::addPostExec(inExec func,
				   void   *data)
{
	return addXExec(postExec.exec, func, data);
}

//-------------------------------------------------------------------

void
xexec::disablePostExec(int position)
{
	setStatXExec(postExec.exec, position, false);
}

//-------------------------------------------------------------------

void
xexec::delPostExec(int position)
{
	delXExec(postExec.exec, position);
}

//-------------------------------------------------------------------

void
xexec::enablePostExec(int position)
{
	setStatXExec(postExec.exec, position, true);
}

//-------------------------------------------------------------------

void
xexec::enableAllPreExec() const
{
	preExec.execDisabled = false;
}

//-------------------------------------------------------------------

void
xexec::enableAllPostExec() const
{
	postExec.execDisabled = false;
}

//-------------------------------------------------------------------

void
xexec::disableAllPreExec() const
{
	preExec.execDisabled = true;
}

//-------------------------------------------------------------------

void
xexec::disableAllPostExec() const
{
	postExec.execDisabled = true;
}

//-------------------------------------------------------------------

void
xexec::enableAllPreExec()
{
	preExec.execDisabled = false;
}

//-------------------------------------------------------------------

void
xexec::enableAllPostExec()
{
	postExec.execDisabled = false;
}

//-------------------------------------------------------------------

void
xexec::disableAllPreExec()
{
	preExec.execDisabled = true;
}

//-------------------------------------------------------------------

void
xexec::disableAllPostExec()
{
	postExec.execDisabled = true;
}

//-------------------------------------------------------------------

void
xexec::disableAll()
{
	postExec.execDisabled = true;
	preExec.execDisabled = true;
}

//-------------------------------------------------------------------

void
xexec::enableAll()
{
	postExec.execDisabled = false;
	preExec.execDisabled = false;
}

//-------------------------------------------------------------------

void
xexec::replacePostExec(int    position,
					   inExec func,
					   void   *data)
{
	replaceXExec(postExec.exec, position, func, data);
}

//-------------------------------------------------------------------

void
xexec::replacePreExec(int    position,
					  inExec func,
					  void   *data)
{
	replaceXExec(preExec.exec, position, func, data);
}

//-------------------------------------------------------------------

void
xexec::replaceXExec(dodoList<__xexecItem__ *> &list,
					int                       position,
					inExec                    func,
					void                      *data)
{
	if (getXexec(list, position)) {
#ifdef DL_EXT
		if ((*current)->handle != NULL) {
			deinitXexecModule deinit;

			deinit = (deinitXexecModule)dlsym((*current)->handle, "deinitXexecModule");
			if (deinit != NULL)
				deinit();

#ifndef DL_FAST
			if (dlclose((*current)->handle) != 0)
				throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_DELXEXEC, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

			(*current)->handle = NULL;
		}
#endif

		(*current)->func = func;
		(*current)->data = data;
		(*current)->enabled = true;

#ifdef DL_EXT
																						(*current)->handle = NULL;
#endif
	} else
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_REPLACEXEXEC, exception::ERRNO_LIBDODO, XEXECEX_EXECNOTFOUND, XEXECEX_EXECNOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
xexec::performXExec(__xexecItemList__ &list) const
{
	if (list.execDisabled)
		return ;

	dodoList<__xexecItem__ *>::iterator i(list.exec.begin()), j(list.exec.end());

	for (; i != j; ++i) {
		if ((*i)->enabled) {
			if (safeHooks) {
				disableAllPostExec();
				disableAllPreExec();
			}
			(*i)->func(execObjectData, execObject, (*i)->data);
			if (safeHooks) {
				enableAllPostExec();
				enableAllPreExec();
			}
		}
	}
}

//-------------------------------------------------------------------

#ifdef DL_EXT
__xexecCounts__::__xexecCounts__() : pre(-1),
									 post(-1)
{
}

//-------------------------------------------------------------------

int
xexec::addXExecModule(dodoList<__xexecItem__ *> &list,
					  const dodoString          &module,
					  void                      *data,
					  void                      *toInit)
{
	__xexecItem__ *e = new __xexecItem__;

	e->data = data;
	e->position = ++execs;
	e->enabled = true;

#ifdef DL_FAST
	e->handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	e->handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (e->handle == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADDXEXECMODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initXexecModule init = (initXexecModule)dlsym(e->handle, "initXexecModule");
	if (init == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADDXEXECMODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	inExec in = (inExec)dlsym(e->handle, init(toInit).hook);
	if (in == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADDXEXECMODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	e->func = in;

	list.push_back(e);

	return e->position;
}

//-------------------------------------------------------------------

int
xexec::addPostExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return addXExecModule(postExec.exec, module, data, toInit);
}

//-------------------------------------------------------------------

int
xexec::addPreExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return addXExecModule(preExec.exec, module, data, toInit);
}

//-------------------------------------------------------------------

__xexecMod__
xexec::getModuleInfo(const dodoString &module,
					 void             *toInit)
{
#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initXexecModule init = (initXexecModule)dlsym(handle, "initXexecModule");
	if (init == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__xexecMod__ mod = init(toInit);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

	return mod;
}

//-------------------------------------------------------------------

__xexecCounts__
xexec::addExec(const dodoString &module,
			   void             *data,
			   void             *toInit)
{
	__xexecItem__ *e = new __xexecItem__;

	e->data = data;
	e->enabled = true;

#ifdef DL_FAST
	e->handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	e->handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (e->handle == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADDXEXECMODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	initXexecModule init = (initXexecModule)dlsym(e->handle, "initXexecModule");
	if (init == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADDXEXECMODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__xexecMod__ info = init(toInit);

	inExec in = (inExec)dlsym(e->handle, info.hook);
	if (in == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADDXEXECMODULE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	e->func = in;

	__xexecCounts__ count;

	switch (info.execType) {
		case XEXEC_MODULEACTIONTYPE_POST:

			e->position = ++execs;
			postExec.exec.push_back(e);
			count.post = e->position;

			break;

		case XEXEC_MODULEACTIONTYPE_PRE:

			e->position = ++execs;
			preExec.exec.push_back(e);
			count.pre = e->position;

			break;

		case XEXEC_MODULEACTIONTYPE_BOTH:

			e->position = ++execs;
			postExec.exec.push_back(e);
			count.post = e->position;

			e->position = ++execs;
			preExec.exec.push_back(e);
			count.pre = e->position;

			break;
	}

	return count;
}

#endif

//-------------------------------------------------------------------

bool
xexec::getXexec(dodoList<__xexecItem__ *> &list,
				int                       position)
{
	dodoList<__xexecItem__ *>::iterator i(list.begin()), j(list.end());
	for (; i != j; ++i) {
		if ((*i)->position == position) {
			current = i;

			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------

