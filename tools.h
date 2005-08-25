/***************************************************************************
 *            tools.h
 *
 *  Wed Aug 24 00:42:51 2005
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
 
#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <string>
#include <vector>


#include "directives.h"
#include "types.h"
#include "dodoBase.h"

namespace dodo
{
	class tools : virtual public dodoBase
	{
		public:
			virtual tools *getSelf();
			/**
			* adds/removes option to flag
			*/
			static void addF(int &flag, unsigned int statement);
			static void removeF(int &flag, unsigned int statement);
			
			static void replace(pchar needle, pchar replacement, std::string &data);
		
			///makes string of fields, separated with separator; frame = symbol, that is used to frame the field
			static std::string implode(const stringArr &fields, escape escapeF, std::string separator, std::string frame);
			static std::string implode(const stringArr &fields, std::string separator);
			static std::string implode(const stringArr &fields, escape escapeF, std::string separator);
	};
};
#endif /* _TOOLS_H */
