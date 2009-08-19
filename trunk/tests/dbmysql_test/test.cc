/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;

using namespace std;

#ifdef MYSQL_EXT
using namespace data::base;

#ifndef DATABASE_WO_XEXEC
void
hook(__xexecCollectedData__ *odata,
	 short int type,
	 void *udata)
{
	__xexecDataBaseAccumulatorCollectedData__ *sql = (__xexecDataBaseAccumulatorCollectedData__ *)odata;

	if (sql->operType == DATABASE_OPERATION_EXEC)
	{
		cout << endl << endl << "request: " << dynamic_cast<sql::constructor *>(sql->executor)->queryCollect() << endl << endl;
	}
}
#endif
#endif

int main(int argc, char **argv)
{
	long now = tools::time::now();

#ifdef MYSQL_EXT
	try
	{
		mysql pp(__connectionInfo__("test", "localhost", "root", "password", "", 3306));

#ifndef DATABASE_WO_XEXEC
		pp.addXExec(XEXEC_ACTION_PREEXEC, ::hook, NULL);
#endif

		try
		{
			pp.exec("DROP TABLE test");
			pp.exec("DROP TABLE test1");
		}
		catch (...)
		{
		}

		pp.exec("CREATE TABLE test (id int(11) NOT NULL auto_increment, d int(11), dot text NOT NULL, operation text NOT NULL, PRIMARY KEY  (id))");
		pp.exec("CREATE TABLE test1 (id int(11) NOT NULL auto_increment, d int(11), dot text NOT NULL, operation text NOT NULL, PRIMARY KEY  (id))");

		pp.getFieldsTypes("test");
		pp.getFieldsTypes("test1");

		dodoStringArray fields;
		__tuples__ storage;

		pp.select("test");
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

		dodoStringMap arr;
		dodoArray<dodoStringMap> assA;
		arr["dot"] = "20\"05`''-'07-08";
		arr["operation"] = "m\nu";
		assA.push_back(arr);
		arr["dot"] = "20\"05`''-'07-08";
		arr["operation"] = "n\nu";
		assA.push_back(arr);

		pp.insert("test", assA);
		pp.exec();

		arr.clear();

		pp.disconnect();
		pp.connect(__connectionInfo__("test", "localhost", "root", "password", "", 3306));

		arr["d"] = "100000";
		pp.update("test", arr);
		pp.exec();

		fields.clear();
		fields.push_back("dot");
		fields.push_back("operation");

		pp.insert("test", values, fields);
		pp.exec();

#ifndef DATABASE_WO_XEXEC
		pp.disableXExecs = true;
#endif

		for (int o = 0; o < 100000; o++)
		{
			pp.insert("test", values, fields);
			pp.exec();
		}

#ifndef DATABASE_WO_XEXEC
		pp.disableXExecs = false;
#endif

		dodoStringArray uni;

		pp.select("test", dodoStringArray(), "id>1");
		pp.limit(10);
		pp.offset(23);

		dodoString sub = pp.queryCollect();
		uni.push_back(sub);
		uni.push_back(sub);
		pp.subquery(uni);
		sub = pp.queryCollect();

		pp.select("test", dodoStringArray(), "id<100");

		uni.clear();
		uni.push_back(sub);
		uni.push_back(pp.queryCollect());
		pp.subquery(uni, SUBREQUEST_UNION_ALL);

		pp.order("id desc");
		pp.limit(5);
		pp.exec();

		pp.select("test");
		pp.limit(10);
		pp.exec();

		storage = pp.fetch();

		dodoArray<dodoStringArray>::iterator o(storage.rows.begin()), p(storage.rows.end());
		dodoStringArray::iterator m, n;
		for (; o != p; o++)
		{
			m = o->begin();
			n = o->end();
			for (; m != n; m++)
				cout << *m << "\t";
			cout << endl;
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}
#else
	cout << "No MySQL extension was compiled!";
#endif

	now = tools::time::now() - now;

	cout << "SpentTime: " << now << endl;

	return 0;
}
