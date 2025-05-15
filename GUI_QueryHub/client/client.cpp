#include "client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QDebug>

Client::Client(QObject* parent) : QObject(parent), socket(new QTcpSocket(this)), expectedLength(0), responseReceived(false), loop(new QEventLoop(this)) {
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
}

bool Client::connectToServer(const QString& host, int port) {
    socket->connectToHost(host, port);
    if (socket->waitForConnected(3000)) {
        qDebug() << "Connected to server at" << host << ":" << port;
        return true;
    } else {
        qDebug() << "Failed to connect:" << socket->errorString();
        return false;
    }
}

QJsonArray Client::handle_sql(const QString& sql) {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Not connected to server";
        return QJsonArray();
    }
    QByteArray request = sql.toUtf8();
    qint32 length = request.size();
    qDebug() << "Sending SQL query of length" << length << ":" << sql;
    socket->write(reinterpret_cast<char*>(&length), sizeof(qint32));
    socket->write(request);

    responseReceived = false;
    expectedLength = 0;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, loop, &QEventLoop::quit);
    timer.start(5000); // 5-second timeout
    connect(socket, &QTcpSocket::disconnected, loop, &QEventLoop::quit);
    loop->exec();

    if (responseReceived) {
        qDebug() << "Received response:" << responseArray;
        return responseArray;
    } else {
        qDebug() << "No response received (timeout or disconnected)";
        return QJsonArray();
    }
}

void Client::onReadyRead() {
    while (socket->bytesAvailable() >= static_cast<qint64>(sizeof(qint32))) {
        if (expectedLength == 0) {
            socket->read(reinterpret_cast<char*>(&expectedLength), sizeof(qint32));
            qDebug() << "Expecting response of length:" << expectedLength;
        }
        if (expectedLength > 0 && socket->bytesAvailable() >= expectedLength) {
            QByteArray data = socket->read(expectedLength);
            expectedLength = 0;
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isArray()) {
                responseArray = doc.array();
                responseReceived = true;
                qDebug() << "Response parsed successfully";
                loop->quit();
            } else {
                qDebug() << "Invalid JSON response";
            }
        }
    }
}

bool Client::login(const QString &username,const QString &pwd) {
    QJsonArray response = handle_sql(QString("login %1 p %2;").arg(username,pwd));
    qDebug() << response;
    int code = response[0].toObject()["code"].toInt();
    if(code==0) return true;
    return false;
}

std::vector<std::string> Client::get_all_database() {
    QJsonArray result = handle_sql("SHOW DATABASES;");
    if (!result.isEmpty()) {
        QJsonObject obj = result[0].toObject();
        QJsonArray data = obj["data"].toArray();
        std::vector<std::string> databases;
        for (const QJsonValue& val : data) {
            databases.push_back(val.toString().toStdString());
        }
        return databases;
    }
    return {};
}

std::vector<std::string> Client::get_all_tables(const std::string& database) {
    QString sql1 = QString("USE %1;").arg(QString::fromStdString(database));
    QString sql2 = QString("SHOW TABLES;");

    handle_sql(sql1);
    QJsonArray result = handle_sql(sql2);

    if (!result.isEmpty()) {
        QJsonObject obj = result[0].toObject();
        QJsonArray data = obj["data"].toArray();
        std::vector<std::string> tables;
        for (const QJsonValue& val : data) {
            tables.push_back(val.toString().toStdString());
        }
        return tables;
    }
    return {};
}

QJsonArray Client::describe_table(const std::string& database, const std::string& table) {
    QString sql1 = QString("USE %1;").arg(QString::fromStdString(database));
    QString sql2 = QString("DESC %1;").arg(QString::fromStdString(table));

    handle_sql(sql1);
    QJsonArray result = handle_sql(sql2);
    if (!result.isEmpty()) {
        QJsonObject obj = result[0].toObject();
        return obj["data"].toArray();
    }
    return QJsonArray();
}

std::vector<std::vector<std::string>> Client::select_table(const std::string& database, const std::string& table) {
    QString sql1 = QString("USE %1;").arg(QString::fromStdString(database));
    QString sql2 = QString("SELECT (*) FROM %1;").arg(QString::fromStdString(table));

    handle_sql(sql1);
    QJsonArray result = handle_sql(sql2);

    if (result.isEmpty()) {
        return {};
    }

    QJsonObject obj = result[0].toObject();
    QJsonArray data = obj["data"].toArray();
    std::vector<std::vector<std::string>> tableData;

    for (const QJsonValue& rowVal : data) {
        QJsonArray rowArray = rowVal.toArray();
        std::vector<std::string> row;
        for (const QJsonValue& cell : rowArray) {
            if (cell.isString()) {
                row.push_back(cell.toString().toStdString());
            } else if (cell.isDouble()) {
                double num = cell.toDouble();
                if (num == std::floor(num)) {
                    row.push_back(std::to_string(static_cast<int>(num)));  // 整数
                } else {
                    row.push_back(std::to_string(num));  // 浮点数
                }
            } else if (cell.isBool()) {
                row.push_back(cell.toBool() ? "true" : "false");
            } else if (cell.isNull()) {
                row.push_back("NULL");
            } else {
                row.push_back("");  // 未知类型
            }
        }
        tableData.push_back(row);
    }
    return tableData;
}
