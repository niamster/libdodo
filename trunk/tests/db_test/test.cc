#include <libdodo/baseEx.h>
#include <libdodo/dbConnector.h>
#include <libdodo/dbSqlConstructor.h>
#include <libdodo/dbPostgresql.h>
#include <libdodo/dbMysql.h>
#include <libdodo/dbSqlite.h>
#include <libdodo/toolsFilesystem.h>

#include <iostream>

using namespace dodo;
using namespace db;

using namespace std;

int main(int argc, char **argv)
{	
	connector *pp;	
	
	if (argc == 2)
	{
		if (strcasecmp(argv[1],"postgresql") == 0)
#ifdef POSTGRESQL_EXT
				pp = new postgresql;
#else
				return 1;
#endif
			
		else if (strcasecmp(argv[1],"mysql") == 0)
#ifdef MYSQL_EXT
				pp = new mysql;
#else
				return 1;
#endif
			
		else if (strcasecmp(argv[1],"sqlite") == 0)
#ifdef SQLITE_EXT	
				pp = new sqlite;
#else
				return 1;
#endif
						
		else
			return 1;
	}
	else
		return 1;
	
	try
	{
		__connectorInfo info;
		
		info.db = "test";
		info.host = "localhost";
		
		if (strcasecmp(argv[1],"postgres") == 0)
		{
			info.port = 5432;
			info.user = "postgres";
		}
		else if (strcasecmp(argv[1],"mysql") == 0)
		{
			info.path = "/var/run/mysqld/mysqld.sock";
			info.user = "root";
			info.password = "password";
		}
		else if (strcasecmp(argv[1],"sqlite") == 0)
		{
			info.path = "test.lite";
		}				
		
		pp->setDbInfo(info);
		pp->connect();	

		pp->deleteTable("test");
		
		try
		{
			pp->exec();
		}
		catch(...)
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
		
		fi.name = "d";
		fi.type = CONNECTOR_FIELDTYPE_INTEGER;
		fi.flag = CONNECTOR_FIELDFLAG_NULL;
		ti.fields.push_back(fi);		
		
		fi.name = "b";
		fi.type = CONNECTOR_FIELDTYPE_LONGBLOB;
		fi.flag = CONNECTOR_FIELDFLAG_NULL;
		ti.fields.push_back(fi);		
		
		pp->createTable(ti);
		pp->exec();
		
		dodoStringMap arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		arr["d"] = "1";
		
		dodoArray<dodoString> select;
		select.push_back("date");
		select.push_back("operation");
		select.push_back("b");
		
		for (int i=0;i<10;i++)
		{
			pp->select("test",select,"id<20 or operation='um'");
			pp->exec();
			
			pp->fetch();
			
			pp->insert("test",arr);
			cout << ((sqlConstructor *)pp)->queryCollect() << endl;
			pp->exec();
			
			arr["d"] = "d+1";
			arr["operation"] = "um";
			pp->update("test",arr);
			cout << ((sqlConstructor *)pp)->queryCollect() << endl;
			arr["operation"] = "mu";
			arr["d"] = "1";
			pp->exec();
		}
		
		pp->select("test",select,"operation='um'");
		pp->exec();
		
		__connectorStorage store = pp->fetch();
		
		cout << store.rows.size() << endl;
		
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

		arr.clear();

		toolsFilesystem::unlink("test.1");
		toolsFilesystem::unlink("test.2");

		dodoString dt = toolsFilesystem::getFileContents("test");
		toolsFilesystem::writeToFile("test.1", dt);

		if (strcasecmp(argv[1],"sqlite") == 0 || strcasecmp(argv[1],"postgres") == 0)
			arr["b"] = "$1";
		else
			arr["b"] = dt;

		if (strcasecmp(argv[1],"sqlite") == 0 || strcasecmp(argv[1],"postgres") == 0)
		{
			dodoStringArray blobs;
			blobs.push_back(dt);

			if (strcasecmp(argv[1],"sqlite") == 0)
#ifdef SQLITE_EXT
					((sqlite *)pp)->setBLOBValues(blobs);
#else
					;
#endif
			else
			{
				if (strcasecmp(argv[1],"postgres") == 0)
#ifdef POSTGRESQL_EXT
						((postgresql *)pp)->setBLOBValues(blobs);
#else
						;
#endif
			}
                	
			((sqlConstructor *)pp)->preventFraming = true;
            		((sqlConstructor *)pp)->preventEscaping = true;
		
			arr["date"] = "'2005-07-08'";
			arr["operation"] = "'ma'";
		}
		else
		{
			arr["date"] = "2005-07-08";
			arr["operation"] = "ma";
		}

		pp->insert("test",arr);

		if (strcasecmp(argv[1],"sqlite") == 0)
#ifdef SQLITE_EXT
			addFlag(((sqlite *)pp)->hint, SQLITE_HINT_BLOB);
#else
			;
#endif
		else 
			if (strcasecmp(argv[1],"postgres") == 0)
#ifdef POSTGRESQL_EXT
				addFlag(((postgresql *)pp)->hint, POSTGRESQL_HINT_BLOB);
#else
				;
#endif
		
		pp->exec();

		pp->select("test",select,"operation='ma'");
		if (strcasecmp(argv[1],"postgres") == 0)
#ifdef SQLITE_EXT
			addFlag(((sqlite *)pp)->hint, SQLITE_HINT_BLOB);
#else
			;
#endif

		pp->exec();

		store = pp->fetch();

		if (store.fields.size() == 3 && store.rows.size() > 0)
			toolsFilesystem::writeToFile("test.2",(*store.rows.begin())[2]);	
	}
	catch(baseEx ex)
	{
		cout << (string)ex << endl << ex.message << endl << ex.line << endl << endl;
	}

	delete pp;

	return 0;
}
