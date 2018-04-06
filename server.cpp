#include "ServerSocket.h"
#include "SocketException.h"
#include <string>
#include <iostream>
#include <map>
#include <list>
#include <cstdlib>
#include <thread>

using namespace std;

class Connection {
    public:
        string user;
        ServerSocket* socket;
};

void launchThread (Connection* con, list<Connection*>* connections) {
    while (true) {
        string data;
        *con->socket >> data;
        string command = data.substr(0, data.find("="));
        string value = data.substr(data.find("=") + 1, data.size());

        if (command == "USER") {
            con->user = value;
        } else if (command == "MSG") {
            for (Connection* connection : *connections) {
                if (con->socket != connection->socket) {
                    *connection->socket << con->user + ": " + value;
                }
            }
        }
        cout << "=>user: " << con->user << "\n=>command: " << command << "\n=>value: " << value << "\n";
    }
}

int main (int argc, char* argv[]) {
    if(argc != 2) {
        cout << "invalid number of arguments: call with `./server port`" << endl;
        return 1;
    }

    int port = atoi(argv[1]);
    cout << "running on port "<< port << "\n";

    list<Connection*>* connections = new list<Connection*>();

    try {
        ServerSocket server(port);
        while (true) {
            ServerSocket* new_sock = new ServerSocket();
            new_sock->set_non_blocking(true);
            server.accept(*new_sock);
            try {
                Connection* con = new Connection();
                con->socket = new_sock;
                connections->push_back(con);
                thread(launchThread, con, connections).detach();
            } catch ( SocketException& ) {
                delete new_sock;
            }
        }
    } catch ( SocketException& e ) {
        cout << "Exception was caught:" << e.description() << "\nExiting" << endl;
    }

    return 0;
}
