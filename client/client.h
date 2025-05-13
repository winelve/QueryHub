// client.h
#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <thread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QStringList>

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

// 定义表结构描述
struct ColumnInfo {
    QString columnName;
    QString dataType;
    QString charsetName;
};

class Client {
public:
    Client(const std::string& server_ip, int server_port);
    ~Client();

    bool connect();
    void disconnect();
    bool sendRequest(const std::string& request);
    void run();

private:
    void receiveResponses();
    void parseJsonResponse(const std::string& response);

    // 各种响应处理函数
    void handleShowDatabases(const QJsonArray& data);
    void handleShowTables(const QJsonArray& data);
    void handleDescribeTable(const QJsonArray& data);
    void handleSelect(const QJsonArray& data);
    void handleGenericResponse(const QString& func, const QJsonArray& data, const QString& code);

    std::string server_ip_;
    int server_port_;
    socket_t client_socket_;
    bool connected_;
    std::thread receive_thread_;

    // 用于处理接收到的不完整JSON数据
    std::string buffer_;
};

#endif // CLIENT_H
