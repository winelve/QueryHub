#include <QDebug>

#include "parser.h"


int main() {

    Parser p;
    p.parse_sql("ALter table users;");


    return 0;
}
