#include <libdodo/baseEx.h>
#include <libdodo/dbSqlite.h>
#include <libdodo/toolsFilesystem.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
#ifdef SQLITE_EXT
	
		dbSqlite pp;	
		try
		{
			
			toolsFilesystem::unlink("test.lite",true);
			
			__dbInfo info;
			
			info.path = "test.lite";
			
			pp.setDbInfo(info);
			pp.connect();	
	
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
			fi.flag = DBBASE_FIELDFLAG_AUTO_INCREMENT;
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
				//cout << pp.queryCollect() << endl;
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
			
			__dbStorage store = pp.fetch();
			
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
