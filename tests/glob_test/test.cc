
#include <mysqlinterface.h>

using namespace dodo;
using namespace std;

void 
resolver(mysqlppEx* my)
{
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
	sql->restore();
}

void 
journal(dodoBase *base, 
		void *yep)
{
	mysqlpp *child = reinterpret_cast<mysqlpp *>(base);

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
		int pos = pp.addPreExec(hook,&pp,(void *)"id");
		pp.addPostExec(&journal,&pp,(void *)"journal");
		
		pp.delPreExec(pos);
		
		pp.setSqlInfo("test","",3306,"","Dmitrik");
		pp.connect();
		
		__fieldInfo fi;
		fi.comment = " !!";
		fi.onDelete = SET_DEFAULT;
		fi.name = "field";
		fi.flag |= AUTO_INCREMENT|KEY;
		fi.type = CHAR;
		fi.length = 10;
		
		pp.createField(fi,"table");
		pp.exec();
		
		
		std::vector<std::string> fields;
		
		fields.push_back("date");
		fields.push_back("operation");
		std::vector<std::string> values;
		values.push_back("20\"05`''-'07-08");
		values.push_back("mu");

		pp.limit(10);
		pp.offset(23);
		pp.offset(3);
		pp.unlimit();
		
		std::vector<stringArr> aa;
		aa.push_back(values);
		aa.push_back(values);
		
		assocArr arr;
		arr["date"] = "20\"05`''-'07-08";
		arr["operation"] = "mu";
		std::vector<assocArr> assA;
		assA.push_back(arr);
		arr["date"] = "20\"05`''-'07-08";
		arr["operation"] = "me";
		assA.push_back(arr);
		
		pp.insert("t",assA);
		pp.insertSelect("test1","test2",fields,values);
		//pp.insert("test1",values,fields);
		pp.setAddInsSt(INSERT_IGNORE);
		pp.setAddSelSt(SELECT_DISTINCT);
		pp.setMyAddSelSt(SELECT_BIG_RESULT);
		pp.exec();
		cout << endl << pp.queryCollect() << endl;
		for (int o=0;o<1000000;o++)
		{
			pp.insert("log",values,fields);
			cout << pp.queryCollect() << endl;
			pp.exec();
		}
		pp.select("log",fields,"id>1");

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
		
		__sqlStorage storage = pp.fetch();
		pp.debug(storage);

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
	return 0;
}
