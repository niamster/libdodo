#include <mysqlinterface.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		mysqlpp pp;	
		pp.setSqlInfo(string("test"),string(""),3306,string(""),string("Dmitrik"));
		pp.connect();		
		
		map<string,string> arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		
		vector<string> select;
		select.push_back("date");
		select.push_back("operation");
		
		for (int i=0;i<1000;i++)
		{
			pp.select("log",select,"`id`<20 or `operation`='mu'");
			pp.exec();
			pp.fetch();
			pp.insert("log",arr);
			pp.exec();
			arr["operation"] = "um";
			pp.update("log",arr);
			arr["operation"] = "mu";
			pp.limit(50);
			pp.exec();
			
		}
	}
	catch(mysqlppEx ex)
	{
		printf("MYexception was in:%d; in %s at %d\n",ex.reason,ex.file.c_str(),ex.line);
	}
	catch(sqlBaseEx ex)
	{
		printf("BASEexception was in:%d; in %s at %d\n",ex.reason,ex.file.c_str(),ex.line);
	}	
	return 0;
}