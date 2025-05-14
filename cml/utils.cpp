#include "all_constraints.h"
#include "constants.h"
#include "sqltool.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

void printJs(const QJsonObject &jsonObj) {
    QJsonDocument doc(jsonObj);
    QString jsonString = doc.toJson(QJsonDocument::Indented); // 格式化输出
    qDebug().noquote() << jsonString;
}


Constraint* CreateConstraint(const QString& tb_name,const QString& cs_type,const QString& cname,const QJsonArray& csparams) {
    QString constraint_name = tb_name+ "_"+ cname + "_" + cs_type;
    if(cs_type=="not_null") {
        return new NotNullConstraint(cname.toStdString(),constraint_name.toStdString());
    }

    else if (cs_type=="default") {
        return new DefaultConstraint(cname.toStdString(),constraint_name.toStdString(),csparams[0].toString());
    }

    else if (cs_type=="unique") {
        return new UniqueConstraint(cname.toStdString(),constraint_name.toStdString());
    }

    else if (cs_type=="primary_key") {
        return new PrimaryKeyConstraint(cname.toStdString(),constraint_name.toStdString());
    }

    else if (cs_type=="foregin_key") {
        return new ForeignKeyConstraint(cname.toStdString(),constraint_name.toStdString(),csparams[0].toString().toStdString(),csparams[1].toString().toStdString());
    }
    return nullptr;
}

void printStdV(const std::vector<std::string>& sv) {
    QString output;
    for (const auto& str : sv) {
        output += QString::fromStdString(str) + ", ";
    }
    output.chop(1); // 移除最后一个多余的制表符
    qDebug() << output;
}

int map_operator(const QString& op) {
    if (op=="<=") {
        return sLessEqualCondition;
    }
    else if (op==">=") {
        return sLargerEqualCondition;
    }
    else if (op=="=") {
        return sEqualCondition;
    }
    else if (op=="<") {
        return sLessCondition;
    }
    else if (op==">") {
        return sLargerCondition;
    }
    else if (op=="!=") {
        return sNotEqualCondition;
    }
    return -1;
}


void PrintSelectRes(const std::vector<std::vector<std::any>>& returnRecords) {
    if (returnRecords.empty()) {
        qDebug() << "No records found.";
        return;
    }

    // 字段名称（第一行）
    const std::vector<std::any>& headers = returnRecords[0];

    // 计算每列的最大宽度
    std::vector<size_t> columnWidths(headers.size(), 0);
    for (size_t col = 0; col < headers.size(); ++col) {
        // 初始化列宽为字段名长度
        columnWidths[col] = sqlTool::AnyToString(headers[col]).length();
    }

    // 计算数据列的最大宽度
    for (size_t row = 1; row < returnRecords.size(); ++row) {
        for (size_t col = 0; col < headers.size(); ++col) {
            size_t currentWidth = sqlTool::AnyToString(returnRecords[row][col]).length();
            columnWidths[col] = std::max(columnWidths[col], currentWidth);
        }
    }

    // 准备输出字符串
    QString outputStr;
    QTextStream out(&outputStr);

    // 输出分隔线
    auto printSeparator = [&]() {
        for (size_t width : columnWidths) {
            out << "+" << QString(width + 2, '-');
        }
        out << "+" << Qt::endl;
    };

    // 打印表头
    printSeparator();
    out << "|";
    for (size_t col = 0; col < headers.size(); ++col) {
        QString header = QString::fromStdString(sqlTool::AnyToString(headers[col]));
        out << " " << QString("%1").arg(header, -static_cast<int>(columnWidths[col])) << " |";
    }
    out << Qt::endl;
    printSeparator();

    // 打印数据行
    for (size_t row = 1; row < returnRecords.size(); ++row) {
        out << "|";
        for (size_t col = 0; col < headers.size(); ++col) {
            QString cellValue = QString::fromStdString(sqlTool::AnyToString(returnRecords[row][col]));
            out << " " << QString("%1").arg(cellValue, -static_cast<int>(columnWidths[col])) << " |";
        }
        out << Qt::endl;
    }
    printSeparator();

    // 打印总记录数
    out << "Total " << (returnRecords.size() - 1) << " record(s)" << Qt::endl;

    // 使用 qDebug() 输出
    qDebug().noquote() << outputStr;
}


QJsonDocument makeJsDocument(const QString& code,const QString& func,const QJsonArray& data) {
    QJsonObject send_data;
    send_data["code"] = code;
    send_data["func"] = func;
    send_data["data"] = data;
    return QJsonDocument(send_data);
}

QJsonDocument makeJsDocument(const int code,const QString& func,const QJsonArray& data) {
    QJsonObject send_data;
    send_data["code"] = QString::number(code);
    send_data["func"] = func;
    send_data["data"] = data;
    return QJsonDocument(send_data);
}

