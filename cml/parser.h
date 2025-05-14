#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QMap>

#include "sqlcmd.h"

class Parser
{
public:
    Parser();

    QList<QString> split_sqls(const QString& sqls);
    QJsonObject parse_sql(const QString &sql);


    QMap<QString,QList<QString>> test_sql_map() {
        QMap<QString,QList<QString>> all_test_sql = {
            {"create",create_parser.test_create_re},
            {"alter",alter_parser.test_alter_re},
            {"drop",drop_parser.test_drop_re},
        };
        return all_test_sql;
    }

private:
    QString parse_first_word_(const QString &sql);

    CreateParser create_parser;
    AlterParser alter_parser;
    DropParser drop_parser;
    DMLParser dml_parser;
    OtherCmdParser other_parser;


    // Regular expression to match SQL statements ending with semicolon
    const QRegularExpression sqlStatementPattern{
        QStringLiteral("[^;]+;"),
        QRegularExpression::CaseInsensitiveOption
    };

};

#endif // PARSER_H
