#include "commandlinewindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>

CommandLineWindow::CommandLineWindow(QWidget *parent) : QWidget(parent) {
    // 初始化界面元素
    setWindowTitle("SQL Command Line");
    resize(800, 600);

    mainLayout = new QVBoxLayout(this);

    // 输出显示区域
    outputDisplay = new QTextEdit(this);
    outputDisplay->setReadOnly(true);
    outputDisplay->setStyleSheet("QTextEdit { background-color: #2b2b2b; color: #f0f0f0; font-family: Consolas, Courier, monospace; }");
    mainLayout->addWidget(outputDisplay);

    // 状态标签
    statusLabel = new QLabel("Not Connected", this);
    statusLabel->setStyleSheet("QLabel { color: #ff5555; }");
    mainLayout->addWidget(statusLabel);

    // 输入行
    inputLine = new QLineEdit(this);
    inputLine->setStyleSheet("QLineEdit { background-color: #2b2b2b; color: #f0f0f0; font-family: Consolas, Courier, monospace; }");
    connect(inputLine, &QLineEdit::returnPressed, this, &CommandLineWindow::processInput);
    mainLayout->addWidget(inputLine);

    // 初始化Client
    client = new Client(this);
    isConnected = false;
    waitingForConnection = false;

    // 显示欢迎信息和连接提示
    appendToOutput("=== SQL Command Line Client ===");
    appendToOutput("Enter server details to connect (format: host:port)");
    showConnectionPrompt();

    inputLine->setFocus();
}

// 修改 processInput 方法中处理连接部分的代码

void CommandLineWindow::processInput() {
    QString input = inputLine->text().trimmed();
    inputLine->clear();

    // 显示输入内容到输出区域
    appendToOutput("> " + input);

    if (input.isEmpty()) {
        return;
    }

    // 处理退出命令
    if (input.toLower() == "quit") {
        appendToOutput("Exiting application...");
        QApplication::quit();
        return;
    }
    else if(input.toLower() == "clear" || input.toLower() == "cls") {
        outputDisplay->clear();
    }

    // 连接服务器
    else if (!isConnected && !waitingForConnection) {
        waitingForConnection = true;

        // 修复冒号导致的崩溃问题
        if (input.contains(":")) {
            QStringList parts = input.split(":");

            // 确保有两部分，且都不为空，且端口号有效
            if (parts.size() != 2 || parts[0].isEmpty() || parts[1].isEmpty()) {
                appendToOutput("Error: Invalid format. Use 'host:port' (e.g., 127.0.0.1:3306)");
                waitingForConnection = false;
                showConnectionPrompt();
                return;
            }

            bool ok;
            int port = parts[1].toInt(&ok);
            if (!ok || port <= 0 || port > 65535) {
                appendToOutput("Error: Invalid port number. Port must be between 1 and 65535.");
                waitingForConnection = false;
                showConnectionPrompt();
                return;
            }

            QString host = parts[0];
            appendToOutput("Connecting to " + host + ":" + QString::number(port) + "...");

            if (client->connectToServer(host, port)) {
                isConnected = true;
                waitingForConnection = false;
                statusLabel->setText("Connected to " + host + ":" + QString::number(port));
                statusLabel->setStyleSheet("QLabel { color: #55ff55; }");

                appendToOutput("Connection established successfully!");
                appendToOutput("Enter SQL commands or type 'quit' to exit");

                // 获取可用数据库列表
                std::vector<std::string> databases = client->get_all_database();
                if (!databases.empty()) {
                    appendToOutput("Available databases:");
                    for (const auto& db : databases) {
                        appendToOutput("  - " + QString::fromStdString(db),false);
                    }
                }

                showCommandPrompt();
            } else {
                isConnected = false;
                waitingForConnection = false;
                statusLabel->setText("Connection failed");
                statusLabel->setStyleSheet("QLabel { color: #ff5555; }");

                appendToOutput("Failed to connect to server.");
                showConnectionPrompt();
            }
        } else {
            // 输入中没有冒号
            appendToOutput("Error: Invalid format. Use 'host:port' (e.g., 127.0.0.1:3306)");
            waitingForConnection = false;
            showConnectionPrompt();
        }
        return;
    }

    // 处理SQL命令
    else if (isConnected) {
        QJsonArray result = client->handle_sql(input);
        displayResult(result);
        showCommandPrompt();
    }
}

