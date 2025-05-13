#include "server.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

Server::Server(int port, QObject* parent) : QObject(parent), server(new QTcpServer(this)), clientSocket(nullptr), expectedLength(0) {
    connect(server, &QTcpServer::newConnection, this, &Server::onNewConnection);
    if (!server->listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start:" << server->errorString();
    } else {
        qDebug() << "Server started on port" << port;
    }
}

bool Server::isRunning() const {
    return server->isListening();
}

void Server::onNewConnection() {
    if (clientSocket == nullptr) {
        clientSocket = server->nextPendingConnection();
        connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);
        qDebug() << "Client connected from" << clientSocket->peerAddress().toString();
    } else {
        qDebug() << "Another client tried to connect, but server only supports one client.";
    }
}

void Server::onReadyRead() {
    while (clientSocket->bytesAvailable() >= static_cast<qint64>(sizeof(qint32))) {
        if (expectedLength == 0) {
            clientSocket->read(reinterpret_cast<char*>(&expectedLength), sizeof(qint32));
            qDebug() << "Expecting message of length:" << expectedLength;
        }
        if (expectedLength > 0 && clientSocket->bytesAvailable() >= expectedLength) {
            QByteArray data = clientSocket->read(expectedLength);
            expectedLength = 0;
            QString sqls = QString::fromUtf8(data);
            qDebug() << "Received SQL queries:" << sqls;

            QList<QString> sqlList = parser.split_sqls(sqls);
            QJsonArray results;
            for (const QString& sql : sqlList) {
                QJsonObject ast = parser.parse_sql(sql);
                QJsonDocument returnData;
                int code = executor.execute_ast(ast, returnData);

                QJsonObject result = returnData.object();
                results.append(result);

                qDebug() << "code: " << code;
                qDebug() << "returnData: " << returnData;
                qDebug() << "\n\n";
            }

            QJsonDocument responseDoc(results);
            QByteArray response = responseDoc.toJson(QJsonDocument::Compact);
            qint32 length = response.size();
            qDebug() << "Sending response of length:" << length;
            clientSocket->write(reinterpret_cast<char*>(&length), sizeof(qint32));
            clientSocket->write(response);
        }
    }
}

void Server::onClientDisconnected() {
    qDebug() << "Client disconnected";
    clientSocket->deleteLater();
    clientSocket = nullptr;
}
