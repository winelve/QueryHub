#include "executor.h"
#include "infolog.h"

Executor::Executor() {
    params_map_ = func_map_.get_params_js();
}


int Executor::execute_ast(const QJsonObject& ast_root) {

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
            if(ast_data[key].isString()) {
                p_list.append(ast_data[key].toString());
            } else if(ast_data[key].isArray()){
                p_list.append(ast_data[key].toArray());
            } else if(ast_data[key].isObject()) {
                p_list.append(ast_data[key].toObject());
            }
        }

        InfoLog::inst().print("Use Func: ");
        qDebug() << params_js["func"].toArray();
        InfoLog::inst().print("params: ");
        qDebug() << p_list;

        QString func_name = params_js["func"].toArray()[0].toString();
        //执行操作.
        auto map = func_map_.map();
        int res = map[func_name](p_list);
        return res;
    } else if(status=="error"){
        QString root_error_log = ast_root["error_log"].toString();
        QString data_error_log = ast_root["data"]["error_log"].toString();

        InfoLog::inst().print("---------Error Log---------");
        InfoLog::inst().print(root_error_log);
        InfoLog::inst().print(data_error_log);
        return -1;
    } else {
        InfoLog::inst().print("Wrong Status:"+status);
        return -2;
    }
}


QJsonObject Executor::find_params(const QString& cmd,const QString& op,const QString& objetc) {

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



