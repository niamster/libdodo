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
#include <stringTools.h>

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
		dodoString(const std::string &str);

		/**
		 * constructor
		 */
		dodoString(const char *str);

		/**
		 * copy constructor
		 */
		dodoString(const dodoString &str);

		/**
		 * constructor
		 */
		dodoString(const char *str, unsigned long length);

		/**
		 * constructor
		 */
		dodoString(const std::string &str, unsigned long index, unsigned long length);

		/**
		 * constructor
		 */
		dodoString(unsigned long length, const char &ch);

		/**
		 * @param format represents format of the string
		 */
		virtual void format(const dodoString &format, ...);

		/**
		 * removes symbols from the end and from the begin of string
		 * @param symbol is symbol to remove
		 */
		virtual void trim(const char symbol = ' ');

		/**
		 * removes symbols[from given array] from the end and from the begin of string
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		virtual void trim(const char symbols[], int symCount);

		/**
		 * removes symbols[from given array] from the end of string
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		virtual void rTrim(const char symbols[], int symCount);

		/**
		 * removes symbols[from given array] from the begin of string
		 * @param symbols is an array of symbols to remove
		 * @param symCount is amount of symbols in @param symbols
		 */
		virtual void lTrim(const char symbols[], int symCount);

		/**
		 * removes symbols from the end of string
		 * @param symbol is symbol to remove
		 */
		virtual void rTrim(char symbol = ' ');

		/**
		 * removes symbols from the begin of string
		 * @param symbol is symbol to remove
		 */
		virtual void lTrim(char symbol = ' ');

		/**
		 * @param number is number to convert
		 */
		virtual void fromL(long number);

		/**
		 * @param number is number to convert
		 */
		virtual void fromUL(unsigned long number);

		/**
		 * @param number is number to convert
		 */
		virtual void fromI(int number);

		/**
		 * @param number is number to convert
		 */
		virtual void fromUI(unsigned int number);

		/**
		 * @param number is number to convert
		 */
		virtual void fromS(short number);

		/**
		 * @param number is number to convert
		 */
		virtual void fromUS(unsigned short number);

		/**
		 * @param number is number to convert
		 */
		virtual void fromF(float number);

		/**
		 * @param number is number to convert
		 */
		virtual void fromD(double number);

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
