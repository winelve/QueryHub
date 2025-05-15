#include "FuncMap.h"
#include "utils.h"
#include "infolog.h"

FuncMap::FuncMap(): db(DataProcessor::GetInstance()) {
    setup();
    read_js_file();
}

void FuncMap::setup() {

    func_map_["CreateDatabase"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::string db_name = params[0].toString().toStdString();

        // 组织传递JsDoc内容
        int code = db.CreateDatabase(db_name);
        QString func = "CreateDatabase";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["DeleteDatabase"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::string db_name = params[0].toString().toStdString();

        // 组织传递JsDoc内容
        int code = db.DeleteDatabase(db_name);
        QString func = "DeleteDatabase";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["AlterDatabaseName"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::string db_name = params[0].toString().toStdString();
        std::string new_name = params[1].toString().toStdString();

        // 组织传递JsDoc内容
        int code = db.AlterDatabaseName(db_name, new_name);
        QString func = "AlterDatabaseName";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["CreateTable"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString tb_name = params[0].toString();
        QJsonArray columns = params[1].toJsonArray();

        qDebug() << "!!!!!!!!!!!!!!!!!!!!!!debugger:" << columns ;

        std::vector<std::pair<std::string, std::string>> fields;
        std::vector<Constraint *> constraints;

        for(const auto& c : columns) {
            QJsonObject column_js = c.toObject();

            QString cname = column_js["cname"].toString();
            QString ctype = column_js["ctype"].toString();
            QJsonArray cs_array = column_js["constraints"].toArray();

            fields.push_back(std::pair<std::string, std::string>(cname.toStdString(), ctype.toStdString()));
            for(const auto& cs : cs_array) {
                QJsonObject constrain_js = cs.toObject();
                QString cs_type = constrain_js["csname"].toString();
                QJsonArray csparams = constrain_js["params"].toArray();

                Constraint *field_cs = CreateConstraint(tb_name, cs_type, cname, csparams);
                if(!field_cs) {
                    continue;
                }

                constraints.push_back(field_cs);
                if(cs_type == "foreign_key") {
                    QString ref_field_name = csparams[1].toString();
                    QString ccs_name = tb_name + "_" + cname + "_" + cs_type + "refered";
                    constraints.push_back(new ForeignReferedConstraint(ref_field_name.toStdString(), ccs_name.toStdString(), tb_name.toStdString(), cname.toStdString()));
                }
            }
        }
        qDebug() << "size: " << fields.size();

        // 组织传递JsDoc内容

        int code = db.CreateTable(tb_name.toStdString(), fields, constraints);
        QString func = "CreateTable";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["DropTable"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::string tb_name = params[0].toString().toStdString();

        // 组织传递JsDoc内容
        int code = db.DropTable(tb_name);
        QString func = "DropTable";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["AlterTableName"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::string db_name;
        db.GetCurrentDatabase(db_name);
        std::string tb_name = params[0].toString().toStdString();
        std::string new_name = params[1].toString().toStdString();

        // 组织传递JsDoc内容
        int code = db.AlterTableName(db_name, tb_name, new_name);
        QString func = "AlterTableName";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["AlterTableAdd"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString tb_name = params[0].toString();
        QJsonArray columns = params[1].toJsonArray();

        std::vector<std::pair<std::string, std::string>> fields;
        std::vector<Constraint *> constraints;

        for(const auto& c : columns) {
            QJsonObject column_js = c.toObject();

            QString cname = column_js["cname"].toString();
            QString ctype = column_js["ctype"].toString();
            QJsonArray cs_array = column_js["constrains"].toArray();

            fields.push_back(std::pair<std::string, std::string>(cname.toStdString(), ctype.toStdString()));
            for(const auto& cs : cs_array) {
                QJsonObject constrain_js = cs.toObject();
                QString cs_type = constrain_js["csname"].toString();
                QJsonArray csparams = constrain_js["params"].toArray();

                Constraint *field_cs = CreateConstraint(tb_name, cs_type, cname, csparams);
                if(!field_cs) continue;

                constraints.push_back(field_cs);
            }
        }

        int code = 0;
        for(const auto& field : fields) {
            int code1 = db.AlterTableAdd(tb_name.toStdString(), field);
            code = std::max(code, code1);
        }
        for(const auto& cs : constraints) {
            int code2 = db.AlterTableConstraint(tb_name.toStdString(), cs);
            code = std::max(code, code2);
        }

        // 组织传递JsDoc内容
        QString func = "AlterTableAdd";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["AlterTableDrop"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::string tb_name = params[0].toString().toStdString();
        std::string column_name = params[1].toString().toStdString();

        // 组织传递JsDoc内容
        int code = db.AlterTableDrop(tb_name, column_name);
        QString func = "AlterTableDrop";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["AlterTableModify"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::string tb_name = params[0].toString().toStdString();
        std::string column_name = params[1].toString().toStdString();
        std::string type_name = params[2].toString().toStdString();

        // 组织传递JsDoc内容
        int code = db.AlterTableModify(tb_name, std::pair<std::string, std::string>(column_name, type_name));
        QString func = "AlterTableModify";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["UseDatabase"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::string db_name = params[0].toString().toStdString();

        // 组织传递JsDoc内容
        int code = db.UseDatabase(db_name);
        QString func = "UseDatabase";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["ShowDatabases"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::vector<std::string> allDatabases;
        int code = db.ShowDatabases(allDatabases);
        InfoLog::inst().print("---DataBase--");
        printStdV(allDatabases);

        // 组织传递JsDoc内容
        QString func = "ShowDatabases";
        QJsonArray data;
        for(const auto& db : allDatabases) {
            data.append(QString::fromStdString(db));
        }
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["ShowTables"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::vector<std::string> returnTables;
        int code = db.ShowTables(returnTables);
        InfoLog::inst().print("---Tables--");
        printStdV(returnTables);

        // 组织传递JsDoc内容
        QString func = "ShowTables";
        QJsonArray data;
        for(const auto& tb : returnTables) {
            data.append(QString::fromStdString(tb));
        }
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["DescribeTable"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        std::string tb_name = params[0].toString().toStdString();
        std::vector<std::pair<std::string, std::string>> fields;
        std::vector<Constraint*> constraints;

        int code = db.DescribeTable(tb_name, fields, constraints);
        InfoLog::inst().print("Desc Func Used.");

        // 组织传递JsDoc内容
        QString func = "DescribeTable";
        QJsonArray data;
        if(code == 0) { // 假设0表示成功
            for(const auto& field : fields) {
                QJsonObject obj;
                obj["cname"] = QString::fromStdString(field.first);
                obj["type"] = QString::fromStdString(field.second);
                obj["cs_name"] = ""; // 初始为空
                data.append(obj);
            }
            // Update constraints
            for(const auto& cs : constraints) {
                std::string field_name = cs->GetFieldName();
                std::string cs_name = cs->GetConstraintName();
                for(int i = 0; i < data.size(); ++i) {
                    QJsonObject obj = data[i].toObject();
                    if(obj["cname"].toString().toStdString() == field_name) {
                        obj["cs_name"] = QString::fromStdString(cs_name);
                        data[i] = obj; // Use index to update the array
                        break;
                    }
                }
            }
        }
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["AlterTableColumnName"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString tb_name = params[0].toString();
        QString column_name = params[1].toString();
        QString new_name = params[2].toString();

        // 组织传递JsDoc内容
        int code = db.AlterTableColumnName(tb_name.toStdString(), column_name.toStdString(), new_name.toStdString());
        QString func = "AlterTableColumnName";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["AlterTableConstraint"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString tb_name = params[0].toString();
        QString cname = params[1].toString();

        QJsonArray cs_array = params[2].toJsonArray();
        std::vector<Constraint *> constraints;

        for(const auto& cs : cs_array) {
            QJsonObject constrain_js = cs.toObject();
            QString cs_type = constrain_js["csname"].toString();
            QJsonArray csparams = constrain_js["params"].toArray();

            Constraint *field_cs = CreateConstraint(tb_name, cs_type, cname, csparams);
            if(!field_cs) continue;

            constraints.push_back(field_cs);
        }

        int code = 0;
        for(const auto& cons : constraints) {
            code = std::max(code, db.AlterTableConstraint(tb_name.toStdString(), cons));
        }

        // 组织传递JsDoc内容
        QString func = "AlterTableConstraint";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["AlterTableDeleteConstraint"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString tb_name = params[0].toString();
        QString cname = params[1].toString();
        QString cs_name = params[2].toString();
        QString constraint_name = tb_name + "_" + cname + "_" + cs_name;

        // 组织传递JsDoc内容
        int code = db.AlterTableDeleteConstraint(tb_name.toStdString(), constraint_name.toStdString());
        QString func = "AlterTableDeleteConstraint";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["Insert"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString tb_name = params[0].toString();
        QJsonArray column_array = params[1].toJsonArray();
        QJsonArray values_array = params[2].toJsonArray();

        int code = 0;
        for(int i = 0; i < values_array.size(); i++) {
            std::vector<std::pair<std::string, std::string>> records;
            for(int j = 0; j < column_array.size(); j++) {
                QString c_j = column_array[j].toString();
                QString v_ij = values_array[i][j].toString();
                records.push_back(std::pair<std::string, std::string>(c_j.toStdString(), v_ij.toStdString()));
            }
            code = std::max(code, db.Insert(tb_name.toStdString(), records));
        }

        // 组织传递JsDoc内容
        QString func = "Insert";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["Select"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QJsonArray tables = params[0].toJsonArray();
        QJsonArray select_columns = params[1].toJsonArray();
        QJsonArray where = params[2].toJsonArray();
        QJsonArray ordered_by = params[3].toJsonArray();

        std::vector<std::string> tableName;
        std::vector<std::string> fieldName;
        std::vector<std::tuple<std::string, std::string, int>> conditions;
        std::vector<std::string> orderbyKey;

        std::vector<std::vector<std::any>> returnRecords;

        for(const auto& t : tables) {
            tableName.push_back(t.toString().toStdString());
        }
        for(const auto& c : select_columns) {
            fieldName.push_back(c.toString().toStdString());
        }
        for(const auto& c : where) {
            QJsonObject cond = c.toObject();
            QString l = cond["left"].toString();
            QString r = cond["right"].toString();
            QString op = cond["condition"].toString();
            conditions.push_back(std::make_tuple(l.toStdString(), r.toStdString(), map_operator(op)));
        }
        for(const auto& c : ordered_by) {
            QString column = c.toString();
            orderbyKey.push_back(column.toStdString());
        }

        int code = 0;
        if(tableName.size() == 1) {
            code = db.Select(tableName[0], fieldName, conditions, returnRecords, orderbyKey);
        } else if(tableName.size() > 1) {
            code = db.Select(tableName, fieldName, conditions, returnRecords, orderbyKey);
        }
        PrintSelectRes(returnRecords);

        // 组织传递JsDoc内容
        QString func = "Select";
        QJsonArray data;
        if(code == 0 && !returnRecords.empty()) {
            for(const auto& row : returnRecords) {
                QJsonArray rowArray;
                for(const auto& cell : row) {
                    if(cell.type() == typeid(std::string)) {
                        rowArray.append(QString::fromStdString(std::any_cast<std::string>(cell)));
                    } else if(cell.type() == typeid(int)) {
                        rowArray.append(std::any_cast<int>(cell));
                    } else if(cell.type() == typeid(double)) {
                        rowArray.append(std::any_cast<double>(cell));
                    }
                }
                data.append(rowArray);
            }
        }
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["Update"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString tb_name = params[0].toString();
        QJsonArray set_values = params[1].toJsonArray();
        QJsonArray where = params[2].toJsonArray();

        std::string tableName = tb_name.toStdString();
        std::vector<std::pair<std::string, std::string>> values;
        std::vector<std::tuple<std::string, std::string, int>> conditions;


        for(const auto& v : set_values) {
            QJsonObject val = v.toObject();
            QString cname = val["cname"].toString();
            QString val_str = val["values"].toString();
            values.push_back(std::pair<std::string, std::string>(cname.toStdString(), val_str.toStdString()));
        }

        for(const auto& c : where) {
            QJsonObject cond = c.toObject();
            QString l = cond["left"].toString();
            QString r = cond["right"].toString();
            QString op = cond["condition"].toString();
            conditions.push_back(std::make_tuple(l.toStdString(), r.toStdString(), map_operator(op)));
        }


        qDebug() << tableName;
        qDebug() << values;
        qDebug() << conditions.size();

        // 组织传递JsDoc内容
        int code = db.Update(tableName, values, conditions);

        QString func = "Update";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };

    func_map_["Delete"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString tb_name = params[0].toString();
        QJsonArray where = params[1].toJsonArray();

        std::string tableName = tb_name.toStdString();
        std::vector<std::tuple<std::string, std::string, int>> conditions;

        for(const auto& c : where) {
            QJsonObject cond = c.toObject();
            QString l = cond["left"].toString();
            QString r = cond["right"].toString();
            QString op = cond["condition"].toString();
            conditions.push_back(std::make_tuple(l.toStdString(), r.toStdString(), map_operator(op)));
        }

        // 组织传递JsDoc内容
        int code = db.Delete(tableName, conditions);
        QString func = "Delete";
        QJsonArray data;
        returnData = makeJsDocument(code, func, data);

        return code;
    };


    func_map_["Write"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {

        int code = db.Write();
        QString func = "Write";
        QJsonArray data;
        returnData = makeJsDocument(code,func,data);

        return code;
    };

    func_map_["Read"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        int code = db.Read(0);
        QString func = "Read";
        QJsonArray data;
        returnData = makeJsDocument(code,func,data);

        return code;
    };

    func_map_["Login"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString userName = params[0].toString();
        QString userPassword = params[1].toString();

        int code = db.Login(userName.toStdString(),userPassword.toStdString());
        QString func = "Login";
        QJsonArray data;
        returnData = makeJsDocument(code,func,data);

        return code;
    };

    func_map_["CreateUser"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString userName = params[0].toString();
        QString userPassword = params[1].toString();

        int code = db.CreateUser(userName.toStdString(),userPassword.toStdString());
        QString func = "CreateUser";
        QJsonArray data;
        returnData = makeJsDocument(code,func,data);

        return code;
    };

    func_map_["GrantDatabaseOwner"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString db_name = params[0].toString();
        QString user_name = params[1].toString();

        int code = db.GrantDatabaseOwner(db_name.toStdString(),user_name.toStdString());
        QString func = "GrantDatabaseOwner";
        QJsonArray data;
        returnData = makeJsDocument(code,func,data);

        return code;
    };

    func_map_["GrantAuthority"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString username = params[0].toString();
        QString db_name = params[1].toString();
        QString tb_name = params[2].toString();
        QString power = params[3].toString();

        int code = db.GrantAuthority(username.toStdString(),db_name.toStdString(),tb_name.toStdString(),power.toStdString());
        QString func = "GrantAuthority";
        QJsonArray data;
        returnData = makeJsDocument(code,func,data);

        return code;
    };

    func_map_["RevokeDatabaseOwner"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        qDebug() << "0.----------------------------------";
        QString db_name = params[0].toString();
        QString user_name = params[1].toString();

        qDebug() << "1.----------------------------------";
        int code = db.RevokeDatabaseOwner(db_name.toStdString(),user_name.toStdString());
        qDebug() << "2.----------------------------------";
        QString func = "RevokeDatabaseOwner";
        QJsonArray data;
        returnData = makeJsDocument(code,func,data);

        return code;
    };

    func_map_["RevokeAuthority"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString username = params[0].toString();
        QString db_name = params[1].toString();
        QString tb_name = params[2].toString();
        QString power = params[3].toString();

        int code = db.GrantAuthority(username.toStdString(),db_name.toStdString(),tb_name.toStdString(),power.toStdString());
        QString func = "RevokeAuthority";
        QJsonArray data;
        returnData = makeJsDocument(code,func,data);

        return code;
    };

    func_map_["BuildIndex"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {

        QString tb_name = params[0].toString();
        QString cname = params[1].toString();

        int code = db.BuildIndex(tb_name.toStdString(),cname.toStdString());
        QString func = "BuildIndex";
        QJsonArray data;

        returnData = makeJsDocument(code,func,data);
        return code;
    };

    func_map_["DestroyIndex"] = [this](const QVariantList &params, QJsonDocument& returnData) -> int {
        QString tb_name = params[0].toString();
        QString cname = params[1].toString();

        int code = db.DestroyIndex(tb_name.toStdString(),cname.toStdString());
        QString func = "DestroyIndex";
        QJsonArray data;

        returnData = makeJsDocument(code,func,data);
        return code;
    };

}

void FuncMap::read_js_file() {
    QFile file("../../cml/res/params.json");
    file.open(QFile::ReadOnly);
    QByteArray all = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(all);
    params_map_ = doc.object();
    file.close();
}
