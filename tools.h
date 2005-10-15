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
	/**
	 * adds/removes option to flag
	 */
	#define addF(flag,statement) (flag)|=(statement)
	#define removeF(flag,statement) (flag)&=(~(statement))
	
	class tools
	{
		public:
			
			/**
			 * converts long to string
			 */
			static std::string lToString(long number);
			/**
			 * converts double to string
			 */
			static std::string dToString(double number);
			
			static void replace(pchar needle, pchar replacement, std::string &data);
		
			///makes array of strings from string using separtator
			static stringArr explode(const std::string &fields, const std::string &separator);
			static stringArr explode(const std::string &fields, escape escapeF, const std::string &separator);
		
			///makes string of fields, separated with separator; frame = symbol, that is used to frame the field
			static std::string implode(const stringArr &fields, escape escapeF, const std::string &separator, const std::string &frame);
			static std::string implode(const stringArr &fields, escape escapeF, const std::string &separator);
			static std::string implode(const stringArr &fields, const std::string &separator);
			static std::string implode(const stringArr &fields, const std::string &separator, const std::string &frame);
			
			///sets string to size; if it's bigger than size - cut from right; if shotter - adds `symbol` to make it longer
			static void normalize(std::string &a_string, unsigned long size, char adds = ' ');
			
		private:
			inline static std::string dummy(const std::string &data);
	};
};
#endif /* _TOOLS_H */
