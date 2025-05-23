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

    }
    // CREATE INDEX
    else if ((match = createIndexPattern.match(sql)).hasMatch()) {
        ast_data["object"] = "index";
        ast_data["op"] = "create";
        ast_data["tb_name"] = match.captured(1);
        ast_data["cname"] = match.captured(2);
    }

    else if ((match=createRegex.match(sql)).hasMatch()) {
        // 从 mainRegex 的命名捕获组中获取表名和列定义字符串
        QString table_name = match.captured("tableName");
        QString columns_definition_string = match.captured("columnsDefinitionStr");

        ast_data["object"] = "table";
        ast_data["name"] = table_name;
        ast_data["op"] = "create"; // 根据您的JSON输出示例，此字段不包含

        QJsonArray columns_data_array; // 用于存储所有列的JSON对象
        QStringList column_definition_parts = columns_definition_string.split(',', Qt::SkipEmptyParts);
        QString accumulatedErrorLog;

        // 2. 遍历每个列定义部分
        for (const QString& single_column_def_str : column_definition_parts) {
            QString trimmed_column_def = single_column_def_str.trimmed();
            if (trimmed_column_def.isEmpty()) continue;

            QRegularExpressionMatch column_match_result = columnPattern.match(trimmed_column_def);

            // 检查列定义是否匹配，并且基本部分（名称和类型）已捕获
            if (!column_match_result.hasMatch() || column_match_result.captured(1).isEmpty() || column_match_result.captured(2).isEmpty()) {
                QString errorMsg = "Create statement error: Column definition format incorrect for part: >>" + trimmed_column_def + "<<\n";
                accumulatedErrorLog += errorMsg;
                qWarning() << errorMsg.trimmed();
                continue; // 跳过格式不正确的列定义
            }

            QJsonObject column_json_obj;
            column_json_obj["cname"] = column_match_result.captured(1); // 列名
            column_json_obj["ctype"] = column_match_result.captured(2); // 列类型

            QString raw_constraints_string = column_match_result.captured(3).trimmed(); // 原始约束字符串
            QJsonArray constraints_json_array; // 用于存储当前列的所有约束

            // 3. 如果存在约束字符串，则解析约束
            if (!raw_constraints_string.isEmpty()) {
                QRegularExpressionMatchIterator constraint_iterator =
                    constraintPattern.globalMatch(raw_constraints_string);

                while (constraint_iterator.hasNext()) {
                    QRegularExpressionMatch current_constraint_match = constraint_iterator.next();
                    QJsonObject constraint_json_obj;

                    // 检查哪种约束被匹配到
                    if (current_constraint_match.captured("pk").isEmpty() == false) {
                        constraint_json_obj["csname"] = "primary_key";
                    }
                    else if (current_constraint_match.captured("fk").isEmpty() == false) {
                        constraint_json_obj["csname"] = "foregin_key";
                        QJsonArray params_for_json_array;
                        params_for_json_array.append(current_constraint_match.captured("fkTable").trimmed());
                        params_for_json_array.append(current_constraint_match.captured("fkColumn").trimmed());
                        constraint_json_obj["params"] = params_for_json_array;
                    }
                    else if (current_constraint_match.captured("def").isEmpty() == false) {
                        constraint_json_obj["csname"] = "default";
                        QJsonArray params_for_json_array;
                        params_for_json_array.append(current_constraint_match.captured("defValue").trimmed());
                        constraint_json_obj["params"] = params_for_json_array;
                    }
                    else if (current_constraint_match.captured("nn").isEmpty() == false) {
                        constraint_json_obj["csname"] = "not_null";
                    }
                    else if (current_constraint_match.captured("uq").isEmpty() == false) {
                        constraint_json_obj["csname"] = "unique";
                    }

                    // 如果没有参数，添加空的参数数组
                    if (!constraint_json_obj.contains("params")) {
                        constraint_json_obj["params"] = QJsonArray();
                    }
                    constraints_json_array.append(constraint_json_obj);
                }
            }
            column_json_obj["constraints"] = constraints_json_array; // 即使没有约束，也添加空的 constraints 数组
            columns_data_array.append(column_json_obj);
        }

        ast_data["columns"] = columns_data_array;

        // 如果在解析列定义时出现错误，则添加错误信息
        if (!accumulatedErrorLog.isEmpty()) {
            ast_data["status"] = "error";
            ast_data["error_log"] = accumulatedErrorLog.trimmed();
        }
    } else {
        ast_data["status"] = "error";
        ast_data["error_log"] = "can't parser create cmd";
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
    }
    // DROP INDEX
    else if ((match = dropIndexPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "drop";
        ast_data["object"] = "index";
        ast_data["tb_name"] = match.captured(1);
        ast_data["cname"] = match.captured(2);
    }
    else {
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

    // 新增：处理 ALTER TABLE ADD CONSTRAINT
    else if ((match = alterTableAddConstraintPattern.match(sql)).hasMatch()) {
        QString table_name = match.captured(1);
        QString column_name = match.captured(2);
        QString constraint_name = match.captured(3);
        QString params_str = match.captured(4).trimmed(); // 可能为空

        ast_data["object"] = "constraint";
        ast_data["op"] = "add";
        ast_data["tb_name"] = table_name;
        ast_data["cname"] = column_name;

        QJsonArray constraints_array;
        QJsonObject constraint_obj;

        constraint_obj["csname"] = constraint_name;

        // 处理参数
        if (!params_str.isEmpty()) {
            QJsonArray params_array;
            QStringList params = params_str.split(",", Qt::SkipEmptyParts);

            for (const auto& param : params) {
                params_array.append(param.trimmed());
            }

            constraint_obj["params"] = params_array;
        } else {
            constraint_obj["params"] = QJsonArray();
        }

        constraints_array.append(constraint_obj);
        ast_data["constraints"] = constraints_array;
    }
    // 新增：处理 ALTER TABLE DELETE CONSTRAINT
    else if ((match = alterTableDeleteConstraintPattern.match(sql)).hasMatch()) {
        QString table_name = match.captured(1);
        QString column_name = match.captured(2);
        QString constraint_name = match.captured(3);

        ast_data["object"] = "constraint";
        ast_data["op"] = "drop";
        ast_data["tb_name"] = table_name;
        ast_data["cname"] = column_name;
        ast_data["cs_name"] = constraint_name;
    }
    else {
        ast_data["status"] = "error";
        ast_data["error_log"] = ast_data["error_log"].toString() + "Alter didn't match: >>" + sql + "<<\n";
    }

    return ast_data;
}

/*
------------------DML CMD------------------
*/

QJsonObject DMLParser::parseCMD(const QString &sql) {
    QJsonObject ast_data; // 用于存储 "data" 键对应的值
    QRegularExpressionMatch match = insertPattern.match(sql.trimmed()); // 使用 trim 来去除首尾空格

    if (match.hasMatch()) {
        // -------  INSERT -------
        ast_data["op"] = "insert";
        ast_data["object"] = "table";

        // 1. 提取表名
        QString tableName = match.captured(1);
        ast_data["tb_name"] = tableName;

        // 2. 提取并处理列名
        QString columnsString = match.captured(2);
        QStringList columnList = columnsString.split(',', Qt::SkipEmptyParts); // 按逗号分割
        QJsonArray columnsArray;
        for (const QString &col : columnList) {
            columnsArray.append(col.trimmed()); // 去除列名两端的空格并添加到 JSON 数组
        }
        ast_data["columns"] = columnsArray;

        // 3. 提取并处理值列表
        QString valuesString = match.captured(3);
        QJsonArray valuesArray; // 存储所有行值的数组

        // 使用迭代器查找所有匹配的值元组 (...)
        QRegularExpressionMatchIterator it = valuesTuplePattern.globalMatch(valuesString);
        while (it.hasNext()) {
            QRegularExpressionMatch valueTupleMatch = it.next();
            QString singleTupleContent = valueTupleMatch.captured(1);
            QStringList valueList = singleTupleContent.split(',', Qt::SkipEmptyParts); // 按逗号分割单个元组的值
            QJsonArray rowValuesArray; // 存储单行值的数组
            for (const QString &val : valueList) {
                rowValuesArray.append(val.trimmed());
            }
            valuesArray.append(rowValuesArray); // 将单行值的数组添加到主值数组中
        }
        ast_data["values"] = valuesArray;

        //对columns的大小和values的大小进行判断
        bool is_size_eq = true;
        for(int i=0;i<valuesArray.size();i++) {
            const auto& val = valuesArray[i].toArray();
            if(val.size()!=columnsArray.size()) {
                is_size_eq = false;
                break;
            }
        }
        if(!is_size_eq) {
            ast_data["status"] = "error";
            ast_data["error_log"] = QString("column size:" + QString::number(columnsArray.size())) +
                                    QString(". didn't match value size:" + QString::number(valuesArray.size()));
            qDebug() << "columnArray:" << columnsArray;
            qDebug() << "valueArray:" << valuesArray;
        }
        // --------SELECT--------
    } else if ((match = selectPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "select";       // 操作类型
        ast_data["object"] = "table";      // 操作对象

        // 1. 解析 Select 列 (捕获组 1)
        QString selectColsStr = match.captured(1).trimmed(); // 获取列字符串 "c1,c2" 或 "*"
        QJsonArray selectColumnsArray; // 存储 SELECT 列的 JSON 数组
        if (selectColsStr == "*") {
            selectColumnsArray.append("*"); // 如果是星号，直接添加
        } else {
            QStringList selectColList = selectColsStr.split(',', Qt::SkipEmptyParts); // 按逗号分割
            for (const QString &col : selectColList) {
                selectColumnsArray.append(col.trimmed()); // 去除空格并添加
            }
        }
        ast_data["select_columns"] = selectColumnsArray; // 注意: 对于 SELECT，这里是 'select_columns'

        // 2. 解析表名 (捕获组 2 和可选的捕获组 4)
        QJsonArray tablesArray;
        tablesArray.append(match.captured(2).trimmed()); // 添加 FROM 子句的主表名
        if (match.hasCaptured(4)) { // 检查 JOIN 子句是否被捕获
            QString joinStr = match.captured(4).trimmed(); // 获取 JOIN 的表字符串 "tb2,tb3"
            QStringList joinList = joinStr.split(',', Qt::SkipEmptyParts); // 按逗号分割
            for (const QString &tbl : joinList) {
                tablesArray.append(tbl.trimmed()); // 添加 JOIN 的表名
            }
        }
        ast_data["tables"] = tablesArray;

        // 3. 解析 WHERE 子句 (可选的捕获组 3) - 尝试解析简单条件
        QJsonArray whereArray; // 存储 WHERE 条件的 JSON 数组
        if (match.hasCaptured(3)) { // 检查 WHERE 子句是否被捕获
            QString whereStr = match.captured(3); // 获取 WHERE 子句内容 "c1>10 AND c2<100"
            // 尝试通过 AND 或 OR 分割条件 (忽略大小写, 保留分隔符周围的空格)
            // 注意：这种分割方式对于复杂的、带有括号的条件可能不够健壮
            QStringList conditions = whereStr.split(andpattern, Qt::SkipEmptyParts);

            for (const QString &condition : conditions) {
                QRegularExpressionMatch condMatch = whereConditionPattern.match(condition.trimmed()); // 尝试匹配 "左 操作符 右" 格式
                if (condMatch.hasMatch()) {
                    QJsonObject conditionObj; // 创建 JSON 对象表示单个条件
                    conditionObj["left"] = condMatch.captured(1).trimmed();      // 左操作数
                    conditionObj["condition"] = condMatch.captured(2).trimmed(); // 操作符
                    conditionObj["right"] = condMatch.captured(3).trimmed();     // 右操作数 (作为字符串)
                    whereArray.append(conditionObj); // 添加到 WHERE 数组
                } else {
                    qWarning() << "无法解析 WHERE 条件部分:" << condition.trimmed();
                }
            }
        }
        ast_data["where"] = whereArray; // 即使 WHERE 子句为空或未解析，也添加该键

        // 4. 解析 ORDERED BY 子句 (可选的捕获组 5)
        QJsonArray orderedByArray; // 存储 ORDERED BY 列的 JSON 数组
        if (match.hasCaptured(5)) { // 检查 ORDERED BY 子句是否被捕获
            QString orderByStr = match.captured(5).trimmed(); // 获取排序列字符串 "c1,c2"
            QStringList orderByList = orderByStr.split(',', Qt::SkipEmptyParts); // 按逗号分割
            for (const QString &col : orderByList) {
                orderedByArray.append(col.trimmed()); // 去除空格并添加
            }
        }
        ast_data["order_by"] = orderedByArray; // 即使为空也添加该键

        ast_data["status"] = "ok"; // SELECT 解析成功

        // --- 如果 INSERT 和 SELECT 都没有匹配 ---
    } else if ((match = updatePattern.match(sql)).hasMatch()) {
        // --- 匹配成功，解析 UPDATE 语句 ---
        ast_data["op"] = "update";
        ast_data["object"] = "table";

        // 1. 提取表名
        QString tableName = match.captured(1);
        ast_data["tb_name"] = tableName;

        // 2. 解析 SET 子句
        QString setClauseStr = match.captured(2);
        QStringList setItems = setClauseStr.split(',', Qt::SkipEmptyParts);
        QJsonArray setValuesArray;

        for (const QString &item : setItems) {
            QRegularExpressionMatch setMatch = setValuePattern.match(item.trimmed());
            if (setMatch.hasMatch()) {
                QJsonObject setObj;
                setObj["cname"] = setMatch.captured(1).trimmed();  // 列名
                setObj["values"] = setMatch.captured(2).trimmed(); // 值
                setValuesArray.append(setObj);
            } else {
                qWarning() << "无法解析 SET 子句部分:" << item.trimmed();
            }
        }

        ast_data["set_values"] = setValuesArray;

        // 3. 解析 WHERE 子句 (可选的捕获组 3)
        QJsonArray whereArray;
        if (match.hasCaptured(3)) {
            QString whereStr = match.captured(3);
            QStringList conditions = whereStr.split(andpattern,Qt::SkipEmptyParts);

            for (const QString &condition : conditions) {
                QRegularExpressionMatch condMatch = whereConditionPattern.match(condition.trimmed());
                if (condMatch.hasMatch()) {
                    QJsonObject conditionObj;
                    conditionObj["left"] = condMatch.captured(1).trimmed();      // 左操作数
                    conditionObj["condition"] = condMatch.captured(2).trimmed(); // 操作符
                    conditionObj["right"] = condMatch.captured(3).trimmed();     // 右操作数
                    whereArray.append(conditionObj);
                } else {
                    qWarning() << "无法解析 WHERE 条件部分:" << condition.trimmed();
                }
            }
        }
        ast_data["where"] = whereArray;
        ast_data["status"] = "ok"; // UPDATE 解析成功

    } else if ((match = deletePattern.match(sql)).hasMatch()) {
        // --- 匹配成功，解析 DELETE 语句 ---
        ast_data["op"] = "delete";
        ast_data["object"] = "table";

        // 1. 提取表名
        QString tableName = match.captured(1);
        ast_data["tb_name"] = tableName;

        // 2. 解析 WHERE 子句 (可选的捕获组 2)
        QJsonArray whereArray;
        if (match.hasCaptured(2)) {
            QString whereStr = match.captured(2);
            QStringList conditions = whereStr.split(andpattern,Qt::SkipEmptyParts);

            for (const QString &condition : conditions) {
                QRegularExpressionMatch condMatch = whereConditionPattern.match(condition.trimmed());
                if (condMatch.hasMatch()) {
                    QJsonObject conditionObj;
                    conditionObj["left"] = condMatch.captured(1).trimmed();      // 左操作数
                    conditionObj["condition"] = condMatch.captured(2).trimmed(); // 操作符
                    conditionObj["right"] = condMatch.captured(3).trimmed();     // 右操作数
                    whereArray.append(conditionObj);
                } else {
                    qWarning() << "无法解析 WHERE 条件部分:" << condition.trimmed();
                }
            }
        }
        ast_data["where"] = whereArray;

        ast_data["status"] = "ok"; // DELETE 解析成功
    } else {
        ast_data["status"] = "error"; // 设置状态为错误
        ast_data["error_log"] = "未知或不支持的命令格式"; // 错误信息
    }

    return ast_data; // 返回包含解析结果或错误信息的 QJsonObject
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
        ast_data["op"] = "show";
        ast_data["object"] = "database";
    } else if ((match = showTablesPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "show";
        ast_data["object"] = "table";
    } else if ((match = describeTablePattern.match(sql)).hasMatch()) {
        ast_data["op"] = "desc";
        ast_data["object"] = "table";
        ast_data["name"] = match.captured(1);
    }
    // COMMIT
    else if ((match = commitPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "commit";
        ast_data["object"] = "database";
    }
    // ROLLBACK
    else if ((match = rollbackPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "rollback";
        ast_data["object"] = "database";
    }
    // LOGIN
    else if ((match = loginPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "login";
        ast_data["object"] = "database";
        ast_data["username"] = match.captured(1);
        ast_data["pwd"] = match.captured(2);
    }
    // REGISTER
    else if ((match = registerPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "register";
        ast_data["object"] = "user";
        ast_data["username"] = match.captured(1);
        ast_data["pwd"] = match.captured(2);
    }
    // GRANT OWNER
    else if ((match = grantOwnerPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "grant";
        ast_data["object"] = "database";
        ast_data["db_name"] = match.captured(1);
        ast_data["username"] = match.captured(2);
    }
    // REVOKE OWNER
    else if ((match = revokeOwnerPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "revoke";
        ast_data["object"] = "database";
        ast_data["db_name"] = match.captured(1);

        // user_name is optional in the syntax
        if (match.lastCapturedIndex() >= 2 && !match.captured(2).isEmpty()) {
            ast_data["username"] = match.captured(2);
        }else {
            ast_data["username"] = "";
        }

    }
    // GRANT permissions
    else if ((match = grantPattern.match(sql)).hasMatch()) {
        ast_data["op"] = "grant";
        ast_data["object"] = "table";

        // Parse permissions list
        QString powers = match.captured(1);
        QJsonArray powerArray;
        if (powers == "*") {
            powerArray.append("*");
        } else {
            QStringList powerList = powers.split(",");
            for (const QString &power : powerList) {
                powerArray.append(power.trimmed());
            }
        }
        ast_data["power"] = powerArray;

        // Parse db_name and tb_name
        ast_data["db_name"] = match.captured(2);
        ast_data["tb_name"] = match.captured(3);

        // Username
        ast_data["username"] = match.captured(4);
    }
    // REVOKE permissions
    else if ((match = revokePattern.match(sql)).hasMatch()) {
        ast_data["op"] = "revoke";
        ast_data["object"] = "table";

        // Parse permissions list
        QString powers = match.captured(1);
        QJsonArray powerArray;
        if (powers == "*") {
            powerArray.append("*");
        } else {
            QStringList powerList = powers.split(",");
            for (const QString &power : powerList) {
                powerArray.append(power.trimmed());
            }
        }
        ast_data["power"] = powerArray;

        // Parse db_name and tb_name
        ast_data["db_name"] = match.captured(2);
        ast_data["tb_name"] = match.captured(3);

        // Username
        ast_data["username"] = match.captured(4);
    }
    else {
        // No pattern matched
        ast_data["status"] = "error";
        ast_data["error_log"] = "Unknown command format";
    }
    return ast_data;
}




