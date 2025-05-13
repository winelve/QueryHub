// client.cpp
#include "client.h"
#include "utils.h"

#include <QDebug>
#include <cstring>
#include <chrono>
#include <QThread>
#include <QList>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

Client::Client(const std::string& server_ip, int server_port)
    : server_ip_(server_ip), server_port_(server_port), client_socket_(SOCKET_ERROR_VALUE), connected_(false) {}

Client::~Client() {
    disconnect();
}

bool Client::connect() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        qDebug() << "WSAStartup failed";
        return false;
    }
#endif

    // 创建客户端socket
    client_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_ == SOCKET_ERROR_VALUE) {
        qDebug() << "Failed to create socket";
        return false;
    }

    // 连接到服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port_);

    if (inet_pton(AF_INET, server_ip_.c_str(), &server_addr.sin_addr) <= 0) {
        qDebug() << "Invalid address";
        closesocket(client_socket_);
        return false;
    }

    if (::connect(client_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        qDebug() << "Connection failed";
        closesocket(client_socket_);
        return false;
    }

    connected_ = true;
    qDebug() << "Connected to server at" << server_ip_.c_str() << ":" << server_port_;

    // 启动接收响应的线程
    receive_thread_ = std::thread(&Client::receiveResponses, this);

    return true;
}

void Client::disconnect() {
    connected_ = false;

    if (client_socket_ != SOCKET_ERROR_VALUE) {
        closesocket(client_socket_);
        client_socket_ = SOCKET_ERROR_VALUE;
    }

    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }

#ifdef _WIN32
    WSACleanup();
#endif

    qDebug() << "Disconnected from server";
}

bool Client::sendRequest(const std::string& request) {
    if (!connected_) {
        qDebug() << "Not connected to server";
        return false;
    }

    std::string req = request + "\n";
    if (send(client_socket_, req.c_str(), req.length(), 0) < 0) {
        qDebug() << "Failed to send request";
        return false;
    }

    return true;
}

void Client::run() {
    QList<QString> test_sqls = {
        "USE test_db;",
        "SELECT (*) FROM orders;",
        "SELECT (*) FROM orders;",
    };

    int i=0;
    while (connected_ && i<test_sqls.size()) {
        QThread::sleep(3);

        QString sql = test_sqls[i];
        qDebug() << "Sql: " << sql;
        if (sql == "exit") {
            break;
        }

        if (!sendRequest(sql.toStdString())) {
            break;
        }
        i++;
    }
    QThread::sleep(10);
    disconnect();
}

void Client::receiveResponses() {
    char buffer[1024];

    while (connected_) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = recv(client_socket_, buffer, sizeof(buffer) - 1, 0);

        if (bytes_read > 0) {
            // 将接收到的数据追加到缓冲区
            buffer_.append(buffer, bytes_read);

            // 处理可能有多个JSON对象的情况
            size_t pos;
            while ((pos = buffer_.find('\n')) != std::string::npos) {
                std::string json_str = buffer_.substr(0, pos);
                buffer_ = buffer_.substr(pos + 1);

                // 解析收到的JSON
                parseJsonResponse(json_str);
            }
        } else if (bytes_read == 0) {
            qDebug() << "Server closed the connection";
            connected_ = false;
            break;
        } else {
#ifdef _WIN32
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                qDebug() << "recv failed with error:" << error;
                connected_ = false;
                break;
            }
#else
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                qDebug() << "recv failed with error:" << errno;
                connected_ = false;
                break;
            }
#endif

            // 短暂休眠，避免CPU占用过高
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void Client::parseJsonResponse(const std::string& response) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray::fromStdString(response));
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Invalid JSON received:" << response.c_str();
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 获取基本字段
    QString code = jsonObj["code"].toString();
    QString func = jsonObj["func"].toString();
    QJsonArray data = jsonObj["data"].toArray();

    qDebug() << "Received response - Function:" << func << ", Status Code:" << code;

    // 根据func字段分发到不同的处理函数
    if (func == "ShowDatabases") {
        handleShowDatabases(data);
    } else if (func == "ShowTables") {
        handleShowTables(data);
    } else if (func == "DescribeTable") {
        handleDescribeTable(data);
    } else if (func == "Select") {
        handleSelect(data);
    } else {
        // 处理其他类型的响应
        handleGenericResponse(func, data, code);
    }
    qDebug() << "\n\n";
}

void Client::handleShowDatabases(const QJsonArray& data) {
    qDebug() << "Databases:" << data.toVariantList();
}

void Client::handleShowTables(const QJsonArray& data) {
    qDebug() << "Tables:" << data.toVariantList();
}

void Client::handleDescribeTable(const QJsonArray& data) {
    for (const QJsonValue& row : data) {
        QJsonObject columnInfo = row.toObject();
        qDebug() << columnInfo["cname"].toString() << "  " << columnInfo["type"].toString() << "  " << columnInfo["cs_name"].toString();
    }
}

void Client::handleSelect(const QJsonArray& data) {
    if (data.isEmpty()) {
        qDebug() << "No data returned from query.";
        return;
    }

    for(int i=0;i<data.size();i++) {
        qDebug() << data[i];
    }
    PrintJsonSelectRes(data);
}

void Client::handleGenericResponse(const QString& func, const QJsonArray& data, const QString& code) {
    qDebug() << "Function:" << func << ", Status:" << code;
}
