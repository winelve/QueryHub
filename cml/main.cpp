#include <QDebug>


#include "parser.h"

#include <QJsonObject>
#include <QJsonDocument>

void printJs(const QJsonObject &jsonObj) {
    QJsonDocument doc(jsonObj);
    QString jsonString = doc.toJson(QJsonDocument::Indented); // 格式化输出
    qDebug().noquote() << jsonString;
}

int main() {

    Parser p;
    SqlParameters ast_root =  p.parse_sql("Ater table users");;

    printJs(ast_root);

    return 0;
}
