/***************************************************************************
 *            cgiPreprocessor.h
 *
 *  Sun Jan 22 19:05:57 2006
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

#ifndef _CGIPREPROCESSOR_H_
#define _CGIPREPROCESSOR_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libdodo/tools.h>
#include <libdodo/cgiPreprocessorEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsFilesystem.h>

namespace dodo
{
	/**
	 * @class cgiPreprocessor prepares template for cgiProcessor
	 * @note brief reference on usage:
	 * 
	 * elements of syntax:
	 * 
	 * <(	open bracket
	 * )>	close bracket
	 * 
	 * <(*	open bracket for comment
	 * *)>	close bracket for comment
	 * 
	 * <(>	open bracket for non-parsing area
	 * <)>	close bracket for non-parsing area
	 * 
	 * variables begin with $:
	 * $var, $1var
	 * 
	 * possible 4 types of variables:
	 * 	1.string
	 * 	2.array
	 * 	3.hash
	 * 	4.array of hashes
	 * 	
	 * you may itarate each or directly access to their members:
	 * 	1.first symbol of string: $strVar.0
	 * 	2.second element of array: $arrVar.1
	 * 	3.first symbol of second element of array: $arrVar.1.0
	 * 	4.element named "one" of the hash: $hashVar.one
	 * 	5.element named "one" of the second element in array of hashes: $hashArrVar.1.one
	 * 
	 * as key to member of variable may be also variable, but it must be wrapped with '{}':
	 * 	1.element named "one" of the second element in array of hashes, variable 'varOne' has value "one" : $hashArrVar.1.{$varOne}
	 * 	2.element named "one" of the second element in array of hashes, member "one" of the array 'arrVar' has value "one": $hashArrVar.1.{$arrVar.one}
	 * 
	 * pair of (", ', `) will be removed from the statement, so "$var" == '$var' == $var == `$var`
	 * 
	 * namespace:
	 * 	cgiProcessor uses namespaces:
	 * 		variable $b, defined before the in-loop, is avalaible after the in-loop
	 * 		<(for $b in $arr2)>
	 * 			<(for $b in $b)> <(* HERE : `$b in $b` and it's OK! *)>
	 * 				|<(print $b)>
	 * 			<(rof)>
	 * 			|<(print $b.one)>
	 * 		<(rof)>		
	 * 
	 * blocks:
	 * 	include:
	 * 		<(include $template)>
	 * 
	 * 		<(include template.tpl)>
	 * 
	 * 		includes external template file into current and processes
	 * 
	 * 	if:
	 * 		<(if $a == $b )>
	 * 			that's OK 1
	 * 		<(fi)>
	 * 
	 * 		<(if $b )>
	 * 			that's OK 2
	 * 		<(fi)>
	 * 
	 * 		<(if ! $a )>
	 * 			that's OK 3
	 * 		<(else)>
	 * 			smth else ...
	 * 		<(fi)>
	 * 
	 * 		`if` statement allows `==`,`!`,`!=` for strings and `<=`,`>=`,`<`,`>` for floats(converts from strings)
	 * 
	 * 		`if` statement is false if statement is false(for 2 arguments) or if argument == false(string) or empty(for 1 argument)
	 * 	
	 * 	for:
	 * 		//c++ code:
	 * 				cgiProcessor cgip;
	 * 				
	 * 				dodoStringArr arr;
	 * 				arr.push_back("one");
	 * 				arr.push_back("two");
	 * 				arr.push_back("three");
	 * 				cgip.assign("arr",arr);
	 * 				
	 * 				dodoAssocArr arr1;
	 * 				arr1["one"] = "one";
	 * 				arr1["two"] = "two";
	 * 				arr1["three"] = "three";
	 * 				cgip.assign("arr1",arr1);
	 * 				
	 * 				std::vector<dodoAssocArr> arr2;
	 * 				arr2.push_back(arr1);
	 * 				arr1["one"] = "three";
	 * 				arr2.push_back(arr1);
	 * 				cgip.assign("arr2",arr2);
	 * 		
	 * 		//template
	 * 			<(for $b in $arr)>
	 * 				<div class="test"><(print $b)></div>
	 * 			<(rof)>
	 * 
	 * 			<(for $c in $arr2.1)>
	 * 				|<(print $c)>
	 * 			<(rof)>
	 * 
	 * 			<(for $c in $arr1)>
	 * 				|<(print $c)>
	 * 			<(rof)>
	 * 				
	 * 			<(for $b in $arr2)>
	 * 				<(for $b in $b)> <(* HERE : `$b in $b` and it's OK! *)>
	 * 					|<(print $b)>
	 * 				<(rof)>
	 * 				|<(print $b.one)>
	 * 			<(rof)>
	 * 			
	 * 			<(for $b in $arr2)>
	 * 				<(for $b => $c in $b)>
	 * 					<(print $b)> -> <(print $c)>
	 * 				<(rof)>
	 * 			<(rof)>
	 * 			
	 * 			`for` statement allows to iterate through array. to get pair 'key=>value' use `=>` operator(see an example).	
	 * 			in `for` statement you may use `continue` and `break` blocks: 
	 * 			
	 * 			<(for $b in $arr)>
	 * 				<(if $b==2 )>
	 * 					<(continue)>
	 * 				<(fi)>
	 * 				<(print !!!)>
	 * 				<(if $b==4 )>
	 * 					<(break)>
	 * 				<(fi)>
	 * 				<(print @@@)>
	 * 			<(rof)>
	 * 			`break` block may have a strenght - how many loops[in deep] to break => <(break 2)> => breaks 2 loop blocks; if not specified - 1 by default
	 * 			`break` and `continue` statements outside loop blocks do nothing
	 * 			
	 * 		print:
	 * 			<(print {$arr2.{0}.{$one}} )>
	 * 			
	 * 			<(print $arr2.1.{$arr1.{$one}} )>
	 * 			
	 * 			you	may combine some values using ',':
	 * 			<(print $arr2.1.{$arr1.{$one}} , !!!! , {$arr2.{0}.{$one}})>
	 */
	class cgiPreprocessor
	{
		public:

			/**
			 * constructor
			 */
			cgiPreprocessor();

			/**
			 * destructor
			 */
			virtual ~cgiPreprocessor();

			/**
			 * @return preprocessed template
			 * @param path defines path of template
			 */
			virtual dodoString preProcess(const dodoString &path);

			/**
			 * @return preprocessed template
			 * @param buffer defines buffer where template is stored
			 */
			virtual dodoString preProcessString(const dodoString &buffer);

			dodoString tplBasePath; ///< base path for templates[if empty - full/relative path must be defined in preProcess/include]

		protected:

			/**
			 * @return preprocessed template
			 * @param buffer defines buffer where template is stored
			 * @param path defines path of template
			 */
			virtual dodoString _preProcessString(const dodoString &buffer, const dodoString &path);

			/**
			 * @return line number in file
			 * @param newlines defines positions of new lines if template 
			 * @param pos defines position of symbol in template
			 */
			virtual unsigned long getLineNumber(const dodoArray<unsigned long> &newlines, unsigned long pos);

			/**
			 * @return positions of new lines
			 * @param buffer defines buffer where template is stored
			 */
			virtual dodoArray<unsigned long> detectNewLines(const dodoString &buffer);

			dodoArray< dodoArray<unsigned long> > newLinePositions; ///< stack of positions of new lines of templates

			/**
			 * @enum preprocessorStatemetsEnum defines processor statements
			 * @note defines positions of string representation in 'statements' class property
			 */
			enum preprocessorStatemetsEnum
			{
				CGIPREPROCESSOR_PROCESSORSTATEMENT_DODO = 0,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_ST,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_NP,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NP,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_COMM,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_COMM,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_IF,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_ELSE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_IF,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_FOR,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_IN,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_KEY_VALUE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_FOR,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_PRINT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_BREAK,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CONT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_ASSIGN,
				CGIPREPROCESSOR_CGIPREPROCESSOR_PROCESSORSTATEMENT_ASSIGN_OP,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_NS,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NS,
				CGIPREPROCESSOR_CGIPREPROCESSOR_PROCESSORSTATEMENT_INCLUDE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_VERSION,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_DOT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_COMA,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_DOLLAR,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_FALSE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_EQ,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_NE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_LE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_GE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_LT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_GT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_VARPART,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_VARPART,
			};
			
			static const dodoString statements[36]; ///< processor statement[for dodo.*, ...]
	};
};

#endif
