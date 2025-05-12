#include "FuncMap.h"
#include "utils.h"
#include "infolog.h"

FuncMap::FuncMap(): db(DataProcessor::GetInstance()) {
    setup();
    read_js_file();
}


void FuncMap::setup() {

    func_map_["CreateDatabase"] = [this](const QVariantList &params) -> int {
        std::string db_name = params[0].toString().toStdString();
        return db.CreateDatabase(db_name);
    };

    func_map_["DeleteDatabase"] = [this](const QVariantList &params) -> int {
        std::string db_name = params[0].toString().toStdString();
        return db.DeleteDatabase(db_name);
    };

    func_map_["AlterDatabaseName"] = [this](const QVariantList &params) -> int {
        std::string db_name = params[0].toString().toStdString();
        std::string new_name = params[1].toString().toStdString();
        return db.AlterDatabaseName(db_name,new_name);
    };

    func_map_["CreateTable"] = [this](const QVariantList &params) -> int {
        QString tb_name =  params[0].toString();
        QJsonArray columns = params[1].toJsonArray();

        std::vector<std::pair<std::string, std::string>> fields;
        std::vector<Constraint *> constraints;

        //创建每一行的参数信息
        for(const auto& c:columns) {
            QJsonObject column_js = c.toObject();

            QString cname = column_js["cname"].toString();
            QString ctype = column_js["ctype"].toString();
            QJsonArray cs_array = column_js["constraints"].toArray();

            fields.push_back(std::pair<std::string, std::string>(cname.toStdString(),ctype.toStdString()));
            for(const auto& cs: cs_array) {
                QJsonObject constrain_js = cs.toObject();
                QString cs_type = constrain_js["csname"].toString();
                QJsonArray csparams = constrain_js["params"].toArray();

                Constraint *filed_cs = CreateConstraint(tb_name,cs_type,cname,csparams);
                if(!filed_cs) {
                    continue;
                }

                constraints.push_back(CreateConstraint(tb_name,cs_type,cname,csparams));
                if(cs_type=="foregin_key") {
                    // QString ref_tb_name = csparams[0].toString();
                    QString ref_filed_name = csparams[1].toString();
                    QString ccs_name = tb_name+ "_"+ cname + "_" + cs_type + "refered";
                    constraints.push_back(new ForeignReferedConstraint(ref_filed_name.toStdString(),ccs_name.toStdString(),tb_name.toStdString(),cname.toStdString()));
                }
            }
        }
        qDebug() <<"size: " << constraints.size();
        return db.CreateTable(tb_name.toStdString(),fields,constraints);
    };

    func_map_["DropTable"] = [this](const QVariantList &params) -> int {
        std::string tb_name = params[0].toString().toStdString();
        return db.DropTable(tb_name);
    };

    func_map_["AlterTableName"] = [this](const QVariantList &params) -> int {
        std::string db_name;
        db.GetCurrentDatabase(db_name);
        std::string tb_name = params[0].toString().toStdString();
        std::string new_name = params[1].toString().toStdString();
        return db.AlterTableName(db_name,tb_name,new_name);
    };

    func_map_["AlterTableAdd"] = [this](const QVariantList &params) -> int {

        QString tb_name = params[0].toString();
        QJsonArray columns = params[1].toJsonArray();

        std::vector<std::pair<std::string, std::string>> fields;
        std::vector<Constraint *> constraints;

        //创建每一行的参数信息
        for(const auto& c:columns) {
            QJsonObject column_js = c.toObject();

            QString cname = column_js["cname"].toString();
            QString ctype = column_js["ctype"].toString();
            QJsonArray cs_array = column_js["constrains"].toArray();

            fields.push_back(std::pair<std::string, std::string>(cname.toStdString(),ctype.toStdString()));
            for(const auto& cs: cs_array) {
                QJsonObject constrain_js = cs.toObject();
                QString cs_type = constrain_js["csname"].toString();
                QJsonArray csparams = constrain_js["params"].toArray();

                Constraint *filed_cs = CreateConstraint(tb_name,cs_type,cname,csparams);
                if(!filed_cs) continue;

                constraints.push_back(filed_cs);
            }
        }

        //------------新增列
        int res = 0;
        for(const auto& field: fields) {
            int res1 = db.AlterTableAdd(tb_name.toStdString(),field);
            res = std::max(res,res1);
        }
        //------------新增限制
        for(const auto& cs: constraints) {
            int res2 = db.AlterTableConstraint(tb_name.toStdString(),cs);
            res = std::max(res,res2);
        }
        return res;
    };

    func_map_["AlterTableDrop"] = [this](const QVariantList &params) -> int {
        std::string tb_name = params[0].toString().toStdString();
        std::string column_name = params[1].toString().toStdString();
        return db.AlterTableDrop(tb_name,column_name);
    };

    func_map_["AlterTableModify"] = [this](const QVariantList &params) -> int {
        std::string tb_name = params[0].toString().toStdString();
        std::string column_name = params[1].toString().toStdString();
        std::string type_name = params[2].toString().toStdString();
        return db.AlterTableModify(tb_name,std::pair<std::string,std::string>(column_name,type_name));
    };


    func_map_["UseDatabase"] = [this](const QVariantList &params) -> int {
        std::string db_name = params[0].toString().toStdString();
        return db.UseDatabase(db_name);
    };


    func_map_["ShowDatabases"] = [this](const QVariantList &params) -> int {
        std::vector<std::string> allDatabases;
        int res = db.ShowDatabases(allDatabases);
        //下面展示allDatabases的内容.
        InfoLog::inst().print("---DataBase--");
        printStdV(allDatabases);
        return res;
    };

    func_map_["ShowTables"] = [this](const QVariantList &params) -> int {
        std::vector<std::string> returnTables;
        int res = db.ShowTables(returnTables);
        //下面展示returnTables的内容.
        InfoLog::inst().print("---Tables--");
        printStdV(returnTables);
        return res;
    };

    func_map_["DescribeTable"] = [this](const QVariantList &params) -> int {
        std::string tb_name = params[0].toString().toStdString();
        std::vector<std::pair<std::string,std::string>> fields;
        std::vector<Constraint*> constraints;

        int res = db.DescribeTable(tb_name,fields,constraints);
        //下面展示 fields && constraints 的内容.
        InfoLog::inst().print("Desc Func Used.");
        return res;
    };


    func_map_["AlterTableColumnName"] = [this](const QVariantList &params) -> int {
        QString tb_name = params[0].toString();
        QString column_name = params[1].toString();
        QString new_name = params[2].toString();
        return db.AlterTableColumnName(tb_name.toStdString(),column_name.toStdString(),new_name.toStdString());
    };

    func_map_["AlterTableConstraint"] = [this](const QVariantList &params) -> int {
        QString tb_name = params[0].toString();
        QString cname = params[1].toString();

        QJsonArray cs_array = params[2].toJsonArray();
        std::vector<Constraint *> constraints;

        for(const auto& cs: cs_array) {
            QJsonObject constrain_js = cs.toObject();
            QString cs_type = constrain_js["csname"].toString();
            QJsonArray csparams = constrain_js["params"].toArray();

            Constraint *filed_cs = CreateConstraint(tb_name,cs_type,cname,csparams);
            if(!filed_cs) continue;

            constraints.push_back(filed_cs);
        }

        int res = 0;
        for(const auto& cons: constraints) {
            res = std::max(res,db.AlterTableConstraint(tb_name.toStdString(),cons));
        }
        return res;
    };

    func_map_["AlterTableDeleteConstraint"] = [this](const QVariantList &params) -> int {
        QString tb_name = params[0].toString();
        QString cname = params[1].toString();
        QString cs_name = params[2].toString();
        //合成名称
        QString constraint_name = tb_name+"_"+cname+"_"+cs_name;
        return db.AlterTableDeleteConstraint(tb_name.toStdString(),constraint_name.toStdString());
    };

    /*
     * ---------DML---------
    */
    func_map_["Insert"] = [this](const QVariantList &params) -> int {
        QString tb_name = params[0].toString();
        QJsonArray column_array = params[1].toJsonArray();
        QJsonArray values_array = params[2].toJsonArray();

        //构造records
        int res = 0;
        for(int i=0;i<values_array.size();i++) {
            std::vector<std::pair<std::string,std::string>> records;
            for(int j=0;j<column_array.size();j++) {
                QString c_j = column_array[j].toString();
                QString v_ij = values_array[i][j].toString();
                records.push_back(std::pair<std::string,std::string>(c_j.toStdString(),v_ij.toStdString()));
            }
            res = std::max(res,db.Insert(tb_name.toStdString(),records));
        }
        return res;
    };

    func_map_["Select"] = [this](const QVariantList &params) -> int {

        QJsonArray tables = params[0].toJsonArray();
        QJsonArray select_columns = params[1].toJsonArray();
        QJsonArray where = params[2].toJsonArray();
        QJsonArray ordered_by = params[3].toJsonArray();

        std::vector<std::string> tableName;
        std::vector<std::string> fieldName;
        std::vector<std::tuple<std::string, std::string, int>> conditions;
        std::vector<std::string> orderbyKey;

        std::vector<std::vector<std::any>> returnRecords;

        for(const auto& t: tables) {
            tableName.push_back(t.toString().toStdString());
        }

        //组织字段
        for(const auto& c: select_columns) {
            fieldName.push_back(c.toString().toStdString());
        }
        //组织条件
        for(const auto& c: where) {
            QJsonObject cond = c.toObject();
            QString l = cond["left"].toString();
            QString r = cond["right"].toString();
            QString op = cond["condition"].toString();
            conditions.push_back(std::make_tuple(l.toStdString(), r.toStdString(), map_operator(op)));
        }
        //组织order by
        for(const auto& c: ordered_by) {
            QString column = c.toString();
            orderbyKey.push_back(column.toStdString());
        }

        int res = 0;
        if(tableName.size()==1) {
            res = db.Select(tableName[0],
                            fieldName,
                            conditions,
                            returnRecords,
                            orderbyKey);
        } else if(tableName.size()>1) {
            res = db.Select(tableName,
                            fieldName,
                            conditions,
                            returnRecords,
                            orderbyKey);
        }

        PrintSelectRes(returnRecords);
        return res;
    };

    func_map_["Update"] = [this](const QVariantList &params) -> int {
        QString tb_name = params[0].toString();
        QJsonArray set_values = params[1].toJsonArray();
        QJsonArray where = params[2].toJsonArray();

        std::string tableName = tb_name.toStdString();
        std::vector<std::pair<std::string,std::string>> values;
        std::vector<std::tuple<std::string,std::string,int>> conditions;

        //组织column和对应的值
        for(const auto& v:set_values) {
            QJsonObject val = v.toObject();
            QString cname = val["cname"].toString();
            QString val_str = val["values"].toString();
            values.push_back(std::pair<std::string,std::string>(cname.toStdString(),val_str.toStdString()));
        }

        //组织条件
        for(const auto& c: where) {
            QJsonObject cond = c.toObject();
            QString l = cond["left"].toString();
            QString r = cond["right"].toString();
            QString op = cond["condition"].toString();
            conditions.push_back(std::make_tuple(l.toStdString(), r.toStdString(), map_operator(op)));
        }

        return db.Update(tableName,values,conditions);
    };

    func_map_["Delete"] = [this](const QVariantList &params) -> int {
        QString tb_name = params[0].toString();
        QJsonArray where = params[1].toJsonArray();

        std::string tableName = tb_name.toStdString();
        std::vector<std::tuple<std::string,std::string,int>> conditions;

        //组织条件
        for(const auto& c: where) {
            QJsonObject cond = c.toObject();
            QString l = cond["left"].toString();
            QString r = cond["right"].toString();
            QString op = cond["condition"].toString();
            conditions.push_back(std::make_tuple(l.toStdString(), r.toStdString(), map_operator(op)));
        }
        return db.Delete(tableName,conditions);
    };

    // -----模版-----
    // func_map_[""] = [this](const QVariantList &params) -> int {
    //     return -1;
    // };
}


void FuncMap::read_js_file() {
    QFile file("../../cml/res/params.json");
    file.open(QFile::ReadOnly);
    QByteArray all = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(all);
    params_map_ = doc.object();
    file.close(); //有风险,但是无视风险.
}



