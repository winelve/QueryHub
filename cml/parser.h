#ifndef PARSER_H
#define PARSER_H

#include <QString>

#include "sqlcmd.h"
#include "sqlparameters.h"

class Parser
{
public:
    Parser();

    SqlParameters parse_sql(const QString &sql);

private:
    QString parse_first_word_(const QString &sql);

    CreateParser create_parser;
    AlterParser alter_parser;
    DropParser drop_parser;

};

#endif // PARSER_H
