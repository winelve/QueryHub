#ifndef FUNCMAP_H
#define FUNCMAP_H

#include <functional>
#include <QMap>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>

#include "dataprocessor.h"


class FuncMap{
public:
    FuncMap();

    void setup();

    QJsonObject get_params_js() {
        return params_map_;
    }

    QMap<QString,std::function<int(const QVariantList &params,QJsonDocument& returnData)>> map() {
        return func_map_;
    }

private:
    DataProcessor& db;
    QMap<QString,std::function<int(const QVariantList &params,QJsonDocument& returnData)>> func_map_;
    QJsonObject params_map_;

    void read_js_file();
};

#endif // FUNCMAP_H







