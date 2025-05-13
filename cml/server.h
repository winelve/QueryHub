// server.h
#ifndef SERVER_H
#define SERVER_H

#include "parser.h"
#include "executor.h"

#include <string>
#include <vector>
#include <queue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>
#include <QDebug> // 添加QDebug头文件

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#define SOCKET_ERROR_VALUE INVALID_SOCKET
#else
#include <sys/socket.h>
#include <netinet/in.h>
typedef int socket_t;
#define SOCKET_ERROR_VALUE (-1)
#define closesocket close
#endif

class Server {
public:
    Server(int port);
    ~Server();

    bool start();
    void stop();
    void run();

    void sendJsonToClient(socket_t client_socket, const QJsonDocument& jsonDoc);

private:
    void acceptNewConnections();
    void readFromClients();
    void processEventQueue();
    QJsonDocument processRequest(const std::string& request);

    int port_;
    bool running_;
    socket_t server_socket_;
    std::vector<socket_t> client_sockets_;
    std::queue<std::pair<socket_t, std::string>> events_;
    Parser parser_;
    Executor executor_;
};

#endif // SERVER_H
