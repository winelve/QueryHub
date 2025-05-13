#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include "parser.h"
#include "executor.h"

class Server : public QObject {
    Q_OBJECT
public:
    Server(int port, QObject* parent = nullptr);
    bool isRunning() const;
private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();
private:
    QTcpServer* server;
    QTcpSocket* clientSocket;
    qint32 expectedLength;
    Parser parser;
    Executor executor;
};
