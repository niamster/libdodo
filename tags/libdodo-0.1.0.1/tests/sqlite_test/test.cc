#include <dbSqlite.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	dbSqlite pp;	
	try
	{
		
		__dbInfo info;
		info.path = "test.lite";
		
		pp.setDbInfo(info);
		pp.connect();	

/*		__fieldInfo fi;
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
		fi.flag |= AUTO_INCREMENT|KEY;
		ti.fields.push_back(fi);		
		
		pp.createTable(ti);
		cout << pp.queryCollect();
		pp.exec();*/

		
		map<string,string> arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		
		vector<string> select;
		select.push_back("date");
		select.push_back("operation");
		
		/*for (int i=0;i<1000;i++)
		{
			pp.select("leg",select,"`id`<20 or `operation`='mu'");
			pp.exec();
			
			pp.fetch();
			
			pp.insert("leg",arr);
			pp.exec();
			
			arr["operation"] = "um";
			pp.update("leg",arr);
			arr["operation"] = "mu";
			pp.exec();
		}*/

		pp.select("leg",select,"`id`<20 or `operation`='mu'");
		pp.exec();
		
		//cout << pp.fetch().rows.size() << endl;
		
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
		cout << ex;
	}	
	return 0;
}
