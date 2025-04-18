// database.cpp
#include "database.h"

Database::Database() {
    // qDebug() << "Database instance created";
}

Database::~Database() {
    // qDebug() << "Database instance destroyed";
}


bool Database::evaluateCondition(const QString& condition, const QMap<QString, QString>& record) {
    if (condition.isEmpty()) {
        return true; // No condition means all records match
    }

    // Parse the condition string that comes from the interpreter
    // Format is typically "column = value" or "column > value"
    QStringList parts = condition.split(" ");
    if (parts.size() >= 3) {
        QString column = parts[0];
        QString op = parts[1];
        QString value = parts[2];

        // Remove any quotes from the value
        if (value.startsWith("'") && value.endsWith("'")) {
            value = value.mid(1, value.length() - 2);
        }

        if (!record.contains(column)) {
            return false;
        }

        QString recordValue = record[column];

        if (op == "=") {
            return recordValue == value;
        } else if (op == ">") {
            bool ok1, ok2;
            double recordNum = recordValue.toDouble(&ok1);
            double valueNum = value.toDouble(&ok2);
            return ok1 && ok2 && recordNum > valueNum;
        } else if (op == "<") {
            bool ok1, ok2;
            double recordNum = recordValue.toDouble(&ok1);
            double valueNum = value.toDouble(&ok2);
            return ok1 && ok2 && recordNum < valueNum;
        }
    }

    return false; // Unrecognized condition format
}

bool Database::updateRecords(const QString& tableName, const QMap<QString, QString>& values,
                             const QString& whereCondition) {
    if (!tableExists(tableName)) {
        qDebug() << "Cannot update table" << tableName << ": table does not exist";
        return false;
    }

    int updatedCount = 0;
    for (auto& record : tables[tableName].records) {
        if (evaluateCondition(whereCondition, record)) {
            // Only update records that match the condition
            for (auto it = values.begin(); it != values.end(); ++it) {
                record[it.key()] = it.value();
            }
            updatedCount++;
        }
    }

    qDebug() << "Updated" << updatedCount << "records in table" << tableName
             << "where" << whereCondition;
    return updatedCount > 0;
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
    // qDebug() << "Table" << tableName << "dropped";
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

bool Database::deleteRecords(const QString& tableName, const QString& whereCondition) {
    if (!tableExists(tableName)) {
        qDebug() << "Cannot delete from table" << tableName << ": table does not exist";
        return false;
    }

    int initialSize = tables[tableName].records.size();

    // Remove records that match the condition
    QVector<QMap<QString, QString>> remainingRecords;
    for (const auto& record : tables[tableName].records) {
        if (!evaluateCondition(whereCondition, record)) {
            remainingRecords.append(record);
        }
    }

    int deletedCount = initialSize - remainingRecords.size();
    tables[tableName].records = remainingRecords;

    qDebug() << "Deleted" << deletedCount << "records from table" << tableName
             << "where" << whereCondition;
    return deletedCount > 0;
}

QVector<QMap<QString, QString>> Database::selectRecords(const QString& tableName,
                                                        const QStringList& columns,
                                                        const QString& whereCondition) {
    QVector<QMap<QString, QString>> results;

    if (!tableExists(tableName)) {
        qDebug() << "Cannot select from table" << tableName << ": table does not exist";
        return results;
    }

    for (const auto& record : tables[tableName].records) {
        // Only include records that match the WHERE condition
        if (evaluateCondition(whereCondition, record)) {
            QMap<QString, QString> resultRecord;

            // If SELECT *, return all columns
            if (columns.size() == 1 && columns[0] == "*") {
                resultRecord = record;
            } else {
                // Otherwise return only specified columns
                for (const auto& column : columns) {
                    if (record.contains(column)) {
                        resultRecord[column] = record[column];
                    }
                }
            }

            results.append(resultRecord);
        }
    }

    qDebug() << "Selected" << results.size() << "records from table" << tableName
             << "where" << whereCondition;
    return results;
}

// database.cpp 新增函数实现

QStringList Database::listTables() const {
    return tables.keys();
}

bool Database::saveToFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open file for writing:" << filename;
        return false;
    }

    QJsonObject dbObject;
    QJsonArray tablesArray;

    // 遍历所有表
    for (auto it = tables.begin(); it != tables.end(); ++it) {
        QJsonObject tableObject;
        tableObject["name"] = it.key();

        // 保存列信息
        QJsonArray columnsArray;
        for (const auto& column : it.value().columns) {
            QJsonObject columnObject;
            columnObject["name"] = column.name;
            columnObject["type"] = column.type;
            columnsArray.append(columnObject);
        }
        tableObject["columns"] = columnsArray;

        // 保存记录
        QJsonArray recordsArray;
        for (const auto& record : it.value().records) {
            QJsonObject recordObject;
            for (auto recordIt = record.begin(); recordIt != record.end(); ++recordIt) {
                recordObject[recordIt.key()] = recordIt.value();
            }
            recordsArray.append(recordObject);
        }
        tableObject["records"] = recordsArray;

        tablesArray.append(tableObject);
    }

    dbObject["tables"] = tablesArray;

    QJsonDocument doc(dbObject);
    file.write(doc.toJson());
    file.close();

    // qDebug() << "Database saved to" << filename;
    return true;
}

