#include <baseEx.h>
#include <dbMysql.h>
#include <timeTools.h>
#include <iostream>

using namespace dodo;
using namespace std;

#ifdef MYSQL_EXT

	void 
	hook(void *base,
		short int type,
		void *yep)
	{
		dbMysql *sql = (dbMysql *)base;
	
		if (sql->operType == DBMYSQL_OPER_EXEC && sql->getQType() == DBREQUEST_SELECT)
		{
			__collectedData data = sql->collectedData();
			
			cout << endl << endl << "table was " << data.pre_table << endl << endl;
			
			data.pre_limNumber = "70";
			data.pre_fieldsNames[0] = std::string((char *)yep);
		}
	}
	
	void 
	journal(void *base, 
			short int type,
			void *yep)
	{
		dbMysql *child = (dbMysql *)base;
	
		switch (child->getQType())
		{
			case DBREQUEST_INSERT:
			case DBREQUEST_DELETE:
			case DBREQUEST_UPDATE:
				__collectedData data = child->collectedData();
				data.pre_table.assign(std::string((char *)yep));
				child->exec();
			break;
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
			//pp.addPostExec(&journal,(void *)"journal");
			//pp.delPreExec(pos);//removes hook!!
			
			pp.setDbInfo("test","",3306,"","Dmitrik");
			pp.connect();
	
			__tableInfo ti;
	                ti.name = "tab";
	
	                __fieldInfo fi;
	
	                fi.name = "id";
	                fi.type = FIELDTYPE_INTEGER;
	                fi.flag = FIELDPROP_NULL | FIELDPROP_AUTO_INCREMENT;
	                ti.fields.push_back(fi);
			
			fi.name = "dote";
	                fi.flag = 0;
			fi.type = FIELDTYPE_TEXT;
	                ti.fields.push_back(fi);
	
			fi.name = "operation";
			fi.type = FIELDTYPE_TEXT;
			ti.fields.push_back(fi);
	
			try
			{
	                	pp.deleteTable("tab");
				pp.exec();
			}
			catch(...)
			{
			}
	
			pp.createTable(ti);
			cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
			pp.exec();
	
			pp.createIndex("tab","id","id");
			cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
			pp.exec();
	
			/*create field*/
			fi.name = "fi";
			fi.type = FIELDTYPE_CHAR;
			fi.length = 10;
			
			pp.createField(fi,"tab");
			cout << endl << endl << "Query: " << pp.queryCollect() << endl << endl;
			pp.exec();
			
			
			dodoStringArr fields;
			
			fields.push_back("dote");
			fields.push_back("operation");
			
			/* select*/
			pp.select("tab",fields);
			pp.limit(10);
			pp.exec();
			
			dodoStringArr values;
			values.push_back("20\"05`''-'07-08");
			values.push_back("mu");
	
			pp.limit(10);
			pp.offset(23);
			pp.offset(3);//change offset value
	//		pp.unlimit();//no limits!! =)
					
			dodoAssocArr arr;
			arr["dote"] = "20\"05`''-'07-08";
			arr["operation"] = "m\nu";
			
			dodoArray<dodoAssocArr> assA;
	
	///
	        FILE *file = fopen("/bin/ls","r");
	
	        char temp[100];
	
	        fread(temp,100,1,file);
	
	        string str;
	        str.assign(temp,100);
	///
			
			assA.push_back(arr);
			arr["dote"] = "20\"05`''-'07-08";
			arr["operation"] = str;
			assA.push_back(arr);
			
			/*additional statement*/
			pp.setAddInsSt(DBREQUEST_INSERT_IGNORE);//base SQL
			pp.setAddSelSt(DBREQUEST_SELECT_DISTINCT);//base SQL
			pp.setMyAddSelSt(DBREQUEST_SELECT_BIG_RESULT);//mySQL features; defined only in this class
					
			
			pp.insert("tab",assA);//multiply insert
			cout << pp.queryCollect() << endl;
			pp.exec();
			
			pp.insertSelect("test1","test2",fields,values);//insert_select
			cout << pp.queryCollect() << endl;
			//pp.exec();
			
			pp.insert("tab",values,fields);//simple insert
			cout << pp.queryCollect() << endl;
			pp.exec();
			
			for (int o=0;o<10000;o++)
			{
				pp.insert("tab",values,fields);
				//cout << pp.queryCollect() << endl;//show query
				pp.exec();
			}
			
			pp.select("tab",fields,"id>1");
	
			/* creatin' union with sqlStatement that compiles from  'pp.select("log",fields,"id>1");'*/
			dodoStringArr uni;
			uni.push_back(pp.queryCollect());
			uni.push_back(pp.queryCollect());
			pp.subquery(uni);
			
			dodoStringArr uni_all;
			uni_all.push_back(pp.queryCollect());
			uni_all.push_back(pp.queryCollect());
			pp.subquery(uni_all,DBREQUEST_UNION_ALL);
			
			pp.order("id desc");
			pp.limit(5);
			pp.setAddSelSt(DBREQUEST_SELECT_DISTINCT);
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
