#include <libdodo/baseEx.h>
#include <libdodo/dbSqlite.h>
#include <libdodo/toolsFilesystem.h>

#include <iostream>

using namespace std;

using namespace dodo;

#ifdef SQLITE_EXT

using namespace db;

#ifndef DBMYSQL_WO_XEXEC

void 
hook(void *odata,
	short int type,
	void *udata)
{
	__xexexDbAccumulatorCollectedData *sql = (__xexexDbAccumulatorCollectedData *)odata;

	if (sql->operType == SQLITE_OPERATION_EXEC && sql->qType == ACCUMULATOR_REQUEST_SELECT)
	{
		cout << endl << endl << "table: " << sql->table << endl << endl;
	}
}

#endif

#endif

int main(int argc, char **argv)
{
#ifdef SQLITE_EXT
	
	sqlite pp;	
	try
	{
#ifndef DBMYSQL_WO_XEXEC

		int pos = pp.addPreExec(hook,(void *)"id");

#endif
		
		tools::filesystem::unlink("test.lite",true);
		
		__connectorInfo info;
		
		info.path = "test.lite";
		
		pp.setDbInfo(info);
		pp.connect();	

		__connectorField fi;
		
		__connectorTable ti;
		ti.name = "test";
		
		fi.name = "date";
		fi.type = CONNECTOR_FIELDTYPE_TEXT;
		ti.fields.push_back(fi);
		
		fi.name = "operation";
		fi.type = CONNECTOR_FIELDTYPE_TEXT;		
		ti.fields.push_back(fi);
		
		fi.name = "id";
		fi.type = CONNECTOR_FIELDTYPE_INTEGER;
		fi.flag = CONNECTOR_FIELDFLAG_AUTO_INCREMENT;
		ti.fields.push_back(fi);		
		
		pp.createTable(ti);
		cout << pp.queryCollect() << endl;
		pp.exec();
		
		dodoStringMap arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		
		dodoArray<dodoString> select;
		select.push_back("date");
		select.push_back("operation");
		
		for (int i=0;i<10;i++)
		{
			pp.select("test",select,"`id`<20 or `operation`='um'");
			cout << pp.queryCollect() << endl;
			pp.exec();
			cout << "Selected: " << pp.rowsCount() << endl;
			cout << "Selected2: " << pp.fetch().rows.size() << endl;
			cout << "Selected3: " << pp.fetch().rows.size() << endl;
			cout << "Fields: " << pp.fieldsCount() << endl;
			cout << "Fields2: " << pp.fetch().fields.size() << endl;
			
			pp.fetch();
			
			pp.insert("test",arr);
			pp.exec();
			
			arr["operation"] = "um";
			pp.update("test",arr);
			arr["operation"] = "mu";
			pp.exec();

			cout << "Updated: " << pp.affectedRowsCount() << endl;
		}

		pp.select("test",select,"`id`<20 or `operation`='um'");
		pp.exec();
		
		cout << pp.fetch().rows.size() << endl;
		
		__connectorStorage store = pp.fetch();
		
		dodoArray<dodoStringArray>::iterator i(store.rows.begin()), j(store.rows.end());
		
		dodoStringArray::iterator m, n;
		
		for (;i!=j;i++)
		{
			m = i->begin();
			n = i->end();
			for (;m!=n;m++)
				cout << *m << "\t";
			cout << endl;	
		}
		
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << endl << endl;
	}	

#else

	cout << "No SQLite exteinsion was compiled!";
	
#endif
	
	return 0;
}
