/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;

using namespace std;

#ifdef POSTGRESQL_EXT
using namespace data::base;

#ifndef DATABASE_WO_XEXEC
void
hook(__xexecCollectedData__ *odata, short int type, void *udata)
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
#ifdef POSTGRESQL_EXT
	try
	{
		__connectionInfo__ info;
		info.db = "test";
		info.host = "localhost";
		info.port = 5432;
		info.user = "postgres";

		postgresql pp(info);

#ifndef DATABASE_WO_XEXEC
		pp.addXExec(XEXEC_ACTION_PREEXEC, ::hook, NULL);
#endif

		try
		{
			pp.exec("DROP TABLE test");
		}
		catch (...)
		{
		}

		pp.exec("CREATE TABLE test (date text NOT NULL, operation text NOT NULL, id integer default NULL, d integer default NULL, b bytea)");

		__tuples__ store;

		dodoStringMap arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";

		dodoStringArray select;
		select.push_back("date");
		select.push_back("operation");

		for (int i = 0; i < 10; i++)
		{
			pp.select("test", select, "id<20 or operation='um'");
			pp.exec();

			store = pp.fetch();

			pp.insert("test", arr);
			pp.exec();

			arr["operation"] = "um";
			pp.update("test", arr);
			pp.exec();

			arr["operation"] = "mu";
		}

		pp.disconnect();
		pp.connect(info);

		pp.select("test", select, "operation='um'");
		pp.exec();

		cout << pp.fetch().rows.size() << endl;

		store = pp.fetch();

		dodoArray<dodoStringArray>::iterator i(store.rows.begin()), j(store.rows.end());
		dodoStringArray::iterator m, n;
		for (; i != j; i++)
		{
			m = i->begin();
			n = i->end();
			for (; m != n; m++)
				cout << *m << "\t";
			cout << endl;
		}

	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << ex.line;
	}
#else

	cout << "No Postresql extension was compiled";

#endif

	return 0;
}
