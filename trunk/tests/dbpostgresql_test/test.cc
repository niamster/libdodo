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
hook(xexec::__collected_data__ *odata, short int type, void *udata)
{
	accumulator::__collected_data__ *sql = (accumulator::__collected_data__ *)odata;

	if (sql->operType == data::base::connector::OPERATION_EXEC)
	{
		cout << endl << endl << "request: " << dynamic_cast<sql::constructor *>(sql->executor)->construct() << endl << endl;
	}
}
#endif
#endif

int main(int argc, char **argv)
{
#ifdef POSTGRESQL_EXT
	try
	{
		__connection__ info;
		info.db = "test";
		info.host = "localhost";
		info.port = 5432;
		info.user = "postgres";

		postgresql db(info);

#ifndef DATABASE_WO_XEXEC
		db.addXExec(xexec::ACTION_PREEXEC, ::hook, NULL);
#endif

		try
		{
			db.exec("DROP TABLE test");
		}
		catch (...)
		{
		}

		db.exec("CREATE TABLE test (t0 text NOT NULL, t1 text NOT NULL, id integer default NULL, i integer default NULL, b bytea)");

		__tuples__ store;

		dodoStringMap array;
		array["t0"] = "xyz";
		array["t1"] = "abc";

		dodoStringArray select;
		select.push_back("t0");
		select.push_back("t1");

		for (int i = 0; i < 10; i++)
		{
			db.select("test", select, "id<20 or t1='abc'");
			db.exec();

			store = db.fetch();

			db.insert("test", array);
			db.exec();

			array["t1"] = "def";
			db.update("test", array);
			db.exec();

			array["t1"] = "abc";
		}

		db.disconnect();
		db.connect(info);

		db.select("test", select, "t1='def'");
		db.exec();

		cout << db.fetch().rows.size() << endl;

		store = db.fetch();

		dodoArray<dodoStringArray>::iterator i(store.rows.begin()), j(store.rows.end());
		dodoStringArray::iterator m, n;
		for (; i != j; i++)
		{
			m = i->begin();
			n = i->end();
			for (; m != n; m++)
				cout << "[" << *m << "]\t";
			cout << endl;
		}

	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << ex.line;
	}
#else

	cout << "No Postresql extension was compiled";

#endif

	return 0;
}
