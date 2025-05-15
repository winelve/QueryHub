#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QJsonObject>

#include "parser.h"

Parser::Parser() { }

QList<QString> Parser::split_sqls(const QString& sqls) {
    QList<QString> result;
    QRegularExpressionMatchIterator iterator = sqlStatementPattern.globalMatch(sqls);
    while (iterator.hasNext()) {
        QRegularExpressionMatch match = iterator.next();
        QString statement = match.captured().trimmed();
        if (!statement.isEmpty()) {
            result.append(statement);
        }
    }
    return result;
}


QJsonObject Parser::parse_sql(const QString &_sql){

    QString sql = _sql;
    QString first_keyword = parse_first_word_(sql);
    first_keyword = first_keyword.toLower();
    qDebug() << "first_keyword: " << first_keyword;

    QJsonObject ast_root;
    ast_root["status"] = "ok";

    //如果没解析出来,那么直接返回
    if (first_keyword.isEmpty()) {
        ast_root["status"] = "error";
        ast_root["error_log"] = "Can't Reslove the first keyword.";
        return ast_root;
    }

    //注册:
    QMap<QString,BaseCmdParser*> cmd_parsers = {
        {"create",&create_parser},
        {"alter",&alter_parser},
        {"drop",&drop_parser},
        {"use",&other_parser},
        {"show",&other_parser},
        {"desc",&other_parser},
        {"insert",&dml_parser},
        {"select",&dml_parser},
        {"update",&dml_parser},
        {"delete",&dml_parser},
        {"commit",&other_parser},
        {"rollback",&other_parser},
        {"login",&other_parser},
        {"register",&other_parser},
        {"grant",&other_parser},
        {"revoke",&other_parser},
    };

    auto it = cmd_parsers.find(first_keyword);
    //如果第一个 操作关键字 能被识别
    if(it!=cmd_parsers.end()) {
        //先标记状态
        ast_root["cmd"] = first_keyword;
        //解析参数和操作
        QJsonObject ast_data = it.value()->parseCMD(sql);
        //如果内层出现了问题
        if(ast_data["status"] == "error") {
            ast_root["status"] = "error";
            ast_root["error_log"] = ast_root["error_log"].toString()
                                    + first_keyword.toUpper()
                                    + " Parser Inner Error\n";
        }
        //组合标记
        ast_root["data"] = ast_data;
    } else {
        ast_root["status"] = "error";
        ast_root["error_log"] = "Invalid Operation KeyWord: >>" + first_keyword.toUpper() + "<<";
    }

    return ast_root;
}

QString Parser::parse_first_word_(const QString &sql) {
    static const QRegularExpression stateTypePattern{QStringLiteral("^\\s*(\\w+)\\b"),QRegularExpression::CaseInsensitiveOption};
    QRegularExpressionMatch match = stateTypePattern.match(sql);
    if(match.hasMatch()){
        return match.captured(1);
    }
    return QString();
}
