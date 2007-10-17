
#include <mysqlpp.h>

using namespace dodo;
using namespace std;

void 
resolver(mysqlppEx* my)
{
	cout << "!!!!\n";
	cout << endl << my->obj->getErr() << endl;
}

void 
hook(dodoBase *base, 
	void *yep)
{
	sqlBase *sql = dynamic_cast<sqlBase *>(base->getSelf());
	sql->store();
	__collectedData data = sql->getCollectedData();
	data.pre_limNumber = "70";
	data.pre_fieldsNames[0] = std::string((char *)yep);
	//sql->restore();
}

void 
journal(dodoBase *base, 
		void *yep)
{
	mysqlpp *child = dynamic_cast<mysqlpp *>(base);

	switch (child->getQType())
	{
		case INSERT:
		case DELETE:
		case UPDATE:
			__collectedData data = child->getCollectedData();
			data.pre_table.assign(std::string((char *)yep));
			child->exec();
		break;
	}
}

int main(int argc, char **argv)
{	
	//============
	/**
	* testin' mysqlpp!!
	*/
	mysqlpp pp;
	try
	{
		int pos = pp.myAddPreExec(hook,(void *)"id");
		//pp.myAddPostExec(&journal,(void *)"journal");
		/*
		 * int pos = pp.addPreExec(hook,&pp,(void *)"id");
		 * pp.addPostExec(&journal,&pp,(void *)"journal");
		 */
		
		//pp.delPreExec(pos);//removes hook!!
		
		pp.setSqlInfo("test","",3306,"","Dmitrik");
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
		pp.select("ta",fields,"a>1");
		pp.limit(10);
		pp.exec();
		
		
		std::vector<std::string> values;
		values.push_back("20\"05`''-'07-08");
		values.push_back("mu");

		pp.limit(10);
		pp.offset(23);
		pp.offset(3);//change offset value
		pp.unlimit();//no limits!! =)
				
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
		pp.exec();
		pp.insertSelect("test1","test2",fields,values);//insert_select
		pp.exec();
		pp.insert("test1",values,fields);//simple insert
		pp.exec();
		
		for (int o=0;o<1000000;o++)
		{
			pp.insert("log",values,fields);
			cout << pp.queryCollect() << endl;//show query
			pp.exec();
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
		pp.exec();
		
		__sqlStorage storage = pp.fetch();//get result

	}
	catch(mysqlppEx ex)
	{
		cout << "MYexception was in: " << ex.reason << " in " << ex.file.c_str() << " at " << ex.line << endl;
		ex.setResolve(MYSQL_QUERY,resolver);
		ex.resolve();
	}
	catch(sqlBaseEx ex)
	{
		cout << "MYexception was in: " << ex.reason << " in " << ex.file.c_str() << " at " << ex.line << endl;
	}
	
	/* can be:
    catch(baseEx &ex)///!!!!!!!!for base cought must be reference!!!
    {	
		if (ex.getExID() == MYSQL_EX)
		{
		        mysqlppEx *t_ex;
		        t_ex = dynamic_cast<mysqlppEx*> (ex.getSelf());
		        t_ex->setResolve(MYSQL_QUERY,resolver);///for function resolver look upper
		        t_ex->resolve();
		}
       // ...
        else
			cout << "OOOPS, not cought by stringEx\n";
    }
	*/
	return 0;
}
