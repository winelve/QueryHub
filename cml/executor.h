#ifndef EXECUTOR_H
#define EXECUTOR_H

// #include "parser.h"
#include "FuncMap.h"
#include "infolog.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>


class Executor
{
public:
    Executor();

    void execute_ast(const QJsonObject& ast_root) {

        QString status = ast_root["status"].toString();

        if(status=="ok") {

            QString cmd = ast_root["cmd"].toString();
            QJsonObject ast_data = ast_root["data"].toObject();

            QString op = ast_data["op"].toString();
            QString object = ast_data["object"].toString();

            QJsonObject params_js = this->find_params(cmd,op,object);

            QJsonArray params_array = params_js["keys"].toArray();
            QVariantList p_list;

            for(const auto& p:params_array) {
                QString key = p.toString();
                p_list.append(ast_data[key].toString());
            }

            InfoLog::inst().print("Use Func: ");
            qDebug() << params_js["func"].toArray();
            InfoLog::inst().print("params: ");
            qDebug() << p_list;

        } else if(status=="error"){
            QString root_error_log = ast_root["error_log"].toString();
            QString data_error_log = ast_root["data"]["error_log"].toString();

            InfoLog::inst().print("---------Error Log---------");
            InfoLog::inst().print(root_error_log);
            InfoLog::inst().print(data_error_log);
        } else {
            InfoLog::inst().print("Wrong Status:"+status);
        }
    }

private:
    FuncMap func_map_;
    QJsonObject params_map_;

    QJsonObject find_params(const QString& cmd,const QString& op,const QString& objetc) {

        QJsonArray cmds = params_map_["cmds"].toArray();
        for(const auto& c: cmds) {
            if(c.isObject()) {
                QJsonObject cmd_js = c.toObject();
                if(cmd_js["cmd"]==cmd) {
                    QJsonObject obj_js = cmd_js[op].toObject();
                    QJsonObject params_js = obj_js[objetc].toObject();
                    params_js["status"] = "ok";
                    return params_js;
                }
            }
        }
        QJsonObject js; js["status"] = "error";
        return js;
    }






};

#endif // EXECUTOR_H
