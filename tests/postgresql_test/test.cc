#include <dbPostgresql.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	dbPostgresql pp;	
	
	try
	{
		__dbInfo info;
		
		info.db = "test";
		info.host = "localhost";
		info.port = 5432;
		info.user = "postgres";
		
		pp.setDbInfo(info);
		pp.connect();	

		pp.deleteTable("leg");
		pp.exec();

		__fieldInfo fi;
		fi.name = "date";
		fi.type = TEXT;
		
		__tableInfo ti;
		ti.name = "leg";
		ti.fields.push_back(fi);
		
		fi.name = "operation";
		fi.type = TEXT;		
		ti.fields.push_back(fi);
		
		fi.name = "id";
		fi.type = INTEGER;
		fi.flag = _NULL;
		ti.fields.push_back(fi);		
		
		pp.createTable(ti);
		cout << pp.queryCollect() << endl;
		pp.exec();
		
		map<string,string> arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		
		vector<string> select;
		select.push_back("date");
		select.push_back("operation");
		
		for (int i=0;i<10;i++)
		{
			pp.select("leg",select,"id<20 or operation='um'");
			pp.exec();
			
			pp.fetch();
			
			pp.insert("leg",arr);
			pp.exec();
			
			arr["operation"] = "um";
			pp.update("leg",arr);
			arr["operation"] = "mu";
			pp.exec();
		}
		
		pp.select("leg",select,"operation='um'");cout << pp.queryCollect() << endl;
		pp.exec();
		
		cout << pp.fetch().rows.size() << endl;
		
		__dbStorage store = pp.fetch();
		
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
