#include <baseEx.h>
#include <dbSqlite.h>
#include <flushDiskTools.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	dbSqlite pp;	
	try
	{
		
		flushDiskTools::unlink("test.lite",true);
		
		__dbInfo info;
		
		info.path = "test.lite";
		
		pp.setDbInfo(info);
		pp.connect();	

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
		fi.flag = FIELDPROP_AUTO_INCREMENT;
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
			pp.select("leg",select,"`id`<20 or `operation`='um'");
			//cout << pp.queryCollect() << endl;
			pp.exec();
			cout << "Selected: " << pp.rowsCount() << endl;
			cout << "Selected2: " << pp.fetch().rows.size() << endl;
			cout << "Selected3: " << pp.fetch().rows.size() << endl;
			cout << "Fields: " << pp.fieldsCount() << endl;
			cout << "Fields2: " << pp.fetch().fields.size() << endl;
			
			pp.fetch();
			
			pp.insert("leg",arr);
			pp.exec();
			
			arr["operation"] = "um";
			pp.update("leg",arr);
			arr["operation"] = "mu";
			pp.exec();

			cout << "Updated: " << pp.affectedRowsCount() << endl;
		}

		pp.select("leg",select,"`id`<20 or `operation`='um'");
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
		cout << ex << "\t" << ex.line << endl << endl;
	}	
	return 0;
}
