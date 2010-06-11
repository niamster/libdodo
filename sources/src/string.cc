/***************************************************************************
 *            string.cc
 *
 *  Thu Jun  10 2010
 *  Copyright  2010  Ni@m
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <libdodo/string.h>

#include <stdio.h>

using namespace dodo;

string::string() : buf(NULL),
                   length(0)
{
}

//-------------------------------------------------------------------

string::string(const string &s)
{
}

//-------------------------------------------------------------------

string::string(const char *data)
{
}

//-------------------------------------------------------------------

string::string(const char *data,
               unsigned long length)
{
}

//-------------------------------------------------------------------

string::string(char symbol,
               unsigned long count)
{
}

//-------------------------------------------------------------------

string::~string()
{
}

//-------------------------------------------------------------------

const char *
string::data() const
{
}

//-------------------------------------------------------------------

unsigned long
string::size() const
{
}

//-------------------------------------------------------------------

bool
string::empty() const
{
}

//-------------------------------------------------------------------

void
string::erase(unsigned long index,
              unsigned long count)
{
}

//-------------------------------------------------------------------

void
string::clear()
{
}

//-------------------------------------------------------------------

void
string::resize(unsigned long length)
{
}

//-------------------------------------------------------------------

unsigned long
string::find(const string &str,
             unsigned long index) const
{
}

//-------------------------------------------------------------------

string &
string::replace(unsigned long index,
                unsigned long num,
                const string &str)
{
}

//-------------------------------------------------------------------

string &
string::insert(unsigned long index,
               const string &str)
{
}

//-------------------------------------------------------------------

string
string::substr(unsigned long index,
               unsigned long length) const
{
}

//-------------------------------------------------------------------

void
string::reserve(unsigned long count)
{
}

//-------------------------------------------------------------------

string &
string::operator=(const string &data)
{
}

//-------------------------------------------------------------------

string &
string::operator+=(const string &str)
{
}

//-------------------------------------------------------------------

char
string::operator[](unsigned long index) const
{
}
//-------------------------------------------------------------------

string::operator const char *() const
{
}


//-------------------------------------------------------------------

bool
operator<(const string &s1,
          const string &s2)
{
}

//-------------------------------------------------------------------

bool
operator>(const string &s1,
          const string &s2)
{
}

//-------------------------------------------------------------------

bool
operator==(const string &s1,
           const string &s2)
{
}

//-------------------------------------------------------------------

bool
operator==(const string &s1,
           const char *s2)
{
}

//-------------------------------------------------------------------

string &
operator+(const string &s1,
          const string &s2)
{
}

//-------------------------------------------------------------------
