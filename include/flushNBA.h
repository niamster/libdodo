/***************************************************************************
 *            flushNBA.h
 *
 *  Thu Sep 09 03:21:24 2006
 *  Copyright  2006  Ni@m
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


#ifndef _FLUSHNBA_H_
#define _FLUSHNBA_H_

#include <directives.h>

#include <types.h>

namespace dodo
{
	
	/**
	 * @class flushNBA
	 */
	 class flushNBA
	 {
	 	
	 	private:
						 	
	 		/**
	 		 * copy constructor
			 * to prevent copying
	 		 */
	 		flushNBA(flushNBA &rt);
	 			 	
	 	public:
						 	
	 		/**
	 		 * constructor
	 		 */
	 		flushNBA();
	 		
	 		/**
	 		 * destructor
	 		 */
	 		virtual ~flushNBA();
	 };

};

#endif