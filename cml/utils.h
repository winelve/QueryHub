#ifndef UTILS_H
#define UTILS_H

#include "all_constraints.h"
#include <QJsonDocument>


void printJs(const QJsonObject &jsonObj);
Constraint* CreateConstraint(const QString& tb_name,const QString& cs_type,const QString& cname,const QJsonArray& csparams);
int map_operator(const QString& op);
void printStdV(const std::vector<std::string>& sv);
void PrintSelectRes(const std::vector<std::vector<std::any>>& returnRecords);

#endif // UTILS_H