bool Database::loadFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file for reading:" << filename;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Failed to parse JSON from file:" << filename;
        return false;
    }

    QJsonObject dbObject = doc.object();
    if (!dbObject.contains("tables") || !dbObject["tables"].isArray()) {
        qDebug() << "Invalid database file format";
        return false;
    }

    // 清空现有表
    tables.clear();

    // 加载所有表
    QJsonArray tablesArray = dbObject["tables"].toArray();
    for (const auto& tableValue : tablesArray) {
        QJsonObject tableObject = tableValue.toObject();
        Table table;

        table.name = tableObject["name"].toString();

        // 加载列信息
        QJsonArray columnsArray = tableObject["columns"].toArray();
        for (const auto& columnValue : columnsArray) {
            QJsonObject columnObject = columnValue.toObject();
            Table::Column column;
            column.name = columnObject["name"].toString();
            column.type = columnObject["type"].toString();
            table.columns.append(column);
        }

        // 加载记录
        QJsonArray recordsArray = tableObject["records"].toArray();
        for (const auto& recordValue : recordsArray) {
            QJsonObject recordObject = recordValue.toObject();
            QMap<QString, QString> record;

            for (auto it = recordObject.begin(); it != recordObject.end(); ++it) {
                record[it.key()] = it.value().toString();
            }

            table.records.append(record);
        }

        tables[table.name] = table;
    }

    qDebug() << "Database loaded from" << filename << "with" << tables.size() << "tables";
    return true;
}

void Database::printTable(const QString& tableName) const {
    if (!tableExists(tableName)) {
        qDebug() << "Cannot print table" << tableName << ": table does not exist";
        return;
    }

    const Table& table = tables[tableName];

    // 获取所有列名
    QStringList columnNames;
    for (const auto& column : table.columns) {
        columnNames.append(column.name);
    }

    // 打印表格
    printQueryResult(table.records, columnNames);
}

void Database::printQueryResult(const QVector<QMap<QString, QString>>& records,
                                const QStringList& columns) const {
    if (records.isEmpty() || columns.isEmpty()) {
        qDebug() << "No data to print";
        return;
    }

    // 计算每列的最大宽度
    QMap<QString, int> columnWidths;

    // 初始化列宽为列名的长度
    for (const auto& columnName : columns) {
        columnWidths[columnName] = columnName.length();
    }

    // 找出每列数据的最大宽度
    for (const auto& record : records) {
        for (const auto& columnName : columns) {
            if (record.contains(columnName)) {
                int valueLength = record[columnName].length();
                if (valueLength > columnWidths[columnName]) {
                    columnWidths[columnName] = valueLength;
                }
            }
        }
    }

    QString horizontalLine = "+";
    QString headerLine = "|";

    // 构建表头分隔线和表头
    for (const auto& columnName : columns) {
        int width = columnWidths[columnName];
        horizontalLine += QString("-").repeated(width + 2) + "+";
        headerLine += " " + columnName.leftJustified(width) + " |";
    }

    // 打印表格
    QTextStream out(stdout);
    out << horizontalLine << "\n";
    out << headerLine << "\n";
    out << horizontalLine << "\n";

    // 打印记录
    for (const auto& record : records) {
        QString recordLine = "|";

        for (const auto& columnName : columns) {
            int width = columnWidths[columnName];
            QString value = record.value(columnName, "");
            recordLine += " " + value.leftJustified(width) + " |";
        }

        out << recordLine << "\n";
    }

    out << horizontalLine << "\n";
    out.flush();
}
