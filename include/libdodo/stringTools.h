/***************************************************************************
 *            stringTools.h
 *
 *  Sun Oct 30 11:43:19 2007
 *  Copyright  2007  Ni@m
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

#ifndef _STRINGTOOLS_H_
#define _STRINGTOOLS_H_

#include <directives.h>

#include <dodoString.h>

namespace dodo
{
	/**
	 * @class stringToolst extends standart std::string class
	 */
	class stringTools
	{
		public:

		/**
		 * replaces nodes in string
		 * @param needle indicates what to replace in string
		 * @param replacement indicates with what to replace
		 * @param data specifies wher to replace
		 */
		static void replace(const dodoString &needle, const dodoString &replacement, dodoString &data);

		/**
		 * @return true if strings are equal
		 * @param first string to compare
		 * @param second string to compare
		 */
		static bool equal(const dodoString &first, const dodoString &second);

		/**
		 * @return true if strings are equal
		 * @param first string to compare
		 * @param second string to compare
		 * @note it ignores case
		 */
		static bool iequal(const dodoString &first, const dodoString &second);

		/**
		 * @return formated string
		 * @param format represents format of the string
		 */
		static dodoString format(const dodoString &format, ...);

		/**
		 * removes symbols from the end and from the begin of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbol is symbol to remove
		 */
		static dodoString trim(const dodoString &data, const char symbol = ' ');

		/**
		 * removes symbols[from given array] from the end and from the begin of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		static dodoString trim(const dodoString &data, const char symbols[], int symCount);

		/**
		 * removes symbols[from given array] from the end of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		static dodoString rTrim(const dodoString &data, const char symbols[], int symCount);

		/**
		 * removes symbols[from given array] from the begin of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		static dodoString lTrim(const dodoString &data, const char symbols[], int symCount);

		/**
		 * removes symbols from the end of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbol is symbol to remove
		 */
		static dodoString rTrim(const dodoString &data, char symbol = ' ');

		/**
		 * removes symbols from the begin of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbol is symbol to remove
		 */
		static dodoString lTrim(const dodoString &data, char symbol = ' ');

		/**
		 * @return converted long to string
		 * @param number is number to convert
		 */
		static dodoString lToString(long number);

		/**
		 * @return converted unsigned long to string
		 * @param number is number to convert
		 */
		static dodoString ulToString(unsigned long number);

		/**
		 * @return converted int to string
		 * @param number is number to convert
		 */
		static dodoString iToString(int number);

		/**
		 * @return converted unsigned int to string
		 * @param number is number to convert
		 */
		static dodoString uiToString(unsigned int number);

		/**
		 * @return converted short to string
		 * @param number is number to convert
		 */
		static dodoString sToString(short number);

		/**
		 * @return converted unsigned short to string
		 * @param number is number to convert
		 */
		static dodoString usToString(unsigned short number);

		/**
		 * @return converted float to string
		 * @param number is number to convert
		 */
		static dodoString fToString(float number);

		/**
		 * @return converted double to string
		 * @param number is number to convert
		 */
		static dodoString dToString(double number);

		/**
		 * @return string converted to unsigned long
		 */
		static unsigned long stringToUL(const dodoString &data);

		/**
		 * @return string converted to long
		 */
		static long stringToL(const dodoString &data);

		/**
		 * @return string converted to int
		 */
		static int stringToI(const dodoString &data);

		/**
		 * @return string converted to unsigned int
		 */
		static unsigned int stringToUI(const dodoString &data);

		/**
		 * @return string converted to short
		 */
		static short stringToS(const dodoString &data);

		/**
		 * @return string converted to unsigned short
		 */
		static unsigned short stringToUS(const dodoString &data);

		/**
		 * @return string converted to double
		 */
		static double stringToD(const dodoString &data);

		/**
		 * @return string converted to float
		 */
		static float stringToF(const dodoString &data);
	};
};

#endif /*STRINGTOOLS_H_*/
