/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;

using namespace std;

#ifdef MYSQL_EXT
using namespace data::base;

#ifndef DATABASE_WO_XEXEC
void
hook(xexec::__collected_data__ *odata,
     short                     type,
     short                     operation,
     void                      *udata)
{
    accumulator::__collected_data__ *sql = (accumulator::__collected_data__ *)odata;

    if (operation == data::base::connector::OPERATION_EXEC) {
        cout << endl << endl << "request: " << dynamic_cast<sql::constructor *>(sql->executor)->construct() << endl << endl;
    }
}
#endif
#endif

int
main(int  argc,
     char **argv)
{
    long now = tools::time::now();

#ifdef MYSQL_EXT
    try {
        mysql db(__connection__("test", "localhost", "root", "password", "", 3306));

#ifndef DATABASE_WO_XEXEC
        db.addXExec(xexec::ACTION_PREEXEC, ::hook, NULL);
#endif

        try {
            db.exec("DROP TABLE test0");
        } catch (...)   {
        }
        try {
            db.exec("DROP TABLE test1");
        } catch (...)   {
        }

        db.exec("CREATE TABLE test0 (id int(11) NOT NULL auto_increment, i int(11), t0 text NOT NULL, t1 text NOT NULL, PRIMARY KEY  (id))");
        db.exec("CREATE TABLE test1 (id int(11) NOT NULL auto_increment, i int(11), t0 text NOT NULL, t1 text NOT NULL, PRIMARY KEY  (id))");

        db.requestFieldsTypes("test0");
        db.requestFieldsTypes("test1");

        dodoStringArray fields;
        __tuples__ storage;

        db.select("test0");
        db.join("test1", JOIN_JOIN, "test0.t0 = test1.t0");
        db.limit(10);
        db.exec();

        storage = db.fetch();

        dodoStringArray::iterator i = storage.fields.begin(), j = storage.fields.end();
        for (; i != j; ++i)
            cout << "[" << *i << "]\t";
        cout << endl;

        dodoStringArray values;
        values.push_back("a\"bc`''-'de-f");
        values.push_back("abc");

        dodoStringMap array;
        dodoArray<dodoStringMap> mapArray;
        array["t0"] = "a\"b`''c'd-e";
        array["t1"] = "a\nb";
        mapArray.push_back(array);
        array["t0"] = "ab\"c`''-'d-ef";
        array["t1"] = "a\nbc";
        mapArray.push_back(array);

        db.insert("test0", mapArray);
        db.exec();

        array.clear();

        db.disconnect();
        db.connect(__connection__("test", "localhost", "root", "password", "", 3306));

        array["i"] = "100000";
        db.update("test0", array);
        db.exec();

        fields.clear();
        fields.push_back("t0");
        fields.push_back("t1");

        db.insert("test0", values, fields);
        db.exec();

#ifndef DATABASE_WO_XEXEC
        db.disableXExecs = true;
#endif

        for (int o = 0; o < 100000; o++) {
            db.insert("test0", values, fields);
            db.exec();
        }

#ifndef DATABASE_WO_XEXEC
        db.disableXExecs = false;
#endif

        dodoStringArray u;

        db.select("test0", dodoStringArray(), "id>1");
        db.limit(10);
        db.offset(23);

        dodoString subrequest = db.construct();
        u.push_back(subrequest);
        u.push_back(subrequest);
        db.subquery(u);
        subrequest = db.construct();

        db.select("test0", dodoStringArray(), "id<100");

        u.clear();
        u.push_back(subrequest);
        u.push_back(db.construct());
        db.subquery(u, SUBREQUEST_UNION_ALL);

        db.order("id desc");
        db.limit(5);
        db.exec();

        db.select("test0");
        db.limit(10);
        db.exec();

        storage = db.fetch();

        dodoArray<dodoStringArray>::iterator o(storage.rows.begin()), p(storage.rows.end());
        dodoStringArray::iterator m, n;
        for (; o != p; o++) {
            m = o->begin();
            n = o->end();
            for (; m != n; m++)
                cout << "[" << *m << "]\t";
            cout << endl;
        }
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }
#else
    cout << "No MySQL extension was compiled!";
#endif

    now = tools::time::now() - now;

    cout << "SpentTime: " << now << endl;

    return 0;
}
