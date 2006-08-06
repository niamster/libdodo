#include <baseEx.h>
#include <dbMysql.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	dbMysql pp;	
	try
	{
		pp.setDbInfo(string("test"),"",0,"","Dmitrik","/tmp/mysql.sock");
		
		pp.connect();		
		
		map<string,string> arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		
		vector<string> select;
		select.push_back("date");
		select.push_back("operation");
		
		for (int i=0;i<100000;i++)
		{
			pp.select("log",select,"`id`<20 or `operation`='mu'");
			pp.queryCollect();
			//pp.exec();
			//pp.fetch();
			
			pp.insert("log",arr);
			pp.queryCollect();
			//pp.exec();
			
			arr["operation"] = "um";
			pp.update("log",arr);
			arr["operation"] = "mu";
			pp.limit(50);
			pp.queryCollect();
			//pp.exec();
		}
	}
	catch(baseEx ex)
	{
		printf("BASEexception was in: in %s at %ld\n",ex.file.c_str(),ex.line);
	}	
	return 0;
}
