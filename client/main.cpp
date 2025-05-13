// #include "client.h"

#include <QCoreApplication>
#include <QDebug>
#include <QApplication>
#include "commandlinewindow.h"

void PrintStringTable(const std::vector<std::vector<std::string>>& tableData) {
    if (tableData.empty()) {
        qDebug() << "No records found.";
        return;
    }

    // 字段名称（第一行）
    const std::vector<std::string>& headers = tableData[0];

    // 计算每列的最大宽度（至少为字段名长度）
    std::vector<size_t> columnWidths(headers.size(), 0);
    for (size_t col = 0; col < headers.size(); ++col) {
        columnWidths[col] = headers[col].length();
    }

    // 检查数据行并计算最大宽度
    for (size_t row = 1; row < tableData.size(); ++row) {
        if (tableData[row].size() != headers.size()) {
            qWarning() << "Row" << row << "has incorrect column count. Skipping.";
            continue;
        }
        for (size_t col = 0; col < headers.size(); ++col) {
            columnWidths[col] = std::max(columnWidths[col], tableData[row][col].length());
        }
    }

    // 准备输出
    QString outputStr;
    QTextStream out(&outputStr);

    // 打印分隔线
    auto printSeparator = [&]() {
        out << "+";
        for (size_t width : columnWidths) {
            out << QString(width + 2, '-') << "+";
        }
        out << "\n";
    };

    // 打印表头
    printSeparator();
    out << "|";
    for (size_t col = 0; col < headers.size(); ++col) {
        out << " " << QString::fromStdString(headers[col]).leftJustified(columnWidths[col]) << " |";
    }
    out << "\n";
    printSeparator();

    // 打印数据行
    for (size_t row = 1; row < tableData.size(); ++row) {
        out << "|";
        for (size_t col = 0; col < headers.size(); ++col) {
            out << " " << QString::fromStdString(tableData[row][col]).leftJustified(columnWidths[col]) << " |";
        }
        out << "\n";
    }
    printSeparator();

    // 打印记录数
    out << "Total " << (tableData.size() - 1) << " record(s)\n";

    // 输出结果
    qDebug().noquote() << outputStr;
}


// int main(int argc, char *argv[]) {
//     QCoreApplication app(argc, argv); // Initialize Qt event loop

//     QString server_ip = "127.0.0.1";
//     int server_port = 8080;
//     qDebug() << "Server IP: " << server_ip;
//     qDebug() << "Server Port: " << server_port;

//     Client client;
//     if (client.connectToServer(server_ip, server_port)) {
//         qDebug() << "Successfully connected to server";
//     } else {
//         qDebug() << "Failed to connect to server";
//         return 1; // Exit if connection fails
//     }

//     // qDebug() << client.handle_sql("SHOW DATABASES;");
//     // qDebug() << "\n";
//     // qDebug() << client.handle_sql("USE test_db;");
//     // qDebug() << "\n";
//     // qDebug() << client.handle_sql("SHOW TABLES;");
//     // qDebug() << "recived data: " << client.select_table("test_db","orders");


//     return app.exec(); // Run the event loop
// }

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CommandLineWindow window;
    window.show();

    return app.exec();
}


