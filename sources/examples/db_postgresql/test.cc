/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;

using namespace std;

#ifdef POSTGRESQL_EXT
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
#ifdef POSTGRESQL_EXT
    try {
        postgresql::__connection_options__ ci("test", argc>1?argv[1]:"localhost", "postgres", "", 5432);

        postgresql db(ci);

#ifndef DATABASE_WO_XEXEC
        db.addXExec(xexec::ACTION_PREEXEC, ::hook, NULL);
#endif

        try {
            db.exec(sql::query("DROP TABLE test"));
        } catch (...)   {
        }

        db.exec(sql::query("CREATE TABLE test (t0 text NOT NULL, t1 text NOT NULL, id integer default NULL, i integer default NULL, b bytea)"));

        sql::rows rows;

        dodoStringMap array;
        array["t0"] = "xyz";
        array["t1"] = "abc";

        dodoStringArray fields;
        fields.push_back("t0");
        fields.push_back("t1");

        for (int i = 0; i < 10; i++) {
            db.select(sql::condition("test", fields, "id<20 or t1='abc'"));
            db.exec();

            db.fetchedRows(rows);

            db.insert(sql::rows(array), sql::condition("test"));
            db.exec();

            array["t1"] = "def";
            db.update(sql::rows(array), sql::condition("test"));
            db.exec();

            array["t1"] = "abc";
        }

        db.disconnect();
        db.connect(ci);

        db.select(sql::condition("test", fields, "t1='def'"));
        db.exec();

        db.fetchedRows(rows);

        cout << rows.values.size() << endl;

        dodoArray<dodoStringArray>::iterator i(rows.values.begin()), j(rows.values.end());
        dodoStringArray::iterator m, n;
        for (; i != j; i++) {
            m = i->begin();
            n = i->end();
            for (; m != n; m++)
                cout << "[" << *m << "]\t";
            cout << endl;
        }
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << ex.line;
    }
#else

    cout << "No Postresql extension was compiled";

#endif

    return 0;
}