void CommandLineWindow::connectToServer() {
    QString input = inputLine->text().trimmed();
    QStringList parts = input.split(":");
    QString host = parts[0];
    int port = parts[1].toInt();

    if (client->connectToServer(host, port)) {
        isConnected = true;
        waitingForConnection = false;
        statusLabel->setText("Connected to " + host + ":" + QString::number(port));
        statusLabel->setStyleSheet("QLabel { color: #55ff55; }");

        appendToOutput("Connection established successfully!");
        appendToOutput("Enter SQL commands or type 'quit' to exit");

        // 获取可用数据库列表
        std::vector<std::string> databases = client->get_all_database();
        if (!databases.empty()) {
            appendToOutput("\nAvailable databases:");
            for (const auto& db : databases) {
                appendToOutput("  - " + QString::fromStdString(db));
            }
            appendToOutput("");
        }

        showCommandPrompt();
    } else {
        isConnected = false;
        waitingForConnection = false;
        statusLabel->setText("Connection failed");
        statusLabel->setStyleSheet("QLabel { color: #ff5555; }");

        appendToOutput("Failed to connect to server.");
        showConnectionPrompt();
    }
}

void CommandLineWindow::displayResult(const QJsonArray& result) {
    if (result.isEmpty()) {
        appendToOutput("No results or error occurred.");
        return;
    }

    // 只在结果开始时添加一个时间戳
    QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss] ");
    for (int i = 0; i < result.size(); i++) {
        QJsonObject obj = result[i].toObject();

        // 获取状态信息
        QString code = obj["code"].toString();
        QString func = obj["func"].toString();

        // 输出状态信息（无时间戳）
        QString responseInfo;
        if(code=="0") {
            responseInfo = QString("success.");
        } else {
            responseInfo = QString("failed with code %1.\n").arg(code);
        }

        outputDisplay->append(responseInfo);

        // 根据函数类型处理数据
        if (func == "ShowDatabases") {
            handleShowDatabases(obj["data"].toArray());
        } else if (func == "ShowTables") {
            handleShowTables(obj["data"].toArray());
        } else if (func == "DescribeTable") {
            handleDescribeTable(obj["data"].toArray());
        } else if (func == "Select") {
            handleSelect(obj["data"].toArray());
        } else {
            handleOthers(func, obj["data"]);
        }
    }
}


void CommandLineWindow::appendToOutput(const QString& text, bool withTimestamp) {
    if (withTimestamp) {
        // 添加时间戳
        QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss] ");
        outputDisplay->append(timestamp + text);
    } else {
        // 直接输出文本，无时间戳
        outputDisplay->append(text);
    }

    // 滚动到底部
    QTextCursor cursor = outputDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    outputDisplay->setTextCursor(cursor);
}

void CommandLineWindow::showConnectionPrompt() {
    inputLine->setPlaceholderText("Enter host:port to connect");
}

void CommandLineWindow::showCommandPrompt() {
    inputLine->setPlaceholderText("Enter SQL command or 'quit'");
}

void CommandLineWindow::handleShowDatabases(const QJsonArray& data) {
    if (data.isEmpty()) {
        outputDisplay->append("No databases found.");
        return;
    }

    // 计算最长数据库名称的长度
    int maxLength = 8;  // 最小为"Database"的长度
    for (const QJsonValue& val : data) {
        maxLength = qMax(maxLength, val.toString().length());
    }

    // 构建表头和分隔线
    QString separator = "+-" + QString("-").repeated(maxLength) + "-+";

    // 输出表头
    outputDisplay->append("\n" + separator);
    outputDisplay->append("| " + QString("Database").leftJustified(maxLength) + " |");
    outputDisplay->append(separator);

    // 输出数据库列表
    for (const QJsonValue& val : data) {
        outputDisplay->append("| " + val.toString().leftJustified(maxLength) + " |");
    }

    outputDisplay->append(separator);
    outputDisplay->append(QString("%1 row(s) in set\n").arg(data.size()));
}

