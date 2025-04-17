// database.cpp
#include "database.h"

Database::Database() {
    qDebug() << "Database instance created";
}

Database::~Database() {
    qDebug() << "Database instance destroyed";
}

bool Database::createTable(const QString& tableName, const QVector<Table::Column>& columns) {
    if (tableExists(tableName)) {
        qDebug() << "Table" << tableName << "already exists";
        return false;
    }

    Table table;
    table.name = tableName;
    table.columns = columns;

    tables[tableName] = table;
    qDebug() << "Table" << tableName << "created with" << columns.size() << "columns";
    return true;
}

bool Database::dropTable(const QString& tableName) {
    if (!tableExists(tableName)) {
        qDebug() << "Cannot drop table" << tableName << ": table does not exist";
        return false;
    }

    tables.remove(tableName);
    qDebug() << "Table" << tableName << "dropped";
    return true;
}

bool Database::tableExists(const QString& tableName) const {
    return tables.contains(tableName);
}

bool Database::insertRecord(const QString& tableName, const QMap<QString, QString>& values) {
    if (!tableExists(tableName)) {
        qDebug() << "Cannot insert into table" << tableName << ": table does not exist";
        return false;
    }

    tables[tableName].records.append(values);
    qDebug() << "Inserted record into table" << tableName;
    return true;
}

bool Database::updateRecords(const QString& tableName, const QMap<QString, QString>& values,
                             const QString& whereCondition) {
    if (!tableExists(tableName)) {
        qDebug() << "Cannot update table" << tableName << ": table does not exist";
        return false;
    }

    // 实际应该解析whereCondition并选择性更新
    // 这里简化实现，更新所有记录
    int updatedCount = 0;
    for (auto& record : tables[tableName].records) {
        for (auto it = values.begin(); it != values.end(); ++it) {
            record[it.key()] = it.value();
        }
        updatedCount++;
    }

    qDebug() << "Updated" << updatedCount << "records in table" << tableName
             << "where" << whereCondition;
    return true;
}

bool Database::deleteRecords(const QString& tableName, const QString& whereCondition) {
    if (!tableExists(tableName)) {
        qDebug() << "Cannot delete from table" << tableName << ": table does not exist";
        return false;
    }

    // 实际应该解析whereCondition并选择性删除
    // 这里简化实现，清空所有记录
    int deletedCount = tables[tableName].records.size();
    tables[tableName].records.clear();

    qDebug() << "Deleted" << deletedCount << "records from table" << tableName
             << "where" << whereCondition;
    return true;
}

QVector<QMap<QString, QString>> Database::selectRecords(const QString& tableName,
                                                        const QStringList& columns,
                                                        const QString& whereCondition) {
    QVector<QMap<QString, QString>> results;

    if (!tableExists(tableName)) {
        qDebug() << "Cannot select from table" << tableName << ": table does not exist";
        return results;
    }

    // 实际应该解析whereCondition并选择性返回
    // 这里简化实现，返回所有记录的指定列
    for (const auto& record : tables[tableName].records) {
        QMap<QString, QString> resultRecord;

        // 如果是SELECT *，返回所有列
        if (columns.size() == 1 && columns[0] == "*") {
            resultRecord = record;
        } else {
            // 否则只返回指定列
            for (const auto& column : columns) {
                if (record.contains(column)) {
                    resultRecord[column] = record[column];
                }
            }
        }

        results.append(resultRecord);
    }

    qDebug() << "Selected" << results.size() << "records from table" << tableName
             << "where" << whereCondition;
    return results;
}
