/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <string.h>

using namespace dodo;
using namespace data::base;

using namespace std;

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

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    if (argc == 1) {
        cout << "usage: " << argv[0] << " [postgresql|mysql|sqlite]" << endl;

        return 1;
    }

    try {
        __connection__ info;
        info.db = "test";
        if (strcasecmp(argv[1], "postgresql") == 0) {
            info.host = "127.0.0.1";
            info.port = 5432;
            info.user = "postgres";
        } else if (strcasecmp(argv[1], "mysql") == 0)   {
            info.path = "/var/run/mysqld/mysqld.sock";
            info.user = "root";
            info.password = "password";
        } else if (strcasecmp(argv[1], "sqlite") == 0)   {
            info.path = "test.lite";
        }

        connector *db;

            ///parse command line arguments to figure out what db to use
        if (strcasecmp(argv[1], "postgresql") == 0)
#ifdef POSTGRESQL_EXT
            db = new postgresql(info);
#else
            return 1;
#endif

        else if (strcasecmp(argv[1], "mysql") == 0)
#ifdef MYSQL_EXT
            db = new mysql(info);
#else
            return 1;
#endif

        else if (strcasecmp(argv[1], "sqlite") == 0)
#ifdef SQLITE3_EXT
            db = new sqlite(info);
#else
            return 1;
#endif

        else
            return 1;

#ifndef DATABASE_WO_XEXEC
        db->addXExec(xexec::ACTION_PREEXEC, ::hook, (void *)"id");
#endif

            ///define session charset
        if (strcasecmp(argv[1], "postgresql") == 0) {
#ifdef POSTGRESQL_EXT

                          ((postgresql *)db)->setCharset("UTF-8");

            cout << "Encoding: " << ((postgresql *)db)->charset() << endl;

#endif
        } else if (strcasecmp(argv[1], "mysql") == 0)   {
#ifdef MYSQL_EXT

                          ((mysql *)db)->setCharset("UTF-8");

            cout << "Encoding: " << ((mysql *)db)->charset() << endl;

#endif
        }

        try {
            db->exec("DROP TABLE test");
        } catch (dodo::exception::basic &ex)   {
            cout << (dodoString)ex << endl << ex.file << endl << ex.message << endl << ex.line << endl << endl;
        }

        if (strcasecmp(argv[1], "postgresql") == 0)
            db->exec("CREATE TABLE test (t0 text NOT NULL, t1 text NOT NULL, id integer default NULL, i integer default NULL, b bytea)");
        else
            db->exec("CREATE TABLE test (t0 text NOT NULL, t1 text NOT NULL, id integer default NULL, i integer default NULL, b longblob)");

        try {
            ((sql::constructor *)db)->requestFieldsTypes("test");
        } catch (dodo::exception::basic &ex)    {
            cout << (dodoString)ex << endl << ex.file << endl << ex.message << endl << ex.line << endl << endl;

            ((sql::constructor *)db)->setFieldType("test", "t0", sql::FIELD_TEXT);
            ((sql::constructor *)db)->setFieldType("test", "t1", sql::FIELD_TEXT);
            ((sql::constructor *)db)->setFieldType("test", "id", sql::FIELD_NUMERIC);
            ((sql::constructor *)db)->setFieldType("test", "i", sql::FIELD_NUMERIC);
            ((sql::constructor *)db)->setFieldType("test", "b", sql::FIELD_BINARY);
        }

        dodoStringMap array;
        array["t0"] = "xyz";
        array["t1"] = "abc";
        array["i"] = "1";

        dodoArray<dodoString> select;
        select.push_back("t0");
        select.push_back("t1");
        select.push_back("b");

        __tuples__ store;

        for (int i = 0; i < 10; i++) {
            db->select("test", select, "id<20 or t1='def'");
            db->exec();

            store = db->fetch();

            db->insert("test", array);
            db->exec();

            array["i"] = "i+1";
            array["t1"] = "def";
            db->update("test", array);
            db->exec();

            array["t1"] = "abc";
            array["i"] = "1";
        }

        db->select("test", select, "t1='def'");
        db->exec();

        store = db->fetch();

        cout << store.rows.size() << endl;

        dodoArray<dodoStringArray>::iterator i(store.rows.begin()), j(store.rows.end());
        dodoStringArray::iterator m, n;
        for (; i != j; i++) {
            m = i->begin();
            n = i->end();
            for (; m != n; m++)
                cout << "[" << *m << "]\t";
            cout << endl;
        }

#ifndef DATABASE_WO_XEXEC
        db->disableXExecs = true;
#endif

        tools::filesystem::unlink("test.db");
        dodoString b = tools::filesystem::fileContents("test");

        array.clear();

        array["b"] = b;
        array["t0"] = "blob";
        array["t1"] = "";

        db->insert("test", array);
        db->exec();

        db->disconnect();
        db->connect(info);

        db->select("test", select, "t0='blob'");
        db->exec();

        store = db->fetch();

            ///put fetched binary data to file
        if (store.fields.size() == 3 && store.rows.size() > 0)
            tools::filesystem::writeToFile("test.db", (*store.rows.begin())[2]);

        delete db;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << endl << ex.file << endl << ex.message << endl << ex.line << endl << endl;
    }

    return 0;
}
