#include <libdodo/baseEx.h>
#include <libdodo/dbBase.h>
#include <libdodo/dbPostgresql.h>
#include <libdodo/dbMysql.h>
#include <libdodo/dbSqlite.h>
#include <libdodo/dbSqlBase.h>
#include <libdodo/ioDiskTools.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{	
	db *pp;	
	
	if (argc == 2)
	{
		if (strcasecmp(argv[1],"postgres") == 0)
#ifdef POSTGRESQL_EXT
				pp = new dbPostgresql;
#else
				return 1;
#endif
			
		else if (strcasecmp(argv[1],"mysql") == 0)
#ifdef MYSQL_EXT
				pp = new dbMysql;
#else
				return 1;
#endif
			
		else if (strcasecmp(argv[1],"sqlite") == 0)
#ifdef SQLITE_EXT	
				pp = new dbSqlite;
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
		__dbInfo info;
		
		info.db = "test";
		info.host = "localhost";
		
		if (strcasecmp(argv[1],"postgres") == 0)
		{
			info.port = 5432;
			info.user = "postgres";
		}
		else if (strcasecmp(argv[1],"mysql") == 0)
		{
			info.path = "/tmp/mysql.sock";
			info.user = "root";
		}
		else if (strcasecmp(argv[1],"sqlite") == 0)
		{
			info.path = "test.lite";
		}				
		
		pp->setDbInfo(info);
		pp->connect();	

		((dbBase *)pp)->deleteTable("test");
		
		try
		{
			pp->exec();
		}
		catch(...)
		{
		}

		__fieldInfo fi;
		fi.name = "date";
		fi.type = DBBASE_FIELDTYPE_TEXT;
		
		__tableInfo ti;
		ti.name = "test";
		ti.fields.push_back(fi);
		
		fi.name = "operation";
		fi.type = DBBASE_FIELDTYPE_TEXT;		
		ti.fields.push_back(fi);
		
		fi.name = "id";
		fi.type = DBBASE_FIELDTYPE_INTEGER;
		fi.flag = DBBASE_FIELDFLAG_NULL;
		ti.fields.push_back(fi);		
		
		fi.name = "d";
		fi.type = DBBASE_FIELDTYPE_INTEGER;
		fi.flag = DBBASE_FIELDFLAG_NULL;
		ti.fields.push_back(fi);		
		
		fi.name = "b";
		fi.type = DBBASE_FIELDTYPE_LONGBLOB;
		fi.flag = DBBASE_FIELDFLAG_NULL;
		ti.fields.push_back(fi);		
		
		((dbBase *)pp)->createTable(ti);
		pp->exec();
		
		map<dodoString,dodoString> arr;
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
			cout << ((dbSqlBase *)pp)->queryCollect() << endl;
			pp->exec();
			
			arr["d"] = "d+1";
			arr["operation"] = "um";
			pp->update("test",arr);
			cout << ((dbSqlBase *)pp)->queryCollect() << endl;
			arr["operation"] = "mu";
			arr["d"] = "1";
			pp->exec();
		}
		
		pp->select("test",select,"operation='um'");
		pp->exec();
		
		__dbStorage store = pp->fetch();
		
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

		ioDiskTools::unlink("test.1");
		ioDiskTools::unlink("test.2");

		std::string dt = ioDiskTools::getFileContents("test");
		ioDiskTools::append("test.1", dt);

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
					((dbSqlite *)pp)->setBLOBValues(blobs);
#else
					;
#endif
			else
			{
				if (strcasecmp(argv[1],"postgres") == 0)
#ifdef POSTGRESQL_EXT
						((dbPostgresql *)pp)->setBLOBValues(blobs);
#else
						;
#endif
			}
                	
			((dbSqlBase *)pp)->preventFraming = true;
            		((dbSqlBase *)pp)->preventEscaping = true;
		
			arr["date"] = "'2005-07-08'";
			arr["operation"] = "'ma'";
		}
		else
		{
			arr["date"] = "2005-07-08";
			arr["operation"] = "ma";
		}

		pp->insert("test",arr);

		if (strcasecmp(argv[1],"sqlite") == 0 || strcasecmp(argv[1],"postgres") == 0)
			pp->exec("dodo:hint:db:blob");
		else
			pp->exec();

		pp->select("test",select,"operation='ma'");
		pp->exec();

		store = pp->fetch();

		if (store.fields.size() == 3 && store.rows.size() > 0)
			ioDiskTools::append("test.2",(*store.rows.begin())[2]);	
	}
	catch(baseEx ex)
	{
		cout << (string)ex << endl << ex.message << endl << ex.line << endl << endl;
	}

	delete pp;

	return 0;
}
