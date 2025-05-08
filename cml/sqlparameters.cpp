#include "sqlparameters.h"

SqlParameters::SqlParameters() {}


void SqlParameters::add_str(
    const QString& key,
    const QJsonValue& value)
{
    (*this)["key"] = value;
}


void SqlParameters::add_dict(
    const QString& key,
    const QJsonObject& dict)
{


}
