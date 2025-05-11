#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "FuncMap.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>


class Executor
{
public:
    Executor();
    int execute_ast(const QJsonObject& ast_root);

private:
    FuncMap func_map_;
    QJsonObject params_map_;

    QJsonObject find_params(const QString& cmd,const QString& op,const QString& objetc);
};

#endif // EXECUTOR_H
