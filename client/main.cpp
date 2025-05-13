#include "client.h"
#include <QDebug>

int main() {
    std::string server_ip = "127.0.0.1";
    int server_port = 8080;

    qDebug() << "Server IP: " << server_ip;
    qDebug() << "Server Port: " << server_port;
    Client client(server_ip, server_port);

    if (client.connect()) {
        client.run();
    }

    return 0;
}
