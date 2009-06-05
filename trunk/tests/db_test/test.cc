	/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace data::base;

using namespace std;

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

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		cout << "usage: " << argv[0] << " [postgresql|mysql|sqlite]" << endl;

		return 1;
	}

	try
	{
		__connectionInfo__ info;
		info.db = "test";
		if (strcasecmp(argv[1], "postgresql") == 0)
		{
			info.host = "127.0.0.1";
			info.port = 5432;
			info.user = "postgres";
		}
		else if (strcasecmp(argv[1], "mysql") == 0)
		{
			info.path = "/var/run/mysqld/mysqld.sock";
			info.user = "root";
			info.password = "password";
		}
		else if (strcasecmp(argv[1], "sqlite") == 0)
		{
			info.path = "test.lite";
		}
		
		connector *pp;

		///parse command line arguments to figure out what db to use
		if (strcasecmp(argv[1], "postgresql") == 0)
#ifdef POSTGRESQL_EXT
			pp = new postgresql(info);
#else
			return 1;
#endif

		else if (strcasecmp(argv[1], "mysql") == 0)
#ifdef MYSQL_EXT
			pp = new mysql(info);
#else
			return 1;
#endif

		else if (strcasecmp(argv[1], "sqlite") == 0)
#ifdef SQLITE3_EXT
			pp = new sqlite(info);
#else
			return 1;
#endif

		else
			return 1;

#ifndef DATABASE_WO_XEXEC

		int pos = pp->addPreExec(hook, (void *)"id");

#endif

		///define session charset
		if (strcasecmp(argv[1], "postgresql") == 0)
		{
			#ifdef POSTGRESQL_EXT

				((postgresql *)pp)->setCharset("UTF-8");

				cout << "Encoding: " << ((postgresql *)pp)->getCharset() << endl;

			#endif
		}
		else if (strcasecmp(argv[1], "mysql") == 0)
		{
			#ifdef MYSQL_EXT

				((mysql *)pp)->setCharset("UTF-8");

				cout << "Encoding: " << ((mysql *)pp)->getCharset() << endl;

			#endif
		}

		try
		{
			pp->exec("DROP TABLE test");
		}
		catch (dodo::exception::basic &ex)
		{
			cout << (dodoString)ex << endl << ex.file << endl << ex.message << endl << ex.line << endl << endl;
		}

		if (strcasecmp(argv[1], "postgresql") == 0)
			pp->exec("CREATE TABLE test (date text NOT NULL, operation text NOT NULL, id integer default NULL, d integer default NULL, b bytea)");
		else
			pp->exec("CREATE TABLE test (date text NOT NULL, operation text NOT NULL, id integer default NULL, d integer default NULL, b longblob)");

		try
		{
			((sql::constructor *)pp)->getFieldsTypes("test");
		}
		catch(dodo::exception::basic &ex)
		{
			cout << (dodoString)ex << endl << ex.file << endl << ex.message << endl << ex.line << endl << endl;

			((sql::constructor *)pp)->setFieldType("test", "date", sql::FIELDTYPE_TEXT);
			((sql::constructor *)pp)->setFieldType("test", "operation", sql::FIELDTYPE_TEXT);
			((sql::constructor *)pp)->setFieldType("test", "id", sql::FIELDTYPE_NUMERIC);
			((sql::constructor *)pp)->setFieldType("test", "d", sql::FIELDTYPE_NUMERIC);
			((sql::constructor *)pp)->setFieldType("test", "b", sql::FIELDTYPE_BINARY);
		}

		dodoStringMap arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		arr["d"] = "1";

		dodoArray<dodoString> select;
		select.push_back("date");
		select.push_back("operation");
		select.push_back("b");

		__tuples__ store;

		for (int i = 0; i < 10; i++)
		{
			pp->select("test", select, "id<20 or operation='um'");
			pp->exec();

			store = pp->fetch();

			pp->insert("test", arr);
			pp->exec();

			arr["d"] = "d+1";
			arr["operation"] = "um";
			pp->update("test", arr);
			pp->exec();

			arr["operation"] = "mu";
			arr["d"] = "1";
		}

		pp->select("test", select, "operation='um'");
		pp->exec();

		store = pp->fetch();

		cout << store.rows.size() << endl;

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

#ifndef DATABASE_WO_XEXEC

		pp->disablePreExec(pos);

#endif

		tools::filesystem::unlink("test.db");
		dodoString dt = tools::filesystem::getFileContents("test");

		arr.clear();

		arr["b"] = dt;
		arr["date"] = "2005-07-08";
		arr["operation"] = "ma";

		pp->insert("test", arr);
		pp->exec();
		
		pp->disconnect();
		pp->connect(info);

		pp->select("test", select, "operation='ma'");
		pp->exec();

		store = pp->fetch();

		///put fetched binary data to file
		if (store.fields.size() == 3 && store.rows.size() > 0)
			tools::filesystem::writeToFile("test.db", (*store.rows.begin())[2]);
		
		delete pp;
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << endl << ex.file << endl << ex.message << endl << ex.line << endl << endl;
	}

	return 0;
}
