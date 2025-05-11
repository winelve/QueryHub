#include "sqlcmd.h"

#include <QJsonObject>
#include <QJsonArray>

/*
------------------Create CMD------------------
*/
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
            QRegularExpressionMatch column_match = columnPattern.match(def.trimmed());

            // 如果信息格式不正确
            if(!column_match.hasMatch()) {
                ast_data["status"] = "error";
                ast_data["error_log"] = ast_data["error_log"].toString() + "Create didn't match column: >>" + def + "<<\n";
                continue;
            }

            // 将信息提取
            QJsonObject column_js;
            column_js["cname"] = column_match.captured(1);
            column_js["ctype"] = column_match.captured(2);

            // 解析约束条件
            QString constraints_str = column_match.captured(3).trimmed();
            if (!constraints_str.isEmpty()) {
                QJsonArray constraints_array;
                // 迭代匹配所有约束
                QRegularExpressionMatchIterator constraint_matches =
                    constraintPattern.globalMatch(constraints_str);

                while (constraint_matches.hasNext()) {
                    QRegularExpressionMatch constraint_match = constraint_matches.next();
                    QJsonObject constraint_obj;

                    QString constraint_name = constraint_match.captured(1);
                    QString params_str = constraint_match.captured(2); // 可能为空

                    constraint_obj["csname"] = constraint_name;

                    // 处理参数
                    if (!params_str.isEmpty()) {
                        QJsonArray params_array;
                        QStringList params = params_str.split(",", Qt::SkipEmptyParts);

                        for (const auto& param : params) {
                            params_array.append(param.trimmed());
                        }

                        constraint_obj["csparams"] = params_array;
                    } else {
                        constraint_obj["csparams"] = QJsonArray();
                    }

                    constraints_array.append(constraint_obj);
                }
                column_js["constraints"] = constraints_array;
            } else {
                column_js["constraints"] = QJsonArray();
            }
            // 添加到JsonArray
            columns_data.append(column_js);
        }
        ast_data["columns"] = columns_data;
    }
    return ast_data;
}


/*
------------------Drop CMD------------------
*/
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


/*
------------------Alter CMD------------------
*/
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
    // 处理 ALTER TABLE ADD 的代码段
    else if ((match = alterTableAddPattern.match(sql)).hasMatch()) {
        QString table_name = match.captured(1);
        QString column_name = match.captured(2);
        QString column_type = match.captured(3);
        QString constraints_str = match.captured(4).trimmed();

        ast_data["object"] = "column";
        ast_data["op"] = "add";
        ast_data["table_name"] = table_name;
        QJsonArray columns_data;
        QJsonObject column_js;

        column_js["cname"] = column_name;
        column_js["ctype"] = column_type;

        // 解析约束条件
        if (!constraints_str.isEmpty()) {
            QJsonArray constraints_array;

            // 迭代匹配所有约束
            QRegularExpressionMatchIterator constraint_matches =
                constraintPattern.globalMatch(constraints_str);

            while (constraint_matches.hasNext()) {
                QRegularExpressionMatch constraint_match = constraint_matches.next();
                QJsonObject constraint_obj;

                QString constraint_name = constraint_match.captured(1);
                QString params_str = constraint_match.captured(2); // 可能为空

                constraint_obj["csname"] = constraint_name;
                // 处理参数
                if (!params_str.isEmpty()) {
                    QJsonArray params_array;
                    QStringList params = params_str.split(",", Qt::SkipEmptyParts);

                    for (const auto& param : params) {
                        params_array.append(param.trimmed());
                    }

                    constraint_obj["csparams"] = params_array;
                } else {
                    constraint_obj["csparams"] = QJsonArray();
                }
                constraints_array.append(constraint_obj);
            }

            column_js["constraints"] = constraints_array;
        } else {
            column_js["constraints"] = QJsonArray();
        }
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

/*
------------------Other CMD------------------
*/
QJsonObject OtherCmdParser::parseCMD(const QString &sql) {
    QJsonObject ast_data;
    ast_data["status"] = "ok";

    QRegularExpressionMatch match;

    if ((match = useDbPattern.match(sql)).hasMatch()) {
        // Handle USE database command
        QString db_name = match.captured(1);
        ast_data["op"] = "use";
        ast_data["object"] = "database";
        ast_data["db_name"] = db_name;

    } else if ((match = showDatabasesPattern.match(sql)).hasMatch()) {
        ast_data["status"] = "error";

    } else if ((match = showTablesPattern.match(sql)).hasMatch()) {
        ast_data["status"] = "error";

    } else if ((match = describeTablePattern.match(sql)).hasMatch()) {
        ast_data["status"] = "error";

    } else {
        // No pattern matched
        ast_data["status"] = "error";
        ast_data["error_log"] = "Unknown command format";
    }
    return ast_data;
}





