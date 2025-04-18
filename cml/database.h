// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>

class Table {
public:
    struct Column {
        QString name;
        QString type;
    };

    QString name;
    QVector<Column> columns;
    QVector<QMap<QString, QString>> records;
};

class Database {
public:
    Database();
    ~Database();

    // 表操作
    bool createTable(const QString& tableName, const QVector<Table::Column>& columns);
    bool dropTable(const QString& tableName);
    bool tableExists(const QString& tableName) const;
    QStringList listTables() const;

    // 数据操作
    bool insertRecord(const QString& tableName, const QMap<QString, QString>& values);
    bool updateRecords(const QString& tableName, const QMap<QString, QString>& values,
                       const QString& whereCondition);
    bool deleteRecords(const QString& tableName, const QString& whereCondition);

    // 查询操作
    QVector<QMap<QString, QString>> selectRecords(const QString& tableName,
                                                  const QStringList& columns,
                                                  const QString& whereCondition = QString());

    // 持久化操作
    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);

    // 表格打印
    void printTable(const QString& tableName) const;
    void printQueryResult(const QVector<QMap<QString, QString>>& records,
                          const QStringList& columns) const;

private:
    bool evaluateCondition(const QString& condition, const QMap<QString, QString>& record);

    QMap<QString, Table> tables;
};

#endif // DATABASE_H
