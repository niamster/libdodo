/***************************************************************************
 *            pcSyncDataCollection.h
 *
 *  Sat Oct 20 02:00:55 2007
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _PCSYNCDATACOLLECTION_H_
#define _PCSYNCDATACOLLECTION_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
	namespace pc {
		namespace sync {
			namespace data {
				/**
				 * @struct __info__
				 * @brief defines shared data
				 */
				struct __info__ {
					unsigned long position; ///< shared data identificator
					void          *data;    ///< shared data
				};

				/**
				 * @class collection
				 * @brief implements collection of shared data
				 */
				class collection {
				  public:

					/**
					 * destructor
					 */
					virtual ~collection() = 0;

					/**
					 * add shared data
					 * @return shared data identificator
					 * @param data defines shared data
					 */
					virtual unsigned long add(void *data) = 0;

					/**
					 * delete data from collection
					 * @param position defines shared data identificator
					 */
					virtual void del(unsigned long position) = 0;

					/**
					 * lock, set data, unlock
					 * @param position defines shared data identificator
					 * @param data defines shared data
					 */
					virtual void set(unsigned long position,
									 void          *data) = 0;

					/**
					 * lock, return data, unlock
					 * @return shared data
					 * @param position defines shared data identificator
					 */
					virtual const void *get(unsigned long position) = 0;

					/**
					 * @return list of shared data in object
					 */
					virtual dodoList<unsigned long> getIds() = 0;
				};
			};
		};
	};
};

#endif

