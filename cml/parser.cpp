#include "parser.h"


#include <QRegularExpression>
#include <QRegularExpressionMatch>


Parser::Parser() { }



SqlParameters Parser::parse_sql(const QString &_sql){

    QString sql = _sql.toLower();
    QString first_keyword = parse_first_word_(sql);
    qDebug() << "first_keyword: " << first_keyword;

    if(!first_keyword.isEmpty()) {

        if(first_keyword=="create") {
            create_parser.parseCMD(sql);

        } else if(first_keyword=="alter") {

        } else if (first_keyword=="drop") {

        }

    }

    return SqlParameters();
}


QString Parser::parse_first_word_(const QString &sql) {
    static const QRegularExpression stateTypePattern{QStringLiteral("^\\s*(\\w+)\\b"),QRegularExpression::CaseInsensitiveOption};
    QRegularExpressionMatch match = stateTypePattern.match(sql);
    if(match.hasMatch()){
        return match.captured(1);
    }
    return QString();
}
