/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidmanager=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io::network;
using namespace pc;

using namespace std;

sync::thread protector;
sync::data::object data(protector);

int
process(void *data)
{
    exchange *ex = (exchange *)data;

    dodo_try {
        if (ex->isBlocked()) {
            cout << "Blocked\n";
            cout.flush();
        } else   {
            cout << "Non blocked\n";
            cout.flush();
        }

        if (ex->isAlive()) {
            cout << "Alive\n";
            cout.flush();
        }

        ex->setInBufferSize(1);
        ex->setOutBufferSize(1);

        ex->bs = 7;
        ex->writeString("test\n");

        dodo::string str = "";
        dodo_try {
            ex->bs = 4;
            str = ex->read();

            cout << str << ":" << str.size() << endl;
            cout.flush();
            if (str == "exit") {
                bool *exit_condition;
                exit_condition = (bool *)::data.acquire();
                *exit_condition = true;
                ::data.release();
            }
        } dodo_catch (dodo::exception::basic *e)   {
            cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
            cout.flush();
        }

        ex->close();

        if (ex->isAlive()) {
            cout << "It's alive\n";
            cout.flush();
        } else   {
            cout << "Closed\n";
            cout.flush();
        }
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    delete ex;

    return 0;
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        server s(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);

        connection::__peer__ info;
        exchange::__init__ accepted;

        s.serve("127.0.0.1", 7778, 3);
        s.setLingerOption(connection::LINGER_OPTION_HARD_CLOSE);
        s.blockInherited = false;
        s.block(false);

        bool exit_condition(false);

        execution::manager manager;

        ::data.set((void *)&exit_condition);

        while (!exit_condition) {
            if (s.accept(accepted, info)) {
                if (s.isBlocked()) {
                    cout << "Blocked\n";
                    cout.flush();
                }

                exchange *ex = new exchange(accepted);
                manager.run(manager.add(execution::thread(::process, (void *)ex, execution::ON_DESTRUCTION_KEEP_ALIVE)));

                dodo_try {
                    dodo::slList<unsigned long> threads = manager.jobs();
                    dodo::slList<unsigned long>::iterator i = threads.begin(), j = threads.end();
                    for (; i != j; ++i)
                        if (manager.isRunning(*i)) {
                            cout << *i << " is running\n";
                            cout.flush();
                        } else   {
                            cout << *i << " is finished\n";
                            manager.remove(*i);
                            cout.flush();
                        }
                } dodo_catch (dodo::exception::basic *e UNUSED)   {
                }
            }
        }

        manager.wait();
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}
