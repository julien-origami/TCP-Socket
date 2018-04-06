#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <thread>
#include <string>
#include <cstdlib>
#include <mutex>
#include <fstream>

using namespace std;

int main (int argc, char* argv[]) {
    if(argc != 3) {
        cout << "invalid number of arguments: call with `./client IP port`" << endl;
        return 1;
    }

    string host = argv[1];
    int port = atoi(argv[2]);

    try {
        ClientSocket client_socket(host, port);

        string user;
        cout << "Please enter a user\n";
        while(user.size() == 0) {
            cout << "pseudo: ";
            cin >> user;
        }

        try {
            client_socket << "USER=" + user;
        } catch ( SocketException& ) {}

        string message;

        thread t1([&]() {
            string reply;
            try {
                while(true) {
                    client_socket >> reply;
                    cout << "\r" << "\n";
                    cout << "\t\t\t" << reply << "\n";
                    string say = "say " + reply;
                    system(say.c_str());
                }
            } catch ( SocketException& ) {}
        });

        do {
            getline(cin, message);
            if (message.compare("exit") != 0 && message.size() > 0) {
                try {
                    client_socket << "MSG=" + message;
                } catch ( SocketException& ) {}
            }
            cout << "\nyou: ";
        } while (message.compare("exit") != 0);
    }  catch ( SocketException& e ) {
        cout << "Exception was caught: " << e.description() << endl;
    }

    return 0;
}
