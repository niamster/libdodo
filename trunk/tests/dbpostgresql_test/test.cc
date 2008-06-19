/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/baseEx.h>
#include <libdodo/dbPostgresql.h>

#include <iostream>

using namespace dodo;

using namespace std;

#ifdef POSTGRESQL_EXT

using namespace db;

#ifndef DB_WO_XEXEC

void
hook(void *odata, short int type, void *udata)
{
	__xexexDbAccumulatorCollectedData *sql = (__xexexDbAccumulatorCollectedData *)odata;

	if (sql->operType == DB_OPERATION_EXEC)
	{
		cout << endl << endl << "request: " << ((sqlConstructor *)(sql->executor))->queryCollect() << endl << endl;
	}
}

#endif

#endif

int main(int argc, char **argv)
{
#ifdef POSTGRESQL_EXT

	postgresql pp;

	try
	{
#ifndef DB_WO_XEXEC

		pp.addPreExec(&hook, NULL);

#endif

		__connectorInfo info;

		info.db = "test";
		info.host = "localhost";
		info.port = 5432;
		info.user = "postgres";

		pp.setDbInfo(info);
		pp.connect();

		try
		{
			pp.deleteTable("test");
			pp.exec();
		}
		catch (...)
		{
		}

		__connectorField fi;
		fi.name = "date";
		fi.type = CONNECTOR_FIELDTYPE_TEXT;

		__connectorTable ti;
		ti.name = "test";
		ti.fields.push_back(fi);

		fi.name = "operation";
		fi.type = CONNECTOR_FIELDTYPE_TEXT;
		ti.fields.push_back(fi);

		fi.name = "id";
		fi.type = CONNECTOR_FIELDTYPE_INTEGER;
		fi.flag = CONNECTOR_FIELDFLAG_NULL;
		ti.fields.push_back(fi);

		pp.createTable(ti);
		cout << pp.queryCollect() << endl;
		pp.exec();

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

			pp.fetch();

			pp.insert("test", arr);
			pp.exec();

			arr["operation"] = "um";
			pp.update("test", arr);
			arr["operation"] = "mu";
			pp.exec();
		}

		pp.select("test", select, "operation='um'"); cout << pp.queryCollect() << endl;
		pp.exec();

		cout << pp.fetch().rows.size() << endl;

		__connectorStorage store = pp.fetch();

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
	catch (baseEx ex)
	{
		cout << (string)ex << ex.line;
	}

#else

	cout << "No Postresql extension was compiled";

#endif

	return 0;
}
