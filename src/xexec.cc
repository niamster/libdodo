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

#include <xexec.h>

using namespace dodo;

xexec::xexec() : safeHooks(true),
	operType(XEXEC_NONE),
	execs(0)
{
	preExec.execDisabled = false;
	postExec.execDisabled = false;
}

//-------------------------------------------------------------------

xexec::~xexec()
{
    #ifdef DL_EXT

	deinitXexecModule deinit;

	std::list<__execItem>::iterator i(preExec.exec.begin()), j(preExec.exec.end());
	for (; i != j; ++i)
	{
		if (i->handle == NULL)
			continue;

		deinit = (deinitXexecModule)dlsym(i->handle, "deinitXexecModule");
		if (deinit != NULL)
			deinit();

		dlclose(i->handle);
	}

	i = postExec.exec.begin();
	j = postExec.exec.end();
	for (; i != j; ++i)
	{
		if (i->handle == NULL)
			continue;

		deinit = (deinitXexecModule)dlsym(i->handle, "deinitXexecModule");
		if (deinit != NULL)
			deinit();

		dlclose(i->handle);
	}

    #endif
}

//-------------------------------------------------------------------

int
xexec::addXExec(std::list<__execItem>&list,
				inExec func,
				void                  *obj,
				short type,
				void                  *data)
{
	__execItem temp;

	temp.data = data;
	temp.obj = obj;
	temp.func = func;
	temp.position = ++execs;
	temp.enabled = true;
	temp.type = type;

    #ifdef DL_EXT
	temp.handle = NULL;
    #endif

	list.push_back(temp);

	return temp.position;
}

//-------------------------------------------------------------------

void
xexec::setStatXExec(std::list<__execItem>&list,
					int position,
					bool stat)
{
	if (getXexec(list, position))
		current->enabled = stat;
}

//-------------------------------------------------------------------

void
xexec::delXExec(std::list<__execItem>&list,
				int position)
{
	if (getXexec(list, position))
	{
        #ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitXexecModule deinit;

			deinit = (deinitXexecModule)dlsym(current->handle, "deinitXexecModule");
			if (deinit != NULL)
				deinit();

			if (dlclose(current->handle) != 0)
                #ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC, XEXEC_DELXEXEC, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
				;
                #endif
		}

        #endif

		list.erase(current);
	}
}

//-------------------------------------------------------------------

