#include <QDebug>


#include "parser.h"
#include "utils.h"


void test_sql() {

    Parser p;
    QMap<QString,QList<QString>> test_sqls = p.test_sql_map();

    for(const auto&[op,sql_list]: test_sqls.asKeyValueRange()) {
        qDebug() << "---------------" << op << "---------------";

        for(auto& sql: sql_list) {
            qDebug() << sql;
            QJsonObject ast_root = p.parse_sql(sql);
            printJs(ast_root);
            qDebug() << "\n\n";
        }
    }

}


int main() {

    test_sql();

    return 0;
}
