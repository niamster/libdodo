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

#ifndef DBMYSQL_WO_XEXEC

void
hook(void *odata,
	 short int type,
	 void *udata)
{
	__xexexDbAccumulatorCollectedData *sql = (__xexexDbAccumulatorCollectedData *)odata;

	if (sql->operType == MYSQL_OPERATION_EXEC && sql->qType == ACCUMULATOR_REQUEST_SELECT)
	{
		cout << endl << endl << "table was " << sql->table << endl << endl;

		sql->limit = "70";
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

#ifndef DBMYSQL_WO_XEXEC

		int pos = pp.addPreExec(hook, (void *)"id");

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
		cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
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
		cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
		pp.exec();

		pp.createIndex("test", "id", "id");
		cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
		pp.exec();

		/*create field*/
		fi.name = "foo";
		fi.type = CONNECTOR_FIELDTYPE_CHAR;
		fi.length = 10;

		pp.createField(fi, "test");
		cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
		pp.exec();

		fi.name = "bar";
		pp.renameField("foo", fi, "test");
		cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
		pp.exec();

		dodoStringArray fields;
		__connectorStorage storage;

		/* select*/
		pp.selectAll("test");
		pp.join("test1", CONNECTOR_JOINTYPE_JOIN, "test.operation = test1.operation");
		pp.limit(10);
		cout << pp.queryCollect() << endl;
		pp.exec();
		storage = pp.fetch();        //get result
		dodoStringArray::iterator i = storage.fields.begin(), j = storage.fields.end();
		for (; i != j; ++i)
			cout << *i << "\t";
		cout << endl;

		dodoStringArray values;
		values.push_back("20\"05`''-'07-08");
		values.push_back("mu");

		pp.limit(10);
		pp.offset(23);
		pp.offset(3);

		dodoStringMap arr;
		dodoArray<dodoStringMap> assA;
		arr["dot"] = "20\"05`''-'07-08";
		arr["operation"] = "m\nu";
		assA.push_back(arr);
		arr["dot"] = "20\"05`''-'07-08";
		arr["operation"] = "n\nu";
		assA.push_back(arr);

		/*additional statement*/
		pp.setAddInsSt(ACCUMULATOR_ADDREQUEST_INSERT_IGNORE);           //base SQL
		pp.setAddSelSt(ACCUMULATOR_ADDREQUEST_SELECT_DISTINCT);         //base SQL
		pp.setMyAddSelSt(MYSQL_ADDREQUEST_SELECT_BIG_RESULT);           //mySQL features; defined only in this class

		pp.insert("test", assA);                                        //multiply insert
		cout << pp.queryCollect() << endl;
		pp.exec();

		fields.clear();
		fields.push_back("dot");
		fields.push_back("operation");

		pp.insert("test", values, fields);     //simple insert
		cout << pp.queryCollect() << endl;
		pp.exec();

		for (int o = 0; o < 100000; o++)
		{
			pp.insert("test", values, fields);
			//cout << pp.queryCollect() << endl;//show query
			pp.exec();
		}

		pp.selectAll("test", "id>1");

		/* creatin' union with sqlStatement that compiles from  'pp.select("log",fields,"id>1");'*/
		dodoStringArray uni;
		uni.push_back(pp.queryCollect());
		uni.push_back(pp.queryCollect());
		pp.subquery(uni);

		dodoStringArray uni_all;
		uni_all.push_back(pp.queryCollect());
		uni_all.push_back(pp.queryCollect());
		pp.subquery(uni_all, CONNECTOR_SUBREQUEST_UNION_ALL);

		pp.order("id desc");
		pp.limit(5);
		pp.setAddSelSt(ACCUMULATOR_ADDREQUEST_SELECT_DISTINCT);
		cout << pp.queryCollect() << endl;        //show query
		pp.exec();

		storage = pp.fetch();        //get result
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
