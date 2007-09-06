/***************************************************************************
 *            dodoString.h
 *
 *  Fri Nov 10 07:07:19 2006
 *  Copyright  2006  Ni@m
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

#ifndef _DODOSTRING_H_
#define _DODOSTRING_H_

#include <directives.h>

#include <string>

namespace dodo
{
	/**
	 * @class dodoString extends standart std::string class
	 */
	class dodoString : public std::string
	{
		public:

		/**
		 * constructor
		 */
		dodoString();

		/**
		 * constructor
		 */
		dodoString(const std::string&str);

		/**
		 * constructor
		 */
		dodoString(const char *str);

		/**
		 * copy constructor
		 */
		dodoString(const dodoString&str);

		/**
		 * constructor
		 */
		dodoString(const char *str, unsigned long length);

		/**
		 * constructor
		 */
		dodoString(const std::string&str, unsigned long index, unsigned long length);

		/**
		 * constructor
		 */
		dodoString(unsigned long length, const char&ch);

		/**
		 * @return true if strings are equal
		 * @param first string to compare
		 * @param second string to compare
		 */
		static bool equal(const dodoString&first, const dodoString&second);

		/**
		 * @return true if strings are equal
		 * @param first string to compare
		 * @param second string to compare
		 * @note it ignores case
		 */
		static bool iequal(const dodoString&first, const dodoString&second);

		/**
		 * @return formated string
		 * @param format represents format of the string
		 */
		static dodoString format(const dodoString&format, ...);

		/**
		 * removes symbols from the end and from the begin of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbol is symbol to remove
		 */
		static dodoString trim(const dodoString&data, const char symbol = ' ');

		/**
		 * removes symbols[from given array] from the end and from the begin of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		static dodoString trim(const dodoString&data, const char symbols[], int symCount);

		/**
		 * removes symbols[from given array] from the end of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		static dodoString rTrim(const dodoString&data, const char symbols[], int symCount);

		/**
		 * removes symbols[from given array] from the begin of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		static dodoString lTrim(const dodoString&data, const char symbols[], int symCount);

		/**
		 * removes symbols from the end of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbol is symbol to remove
		 */
		static dodoString rTrim(const dodoString&data, char symbol = ' ');

		/**
		 * removes symbols from the begin of given string
		 * @return processed string
		 * @param data is string to process
		 * @param symbol is symbol to remove
		 */
		static dodoString lTrim(const dodoString&data, char symbol = ' ');

		/**
		 * removes symbols from the end and from the begin of string
		 * @return processed string
		 * @param symbol is symbol to remove
		 */
		virtual dodoString trim(const char symbol = ' ');

		/**
		 * removes symbols[from given array] from the end and from the begin of string
		 * @return processed string
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		virtual dodoString trim(const char symbols[], int symCount);

		/**
		 * removes symbols[from given array] from the end of string
		 * @return processed string
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		virtual dodoString rTrim(const char symbols[], int symCount);

		/**
		 * removes symbols[from given array] from the begin of string
		 * @return processed string
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		virtual dodoString lTrim(const char symbols[], int symCount);

		/**
		 * removes symbols from the end of string
		 * @return processed string
		 * @param symbol is symbol to remove
		 */
		virtual dodoString rTrim(char symbol = ' ');

		/**
		 * removes symbols from the begin of string
		 * @return processed string
		 * @param symbol is symbol to remove
		 */
		virtual dodoString lTrim(char symbol = ' ');

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
		 * @return converted long to string
		 * @param number is number to convert
		 */
		virtual dodoString fromL(long number);

		/**
		 * @return converted unsigned long to string
		 * @param number is number to convert
		 */
		virtual dodoString fromUL(unsigned long number);

		/**
		 * @return converted int to string
		 * @param number is number to convert
		 */
		virtual dodoString fromI(int number);

		/**
		 * @return converted unsigned int to string
		 * @param number is number to convert
		 */
		virtual dodoString fromUI(unsigned int number);

		/**
		 * @return converted short to string
		 * @param number is number to convert
		 */
		virtual dodoString fromS(short number);

		/**
		 * @return converted unsigned short to string
		 * @param number is number to convert
		 */
		virtual dodoString fromUS(unsigned short number);

		/**
		 * @return converted float to string
		 * @param number is number to convert
		 */
		virtual dodoString fromF(float number);

		/**
		 * @return converted double to string
		 * @param number is number to convert
		 */
		virtual dodoString fromD(double number);

		/**
		 * @return string converted to unsigned long
		 */
		static unsigned long stringToUL(const dodoString&data);

		/**
		 * @return string converted to long
		 */
		static long stringToL(const dodoString&data);

		/**
		 * @return string converted to int
		 */
		static int stringToI(const dodoString&data);

		/**
		 * @return string converted to unsigned int
		 */
		static unsigned int stringToUI(const dodoString&data);

		/**
		 * @return string converted to short
		 */
		static short stringToS(const dodoString&data);

		/**
		 * @return string converted to unsigned short
		 */
		static unsigned short stringToUS(const dodoString&data);

		/**
		 * @return string converted to double
		 */
		static double stringToD(const dodoString&data);

		/**
		 * @return string converted to float
		 */
		static float stringToF(const dodoString&data);

		/**
		 * @return string converted to unsigned long
		 */
		virtual unsigned long toUL();

		/**
		 * @return string converted to long
		 */
		virtual long toL();

		/**
		 * @return string converted to int
		 */
		virtual int toI();

		/**
		 * @return string converted to unsigned int
		 */
		virtual unsigned int toUI();

		/**
		 * @return string converted to short
		 */
		virtual short toS();

		/**
		 * @return string converted to unsigned short
		 */
		virtual unsigned short toUS();

		/**
		 * @return string converted to double
		 */
		virtual double toD();

		/**
		 * @return string converted to float
		 */
		virtual float toF();
	};
};

#endif
