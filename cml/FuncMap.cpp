#include "FuncMap.h"
#include "utils.h"

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
        std::string tb_name =  params[0].toString().toStdString();
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

                Constraint *filed_cs = CreateConstraint(cs_type,cname,csparams);
                if(!filed_cs) continue;

                constraints.push_back(filed_cs);
                // 存疑
                if(cs_type=="foregin_key") {
                    QString ref_tb_name = csparams[0].toString();
                    QString ref_filed_name = csparams[1].toString();
                    constraints.push_back(new ForeignReferedConstraint(ref_filed_name.toStdString(),"ref_foregin_key",tb_name,cname.toStdString()));
                }
            }
        }
        return db.CreateTable(tb_name,fields,constraints);
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

                Constraint *filed_cs = CreateConstraint(cs_type,cname,csparams);
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


}


void FuncMap::read_js_file() {
    QFile file("../../cml/res/params.json");
    file.open(QFile::ReadOnly);
    QByteArray all = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(all);
    params_map_ = doc.object();
    file.close(); //有风险,但是无视风险.
}



