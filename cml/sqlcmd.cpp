#include "sqlcmd.h"

#include <QJsonObject>
#include <QJsonArray>


QJsonObject CreateParser::parseCMD(const QString &sql) {

    QJsonObject ast_data;
    ast_data["status"] = "ok";

    QRegularExpressionMatch match;
    if ((match=createDbPattern.match(sql)).hasMatch()){
        QString db_name = match.captured(1);
        ast_data["object"] = "database";
        ast_data["op"] = "create";
        ast_data["name"] = db_name;

    } else if ((match=createTablePattern.match(sql)).hasMatch()) {
        QString table_name = match.captured(1);
        QString column_str = match.captured(2);

        ast_data["object"] = "table";
        ast_data["op"] = "create";
        ast_data["name"] = table_name;

        QJsonArray columns_data;
        QStringList c_defs = column_str.split(",",Qt::SkipEmptyParts);

        for(const auto& def: c_defs){
            QRegularExpressionMatch column_match;
            column_match = columnPattern.match(def);
            // 如果信息格式不正确
            if(!column_match.hasMatch()) {
                ast_data["status"] = "error";
                ast_data["error_log"] = ast_data["error_log"].toString() + "Create did't match column: >>" + def+ "<<\n";
                continue;
            }

            // 将信息提取
            QJsonObject column_js;
            column_js["cname"] = column_match.captured(1);
            column_js["ctype"] = column_match.captured(2);

            //现在对constrain的解析是有问题的.
            //当前的解析规则是: type 后面的都是constrain.
            //所以只能添加单个的限制. 之后再修改.
            column_js["constrains"] = column_match.captured(3);
            //添加到JsonArray
            columns_data.append(column_js);
        }
        ast_data["columns"] = columns_data;
    }
    return ast_data;
}



QJsonObject DropParser::parseCMD(const QString& sql){
    QJsonObject ast_data;
    ast_data["status"] = "ok";

    QRegularExpressionMatch match;
    if((match=dropTablePattern.match(sql)).hasMatch()){
        QString table_name = match.captured(1);
        ast_data["object"] = "table";
        ast_data["op"] = "drop";
        ast_data["name"] = table_name;

    } else if((match=dropDatabasePattern.match(sql)).hasMatch()) {
        QString db_name = match.captured(1);
        ast_data["object"] = "database";
        ast_data["op"] = "drop";
        ast_data["name"] = db_name;
    } else {
        ast_data["status"] = "error";
        ast_data["error_log"] = ast_data["error_log"].toString() + "Drop did't match : >>" + sql + "<<\n";
    }
    return ast_data;
}


QJsonObject AlterParser::parseCMD(const QString &sql) {
    QJsonObject ast_data;
    ast_data["status"] = "ok";
    QRegularExpressionMatch match;

    // ALTER DATABASE name RENAME TO new_name
    if ((match = alterDbRenamePattern.match(sql)).hasMatch()) {
        QString db_name = match.captured(1);
        QString new_db_name = match.captured(2);
        ast_data["object"] = "database";
        ast_data["op"] = "rename";
        ast_data["name"] = db_name;
        ast_data["new_name"] = new_db_name;
    }
    // ALTER TABLE table_name ADD column_name column_type [constraints]
    else if ((match = alterTableAddPattern.match(sql)).hasMatch()) {
        QString table_name = match.captured(1);
        QString column_name = match.captured(2);
        QString column_type = match.captured(3);
        QString constraints = match.captured(4).trimmed();

        ast_data["object"] = "column";
        ast_data["op"] = "add";
        ast_data["table_name"] = table_name;

        QJsonArray columns_data;
        QJsonObject column_js;
        column_js["cname"] = column_name;
        column_js["ctype"] = column_type;
        column_js["constrains"] = constraints;
        columns_data.append(column_js);

        ast_data["columns"] = columns_data;
    }
    // ALTER TABLE table_name MODIFY column_name new_type
    else if ((match = alterTableModifyPattern.match(sql)).hasMatch()) {
        QString table_name = match.captured(1);
        QString column_name = match.captured(2);
        QString new_type = match.captured(3);

        ast_data["object"] = "column";
        ast_data["op"] = "modify";
        ast_data["table_name"] = table_name;
        ast_data["column_name"] = column_name;
        ast_data["new_type"] = new_type;
    }
    // ALTER TABLE table_name RENAME COLUMN column_name TO new_name
    else if ((match = alterTableRenameColumnPattern.match(sql)).hasMatch()) {
        QString table_name = match.captured(1);
        QString column_name = match.captured(2);
        QString new_name = match.captured(3);

        ast_data["object"] = "column";
        ast_data["op"] = "rename";
        ast_data["table_name"] = table_name;
        ast_data["column_name"] = column_name;
        ast_data["new_name"] = new_name;
    }
    // ALTER TABLE table_name DROP column_name [type]
    else if ((match = alterTableDropPattern.match(sql)).hasMatch()) {
        QString table_name = match.captured(1);
        QString column_name = match.captured(2);
        QString column_type = match.captured(3).trimmed();

        ast_data["object"] = "column";
        ast_data["op"] = "drop";
        ast_data["table_name"] = table_name;
        ast_data["column_name"] = column_name;

        if (!column_type.isEmpty()) {
            ast_data["type"] = column_type;
        }
    }
    // ALTER TABLE table_name RENAME TO new_name
    else if ((match = alterTableRenamePattern.match(sql)).hasMatch()) {
        QString table_name = match.captured(1);
        QString new_table_name = match.captured(2);

        ast_data["object"] = "table";
        ast_data["op"] = "rename";
        ast_data["name"] = table_name;
        ast_data["new_name"] = new_table_name;
    }
    else {
        ast_data["status"] = "error";
        ast_data["error_log"] = ast_data["error_log"].toString() + "Alter didn't match: >>" + sql + "<<\n";
    }

    return ast_data;
}







