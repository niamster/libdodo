/***************************************************************************
 *            systemProcesses.h
 *
 *  Tue Feb 27 08:38:55 2007
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
 
#ifndef _SYSTEMPROCESSES_H_
#define _SYSTEMPROCESSES_H_

#include <directives.h>
#include <types.h>
	
	namespace dodo
	{
	
		/**
		 * @typedef describes function to be passed as thread
		 */
		typedef void *(*threadFunc)(void *);

		class systemProcesses
		{
			private:

				/**
				 * copy constructor
				 * to prevent copying
				 */
				systemProcesses(systemProcesses &sp);

			public:

		};

	};

#endif
