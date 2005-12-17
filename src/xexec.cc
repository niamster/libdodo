/***************************************************************************
 *            xexec.cc
 *
 *  Wed Aug 24 00:39:10 2005
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
 
#include <xexec.h>

using namespace dodo;

//-------------------------------------------------------------------

void 
dummyHook(void *base, xexecObjTypeEnum type, void *data)
{
}

//-------------------------------------------------------------------

xexec::xexec() : safeHooks(true),
				operType(XEXEC_NONE)
				
				#ifdef DL_EXT
				
					,handlesOpened(0)
					
				#endif
{
	preExec.execDisabled = false;
	postExec.execDisabled = false;
}

//-------------------------------------------------------------------

xexec::~xexec()
{	
	#ifdef DL_EXT
	
		deinitXexecModule deinit;	
		for (register int i(0);i<handlesOpened;++i)
		{
			deinit = (deinitXexecModule)dlsym(handles[i], "deinitXexecModule");
			if (deinit != NULL)
				deinit();
			
			dlclose(handles[i]);
		}
			
	#endif
}

//-------------------------------------------------------------------

inline int 
xexec::addXExec(std::vector<__execItem> &list, 
		inExec func,
 		void *obj, 
 		xexecObjTypeEnum type,
		void *data) const
{
	__execItem temp;
	
	temp.data = data;
	temp.obj = obj;
	temp.func = func;
	temp.present = true;
	temp.enabled = true;
	temp.type = type;
	
	list.push_back(temp);

	return list.size();	
}

//-------------------------------------------------------------------

void 
xexec::setStatXExec(std::vector<__execItem> &list, 
		unsigned int position,
		bool stat) const
{
	--position;
	if ((position) <= list.size())
		list[position].enabled = stat;	
}

//-------------------------------------------------------------------

inline void 
xexec::delXExec(std::vector<__execItem> &list, 
		unsigned int position) const
{
	--position;
	if (list[position].present && (position <= list.size()))
	{
		list[position].func = dummyHook;
		list[position].obj = NULL;
		list[position].data = NULL;
		list[position].present = false;
	}
}

//-------------------------------------------------------------------

int 
xexec::_addPreExec(inExec func,
 				void *obj, 
 				xexecObjTypeEnum type,
				void *data) const
{
	return addXExec(preExec.exec,func,obj,type,data);
}

//-------------------------------------------------------------------

void 
xexec::disablePreExec(unsigned int position) const
{
	setStatXExec(preExec.exec,position,false);
}

//-------------------------------------------------------------------

void 
xexec::delPreExec(unsigned int position) const
{
	delXExec(preExec.exec,position);
}

//-------------------------------------------------------------------

void 
xexec::enablePreExec(unsigned int position) const
{
	setStatXExec(preExec.exec,position,true);
}

//-------------------------------------------------------------------

int 
xexec::_addPostExec(inExec func, 
 				void *obj, 
 				xexecObjTypeEnum type,
				void *data) const
{
	return addXExec(postExec.exec,func,obj,type,data);
}

//-------------------------------------------------------------------

void 
xexec::disablePostExec(unsigned int position) const
{
	setStatXExec(postExec.exec,position,false);
}

//-------------------------------------------------------------------

void 
xexec::delPostExec(unsigned int position) const
{
	delXExec(postExec.exec,position);
}

//-------------------------------------------------------------------

void 
xexec::enablePostExec(unsigned int position) const
{
	setStatXExec(postExec.exec,position,true);
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
xexec::disableAll() const
{
	postExec.execDisabled = true;
	preExec.execDisabled = true;
}

//-------------------------------------------------------------------

void 
xexec::enableAll() const
{
	postExec.execDisabled = false;
	preExec.execDisabled = false;
}

//-------------------------------------------------------------------

bool 
xexec::replacePostExec(unsigned int position, 
				inExec func,
				void *data) const
{
	return replaceXExec(postExec.exec,position,func,data);
}

//-------------------------------------------------------------------

bool 
xexec::replacePreExec(unsigned int position, 
				inExec func,
				void *data) const
{
	return replaceXExec(preExec.exec,position,func,data);
}

//-------------------------------------------------------------------

inline bool 
xexec::replaceXExec(std::vector<__execItem> &list, 
			unsigned int position, 
			inExec func,
			void *data) const
{
	--position;
	if (position <= list.size())
	{
		list[position].func = func;
		list[position].data = data;
		list[position].present = true;
		return true;
	}
	else
		return false;
}

//-------------------------------------------------------------------
void 
xexec::performXExec(__execItemList &list) const
{
	if (list.execDisabled)
		return ;
	std::vector<__execItem>::const_iterator i(list.exec.begin()),j(list.exec.end());
	for (;i!=j;++i)
		if (i->present && i->enabled)
		{
			if (safeHooks)
			{
					disableAllPostExec();
					disableAllPreExec();
			}
			i->func(i->obj,i->type,i->data);
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
	xexec::addXExecModule(std::vector<__execItem> &list, 
					void *obj,  
 					xexecObjTypeEnum type,
					const std::string &module, 
					void *data) const
	{
		if (handlesOpened == XEXEC_MAXMODULES)
			return -1;
		
		__execItem temp;
		
		temp.data = data;
		temp.obj = obj;
		temp.present = true;
		temp.enabled = true;
		temp.type = type;
		
		handles[handlesOpened] = dlopen(module.c_str(), RTLD_LAZY);
		if (handles[handlesOpened] == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC,XEXEC_ADDXEXECMODULE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return -1;
			#endif
		
		initXexecModule init = (initXexecModule)dlsym(handles[handlesOpened], "initXexecModule");
		if (init == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_XEXEC,XEXEC_ADDXEXECMODULE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
		#else
			return -1;
		#endif	
		
		inExec in = (inExec)dlsym(handles[handlesOpened], init().hook);
		if (in == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC,XEXEC_ADDXEXECMODULE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return -1;
			#endif
	
		
		temp.func = in;
		
		list.push_back(temp);
		++handlesOpened;
	
		return list.size();		
	}
	
	//-------------------------------------------------------------------
	
	int 
	xexec::_addPostExec(const std::string &module, 
						void *obj, 
 						xexecObjTypeEnum type, 
						void *data) const
	{
		return addXExecModule(postExec.exec,obj,type,module,data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	xexec::_addPreExec(const std::string &module, 
					void *obj,
 					xexecObjTypeEnum type, 
					void *data) const
	{
		return addXExecModule(preExec.exec,obj,type,module,data);
	}
	
	//-------------------------------------------------------------------
	
	xexecMod 
	xexec::getModuleInfo(const std::string &module)
	{
		void *handle = dlopen(module.c_str(), RTLD_LAZY);
		if (handle == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC,XEXEC_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return xexecExMod();
			#endif
			
		initXexecModule init = (initXexecModule)dlsym(handle, "initXexecModule");
		if (init == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC,XEXEC_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return xexecExMod();
			#endif
			
		xexecMod mod = init();
		
		if (dlclose(handle)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC,XEXEC_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return xexecExMod();
			#endif
		
		return mod;	
	}
	
	//-------------------------------------------------------------------
	
	xexecCounts 
	xexec::_addExec(const std::string &module, 
					void *obj, 
 					xexecObjTypeEnum type, 
					void *data) const
	{
		if (handlesOpened == XEXEC_MAXMODULES)
			return xexecCounts();
		
		__execItem temp;
		temp.data = data;
		temp.obj = obj;
		temp.present = true;
		temp.enabled = true;
		temp.type = type;
		
		handles[handlesOpened] = dlopen(module.c_str(), RTLD_LAZY);
		if (handles[handlesOpened] == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC,XEXEC_ADDXEXECMODULE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return xexecCounts();
			#endif
		
		initXexecModule init = (initXexecModule)dlsym(handles[handlesOpened], "initXexecModule");
		if (init == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC,XEXEC_ADDXEXECMODULE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return xexecCounts();
			#endif	
		
		xexecMod info = init();
		
		inExec in = (inExec)dlsym(handles[handlesOpened], info.hook);
		if (in == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_XEXEC,XEXEC_ADDXEXECMODULE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return xexecCounts();
			#endif
	
		
		temp.func = in;
		
		xexecCounts count;
		
		switch (info.execType)
		{
			case XEXECMODULE_PRE:
				preExec.exec.push_back(temp);
				count.pre = preExec.exec.size();
				break;
				
			case XEXECMODULE_POST:
				postExec.exec.push_back(temp);
				count.post = postExec.exec.size();
				break;
				
			case XEXECMODULE_BOTH:
				preExec.exec.push_back(temp);
				postExec.exec.push_back(temp);
				count.post = postExec.exec.size();
				count.pre = preExec.exec.size();
				break;
		}
			
		++handlesOpened;
		
		return count;
	}
	
	//-------------------------------------------------------------------

#endif
