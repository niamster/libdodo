/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/baseEx.h>
#include <libdodo/dbMysql.h>
#include <libdodo/toolsTime.h>

#include <iostream>

using namespace dodo;

using namespace std;

#ifdef MYSQL_EXT

using namespace db;

#ifndef DB_WO_XEXEC

void
hook(void *odata,
	 short int type,
	 void *udata)
{
	__xexecDbAccumulatorCollectedData *sql = (__xexecDbAccumulatorCollectedData *)odata;

	if (sql->operType == DB_OPERATION_EXEC)
	{
		cout << endl << endl << "request: " << ((sqlConstructor *)(sql->executor))->queryCollect() << endl << endl;
	}
}

#endif

#endif

int main(int argc, char **argv)
{
	long now = tools::time::now();

#ifdef MYSQL_EXT

	mysql pp;
	try
	{

#ifndef DB_WO_XEXEC

		int pos = pp.addPreExec(hook, NULL);

#endif

		pp.setDbInfo("test", "", 3306, "root", "password");
		pp.connect();

		try
		{
			pp.deleteTable("test");
			pp.exec();

			pp.deleteTable("test1");
			pp.exec();
		}
		catch (...)
		{
		}

		__connectorTable ti;
		ti.name = "test";

		__connectorField fi;

		fi.name = "id";
		fi.type = CONNECTOR_FIELDTYPE_INTEGER;
		fi.flag = CONNECTOR_FIELDFLAG_NULL | CONNECTOR_FIELDFLAG_AUTO_INCREMENT;
		ti.fields.push_back(fi);

		fi.name = "dot";
		fi.flag = 0;
		fi.type = CONNECTOR_FIELDTYPE_TEXT;
		ti.fields.push_back(fi);

		fi.name = "operation";
		fi.type = CONNECTOR_FIELDTYPE_TEXT;
		ti.fields.push_back(fi);

		pp.createTable(ti);
		pp.exec();

		ti.fields.clear();
		ti.name = "test1";

		fi.name = "id";
		fi.type = CONNECTOR_FIELDTYPE_INTEGER;
		fi.flag = CONNECTOR_FIELDFLAG_NULL | CONNECTOR_FIELDFLAG_AUTO_INCREMENT;
		ti.fields.push_back(fi);

		fi.name = "dot";
		fi.flag = 0;
		fi.type = CONNECTOR_FIELDTYPE_TEXT;
		ti.fields.push_back(fi);

		fi.name = "operation";
		fi.type = CONNECTOR_FIELDTYPE_TEXT;
		ti.fields.push_back(fi);

		pp.createTable(ti);
		pp.exec();

		pp.createIndex("test", "id", "id");
		pp.exec();

		fi.name = "foo";
		fi.type = CONNECTOR_FIELDTYPE_CHAR;
		fi.length = 10;
		pp.createField(fi, "test");
		pp.exec();

		fi.name = "bar";
		pp.renameField("foo", fi, "test");
		pp.exec();

		dodoStringArray fields;
		__tuples storage;

		pp.selectAll("test");
		pp.join("test1", JOINTYPE_JOIN, "test.operation = test1.operation");
		pp.limit(10);
		pp.exec();

		storage = pp.fetch();

		dodoStringArray::iterator i = storage.fields.begin(), j = storage.fields.end();
		for (; i != j; ++i)
			cout << *i << "\t";
		cout << endl;

		dodoStringArray values;
		values.push_back("20\"05`''-'07-08");
		values.push_back("mu");

		pp.limit(10);
		pp.offset(23);

		dodoStringMap arr;
		dodoArray<dodoStringMap> assA;
		arr["dot"] = "20\"05`''-'07-08";
		arr["operation"] = "m\nu";
		assA.push_back(arr);
		arr["dot"] = "20\"05`''-'07-08";
		arr["operation"] = "n\nu";
		assA.push_back(arr);

		pp.setAddInsSt(ACCUMULATOR_ADDREQUEST_INSERT_IGNORE);
		pp.setAddSelSt(ACCUMULATOR_ADDREQUEST_SELECT_DISTINCT);
		pp.setMyAddSelSt(MYSQL_ADDREQUEST_SELECT_BIG_RESULT);

		pp.insert("test", assA);
		pp.exec();

		fields.clear();
		fields.push_back("dot");
		fields.push_back("operation");

		pp.insert("test", values, fields);
		pp.exec();

#ifndef DB_WO_XEXEC
		
		pp.disablePreExec(pos);

#endif

		for (int o = 0; o < 100000; o++)
		{
			pp.insert("test", values, fields);
			pp.exec();
		}

#ifndef DB_WO_XEXEC
		
		pp.enablePreExec(pos);

#endif

		pp.selectAll("test", "id>1");

		dodoStringArray uni;
		dodoStringArray uni_all;
		uni.push_back(pp.queryCollect());
		uni.push_back(pp.queryCollect());
		pp.subquery(uni);
		uni_all.push_back(pp.queryCollect());
		uni_all.push_back(pp.queryCollect());
		pp.subquery(uni_all, SUBREQUEST_UNION_ALL);
		pp.order("id desc");
		pp.limit(5);
		pp.setAddSelSt(ACCUMULATOR_ADDREQUEST_SELECT_DISTINCT);
		pp.exec();

		storage = pp.fetch();
	}
	catch (baseEx ex)
	{
		cout << ex.file << endl << ex.baseErrstr << endl << ex.line << endl << ex.message << endl;
	}

#else

	cout << "No MySQL extension was compiled!";

#endif

	now = tools::time::now() - now;

	cout << "SpentTime: " << now << endl;

	return 0;
}
