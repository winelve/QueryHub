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


Constraint* CreateConstraint(const QString& cs_type,const QString& cname,const QJsonArray& csparams) {
    if(cs_type=="not_null") {
        return new NotNullConstraint(cname.toStdString(),cs_type.toStdString());
    }

    else if (cs_type=="default") {
        return new DefaultConstraint(cname.toStdString(),cs_type.toStdString(),csparams[0].toString());
    }

    else if (cs_type=="unique") {
        return new UniqueConstraint(cname.toStdString(),cs_type.toStdString());
    }

    else if (cs_type=="primary_key") {
        return new PrimaryKeyConstraint(cname.toStdString(),cs_type.toStdString());
    }

    else if (cs_type=="foregin_key") {
        return new ForeignKeyConstraint(cname.toStdString(),cs_type.toStdString(),csparams[0].toString().toStdString(),csparams[1].toString().toStdString());
    }
    return nullptr;
}
