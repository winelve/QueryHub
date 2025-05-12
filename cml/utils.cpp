#include "all_constraints.h"

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

