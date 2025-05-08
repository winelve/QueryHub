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

    Create create_parser;
    Alter alter_parser;
    Drop drop_parser;

};

#endif // PARSER_H
