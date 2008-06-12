/***************************************************************************
 *            cgiProcessor.h
 *
 *  Tue Jan 24 19:05:57 2006
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

#ifndef _CGIPROCESSOR_H_
#define _CGIPROCESSOR_H_

#include <libdodo/directives.h>

#include <libdodo/cgiProcessorEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/cgiPreprocessor.h>
#include <libdodo/cgiFastServer.h>
#include <libdodo/cgiServer.h>

namespace dodo
{
	namespace cgi
	{
		/**
		 * @class cgiProcessor parses template
		 * @note brief reference on usage:
		 * syntax:
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
		 * variables:
		 *      variables begin with $
		 *      example:
		 *              1. $var
		 *              2. $1var
		 *
		 *      processor supports 4 types of variables:
		 *              1. string
		 *              2. array
		 *              3. hash
		 *              4. array of hashes
		 *
		 *      you may itarate each with `for` loop or directly access their members
		 *      example:
		 *              1. the first symbol of the string: $strVar.0
		 *              2. the second element of the array: $arrVar.1
		 *              3. the first symbol of the second element of the array: $arrVar.1.0
		 *              4. the element named "one" of the hash: $hashVar.one
		 *              5. the element named "one" of the second element in the array of the hashes: $hashArrVar.1.one
		 *
		 *      variable can be used as a key to the member. it must be wrapped with '{}'
		 *      example:
		 *              1. element named "one" of the second element in array of hashes(variable 'varOne' has value "one"): $hashArrVar.1.{$varOne}
		 *              2. element named "one" of the second element in array of hashes(array 'hashVar' has member "one" which value is "one"): $hashArrVar.1.{$hashVar.one}
		 *
		 *      pairs of (", ', `) are removed from the statement: "$var" == '$var' == $var == `$var`
		 *
		 * namespace:
		 *      processor uses namespaces, variables have scope of visibility
		 *      example:
		 *              variable defined before the loop is avalaible after the loop with the statement it has before:
		 *              <(for $b in $arr2)>
		 *                      <(for $b in $b)> <(* HERE : `$b in $b` and it's OK! *)>
		 *                              |<(print $b)>
		 *                      <(rof)>
		 *                      |<(print $b.one)>
		 *              <(rof)>
		 *
		 * blocks:
		 *      include:
		 *              include external template file into current and process
		 *              example:
		 *                      <(include $template)>
		 *
		 *                      <(include template.tpl)>
		 *
		 *      if:
		 *              `if` statement allows to use conditional blocks
		 *              `fi` closes `if` block
		 *              `else` block is processed when `if` statement is false
		 *              `if` statement allows `==`,`!`,`!=` for strings and `==`, `!=`, `!`, `<=`,`>=`,`<`,`>` for numeric
		 *              `!` inverts statement value
		 *              `if` statement is false when the statement is false(for 2 arguments) or for one when the argument is 'false' or empty
		 *              example:
		 *                      <(if $a == $b )>
		 *                              that's OK 1
		 *                      <(fi)>
		 *
		 *                      <(if $b )>
		 *                              that's OK 2
		 *                      <(fi)>
		 *
		 *                      <(if ! $a )>
		 *                              that's OK 3
		 *                      <(else)>
		 *                              smth else ...
		 *                      <(fi)>
		 *
		 *      for:
		 *              `for` statement allows to iterate through array/string
		 *              `rof` closes `for` block
		 *              `=>` operator is used to get pairs 'key=>value'
		 *              in `for` statement `continue` and `break` blocks can be used
		 *              `break` block may have a strength - how many loops[in deep] to break => <(break 2)> => breaks 2 loop blocks; if not specified - 1 by default
		 *              `break` and `continue` blocks outside the loop blocks do nothing
		 *
		 *              example:
		 *                      //c++ code:
		 *                      cgiProcessor cgip;
		 *
		 *                      dodoStringArr arr;
		 *                      arr.push_back("one");
		 *                      arr.push_back("two");
		 *                      arr.push_back("three");
		 *                      cgip.assign("arr",arr);
		 *
		 *                      dodoAssocArr arr1;
		 *                      arr1["one"] = "one";
		 *                      arr1["two"] = "two";
		 *                      arr1["three"] = "three";
		 *                      cgip.assign("arr1",arr1);
		 *
		 *                      std::vector<dodoAssocArr> arr2;
		 *                      arr2.push_back(arr1);
		 *                      arr1["one"] = "three";
		 *                      arr2.push_back(arr1);
		 *                      cgip.assign("arr2",arr2);
		 *
		 *                      //template
		 *                      <(for $b in $arr)>
		 *                              <div class="test"><(print $b)></div>
		 *                      <(rof)>
		 *
		 *                      <(for $c in $arr2.1)>
		 *                              |<(print $c)>
		 *                      <(rof)>
		 *
		 *                      <(for $c in $arr1)>
		 *                              |<(print $c)>
		 *                      <(rof)>
		 *
		 *                      <(for $b in $arr2)>
		 *                              <(for $b in $b)> <(* HERE : `$b in $b` and it's OK! *)>
		 *                                      |<(print $b)>
		 *                              <(rof)>
		 *                              |<(print $b.one)>
		 *                      <(rof)>
		 *
		 *                      <(for $b in $arr2)>
		 *                              <(for $b => $c in $b)>
		 *                                      <(print $b)> -> <(print $c)>
		 *                              <(rof)>
		 *                      <(rof)>
		 *
		 *                      <(for $b in $arr)>
		 *                              <(if $b==2 )>
		 *                                      <(continue)>
		 *                              <(fi)>
		 *                              <(print !!!)>
		 *                              <(if $b==4 )>
		 *                                      <(break)>
		 *                              <(fi)>
		 *                              <(print @@@)>
		 *                      <(rof)>
		 *
		 *      print:
		 *              print variable value
		 *              values can me combined with ','
		 *              example:
		 *                      <(print {$arr2.{0}.{$one}} )>
		 *
		 *                      <(print $arr2.1.{$arr1.{$one}} )>
		 *
		 *                      <(print $arr2.1.{$arr1.{$one}} , !!!! , {$arr2.{0}.{$one}})>
		 */
		class processor : public preprocessor
		{
			public:

				/**
				 * constructor
				 * @param CGI defines instance of cgi object
				 */
				processor(server &CGI);

				/**
				 * destructor
				 */
				virtual ~processor();

				/**
				 * @return parsed template
				 * @param path defines path of template
				 */
				virtual dodoString process(const dodoString &path);

				/**
				 * @return parsed template
				 * @param buffer defines buffer where template is stored
				 */
				virtual dodoString processString(const dodoString &buffer);

				/**
				 * set variable
				 * @param varName defines name of variable
				 * @param varVal defines value of variable
				 */
				virtual void assign(dodoString varName, const dodoString &varVal);

				/**
				 * set variable
				 * @param varName defines name of variable
				 * @param varVal defines value of variable(array)
				 */
				virtual void assign(dodoString varName, const dodoStringArray &varVal);

				/**
				 * set variable
				 * @param varName defines name of variable
				 * @param varVal defines value of variablee(hash)
				 */
				virtual void assign(dodoString varName, const dodoStringMap &varVal);

				/**
				 * set variable
				 * @param varName defines name of variable
				 * @param varVal defines value of variable(array of hashes)
				 */
				virtual void assign(dodoString varName, const dodoArray<dodoStringMap> &varVal);

				/**
				 * pass parsed template to CGI output
				 * @param path indicates path where template stays
				 */
				virtual void display(const dodoString &path);

				/**
				 * clear internal data[assigned vars, etc.]
				 */
				virtual void clear();

			protected:

				/**
				 * @return parsed template from preprocessored buffer
				 * @param buffer defines buffer where template is stored
				 * @param path defines path of template
				 */
				virtual dodoString _processString(const dodoString &buffer, const dodoString &path);

				/**
				 * process `if` statement
				 * @return position of cursor where to continue processing
				 * @param buffer defines buffer where template is stored
				 * @param start defines position in buffer after ')>' of `<( if ... )>` block
				 * @param statement defines `if` statement
				 * @param processed defines buffer where to add processed block
				 * @param path defines path of template
				 */
				virtual unsigned long _if(const dodoString &buffer, unsigned long start, const dodoString &statement, dodoString &processed, const dodoString &path);

				/**
				 * process `for` statement
				 * @return position of cursor where to continue processing
				 * @param buffer defines buffer where template is stored
				 * @param start defines position in buffer after ')>' of `<( for ... )>` block
				 * @param statement defines `for` statement
				 * @param processed defines buffer where to add processed block
				 * @param path defines path of template
				 */
				virtual unsigned long _for(const dodoString &buffer, unsigned long start, const dodoString &statement, dodoString &processed, const dodoString &path);

				/**
				 * process `for` statement
				 * @return position of cursor where to continue processing
				 * @param buffer defines buffer where template is stored
				 * @param start defines position in buffer after ')>' of `<( namespace ... )>` block
				 * @param processed defines buffer where to add processed block
				 * @param path defines path of template
				 */
				virtual unsigned long _ns(const dodoString &buffer, unsigned long start, dodoString &processed, const dodoString &path);

				/**
				 * process `print` statement
				 * @return position of cursor where to continue processing
				 * @param start defines position in buffer after ')>' of `<( print ... )>` block
				 * @param statement defines `print` statement
				 * @param processed defines buffer where to add processed block
				 * @param path defines path of template
				 */
				virtual unsigned long _print(unsigned long start, const dodoString &statement, dodoString &processed, const dodoString &path);

				/**
				 * process `break` statement
				 * @return true if process must break
				 * @param start defines position in buffer after ')>' of `<( break ... )>` block
				 * @param statement defines `break` statement
				 * @param path defines path of template
				 */
				virtual bool _break(unsigned long start, const dodoString &statement, const dodoString &path);

				/**
				 * process `assign` statement
				 * @return position of cursor where to continue processing
				 * @param start defines position in buffer after ')>' of `<( assign ... )>` block
				 * @param statement defines `assign` statement
				 * @param path defines path of template
				 */
				virtual unsigned long _assign(unsigned long start, const dodoString &statement, const dodoString &path);

				/**
				 * process `include` statement
				 * @return position of cursor where to continue processing
				 * @param start defines position in buffer after ')>' of `<( include ... )>` block
				 * @param statement defines `include` statement
				 * @param processed indicates string where to add result
				 * @param path defines path of template
				 */
				virtual unsigned long _include(unsigned long start, const dodoString &statement, dodoString &processed, const dodoString &path);

				/**
				 * clean namespace variable and bring back to life vars of prevous namespace that were overwritten
				 */
				virtual void cleanNamespace();

				/**
				 * @return position in buffer after ')>' of the end block
				 * @param buffer defines buffer where template is stored
				 * @param start defines position in buffer after ')>' of the block
				 * @param st defines open statement[if, for ...]
				 * @param ts defines close statement[fi, rof ...]
				 * @param path defines path of template
				 */
				virtual unsigned long blockEnd(const dodoString &buffer, unsigned long start, const dodoString &st, const dodoString &ts, const dodoString &path);

				/**
				 * @return true if path is in `processed` list
				 * @param path defines path that might cause resursive include
				 */
				virtual bool recursive(const dodoString &path);

				/**
				 * @return value of the variable
				 * @param varName defines name of variable
				 * @param start defines position in file
				 * @param path defines path of template
				 */
				virtual dodoString getVar(const dodoString &varName, unsigned long start, const dodoString &path);

				/**
				 * @return interpolated name of the variable[expanded from {}]
				 * @param varName defines name of variable
				 * @param start defines position in file
				 * @param path defines path of template
				 */
				virtual dodoString getVarName(const dodoString &varName, unsigned long start, const dodoString &path);

				/**
				 * @return extracted data(e.g. removes pairs of ",',`)
				 * @param statement defines statement that needs extraction from the pairs of ",',`
				 */
				virtual dodoString trim(const dodoString &statement);

				dodoList<dodoString> processed;                                                                         ///< files that will be skipped due to the recursion

				dodoStringMap global;                                                                                   ///< set of global variables[user-set]
				dodoMap<dodoString, dodoStringArray> globalArray;                                                       ///< global user-defined variables(arrays)
				dodoMap<dodoString, dodoStringMap> globalHash;                                                          ///< global user-defined variables(hashes)
				dodoMap<dodoString, dodoArray<dodoStringMap> > globalArrayHash;                                         ///< global user-defined variables(array of hashes)

				dodoMap<unsigned int, dodoStringMap> local;                                                             ///< local variables[occur in `for` block]
				dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> > localHash;                                   ///< local variables(hashes)[occur in `for` block]

				dodoStringMap dodo;                                                                                     ///< auxillary variables[dodo defined][for dodo.*]

				bool continueFlag;                                                                                      ///< indicates `continue` state

				unsigned int breakDeepness;                                                                             ///< deepness of the break
				unsigned int loopDeepness;                                                                              ///< deepness of the loop

				unsigned long iterator;                                                                                 ///< amount of iterations of the loop

				unsigned int namespaceDeepness;                                                                         ///< deepness of the namespace

				server &CGI;                                                                                            ///< cgi object through what output will be performed
		};
	};
};

#endif
