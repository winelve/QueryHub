#ifndef SQLCMD_H
#define SQLCMD_H

#include <QString>
#include <QRegularExpression>
#include <QStringLiteral>


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
        "ALTER TABLE users ADD age INT NOT_NULL;",
        "ALTER TABLE users MODIFY name VARCHAR;",
        "ALTER TABLE users RENAME COLUMN name TO full_name;",
        "ALTER TABLE users DROP email CHAR;",
        "ALTER TABLE users DROP email;",
        "ALTER TABLE users RENAME TO user;",
        "ALTER TABLE users ADD CONSTRAINT name PRIMARY_KEY(id);",
        "ALTER TABLE users DELETE CONSTRAINT name PRIMARY_KEY;"
    };
private:
    const QRegularExpression alterDbRenamePattern{
        QStringLiteral("^ALTER\\s+DATABASE\\s+(\\w+)\\s+RENAME\\s+TO\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression alterTableAddPattern{
        QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+ADD\\s+(?!CONSTRAINT\\s+)(\\w+)\\s+(\\w+)\\s*((?:\\s+\\w+(?:\\([^)]*\\))?)*?)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression alterTableModifyPattern{
        QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+MODIFY\\s+(\\w+)\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression alterTableRenameColumnPattern{
        QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+RENAME\\s+COLUMN\\s+(\\w+)\\s+TO\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression alterTableDropPattern{
        QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+DROP\\s+(\\w+)\\s*(\\w+)?\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression alterTableRenamePattern{
        QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+RENAME\\s+TO\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression constraintPattern{
        QStringLiteral("\\b(\\w+)(?:\\(([^)]+)\\))?"),
        QRegularExpression::CaseInsensitiveOption
    };

    // 新增：Add Constraint 模式
    const QRegularExpression alterTableAddConstraintPattern{
        QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+ADD\\s+CONSTRAINT\\s+(\\w+)\\s+(\\w+)(?:\\(([^)]+)\\))?\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    // 新增：Delete Constraint 模式
    const QRegularExpression alterTableDeleteConstraintPattern{
        QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+DELETE\\s+CONSTRAINT\\s+(\\w+)\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
};

//---------------
class CreateParser:public BaseCmdParser
{
public:
    QJsonObject parseCMD(const QString &sql) override;
    QList<QString> test_create_re = {
        "CREATE DATABASE my_db;",
        "CREATE TABLE users (id INT PRIMARY_KEY AUTO_INCREMENT,name VARCHAR NOT_NULL DEFAULT(unknown),email CHAR);"
    };
private:
    const QRegularExpression createDbPattern{
        QStringLiteral("^CREATE\\s+DATABASE\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression createIndexPattern{
        QStringLiteral("^CREATE\\s+INDEX\\s+ON\\s+(\\w+)\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    // const QRegularExpression createTablePattern{
    //     QStringLiteral("^CREATE\\s+TABLE\\s+(\\w+)\\s*\\(\\s*([\\s\\S]*?)\\s*\\)\\s*;"),
    //     QRegularExpression::CaseInsensitiveOption
    // };

    // const QRegularExpression columnPattern{
    //     QStringLiteral("^(\\w+)\\s+(\\w+)\\s*((?:\\s+\\w+(?:\\([^)]*\\))?)*?)(?=\\s*,|\\s*$)"),
    //     QRegularExpression::CaseInsensitiveOption
    // };

    // // 可以添加一个新的正则表达式来匹配单个约束
    // const QRegularExpression constraintPattern{
    //     QStringLiteral("\\b(\\w+)(?:\\(([^)]+)\\))?"),
    //     QRegularExpression::CaseInsensitiveOption
    // };
    //-------------------------------------------------------
    // 第一个正则保持不变
    const QRegularExpression createRegex{
        QStringLiteral("^CREATE TABLE\\s+(?<tableName>\\w+)\\s*\\((?<columnsDefinitionStr>.*?)\\)\\s*;$"),
        QRegularExpression::CaseInsensitiveOption // 忽略大小写
    };

    // 修改第二个正则，处理新的外键语法
    const QRegularExpression columnPattern{
        QStringLiteral("^\\s*(?<colName>\\w+)\\s+(?<colType>\\w+)(?<constraintsStr>(?:\\s+(?:primary_key|foregin_key\\s*\\([^)]*\\)|default\\s*\\([^)]*\\)|not_null|unique))*)\\s*$")
    };

    // 修改第三个正则，处理新的外键语法（没有逗号）
    const QRegularExpression constraintPattern{
        QStringLiteral("(?<pk>primary_key)|"                                          // primary_key
                       "(?<fk>foregin_key)\\s*\\(\\s*(?<fkTable>\\w+)\\s+(?<fkColumn>\\w+)\\s*\\)|" // foregin_key(ref_tb ref_column)
                       "(?<def>default)\\s*\\(\\s*(?<defValue>[^)]+)\\s*\\)|"         // default(value) - value 可以是 'string' 或数字等
                       "(?<nn>not_null)|"                                             // not_null
                       "(?<uq>unique)")                                               // unique
    };


};

class DropParser:public BaseCmdParser{
public:
    QJsonObject parseCMD(const QString& sql) override;

    QList<QString> test_drop_re = {
        "DROP TABLE users;",
        "DROP DATABASE my_db;"
    };
private:
    const QRegularExpression dropTablePattern{
        QStringLiteral("^DROP\\s+TABLE\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression dropDatabasePattern{
        QStringLiteral("^DROP\\s+DATABASE\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression dropIndexPattern{
        QStringLiteral("^DROP\\s+INDEX\\s+ON\\s+(\\w+)\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

};

class DMLParser: public BaseCmdParser {
public:
    QJsonObject parseCMD(const QString &sql) override;

private:
    // ----- INSERT -----
    const QRegularExpression insertPattern{
        QStringLiteral("^INSERT\\s+INTO\\s+(\\w+)\\s*\\(([^)]+)\\)\\s+VALUES\\s+([\\s\\S]*?);$"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression valuesTuplePattern{
        QStringLiteral("\\(([^)]+)\\)")
    };


    // ---- SELECT  ----
    const QRegularExpression selectPattern{
        QStringLiteral(
            "^SELECT\\s+"                   // SELECT
            "\\(([^)]+)\\)"                 // (columns) - Group 1
            "\\s+FROM\\s+"                  // FROM
            "(\\w+)"                        // table - Group 2
            "(?:\\s+WHERE\\s+\\(([^)]+)\\))?" // Optional WHERE (...) - Group 3
            "(?:\\s+JOIN\\s+\\(([^)]+)\\))?"  // Optional JOIN (...) - Group 4
            "(?:\\s+ORDER\\s+BY\\s+\\(([^)]+)\\))?" // Optional ORDER BY (...) - Group 5
            "\\s*;$"                        // Optional whitespace and semicolon at end
            ),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression andpattern{
        QStringLiteral("\\s+(AND|OR)\\s+"),
        QRegularExpression::CaseInsensitiveOption}
    ;

    const QRegularExpression whereConditionPattern{
        QStringLiteral(
            "([a-zA-Z0-9_]+)\\s*"           // 左侧操作数 (字母、数字、下划线)
            "([><=!]=?)"                    // 比较运算符
            "\\s*([a-zA-Z0-9_]+|\\d+)"     // 右侧操作数 (字母、数字、下划线或数字)
            )
    };

    // ----- UPDATE -----
    const QRegularExpression updatePattern{
        QStringLiteral(
            "^UPDATE\\s+"                   // UPDATE
            "(\\w+)\\s+"                    // table name - Group 1
            "SET\\s+\\(([^)]+)\\)"          // SET (c1=v1,c2=v2,...) - Group 2
            "(?:\\s+WHERE\\s+\\(([^)]+)\\))?" // Optional WHERE (...) - Group 3
            "\\s*;$"                        // Optional whitespace and semicolon at end
            ),
        QRegularExpression::CaseInsensitiveOption
    };

    // For parsing SET clause key-value pairs
    const QRegularExpression setValuePattern{
        QStringLiteral(
            "([a-zA-Z0-9_]+)\\s*=\\s*([a-zA-Z0-9_]+|\\d+)"  // column=value
            )
    };

    // ----- DELETE -----
    const QRegularExpression deletePattern{
        QStringLiteral(
            "^DELETE\\s+"                   // DELETE 关键字
            "(\\w+)\\s+"                    // 表名 - 捕获组 1
            "(?:WHERE\\s+\\(([^)]+)\\))?"   // 可选的 WHERE (...) - 捕获组 2
            "\\s*;$"                        // 可选的空格和结尾分号
            ),
        QRegularExpression::CaseInsensitiveOption
    };

};

class OtherCmdParser : public BaseCmdParser
{
public:
    QJsonObject parseCMD(const QString &sql) override;
    QList<QString> test_other_re = {
        "USE my_db;",
        "SHOW DATABASES;",
        "SHOW TABLES;",
        "DESCRIBE users;"
    };
private:
    const QRegularExpression useDbPattern{
        QStringLiteral("^USE\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression showDatabasesPattern{
        QStringLiteral("^SHOW\\s+DATABASES\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression showTablesPattern{
        QStringLiteral("^SHOW\\s+TABLES\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpression describeTablePattern{
        QStringLiteral("^DESC\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
    //--------------------------最后10个-----------------------------------
    const QRegularExpression commitPattern{
        QStringLiteral("^COMMIT\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression rollbackPattern{
        QStringLiteral("^ROLLBACK\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression loginPattern{
        QStringLiteral("^LOGIN\\s+(\\w+)\\s+P\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression registerPattern{
        QStringLiteral("^REGISTER\\s+(\\w+)\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression dropIndexPattern{
        QStringLiteral("^DROP\\s+INDEX\\s+ON\\s+(\\w+)\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression grantOwnerPattern{
        QStringLiteral("^GRANT\\s+OWNER\\s+ON\\s+(\\w+)\\s+TO\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression revokeOwnerPattern{
        QStringLiteral("^REVOKE\\s+OWNER\\s+ON\\s+(\\w+)(?:\\s+FROM\\s+(\\w+))?\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression grantPattern{
        QStringLiteral("^GRANT\\s+\\(?([\\w\\*,]+)\\)?\\s+ON\\s+\\(?([\\w\\*]+)\\.([\\w\\*]+)\\)?\\s+TO\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpression revokePattern{
        QStringLiteral("^REVOKE\\s+\\(?([\\w\\*,]+)\\)?\\s+ON\\s+\\(?([\\w\\*]+)\\.([\\w\\*]+)\\)?\\s+FROM\\s+(\\w+)\\s*;"),
        QRegularExpression::CaseInsensitiveOption
    };
};



#endif // SQLCMD_H
