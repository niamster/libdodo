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

xexec::xexec() : safeXExecs(true),
				 operType(XEXEC_OPERTYPE_NONE),
				 execs(0),
				 execObject(XEXEC_OBJECT_XEXEC),
				 execObjectData(NULL),
				 disabledXExecs(false)
{
}

//-------------------------------------------------------------------

xexec::~xexec()
{
#ifdef DL_EXT
	xexecModuleDeinit deinit;

	dodoList<__xexecItem__>::iterator i(preExec.begin()), j(preExec.end());
	for (; i != j; ++i) {
		if (i->handle == NULL)
			continue;

		deinit = (xexecModuleDeinit)dlsym(i->handle, "xexecModuleDeinit");
		if (deinit != NULL)
			deinit();

#ifndef DL_FAST
		dlclose(i->handle);
#endif
	}

	i = postExec.begin();
	j = postExec.end();
	for (; i != j; ++i) {
		if (i->handle == NULL)
			continue;

		deinit = (xexecModuleDeinit)dlsym(i->handle, "xexecModuleDeinit");
		if (deinit != NULL)
			deinit();

#ifndef DL_FAST
		dlclose(i->handle);
#endif
	}
#endif
}

//-------------------------------------------------------------------

int
xexec::addXExec(dodoList<__xexecItem__> &list,
				hook                    func,
				void                      *data)
{
	__xexecItem__ e;

	e.data = data;
	e.func = func;
	e.id   = ++execs;

#ifdef DL_EXT
	e.handle = NULL;
#endif

	list.push_back(e);

	return e.id;
}

//-------------------------------------------------------------------

void
xexec::delXExec(int id)
{
	if (getXexec(preExec, id))
		preExec.erase(current);
	else if (getXexec(postExec, id))
		postExec.erase(current);
	else
		return;

#ifdef DL_EXT
	if (current->handle != NULL) {
		xexecModuleDeinit deinit;

		deinit = (xexecModuleDeinit)dlsym(current->handle, "xexecModuleDeinit");
		if (deinit != NULL)
			deinit();

#ifndef DL_FAST
		if (dlclose(current->handle) != 0)
			throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_DEL, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif
	}
#endif
}

//-------------------------------------------------------------------

int
xexec::addXExec(short type,
		   hook func,
		   void   *data)
{
	if (type == XEXEC_ACTION_PREEXEC)
		return addXExec(preExec, func, data);
	else if (type == XEXEC_ACTION_POSTEXEC)
		return addXExec(postExec, func, data);
	else
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADD, exception::ERRNO_LIBDODO, XEXECEX_WRONGTYPE, XEXECEX_WRONGTYPE_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
xexec::performPreExec() const
{
	performXExec(preExec);
}

//-------------------------------------------------------------------

void
xexec::performPostExec() const
{
	performXExec(postExec);
}

//-------------------------------------------------------------------

void
xexec::performXExec(dodoList<__xexecItem__> &list) const
{
	if (disabledXExecs)
		return;

	dodoList<__xexecItem__>::iterator i(list.begin()), j(list.end());

	for (; i != j; ++i) {
		if (safeXExecs)
			disabledXExecs = true;

		i->func(execObjectData, execObject, i->data);

		if (safeXExecs)
			disabledXExecs = false;
	}
}

//-------------------------------------------------------------------

#ifdef DL_EXT
__xexecModule__
xexec::getModuleInfo(const dodoString &module,
					 void             *initData)
{
#ifdef DL_FAST
	void *handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	xexecModuleInit init = (xexecModuleInit)dlsym(handle, "xexecModuleInit");
	if (init == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__xexecModule__ mod = init(initData);

#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_GETMODULEINFO, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

	return mod;
}

//-------------------------------------------------------------------

void
xexec::addXExec(const dodoString &module,
		   void             *data,
		   int			  &preExecId,
		   int			  &postExecId,
		   void             *initData)
{
	__xexecItem__ e;

	e.data = data;

#ifdef DL_FAST
	e.handle = dlopen(module.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	e.handle = dlopen(module.c_str(), RTLD_LAZY);
#endif
	if (e.handle == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	xexecModuleInit init = (xexecModuleInit)dlsym(e.handle, "xexecModuleInit");
	if (init == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	__xexecModule__ info = init(initData);

	hook in = (hook)dlsym(e.handle, info.hook);
	if (in == NULL)
		throw exception::basic(exception::ERRMODULE_XEXEC, XEXECEX_ADD, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	e.func = in;

	if (info.type&XEXEC_ACTION_PREEXEC) {
		e.id = ++execs;
		preExec.push_back(e);
		preExecId = e.id;
	}

	if (info.type&XEXEC_ACTION_POSTEXEC) {
		e.id = ++execs;
		postExec.push_back(e);
		postExecId = e.id;
	}
}

#endif

//-------------------------------------------------------------------

bool
xexec::getXexec(dodoList<__xexecItem__> &list,
				int                       id)
{
	dodoList<__xexecItem__>::iterator i(list.begin()), j(list.end());
	for (; i != j; ++i) {
		if (i->id == id) {
			current = i;

			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------

