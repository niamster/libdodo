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

xexec *
xexec::getSelf()
{
	return this;
}

//-------------------------------------------------------------------

void 
dummyHook(dodoBase *base, void *data)
{
}

//-------------------------------------------------------------------

xexec::xexec():safeHooks(true)
{
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

inline void 
xexec::setStatXExec(std::vector<__execItem> &list, 
		unsigned int position,
		bool stat) const
{
	position--;
	if (position <= list.size())
		list[position].enabled = stat;	
}

//-------------------------------------------------------------------

inline void 
xexec::delXExec(std::vector<__execItem> &list, 
		unsigned int position) const
{
	position--;
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
xexec::addPreExec(inExec func,
 				dodoBase *obj,
				void *data) const
{
	return addXExec(preExec,func,obj,data);
}

//-------------------------------------------------------------------

void 
xexec::disablePreExec(unsigned int position) const
{
	setStatXExec(preExec,position,false);
}

//-------------------------------------------------------------------

void 
xexec::delPreExec(unsigned int position) const
{
	delXExec(preExec,position);
}

//-------------------------------------------------------------------

void 
xexec::enablePreExec(unsigned int position) const
{
	setStatXExec(preExec,position,true);
}

//-------------------------------------------------------------------

int 
xexec::addPostExec(inExec func, 
 				dodoBase *obj,
				void *data) const
{
	addXExec(postExec,func,obj,data);
}

//-------------------------------------------------------------------

void 
xexec::disablePostExec(unsigned int position) const
{
	setStatXExec(postExec,position,false);
}

//-------------------------------------------------------------------

void 
xexec::delPostExec(unsigned int position) const
{
	delXExec(postExec,position);
}

//-------------------------------------------------------------------

void 
xexec::enablePostExec(unsigned int position) const
{
	setStatXExec(postExec,position,true);
}

//-------------------------------------------------------------------

inline void 
xexec::setStatAllXExec(std::vector<__execItem> &list,
				bool stat) const
{
	register std::vector<__execItem>::iterator i(list.begin()),j(list.end());
	for (;i!=j;++i)
		i->enabled = stat;
}

//-------------------------------------------------------------------

void 
xexec::enableAllPreExec() const
{
	setStatAllXExec(preExec,true);
}

//-------------------------------------------------------------------

void
xexec::enableAllPostExec() const
{
	setStatAllXExec(postExec,true);
}

//-------------------------------------------------------------------

void 
xexec::disableAllPreExec() const
{
	setStatAllXExec(preExec,false);	
}

//-------------------------------------------------------------------

void 
xexec::disableAllPostExec() const
{
	setStatAllXExec(postExec,false);
}

//-------------------------------------------------------------------

bool 
xexec::replacePostExec(unsigned int position, 
				inExec func, 
 				dodoBase *obj,
				void *data) const
{
	return replaceXExec(postExec,position,func,obj,data);
}

//-------------------------------------------------------------------

bool 
xexec::replacePreExec(unsigned int position, 
				inExec func,
 				dodoBase *obj,
				void *data) const
{
	return replaceXExec(preExec,position,func,obj,data);
}

//-------------------------------------------------------------------

inline bool 
xexec::replaceXExec(std::vector<__execItem> &list, 
			unsigned int position, 
			inExec func, 
 			dodoBase *obj,
			void *data) const
{
	position--;
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
xexec::performXExec(std::vector<__execItem> &list) const
{
	std::vector<__execItem>::const_iterator i(list.begin()),j(list.end());
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

void 
xexec::setSafeHooks(bool state) const
{
	safeHooks = state;
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
