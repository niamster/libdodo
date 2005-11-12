/***************************************************************************
 *            dodoBase.h
 *
 *  Wed Aug 24 01:31:48 2005
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
 
#ifndef _DODOBASE_H_
#define _DODOBASE_H_

namespace dodo
{
	class dodoBase
	{
		public:
		
			/**
			 * returns this, casted to dodoBase *
			 * 
			 * 		sometimes it's useful, if u have `class a: public b, public c {...}` and `class b: public d {...}`
			 * 		also u have void func(d *);
			 * 		u want to cast `d` to `b`, so you need getSelf() function; due to it's virtuality, it'll find proper class!!
			 * 
			 */
			virtual dodoBase *getSelf();
					
			/**
			 * constructor/destructor
			 */
			dodoBase();
			~dodoBase();
	};
};

#endif /* _DODOBASE_H */
