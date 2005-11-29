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

#include <directives.h>

#include <types.h>
#include <dodoBase.h>

namespace dodo
{
	/**
	 * @def adds bit flag to statement
	 */
	#define addF(flag,statement) (flag)|=(statement)
	/**
	 * @def removes bit flag to statement
	 */
	#define removeF(flag,statement) (flag)&=(~(statement))

	/**
	 * @typedef that describes function that will escapes parts in explode/implode
	 * @return converted string
	 */
	typedef std::string (*escape)(const std::string &);
		
	/**
	 * @class tools present different usefull functions
	 */
	class tools
	{
		public:
			
			/**
			 * removes symbols from the end of give string
			 * @return processed string
			 * @param data is string to process
			 * @param symbol is symbol to remove
			 */
			static std::string rTrim(const std::string &data, char symbol=' ');
			
			/**
			 * removes symbols from the begin of give string
			 * @return processed string
			 * @param data is string to process
			 * @param symbol is symbol to remove
			 */
			static std::string lTrim(const std::string &data, char symbol=' ');
						
			/**
			 * @return converted long to string
			 * @param number is number to convert
			 */
			static std::string lToString(long number);
			
			/**
			 * @return converted double to string
			 * @param number is number to convert
			 */
			static std::string dToString(double number);
			
			/**
			 * replaces nodes in string
			 * @param needle indicates what to replace in string
			 * @param replacement indicates with what to replace
			 * @param data specifies wher to replace
			 */
			static void replace(pchar needle, pchar replacement, std::string &data);
		
			/**
			 * @return array of strings made from string using separtator
			 * @param fields is string to explode
			 * @param separator is value that separates parts
			 */
			static stringArr explode(const std::string &fields, const std::string &separator);
					
			/**
			 * @return array of strings made from string using separtator
			 * @param escapeF is callback function that will process with every made node
			 * @param fields is string to explode
			 * @param separator is value that separates parts
			 */
			static stringArr explode(const std::string &fields, escape escapeF, const std::string &separator);
			
			/**
			 * @return string of fields, separated with separator;
			 * @param fields is an array of nodes that will fiil the result string
			 * @param escapeF is callback function that will process with every node
			 * @param separator is value that separates parts
			 * @param frame is node that will cover node [frame='; node => 'node']
			 */
			static std::string implode(const stringArr &fields, escape escapeF, const std::string &separator, const std::string &frame);
			
			/**
			 * @return string of fields, separated with separator;
			 * @param fields is an array of nodes that will fiil the result string
			 * @param escapeF is callback function that will process with every node
			 * @param separator is value that separates parts
			 */
			static std::string implode(const stringArr &fields, escape escapeF, const std::string &separator);
			
			/**
			 * @return string of fields, separated with separator;
			 * @param fields is an array of nodes that will fiil the result string
			 * @param separator is value that separates parts
			 */
			static std::string implode(const stringArr &fields, const std::string &separator);
			
			/**
			 * @return string of fields, separated with separator;
			 * @param fields is an array of nodes that will fiil the result string
			 * @param separator is value that separates parts
			 * @param frame is node that will cover node [frame='; node => 'node']
			 */
			static std::string implode(const stringArr &fields, const std::string &separator, const std::string &frame);
			
		private:
			
			/**
			 * dummy callback function for implode/explode
			 * nothing does with passed data
			 */
			inline static std::string dummy(const std::string &data);
	};
};
#endif /* _TOOLS_H */
