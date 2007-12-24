/***************************************************************************
 *            db.cc
 *
 *  Sun Jan  15 19:45:19 2005
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

#include <libdodo/db.h>

using namespace dodo;

__xexexDbBaseCollectedData::__xexexDbBaseCollectedData(
	dodoString &a_pre_where,
	dodoStringArr &a_pre_fieldsNames,
	dodoArray<dodoStringArr> &a_pre_fieldsVal,
	dodoString &a_pre_table,
	dodoString &a_pre_tableTo,
	dodoString &a_pre_order,
	dodoString &a_pre_having,
	dodoString &a_pre_group,
	dodoString &a_pre_limNumber,
	dodoString &a_pre_limOffset,
	dodoStringArr &a_pre_subQ,
	int &a_qType,
	int &a_qShift,
	int &a_qSelShift,
	int &a_qInsShift,
	int &a_qUpShift,
	int &a_qDelShift,
	int &a_operType) :	pre_where(a_pre_where),
			pre_fieldsNames(a_pre_fieldsNames),
			pre_fieldsVal(a_pre_fieldsVal),
			pre_table(a_pre_table),
			pre_tableTo(a_pre_tableTo),
			pre_order(a_pre_order),
			pre_having(a_pre_having),
			pre_group(a_pre_group),
			pre_limNumber(a_pre_limNumber),
			pre_limOffset(a_pre_limOffset),
			pre_subQ(a_pre_subQ),
			qType(a_qType),
			qShift(a_qShift),
			qSelShift(a_qSelShift),
			qInsShift(a_qInsShift),
			qUpShift(a_qUpShift),
			qDelShift(a_qDelShift),
			operType(a_operType)
{
}

//-------------------------------------------------------------------

db::db() : connected(false),
		qType(-1),
		qShift(DB_EMPTY),
		qSelShift(DB_EMPTY),
		qInsShift(DB_EMPTY),
		qUpShift(DB_EMPTY),
		qDelShift(DB_EMPTY),
		collectedData(pre_where,
					   pre_fieldsNames,
					   pre_fieldsVal,
					   pre_table,
					   pre_tableTo,
					   pre_order,
					   pre_having,
					   pre_group,
					   pre_limNumber,
					   pre_limOffset,
					   pre_subQ,
					   qType,
					   qShift,
					   qSelShift,
					   qInsShift,
					   qUpShift,
					   qDelShift,
					   operType)
{
}

//-------------------------------------------------------------------

db::~db()
{
}

//-------------------------------------------------------------------
