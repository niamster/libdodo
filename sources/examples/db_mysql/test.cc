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
    sql::constructor::__collected_data__ *sql = (sql::constructor::__collected_data__ *)odata;

    if (operation == data::base::connector::OPERATION_EXEC && sql->query)
        cout << endl << endl << "request: " << sql->query->sql.data() << endl << endl;
}
#endif
#endif

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    long now = tools::time::now();

#ifdef MYSQL_EXT
    try {
        mysql::__connection_options__ ci("test", argc>1?argv[1]:"localhost", "root", "password", "", 3306);

        mysql db(ci);

#ifndef DATABASE_WO_XEXEC
        db.addXExec(xexec::ACTION_PREEXEC, ::hook, NULL);
#endif

        try {
            db.exec(sql::query("DROP TABLE test0"));
        } catch (...)   {
        }
        try {
            db.exec(sql::query("DROP TABLE test1"));
        } catch (...)   {
        }

        db.exec(sql::query("CREATE TABLE test0 (id int(11) NOT NULL auto_increment, i int(11), t0 text NOT NULL, t1 text NOT NULL, PRIMARY KEY  (id))"));
        db.exec(sql::query("CREATE TABLE test1 (id int(11) NOT NULL auto_increment, i int(11), t0 text NOT NULL, t1 text NOT NULL, PRIMARY KEY  (id))"));

        db.requestFieldsTypes("test0");
        db.requestFieldsTypes("test1");

        sql::rows rows;
        dodoStringArray fields;

        db.select(sql::condition("test0").limit(10).join("test1", "test0.t0 = test1.t0"));
        db.exec();

        db.fetchedRows(rows);

        dodoStringArray::iterator i = rows.fields.begin(), j = rows.fields.end();
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

        db.insert(sql::rows(mapArray), sql::condition("test0"));
        db.exec();

        array.clear();

        db.disconnect();
        db.connect(ci);

        array["i"] = "100000";
        db.update(sql::rows(array), sql::condition("test0"));
        db.exec();

        fields.clear();
        fields.push_back("t0");
        fields.push_back("t1");

        db.insert(sql::rows(values, fields), sql::condition("test0"));
        db.exec();

#ifndef DATABASE_WO_XEXEC
        db.disableXExecs = true;
#endif

        for (int o = 0; o < 100000; o++) {
            db.insert(sql::rows(values, fields), sql::condition("test0"));
            db.exec();
        }

#ifndef DATABASE_WO_XEXEC
        db.disableXExecs = false;
#endif

        dodoString u;
        db.select(sql::condition("test0", "id>1").limit(10).offset(20));
        u = db.construct();
        u += " union all ";
        db.select(sql::condition("test0", "id<100"));
        u += db.construct();
        db.exec(sql::query(u));

        db.select(sql::condition("test0").limit(10));
        db.exec();

        db.fetchedRows(rows);

        dodoArray<dodoStringArray>::iterator o(rows.values.begin()), p(rows.values.end());
        dodoStringArray::iterator m, n;
        for (; o != p; o++) {
            m = o->begin();
            n = o->end();
            for (; m != n; m++)
                cout << "[" << *m << "]\t";
            cout << endl;
        }
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl << ex.backtrace();
    }
#else
    cout << "No MySQL extension was compiled!";
#endif

    now = tools::time::now() - now;

    cout << "SpentTime: " << now << endl;

    return 0;
}
