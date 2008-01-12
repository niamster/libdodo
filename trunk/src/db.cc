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

__dbInfo::__dbInfo(const dodoString &a_db,
				   const dodoString &a_host,
				   const dodoString &a_user,
				   const dodoString &a_password,
				   const dodoString &a_path,
				   int a_port) : db(a_db),
								 host(a_host),
								 user(a_user),
								 password(a_password),
								 path(a_path),
								 port(a_port)
{
}

//-------------------------------------------------------------------

__dbInfo::__dbInfo()
{
}

//-------------------------------------------------------------------

__xexexDbBaseCollectedData::__xexexDbBaseCollectedData(
	dodoString &a_pre_where,
	dodoStringArray &a_pre_fieldsNames,
	dodoArray<dodoStringArray> &a_pre_fieldsVal,
	dodoString &a_pre_table,
	dodoString &a_pre_tableTo,
	dodoString &a_pre_order,
	dodoString &a_pre_having,
	dodoString &a_pre_group,
	dodoString &a_pre_limNumber,
	dodoString &a_pre_limOffset,
	dodoStringArray &a_pre_subQueries,
	dodoStringArray &a_pre_joinTables,
	dodoStringArray &a_pre_joinConds,
	dodoArray<int> &a_pre_joinTypes,
	int &a_qType,
	int &a_qShift,
	int &a_qSelShift,
	int &a_qInsShift,
	int &a_qUpShift,
	int &a_qDelShift,
	int &a_operType,
	void *a_executor) : pre_where(a_pre_where),
						pre_fieldsNames(a_pre_fieldsNames),
						pre_fieldsVal(a_pre_fieldsVal),
						pre_table(a_pre_table),
						pre_tableTo(a_pre_tableTo),
						pre_order(a_pre_order),
						pre_having(a_pre_having),
						pre_group(a_pre_group),
						pre_limNumber(a_pre_limNumber),
						pre_limOffset(a_pre_limOffset),
						pre_subQueries(a_pre_subQueries),
						pre_joinTables(a_pre_joinTables),
						pre_joinConds(a_pre_joinConds),
						pre_joinTypes(a_pre_joinTypes),
						qType(a_qType),
						qShift(a_qShift),
						qSelShift(a_qSelShift),
						qInsShift(a_qInsShift),
						qUpShift(a_qUpShift),
						qDelShift(a_qDelShift),
						operType(a_operType),
						executor(a_executor)
{
}

//-------------------------------------------------------------------

__dbStorage::__dbStorage(dodoArray<dodoStringArray> a_rows, dodoStringArray a_fields) : rows(a_rows),
																						fields(a_fields)
{
}

//-------------------------------------------------------------------

__dbStorage::__dbStorage()
{
	
}

//-------------------------------------------------------------------
	
db::db() : connected(false),
		   reconnect(true),
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
						 pre_subQueries,
						 pre_joinTables,
						 pre_joinConds,
						 pre_joinTypes,
						 qType,
						 qShift,
						 qSelShift,
						 qInsShift,
						 qUpShift,
						 qDelShift,
						 operType,
						 (void *) this)
{
}

//-------------------------------------------------------------------

db::~db()
{
}

//-------------------------------------------------------------------