void CommandLineWindow::handleShowTables(const QJsonArray& data) {
    if (data.isEmpty()) {
        outputDisplay->append("No tables found.");
        return;
    }

    // 计算最长表名的长度
    int maxLength = 6;  // 最小为"Tables"的长度
    for (const QJsonValue& val : data) {
        maxLength = qMax(maxLength, val.toString().length());
    }

    // 构建表头和分隔线
    QString separator = "+-" + QString("-").repeated(maxLength) + "-+";

    // 输出表头
    outputDisplay->append("\n" + separator);
    outputDisplay->append("| " + QString("Tables").leftJustified(maxLength) + " |");
    outputDisplay->append(separator);

    // 输出表列表
    for (const QJsonValue& val : data) {
        outputDisplay->append("| " + val.toString().leftJustified(maxLength) + " |");
    }

    outputDisplay->append(separator);
    outputDisplay->append(QString("%1 row(s) in set\n").arg(data.size()));
}

void CommandLineWindow::handleDescribeTable(const QJsonArray& data) {
    if (data.isEmpty()) {
        outputDisplay->append("No columns found.");
        return;
    }

    // 确定每列的最大宽度
    int cnameWidth = 5;  // "Field"的长度
    int typeWidth = 4;   // "Type"的长度
    int csNameWidth = 10; // "Collation"的长度

    for (const QJsonValue& val : data) {
        QJsonObject column = val.toObject();
        cnameWidth = qMax(cnameWidth, column["cname"].toString().length());
        typeWidth = qMax(typeWidth, column["type"].toString().length());
        csNameWidth = qMax(csNameWidth, column["cs_name"].toString().length());
    }

    // 构建分隔线
    QString separator = "+";
    separator += QString("-").repeated(cnameWidth + 2) + "+";
    separator += QString("-").repeated(typeWidth + 2) + "+";
    separator += QString("-").repeated(csNameWidth + 2) + "+";

    // 输出表头
    outputDisplay->append("\n" + separator);

    // 构建并输出表头行
    QString headerLine = "| ";
    headerLine += QString("Field").leftJustified(cnameWidth) + " | ";
    headerLine += QString("Type").leftJustified(typeWidth) + " | ";
    headerLine += QString("Collation").leftJustified(csNameWidth) + " |";
    outputDisplay->append(headerLine);

    outputDisplay->append(separator);

    // 输出数据
    for (const QJsonValue& val : data) {
        QJsonObject column = val.toObject();
        QString cname = column["cname"].toString();
        QString type = column["type"].toString();
        QString csName = column["cs_name"].toString();

        QString rowLine = "| ";
        rowLine += cname.leftJustified(cnameWidth) + " | ";
        rowLine += type.leftJustified(typeWidth) + " | ";
        rowLine += csName.leftJustified(csNameWidth) + " |";

        outputDisplay->append(rowLine);
    }

    outputDisplay->append(separator);
    outputDisplay->append(QString("%1 row(s) in set\n").arg(data.size()));
}

