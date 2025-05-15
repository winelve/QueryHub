#ifndef COMMANDLINEWINDOW_H
#define COMMANDLINEWINDOW_H

#include <QtWidgets>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include "client.h"

class CommandLineWindow : public QWidget {
    Q_OBJECT

public:
    CommandLineWindow(QWidget *parent = nullptr);

private slots:
    void processInput();
    void connectToServer();
    void displayResult(const QJsonArray& result);

private:
    QVBoxLayout *mainLayout;
    QTextEdit *outputDisplay;
    QLineEdit *inputLine;
    QLabel *statusLabel;

    Client *client;
    bool isConnected;
    bool waitingForConnection;

    void showConnectionPrompt();
    void showCommandPrompt();

    // 修改 appendToOutput 方法以避免每行添加时间戳
    void appendToOutput(const QString& text, bool withTimestamp = true);

    void handleShowDatabases(const QJsonArray& data);
    void handleShowTables(const QJsonArray& data);
    void handleDescribeTable(const QJsonArray& data);
    void handleSelect(const QJsonArray& data);
    void handleOthers(const QString& funcName, const QJsonValue& data);
    //读取sql脚本
    QString read_sql_file(const QString& path);
};

#endif // COMMANDLINEWINDOW_H
