#ifndef SQLCMD_H
#define SQLCMD_H

#include <QString>
#include <QRegularExpression>
#include <QStringLiteral>

#include "sqlparameters.h"

class BaseCmdParser {
public:
    virtual ~BaseCmdParser() = default;  // Virtual destructor for proper polymorphism
    virtual QJsonObject parseCMD(const QString &sql) = 0;  // Corrected method name (parser -> parse)
};

class AlterParser:public BaseCmdParser {
public :
    QJsonObject parseCMD(const QString &sql) override;
    QList<QString> test_alter_re = {
        "ALTER DATABASE my_db RENAME TO new_db;",
        "ALTER TABLE users ADD age INT NOT NULL;",
        "ALTER TABLE users MODIFY name VARCHAR;",
        "ALTER TABLE users RENAME COLUMN name TO full_name;",
        "ALTER TABLE users DROP email CHAR;",
        "ALTER TABLE users DROP email;",
        "ALTER TABLE users RENAME TO user;",
    };
private:
    const QRegularExpression alterDbRenamePattern{
                                                  QStringLiteral("^ALTER\\s+DATABASE\\s+(\\w+)\\s+RENAME\\s+TO\\s+(\\w+)\\s*;"),
                                                  QRegularExpression::CaseInsensitiveOption};
    const QRegularExpression alterTableAddPattern{
                                                  QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+ADD\\s+(\\w+)\\s+(\\w+)\\s*([^\\s;][^;]*?)?\\s*;"),
                                                  QRegularExpression::CaseInsensitiveOption};
    const QRegularExpression alterTableModifyPattern{
                                                     QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+MODIFY\\s+(\\w+)\\s+(\\w+)\\s*;"),
                                                     QRegularExpression::CaseInsensitiveOption};
    const QRegularExpression alterTableRenameColumnPattern{
                                                           QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+RENAME\\s+COLUMN\\s+(\\w+)\\s+TO\\s+(\\w+)\\s*;"),
                                                           QRegularExpression::CaseInsensitiveOption};
    const QRegularExpression alterTableDropPattern{
                                                   QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+DROP\\s+(\\w+)\\s*(\\w+)?\\s*;"),
                                                   QRegularExpression::CaseInsensitiveOption};
    const QRegularExpression alterTableRenamePattern{
                                                     QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+RENAME\\s+TO\\s+(\\w+)\\s*;"),
                                                     QRegularExpression::CaseInsensitiveOption};
};
//---------------
class CreateParser:public BaseCmdParser
{
public:
    QJsonObject parseCMD(const QString &sql) override;
    QList<QString> test_create_re = {
        "CREATE DATABASE my_db;",
        "CREATE TABLE users (id INT PRIMARY KEY AUTO_INCREMENT,name VARCHAR NOT NULL DEFAULT 'unknown',email CHAR);"
    };
private:
    const QRegularExpression createDbPattern{QStringLiteral("^CREATE\\s+DATABASE\\s+(\\w+)\\s*;"),
                                             QRegularExpression::CaseInsensitiveOption};
    const QRegularExpression createTablePattern{QStringLiteral("^CREATE\\s+TABLE\\s+(\\w+)\\s*\\(\\s*([\\s\\S]*?)\\s*\\)\\s*;"),
                                                QRegularExpression::CaseInsensitiveOption};
    const QRegularExpression columnPattern{QStringLiteral("^(\\w+)\\s+(\\w+)\\s*([^\\s,][^,]*?)?(?=\\s*,|\\s*$)"),
                                           QRegularExpression::CaseInsensitiveOption};
};

class DropParser:public BaseCmdParser{
public:
    QJsonObject parseCMD(const QString& sql) override;

    QList<QString> test_drop_re = {
        "DROP TABLE users;",
        "DROP DATABASE my_db;"
    };
private:
    const QRegularExpression dropTablePattern{ QStringLiteral("^DROP\\s+TABLE\\s+(\\w+)\\s*;"),
                                              QRegularExpression::CaseInsensitiveOption};
    const QRegularExpression dropDatabasePattern{ QStringLiteral("^DROP\\s+DATABASE\\s+(\\w+)\\s*;"),
                                                 QRegularExpression::CaseInsensitiveOption};
};

#endif // SQLCMD_H
