#ifndef FUNCMAP_H
#define FUNCMAP_H


#include <functional>
#include <QMap>
#include <QVariant>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>

#include "dataprocessor.h"

class FuncMap{

public:
    FuncMap():db(DataProcessor::GetInstance()) {
        setup();
        read_js_file();
    }

    void setup() {

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

        // func_map_["CreateTable"] = [this](const QVariantList &params) -> int {
        //     std::string tb_name =  params[0].toString().toStdString();

        //     return db.CreateTable();
        // };

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

        // func_map_["AlterTableAdd"] = [this](const QVariantList &params) -> int {
        //     return db.AlterTableAdd(params[0].toString().toStdString());
        // };

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

    }

    QJsonObject get_params_js() {
        return params_map_;
    }


    QMap<QString,std::function<int(const QVariantList &)>> map() {
        return func_map_;
    }

private:
    DataProcessor db;
    QMap<QString,std::function<int(const QVariantList &)>> func_map_;
    QJsonObject params_map_;

    void read_js_file() {
        QFile file("../../cml/res/params.json");
        file.open(QFile::ReadOnly);
        QByteArray all = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(all);
        params_map_ = doc.object();
        file.close(); //有风险,但是无视风险.
    }

};




















#endif // FUNCMAP_H
