#include <baseEx.h>
#include <dbBase.h>
#include <dbPostgresql.h>
#include <dbMysql.h>
#include <dbSqlite.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	dbInterface *pp;	
	
	if (argc == 2)
	{
		if (strcasecmp(argv[1],"postgres") == 0)
			pp = new dbPostgresql;
			
		if (strcasecmp(argv[1],"mysql") == 0)	
			pp = new dbMysql;
			
		if (strcasecmp(argv[1],"sqlite") == 0)	
			pp = new dbSqlite;			
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
		if (strcasecmp(argv[1],"mysql") == 0)
		{
			info.path = "/tmp/mysql.sock";
			info.user = "root";
			info.password = "Dmitrik";
		}
		if (strcasecmp(argv[1],"sqlite") == 0)
		{
			info.path = "test.lite";
		}				
		
		pp->setDbInfo(info);
		pp->connect();	

		((dbBase *)pp)->deleteTable("leg");
		
		try
		{
			pp->exec();
		}
		catch(...)
		{
		}

		__fieldInfo fi;
		fi.name = "date";
		fi.type = FIELDTYPE_TEXT;
		
		__tableInfo ti;
		ti.name = "leg";
		ti.fields.push_back(fi);
		
		fi.name = "operation";
		fi.type = FIELDTYPE_TEXT;		
		ti.fields.push_back(fi);
		
		fi.name = "id";
		fi.type = FIELDTYPE_INTEGER;
		fi.flag = FIELDPROP_NULL;
		ti.fields.push_back(fi);		
		
		((dbBase *)pp)->createTable(ti);
		pp->exec();
		
		map<string,string> arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		
		vector<string> select;
		select.push_back("date");
		select.push_back("operation");
		
		for (int i=0;i<10;i++)
		{
			pp->select("leg",select,"id<20 or operation='um'");
			pp->exec();
			
			pp->fetch();
			
			pp->insert("leg",arr);
			pp->exec();
			
			arr["operation"] = "um";
			pp->update("leg",arr);
			arr["operation"] = "mu";
			pp->exec();
		}
		
		pp->select("leg",select,"operation='um'");
		pp->exec();
		
		__dbStorage store = pp->fetch();
		
		cout << store.rows.size() << endl;
		
		std::vector<stringArr>::iterator i(store.rows.begin()), j(store.rows.end());
		
		stringArr::iterator m, n;
		
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
		cout << ex << ex.line;
	}	
	return 0;
}
