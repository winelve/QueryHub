// server.cpp
#include "server.h"
#include <cstring>
#include <fcntl.h>
#include <algorithm>
#include <QDebug> // 添加QDebug头文件

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

Server::Server(int port) : port_(port), running_(false), server_socket_(SOCKET_ERROR_VALUE) {}

Server::~Server() {
    stop();
}

bool Server::start() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        qDebug() << "WSAStartup failed";
        return false;
    }
#endif

    // 创建服务器socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ == SOCKET_ERROR_VALUE) {
        qDebug() << "Failed to create socket";
        return false;
    }

// 设置为非阻塞模式
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(server_socket_, FIONBIO, &mode);
#else
    int flags = fcntl(server_socket_, F_GETFL, 0);
    fcntl(server_socket_, F_SETFL, flags | O_NONBLOCK);
#endif

    // 允许地址重用
    int opt = 1;
    setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    // 绑定地址和端口
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    if (bind(server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        qDebug() << "Bind failed";
        closesocket(server_socket_);
        return false;
    }

    // 开始监听
    if (listen(server_socket_, 10) < 0) {
        qDebug() << "Listen failed";
        closesocket(server_socket_);
        return false;
    }

    running_ = true;
    qDebug() << "Server started on port" << port_;
    return true;
}

void Server::stop() {
    running_ = false;

    // 关闭所有客户端连接
    for (auto& socket : client_sockets_) {
        closesocket(socket);
    }
    client_sockets_.clear();

    // 关闭服务器socket
    if (server_socket_ != SOCKET_ERROR_VALUE) {
        closesocket(server_socket_);
        server_socket_ = SOCKET_ERROR_VALUE;
    }

#ifdef _WIN32
    WSACleanup();
#endif

    qDebug() << "Server stopped";
}

void Server::run() {
    if (!running_) {
        qDebug() << "Server not started";
        return;
    }

    while (running_) {
        // 检查新连接
        acceptNewConnections();

        // 从客户端读取数据
        readFromClients();

        // 处理事件队列
        processEventQueue();
    }
}

void Server::acceptNewConnections() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    socket_t client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket != SOCKET_ERROR_VALUE) {
// 设置客户端socket为非阻塞
#ifdef _WIN32
        u_long mode = 1;
        ioctlsocket(client_socket, FIONBIO, &mode);
#else
        int flags = fcntl(client_socket, F_GETFL, 0);
        fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);
#endif

        // 添加到客户端列表
        client_sockets_.push_back(client_socket);

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        qDebug() << "New client connected:" << client_ip << ":" << ntohs(client_addr.sin_port)
                 << "(socket:" << client_socket << ")";

        // 发送欢迎消息
        std::string welcome_msg = "Welcome to the server!\n";
        send(client_socket, welcome_msg.c_str(), welcome_msg.length(), 0);
    }
}

void Server::readFromClients() {
    char buffer[1024];

    // 遍历所有客户端
    for (auto it = client_sockets_.begin(); it != client_sockets_.end();) {
        socket_t client_socket = *it;
        memset(buffer, 0, sizeof(buffer));

        int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_read > 0) {
            // 收到数据，将其放入事件队列
            std::string request(buffer, bytes_read);
            // 去除尾部换行符
            if (!request.empty() && request.back() == '\n') {
                request.pop_back();
            }
            if (!request.empty()) {
                events_.push({client_socket, request});
                qDebug() << "Received from client" << client_socket << ":" << request.c_str();
            }
            ++it;
        } else if (bytes_read == 0 ||
                   (bytes_read < 0 &&
#ifdef _WIN32
                    WSAGetLastError() != WSAEWOULDBLOCK
#else
                    errno != EAGAIN && errno != EWOULDBLOCK
#endif
                    )) {
            // 客户端断开连接或出错
            qDebug() << "Client disconnected (socket:" << client_socket << ")";
            closesocket(client_socket);
            it = client_sockets_.erase(it);
        } else {
            // 没有数据或者EWOULDBLOCK
            ++it;
        }
    }
}

void Server::processEventQueue() {
    // 处理事件队列中的所有事件
    while (!events_.empty()) {
        auto event = events_.front();
        events_.pop();

        socket_t client_socket = event.first;
        std::string request = event.second;

        // 处理请求（这里只是简单地将请求转为大写）
        QJsonDocument send_doc = processRequest(request);

        // 将结果发送回客户端
        if (std::find(client_sockets_.begin(), client_sockets_.end(), client_socket) != client_sockets_.end()) {
            sendJsonToClient(client_socket,send_doc);
            qDebug() << "Sent to client" << client_socket << ":" << send_doc;
        }
        qDebug() << "\n\n";
    }
}

QJsonDocument Server::processRequest(const std::string& sql) {
    qDebug() << "run sql:" << sql;
    QJsonDocument send_doc;
    QJsonObject ast_root = parser_.parse_sql(QString::fromStdString(sql));
    int code = executor_.execute_ast(ast_root,send_doc);
    qDebug() << "return code:" << code;
    return send_doc;
}

void Server::sendJsonToClient(socket_t client_socket, const QJsonDocument& jsonDoc) {
    QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);
    data.append('\n');
    send(client_socket, data.constData(), data.size(), 0);
    qDebug() << "Sent JSON to client" << client_socket;
}

// int main() {
//     Server server(8080);

//     if (server.start()) {
//         server.run();
//     }

//     return 0;
// }
