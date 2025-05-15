#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <QJsonArray>
#include <QEventLoop>

#include <vector>
#include <string>

class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject* parent = nullptr);
    bool connectToServer(const QString& host, int port);
    QJsonArray handle_sql(const QString& sql);

    std::vector<std::string> get_all_database();
    std::vector<std::string> get_all_tables(const std::string& database);
    QJsonArray describe_table(const std::string& database, const std::string& table);
    std::vector<std::vector<std::string>> select_table(const std::string& database, const std::string& table);
    bool login(const QString &username,const QString &pwd);
private slots:
    void onReadyRead();
private:
    QTcpSocket* socket;
    qint32 expectedLength;
    QJsonArray responseArray;
    bool responseReceived;
    QEventLoop* loop;
};


#endif
