#ifndef PARSER_H
#define PARSER_H

#include <QList>
#include <QString>
#include <QRegularExpression>
#include <QStringLiteral>

class Parser
{
public:
    Parser();
    void parse_sql(const QString &sql);

    void test() {}
private:


    class BaseCMD {
    public:
        virtual ~BaseCMD() = default;  // Virtual destructor for proper polymorphism
        virtual void parseCMD(const QString &sql) = 0;  // Corrected method name (parser -> parse)
    };
    // ------------------------
    class Alter:public BaseCMD {
    public :
        void parseCMD(const QString &sql) override;
        QList<QString> test_alter_re = {
            "ALTER DATABASE my_db RENAME new_db;",
            "ALTER TABLE users ADD age INT NOT NULL;",
            "ALTER TABLE users MODIFY name VARCHAR;",
            "ALTER TABLE users RENAME name full_name;",
            "ALTER TABLE users DROP email CHAR;",
            "ALTER TABLE users DROP email;"
        };
    private:
        const QRegularExpression alterDbPattern{ QStringLiteral("^ALTER\\s+DATABASE\\s+(\\w+)\\s+RENAME\\s+(\\w+)\\s*;"),
                                                 QRegularExpression::CaseInsensitiveOption};
        const QRegularExpression alterTableAddPattern{ QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+ADD\\s+(\\w+)\\s+(\\w+)\\s*([^\\s;][^;]*?)?\\s*;"),
                                                        QRegularExpression::CaseInsensitiveOption};
        const QRegularExpression alterTableModifyPattern{QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+MODIFY\\s+(\\w+)\\s+(\\w+)\\s*;"),
                                                          QRegularExpression::CaseInsensitiveOption};
        const QRegularExpression alterTableRenamePattern{QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+RENAME\\s+(\\w+)\\s+(\\w+)\\s*;"),
                                                          QRegularExpression::CaseInsensitiveOption};
        const QRegularExpression alterTableDropPattern{QStringLiteral("^ALTER\\s+TABLE\\s+(\\w+)\\s+DROP\\s+(\\w+)\\s*(\\w+)?\\s*;"),
                                                        QRegularExpression::CaseInsensitiveOption};
    };
    //---------------
    class Create:public BaseCMD
    {
    public:
        void parseCMD(const QString &sql) override {
            QRegularExpressionMatch match;
            if ((match=createDbPattern.match(sql)).hasMatch()){
                QString db_name = match.captured(1);

            }
        }

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

    class Drop:public BaseCMD{
    public:
        void parseCMD(const QString& sql) override;
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

};

#endif // PARSER_H
