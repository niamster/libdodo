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
    sql::constructor::__collected_data__ *sql = (sql::constructor::__collected_data__ *)odata;

    if (operation == data::base::connector::OPERATION_EXEC && sql->query)
        cout << endl << endl << "request: " << sql->query->sql.data() << endl << endl;
}
#endif

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    if (argc == 1) {
        cout << "usage: " << argv[0] << " [postgresql [host]|mysql [host]|sqlite]" << endl;

        return 1;
    }

    __connection_options__ *ci;
    connector *db;

    try {
        if (strcasecmp(argv[1], "postgresql") == 0) {
#ifdef POSTGRESQL_EXT
            ci = new postgresql::__connection_options__("test", argc>2?argv[2]:"localhost", "postgres", "", 5432);

            db = new postgresql(*ci);
#else
            return 1;
#endif
        } else if (strcasecmp(argv[1], "mysql") == 0)   {
#ifdef MYSQL_EXT
            ci = new mysql::__connection_options__("test", argc>2?argv[2]:"localhost", "root", "password", "", 3306);

            db = new mysql(*ci);
#else
            return 1;
#endif
        } else if (strcasecmp(argv[1], "sqlite") == 0)   {
#ifdef SQLITE3_EXT
            ci = new sqlite::__connection_options__("test.lite");

            db = new sqlite(*ci);
#else
            return 1;
#endif
        }

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
            db->exec(sql::query("DROP TABLE test"));
        } catch (dodo::exception::basic &ex)   {
            cout << (dodoString)ex << endl << ex.file << endl << ex.message << endl << ex.line << endl << endl;
        }

        if (strcasecmp(argv[1], "postgresql") == 0)
            db->exec(sql::query("CREATE TABLE test (t0 text NOT NULL, t1 text NOT NULL, id integer default NULL, i integer default NULL, b bytea)"));
        else
            db->exec(sql::query("CREATE TABLE test (t0 text NOT NULL, t1 text NOT NULL, id integer default NULL, i integer default NULL, b longblob)"));

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

        dodoArray<dodoString> fields;
        fields.push_back("t0");
        fields.push_back("t1");
        fields.push_back("b");

        sql::rows rows;

        for (int i = 0; i < 10; i++) {
            db->select(sql::condition("test", fields, "id<20 or t1='def'"));
            db->exec();

            db->fetchedRows(rows);

            db->insert(sql::rows(array), sql::condition("test"));
            db->exec();

            array["i"] = "i+1";
            array["t1"] = "def";
            db->update(sql::rows(array), sql::condition("test"));
            db->exec();

            array["t1"] = "abc";
            array["i"] = "1";
        }

        db->select(sql::condition("test", fields, "t1='def'"));
        db->exec();

        db->fetchedRows(rows);

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

#ifndef DATABASE_WO_XEXEC
        db->disableXExecs = true;
#endif

        ///put binary data into data base
        tools::filesystem::unlink("test.db");
        dodoString b = tools::filesystem::fileContents("test");

        array.clear();

        array["b"] = b;
        array["t0"] = "blob";
        array["t1"] = "";

        db->insert(sql::rows(array), sql::condition("test"));
        db->exec();

        db->disconnect();
        db->connect(*ci);

        db->select(sql::condition("test", fields, "t0='blob'"));
        db->exec();

        db->fetchedRows(rows);

        ///put fetched binary data to file
        if (rows.fields.size() == 3 && rows.values.size() > 0)
            tools::filesystem::writeToFile("test.db", (*rows.values.begin())[2]);

        delete db;
        delete ci;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << endl << ex.file << endl << ex.message << endl << ex.line << endl << endl;
    }

    return 0;
}
