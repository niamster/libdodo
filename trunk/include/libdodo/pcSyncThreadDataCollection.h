/***************************************************************************
 *            pcSyncThreadDataCollection.h
 *
 *  Tue Nov 29 23:31:55 2005
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

#ifndef _PCSYNCTHREADDATACOLLECTION_H_
#define _PCSYNCTHREADDATACOLLECTION_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/pcSyncDataCollection.h>
#include <libdodo/pcSyncThreadDataCollectionEx.h>
#include <libdodo/pcSyncThreadStack.h>

namespace dodo
{
	namespace pc
	{
		namespace sync
		{
			namespace thread
			{
				namespace data
				{
					/**
					 * @class collection
					 * @brief implements collection of shared data for threads
					 */
					class collection : public sync::data::collection,
									   virtual public sync::thread::stack
					{
					  private:

						/**
						 * copy constructor
						 * @note to prevent copying
						 */
						collection(collection &sts);

					  public:

						/**
						 * constructor
						 */
						collection();

						/**
						 * destructor
						 */
						virtual ~collection();

						/**
						 * add shared data
						 * @return shared data identificator
						 * @param data defines shared data
						 */
						virtual unsigned long add(void *data);

						/**
						 * delete data from collection
						 * @param position defines shared data identificator
						 */
						virtual void del(unsigned long position);

						/**
						 * lock, set data, unlock
						 * @param position defines shared data identificator
						 * @param data defines shared data
						 */
						virtual void set(unsigned long position,
										 void          *data);

						/**
						 * lock, return data, unlock
						 * @return shared data
						 * @param position defines shared data identificator
						 */
						virtual const void *get(unsigned long position);

						/**
						 * @return list of shared data in object
						 */
						virtual dodoList<unsigned long> getIds();

					  protected:

						/**
						 * search shared data by identificator
						 * @return true if found
						 * @param position describes defines shared data identificator
						 * @note this sets internal class parameter 'current' to found shared data
						 */
						virtual bool getShare(unsigned long position);

						dodoList<pc::sync::data::__info> shares;            ///< identificators of shared data

						unsigned long shareNum;                             ///< number of registered shares

						dodoList<pc::sync::data::__info>::iterator current; ///< iterator for list of shared data[for matched with getShare method]
					};
				};
			};
		};
	};
};

#endif