int
xexec::_addPreExec(inExec func,
				   void   *obj,
				   short type,
				   void   *data)
{
	return addXExec(preExec.exec, func, obj, type, data);
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
xexec::_addPostExec(inExec func,
					void   *obj,
					short type,
					void   *data)
{
	return addXExec(postExec.exec, func, obj, type, data);
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

bool
xexec::replacePostExec(int position,
					   inExec func,
					   void   *data)
{
	return replaceXExec(postExec.exec, position, func, data);
}

//-------------------------------------------------------------------

bool
xexec::replacePreExec(int position,
					  inExec func,
					  void   *data)
{
	return replaceXExec(preExec.exec, position, func, data);
}

//-------------------------------------------------------------------

bool
xexec::replaceXExec(std::list<__execItem>&list,
					int position,
					inExec func,
					void                  *data)
{
	if (getXexec(list, position))
	{
        #ifdef DL_EXT

		if (current->handle != NULL)
		{
			deinitXexecModule deinit;

			deinit = (deinitXexecModule)dlsym(current->handle, "deinitXexecModule");
			if (deinit != NULL)
				deinit();

			if (dlclose(current->handle) != 0)
                #ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC, XEXEC_DELXEXEC, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
                #endif

				current->handle = NULL;
		}

        #endif

		current->func = func;
		current->data = data;
		current->enabled = true;

        #ifdef DL_EXT
		current->handle = NULL;
        #endif

		return true;
	}
	else
		return false;
}

//-------------------------------------------------------------------
void
xexec::performXExec(__execItemList&list) const
{
	if (list.execDisabled)
		return ;

	std::list<__execItem>::iterator i(list.exec.begin()), j(list.exec.end());

	for (; i != j; ++i)
		if (i->enabled)
		{
			if (safeHooks)
			{
				disableAllPostExec();
				disableAllPreExec();
			}
			i->func(i->obj, i->type, i->data);
			if (safeHooks)
			{
				enableAllPostExec();
				enableAllPreExec();
			}
		}
}

//-------------------------------------------------------------------

#ifdef DL_EXT

xexecCounts::xexecCounts() : pre(-1),
	post(-1)
{
}

//-------------------------------------------------------------------

int
xexec::addXExecModule(std::list<__execItem>&list,
					  void                  *obj,
					  short type,
					  const dodoString&module,
					  void                  *data,
					  void                  *toInit)
{
	__execItem temp;

	temp.data = data;
	temp.obj = obj;
	temp.position = ++execs;
	temp.enabled = true;
	temp.type = type;

	temp.handle = dlopen(module.c_str(), RTLD_LAZY);
	if (temp.handle == NULL)
            #ifndef NO_EX
		throw baseEx(ERRMODULE_XEXEC, XEXEC_ADDXEXECMODULE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
		return -1;
            #endif

	initXexecModule init = (initXexecModule)dlsym(temp.handle, "initXexecModule");
	if (init == NULL)
            #ifndef NO_EX
		throw baseEx(ERRMODULE_XEXEC, XEXEC_ADDXEXECMODULE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
		return -1;
            #endif

	inExec in = (inExec)dlsym(temp.handle, init(toInit).hook);
	if (in == NULL)
            #ifndef NO_EX
		throw baseEx(ERRMODULE_XEXEC, XEXEC_ADDXEXECMODULE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
		return -1;
            #endif

	temp.func = in;

	list.push_back(temp);

	return temp.position;
}

//-------------------------------------------------------------------

int
xexec::_addPostExec(const dodoString&module,
					void             *obj,
					short type,
					void             *data,
					void             *toInit)
{
	return addXExecModule(postExec.exec, obj, type, module, data, toInit);
}

//-------------------------------------------------------------------

int
xexec::_addPreExec(const dodoString&module,
				   void             *obj,
				   short type,
				   void             *data,
				   void             *toInit)
{
	return addXExecModule(preExec.exec, obj, type, module, data, toInit);
}

//-------------------------------------------------------------------

xexecMod
xexec::getModuleInfo(const dodoString&module,
					 void             *toInit)
{
	void *handle = dlopen(module.c_str(), RTLD_LAZY);
	if (handle == NULL)
            #ifndef NO_EX
		throw baseEx(ERRMODULE_XEXEC, XEXEC_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
		return xexecMod();
            #endif

	initXexecModule init = (initXexecModule)dlsym(handle, "initXexecModule");
	if (init == NULL)
            #ifndef NO_EX
		throw baseEx(ERRMODULE_XEXEC, XEXEC_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
		return xexecMod();
            #endif

	xexecMod mod = init(toInit);

	if (dlclose(handle) != 0)
            #ifndef NO_EX
		throw baseEx(ERRMODULE_XEXEC, XEXEC_GETMODULEINFO, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
		return mod;
            #endif

	return mod;
}

//-------------------------------------------------------------------

xexecCounts
xexec::_addExec(const dodoString&module,
				void             *obj,
				short type,
				void             *data,
				void             *toInit)
{
	__execItem temp;

	temp.data = data;
	temp.obj = obj;
	temp.enabled = true;
	temp.type = type;

	temp.handle = dlopen(module.c_str(), RTLD_LAZY);
	if (temp.handle == NULL)
            #ifndef NO_EX
		throw baseEx(ERRMODULE_XEXEC, XEXEC_ADDXEXECMODULE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
		return xexecCounts();
            #endif

	initXexecModule init = (initXexecModule)dlsym(temp.handle, "initXexecModule");
	if (init == NULL)
            #ifndef NO_EX
		throw baseEx(ERRMODULE_XEXEC, XEXEC_ADDXEXECMODULE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
		return xexecCounts();
            #endif

	xexecMod info = init(toInit);

	inExec in = (inExec)dlsym(temp.handle, info.hook);
	if (in == NULL)
            #ifndef NO_EX
		throw baseEx(ERRMODULE_XEXEC, XEXEC_ADDXEXECMODULE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
            #else
		return xexecCounts();
            #endif


	temp.func = in;

	xexecCounts count;

	switch (info.execType)
	{
		case XEXECMODULE_POST:

			temp.position = ++execs;
			postExec.exec.push_back(temp);
			count.post = temp.position;

			break;

		case XEXECMODULE_PRE:

			temp.position = ++execs;
			preExec.exec.push_back(temp);
			count.pre = temp.position;

			break;

		case XEXECMODULE_BOTH:

			temp.position = ++execs;
			postExec.exec.push_back(temp);
			count.post = temp.position;

			temp.position = ++execs;
			preExec.exec.push_back(temp);
			count.pre = temp.position;

			break;
	}

	return count;
}

#endif

//-------------------------------------------------------------------

bool
xexec::getXexec(std::list<__execItem>&list,
				int position)
{
	std::list<__execItem>::iterator i(list.begin()), j(list.end());
	for (; i != j; ++i)
		if (i->position == position)
		{
			current = i;

			return true;
		}

	return false;
}

//-------------------------------------------------------------------
