// IDBMSInterface.h
#pragma once
#include <QObject>
#include <QStringList>
#include <QVariant>

class IDBMSInterface : public QObject
{
    Q_OBJECT
public:
    // 连接状态枚举（无需.cpp，Q_ENUM会自动生成元数据代码）
    enum ConnectionState {
        Disconnected = 0,
        Connecting,
        Connected,
        Error
    };
    Q_ENUM(ConnectionState)

    explicit IDBMSInterface(QObject *parent = nullptr) : QObject(parent) {}

    // 纯虚函数（不需要默认实现）
    virtual void connectToDB(const QString &config) = 0;
    virtual void disconnect() = 0;
    virtual void fetchDatabases() = 0;
    virtual void fetchTables(const QString &dbName) = 0;
    virtual void fetchTableStructure(const QString &dbName,
                             const QString &tableName) = 0;
    virtual void executeQuery(const QString &query) = 0;
    virtual void fetchTableData(const QString &dbName,
                        const QString &tableName,
                        int limit,
                        int offset) = 0;

    virtual void beginTransaction() = 0;
    virtual void commitTransaction()  = 0;
    virtual void rollbackTransaction() = 0;


signals:
    // 信号只需声明
    void connectionStateChanged(ConnectionState state, const QString &msg);
    void databasesFetched(const QStringList &databases);


protected:
    // 内联工具方法（直接实现在头文件中）
    QVariant::Type convertType(int dbmsType) const {
        static const QHash<int, QVariant::Type> typeMap = {
            {0, QVariant::String},
            {1, QVariant::Int},

        };
        return typeMap.value(dbmsType, QVariant::String);
    }
};