void CommandLineWindow::handleSelect(const QJsonArray& data) {
    if (data.isEmpty()) {
        outputDisplay->append("Empty result set.");
        return;
    }

    // 确保至少有一行（表头）
    if (data.size() < 1) {
        outputDisplay->append("Invalid result format: missing header row.");
        return;
    }

    // 获取表头行
    QJsonArray header = data[0].toArray();
    int columnCount = header.size();

    if (columnCount == 0) {
        outputDisplay->append("No columns in result set.");
        return;
    }

    // 确定每列的最大宽度
    QVector<int> columnWidths(columnCount, 0);

    // 处理表头
    for (int col = 0; col < columnCount; col++) {
        columnWidths[col] = header[col].toString().length();
    }

    // 处理数据行
    for (int row = 1; row < data.size(); row++) {
        QJsonArray rowData = data[row].toArray();
        for (int col = 0; col < qMin(columnCount, rowData.size()); col++) {
            QString cellText;
            QJsonValue cell = rowData[col];

            if (cell.isString()) {
                cellText = cell.toString();
            } else if (cell.isDouble()) {
                double value = cell.toDouble();
                if (value == floor(value)) {
                    cellText = QString::number(static_cast<qint64>(value));
                } else {
                    cellText = QString::number(value);
                }
            } else if (cell.isBool()) {
                cellText = cell.toBool() ? "true" : "false";
            } else if (cell.isNull()) {
                cellText = "NULL";
            } else {
                cellText = "[complex value]";
            }

            columnWidths[col] = qMax(columnWidths[col], cellText.length());
        }
    }

    // 构建分隔线
    QString separator = "+";
    for (int width : columnWidths) {
        separator += QString("-").repeated(width + 2) + "+";
    }

    // 输出表头
    outputDisplay->append("\n" + separator);

    // 准备表头数据
    QStringList headerTexts;
    for (int col = 0; col < columnCount; col++) {
        headerTexts << header[col].toString();
    }

    // 输出表头行（修复）
    QString headerLine = "| ";
    for (int col = 0; col < columnCount; col++) {
        headerLine += headerTexts[col].leftJustified(columnWidths[col]) + " | ";
    }
    outputDisplay->append(headerLine);
    outputDisplay->append(separator);

    // 输出数据行（修复）
    for (int row = 1; row < data.size(); row++) {
        QJsonArray rowData = data[row].toArray();
        QStringList rowTexts;

        for (int col = 0; col < columnCount; col++) {
            QString cellText;

            if (col < rowData.size()) {
                QJsonValue cell = rowData[col];

                if (cell.isString()) {
                    cellText = cell.toString();
                } else if (cell.isDouble()) {
                    double value = cell.toDouble();
                    if (value == floor(value)) {
                        cellText = QString::number(static_cast<qint64>(value));
                    } else {
                        cellText = QString::number(value);
                    }
                } else if (cell.isBool()) {
                    cellText = cell.toBool() ? "true" : "false";
                } else if (cell.isNull()) {
                    cellText = "NULL";
                } else {
                    cellText = "[complex value]";
                }
            } else {
                cellText = "";  // 处理列数不一致的情况
            }

            rowTexts << cellText;
        }

        // 格式化行输出
        QString dataLine = "| ";
        for (int col = 0; col < columnCount; col++) {
            dataLine += rowTexts[col].leftJustified(columnWidths[col]) + " | ";
        }
        outputDisplay->append(dataLine);
    }

    outputDisplay->append(separator);
    outputDisplay->append(QString("%1 row(s) in set\n").arg(data.size() - 1));  // 减去表头行
}

void CommandLineWindow::handleOthers(const QString& funcName, const QJsonValue& data) {

    if (data.isArray() && !data.toArray().isEmpty()) {
        QJsonArray array = data.toArray();
        outputDisplay->append("Result data:");

        // 尝试以合理的方式显示数据
        for (int i = 0; i < array.size(); i++) {
            QJsonValue item = array[i];
            if (item.isObject()) {
                QJsonObject obj = item.toObject();
                QStringList keys = obj.keys();

                for (const QString& key : keys) {
                    outputDisplay->append(QString("  %1: %2").arg(key, obj[key].toVariant().toString()));
                }

                if (i < array.size() - 1) {
                    outputDisplay->append("  ---");
                }
            } else {
                outputDisplay->append(QString("  %1").arg(item.toVariant().toString()));
            }
        }
    } else if (data.isObject() && !data.toObject().isEmpty()) {
        QJsonObject obj = data.toObject();
        QStringList keys = obj.keys();

        outputDisplay->append("Result data:");
        for (const QString& key : keys) {
            outputDisplay->append(QString("  %1: %2").arg(key, obj[key].toVariant().toString()));
        }
    }
}


