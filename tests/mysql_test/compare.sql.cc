#include <dbMysql.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	dbMysql pp;	
	try
	{
		pp.setDbInfo(string("test"),string(""),0,string(""),string("Dmitrik"),"/tmp/mysql.sock");
		pp.connect();		
		
		map<string,string> arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		
		vector<string> select;
		select.push_back("date");
		select.push_back("operation");
		
		for (int i=0;i<3000;i++)
		{
			pp.select("log",select,"`id`<20 or `operation`='um'");
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
	catch(baseEx ex)
	{
		printf("BASEexception was in: in %s at %d\n",ex.file.c_str(),ex.line);
	}	
	return 0;
}
