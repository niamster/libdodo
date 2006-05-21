#include <dbMysql.h>

using namespace dodo;
using namespace std;

void 
hook(void *base,
	xexecObjTypeEnum type,
	void *yep)
{
	dbMysql *sql = (dbMysql *)base;
//	dbBase *sql = (dbBase *)base;

	if (sql->operType == DBMYSQL_OPER_EXEC)
	{
		__collectedData data = sql->collectedData();
		
		cout << endl << endl << "table was" << data.pre_table << endl << endl;
		
		data.pre_table = "ti";
		data.pre_limNumber = "70";
		data.pre_fieldsNames[0] = std::string((char *)yep);
	}
}

void 
journal(void *base, 
		xexecObjTypeEnum type,
		void *yep)
{
	dbMysql *child = (dbMysql *)base;

	switch (child->getQType())
	{
		case INSERT:
		case DELETE:
		case UPDATE:
			__collectedData data = child->collectedData();
			data.pre_table.assign(std::string((char *)yep));
			child->exec();
		break;
	}
}

int main(int argc, char **argv)
{
	/**
	* testin' dbMysql!!
	*/
	dbMysql pp;
	try
	{
		
		int pos = pp.addPreExec(hook,(void *)"id");
		//pp.addPostExec(&journal,(void *)"journal");
		//pp.delPreExec(pos);//removes hook!!
		
		pp.setDbInfo("test","",3306,"","Dmitrik");
		pp.connect();
		
		/* create field
		__fieldInfo fi;
		fi.comment = " !!";
		fi.onDelete = SET_DEFAULT;
		fi.name = "field";
		fi.flag |= AUTO_INCREMENT|KEY;
		fi.type = CHAR;
		fi.length = 10;
		
		pp.createField(fi,"table");
		pp.exec();
		*/
		
		std::vector<std::string> fields;
		
		fields.push_back("date");
		fields.push_back("operation");
		
		/* select*/
		/*pp.select("ta",fields,"a>1");
		pp.limit(10);
		pp.exec();*/
		
		std::vector<std::string> values;
		values.push_back("20\"05`''-'07-08");
		values.push_back("mu");

		pp.limit(10);
		pp.offset(23);
		pp.offset(3);//change offset value
//		pp.unlimit();//no limits!! =)
				
		assocArr arr;
		arr["date"] = "20\"05`''-'07-08";
		arr["operation"] = "mu";
		
		std::vector<assocArr> assA;
		
		assA.push_back(arr);
		arr["date"] = "20\"05`''-'07-08";
		arr["operation"] = "me";
		assA.push_back(arr);
		
		/*additional statement*/
		pp.setAddInsSt(INSERT_IGNORE);//base SQL
		pp.setAddSelSt(SELECT_DISTINCT);//base SQL
		pp.setMyAddSelSt(SELECT_BIG_RESULT);//mySQL features; defined only in this class
				
		
		pp.insert("t",assA);//multiply insert
		cout << pp.queryCollect() << endl;
		//pp.exec();
		
		exit(0);
		
		pp.insertSelect("test1","test2",fields,values);//insert_select
		cout << pp.queryCollect() << endl;
		//pp.exec();
		
		pp.insert("test1",values,fields);//simple insert
		cout << pp.queryCollect() << endl;
		//pp.exec();
		
		//for (int o=0;o<1000000;o++)
		{
			pp.insert("log",values,fields);
			cout << pp.queryCollect() << endl;//show query
			//pp.exec();
		}
		
		pp.select("log",fields,"id>1");

		/* creatin' union with sqlStatement that compiles from  'pp.select("log",fields,"id>1");'*/
		std::vector<std::string> uni;
		uni.push_back(pp.queryCollect());
		uni.push_back(pp.queryCollect());
		pp.subquery(uni);
		
		std::vector<std::string> uni_all;
		uni_all.push_back(pp.queryCollect());
		uni_all.push_back(pp.queryCollect());
		pp.subquery(uni_all,UNION_ALL);
		
		pp.order("id desc");
		pp.limit(5);
		pp.setAddSelSt(SELECT_DISTINCT);
		cout << pp.queryCollect() << endl;//show query
		//pp.exec();
		
		__dbStorage storage = pp.fetch();//get result

	}
    catch(baseEx ex)
    {	
		cout << ex.file << endl << ex.baseErrstr << endl;
    }
	return 0;
}
