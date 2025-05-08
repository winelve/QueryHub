#ifndef SQLPARAMETERS_H
#define SQLPARAMETERS_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

class SqlParameters : public QJsonObject
{
public:
    SqlParameters();

    void add_str(const QString& key,const QJsonValue& value);
    void add_dict(const QString& key,const QJsonObject& dict);
    void add_array();

};

#endif // SQLPARAMETERS_H
