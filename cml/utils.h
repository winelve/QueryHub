#ifndef UTILS_H
#define UTILS_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

void printJs(const QJsonObject &jsonObj) {
    QJsonDocument doc(jsonObj);
    QString jsonString = doc.toJson(QJsonDocument::Indented); // 格式化输出
    qDebug().noquote() << jsonString;
}

void test_read_json() {

    QFile file("../../cml/res/params.json");
    QFileInfo fileInfo(file);
    QString absoluteFilePath = fileInfo.absoluteFilePath();
    qDebug()  << absoluteFilePath;


    file.open(QFile::ReadOnly);

    QByteArray all = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(all);

    if(doc.isObject()) {
        printJs(doc.object());
    }
    else {
        qDebug() << "Not object of js.";
    }

}




#endif // UTILS_H




