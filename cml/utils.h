#ifndef UTILS_H
#define UTILS_H

#include <QJsonObject>
#include <QJsonDocument>

void printJs(const QJsonObject &jsonObj) {
    QJsonDocument doc(jsonObj);
    QString jsonString = doc.toJson(QJsonDocument::Indented); // 格式化输出
    qDebug().noquote() << jsonString;
}



#endif // UTILS_H




