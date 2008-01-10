#include <libdodo/baseEx.h>
#include <libdodo/dbMysql.h>
#include <libdodo/timeTools.h>

#include <iostream>

using namespace dodo;
using namespace std;

#ifdef MYSQL_EXT

	void 
	hook(void *base,
		short int type,
		void *yep)
	{
		__xexexDbBaseCollectedData *sql = (__xexexDbBaseCollectedData *)base;
	
		if (sql->operType == DBMYSQL_OPERATION_EXEC && sql->qType == DBBASE_REQUEST_SELECT)
		{
			cout << endl << endl << "table was " << sql->pre_table << endl << endl;
			
			sql->pre_limNumber = "70";
			sql->pre_fieldsNames[0] = std::string((char *)yep);
		}
	}

#endif

int main(int argc, char **argv)
{
	long now = timeTools::now();

#ifdef MYSQL_EXT

		dbMysql pp;
		try
		{
			
			int pos = pp.addPreExec(hook,(void *)"id");
			
			pp.setDbInfo("test","",3306,"root", "");
			pp.connect();
	
			__tableInfo ti;
            ti.name = "test";

            __fieldInfo fi;

            fi.name = "id";
            fi.type = DBBASE_FIELDTYPE_INTEGER;
            fi.flag = DBBASE_FIELDFLAG_NULL | DBBASE_FIELDFLAG_AUTO_INCREMENT;
            ti.fields.push_back(fi);
			
			fi.name = "dot";
			fi.flag = 0;
			fi.type = DBBASE_FIELDTYPE_TEXT;
			ti.fields.push_back(fi);
	
			fi.name = "operation";
			fi.type = DBBASE_FIELDTYPE_TEXT;
			ti.fields.push_back(fi);
	
			try
			{
				pp.deleteTable("test");
				pp.exec();
			}
			catch(...)
			{
			}
	
			pp.createTable(ti);
			cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
			pp.exec();
	
			pp.createIndex("test","id","id");
			cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
			pp.exec();
	
			/*create field*/
			fi.name = "fi";
			fi.type = DBBASE_FIELDTYPE_CHAR;
			fi.length = 10;
			
			pp.createField(fi,"test");
			cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
			pp.exec();
			
			
			dodoStringArray fields;
			
			fields.push_back("dot");
			fields.push_back("operation");
			
			/* select*/
			pp.select("test",fields);
			pp.limit(10);
			pp.exec();
			
			dodoStringArray values;
			values.push_back("20\"05`''-'07-08");
			values.push_back("mu");
	
			pp.limit(10);
			pp.offset(23);
			pp.offset(3);
					
			dodoStringMap arr;
			dodoArray<dodoStringMap> assA;
			arr["dot"] = "20\"05`''-'07-08";
			arr["operation"] = "m\nu";
			assA.push_back(arr);
			arr["dot"] = "20\"05`''-'07-08";
			arr["operation"] = "n\nu";
			assA.push_back(arr);
			
			/*additional statement*/
			pp.setAddInsSt(DBBASE_REQUEST_INSERT_IGNORE);//base SQL
			pp.setAddSelSt(DBBASE_REQUEST_SELECT_DISTINCT);//base SQL
			pp.setMyAddSelSt(DBMYSQL_REQUEST_SELECT_BIG_RESULT);//mySQL features; defined only in this class
					
			pp.insert("test",assA);//multiply insert
			cout << pp.queryCollect() << endl;
			pp.exec();
			
			
			pp.insert("test",values,fields);//simple insert
			cout << pp.queryCollect() << endl;
			pp.exec();
			
			for (int o=0;o<100000;o++)
			{
				pp.insert("test",values,fields);
				//cout << pp.queryCollect() << endl;//show query
				pp.exec();
			}
			
			pp.select("test",fields,"id>1");
	
			/* creatin' union with sqlStatement that compiles from  'pp.select("log",fields,"id>1");'*/
			dodoStringArray uni;
			uni.push_back(pp.queryCollect());
			uni.push_back(pp.queryCollect());
			pp.subquery(uni);
			
			dodoStringArray uni_all;
			uni_all.push_back(pp.queryCollect());
			uni_all.push_back(pp.queryCollect());
			pp.subquery(uni_all,DBBASE_REQUEST_UNION_ALL);
			
			pp.order("id desc");
			pp.limit(5);
			pp.setAddSelSt(DBBASE_REQUEST_SELECT_DISTINCT);
			cout << pp.queryCollect() << endl;//show query
			pp.exec();
			
			__dbStorage storage = pp.fetch();//get result
	
		}
	    catch(baseEx ex)
	    {	
			cout << ex.file << endl << ex.baseErrstr << endl << ex.line << endl;
	    }
    
#else
    
    	cout << "No MySQL extension was compiled!";
    	
#endif

    	now = timeTools::now() - now;

	cout << "SpentTime: " << now;
    
	return 0;
}
