#ifndef UTILS_H
#define UTILS_H

#include "all_constraints.h"
#include <QJsonDocument>

void printJs(const QJsonObject &jsonObj);
Constraint* CreateConstraint(const QString& cs_type,const QString& cname,const QJsonArray& csparams);

#endif // UTILS_H




