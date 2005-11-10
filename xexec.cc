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
 
#include "xexec.h"

using namespace dodo;

//-------------------------------------------------------------------

void 
dummyHook(dodoBase *base, void *data)
{
}

//-------------------------------------------------------------------

xexec::xexec() : safeHooks(true)
{
	preExec.execDisabled = false;
	postExec.execDisabled = false;
}

//-------------------------------------------------------------------

xexec::~xexec()
{	
}

//-------------------------------------------------------------------

inline int 
xexec::addXExec(std::vector<__execItem> &list, 
		inExec func,
 		dodoBase *obj,
		void *data) const
{
	__execItem temp;
	
	temp.data = data;
	temp.obj = obj;
	temp.func = func;
	temp.present = true;
	temp.enabled = true;
	
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
 				dodoBase *obj,
				void *data) const
{
	return addXExec(preExec.exec,func,obj,data);
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
 				dodoBase *obj,
				void *data) const
{
	addXExec(postExec.exec,func,obj,data);
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
 				dodoBase *obj,
				void *data) const
{
	return replaceXExec(postExec.exec,position,func,obj,data);
}

//-------------------------------------------------------------------

bool 
xexec::replacePreExec(unsigned int position, 
				inExec func,
 				dodoBase *obj,
				void *data) const
{
	return replaceXExec(preExec.exec,position,func,obj,data);
}

//-------------------------------------------------------------------

inline bool 
xexec::replaceXExec(std::vector<__execItem> &list, 
			unsigned int position, 
			inExec func, 
 			dodoBase *obj,
			void *data) const
{
	--position;
	if (position <= list.size())
	{
		list[position].func = func;
		list[position].data = data;
		list[position].obj = obj;
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
			i->func(i->obj,i->data);
			if (safeHooks)
			{
				enableAllPostExec();
				enableAllPreExec();
			}
		}
}

//-------------------------------------------------------------------

bool 
xexec::exec() const
{
	performXExec(preExec);
	///execute 
	performXExec(postExec);
}

//-------------------------------------------------------------------
