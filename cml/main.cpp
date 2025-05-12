#include <QDebug>

#include "parser.h"
#include "executor.h"
#include "dataprocessor.h"
#include "utils.h"



void test_sql() {
    Parser p;
    Executor e;
    // QMap<QString,QList<QString>> test_sqls = p.test_sql_map();
    QMap<QString, QList<QString>> test_sqls = {
        {"CREATE", {
                       "CREATE DATABASE test_db;",
                       "USE test_db;",
                       "CREATE TABLE users (id INT PRIMARY_KEY, name STRING, email STRING);",
                       "CREATE TABLE orders (order_id INT PRIMARY_KEY, user_id INT, amount STRING);"
                   }},
        {"Other", {
                    "USE test_db;",
                    // "ALTER TABLE orders ADD CONSTRAINT user_id NOT_NULL;",
                    // "ALTER TABLE orders DELETE CONSTRAINT user_id NOT_NULL;"
                      "ALTER TABLE orders RENAME COLUMN user_id TO user_iddddddddd;"
                   }},
    };

    QStringList ss = {"Other"};
    for(const auto&s: ss) {
        qDebug() << "---------------" << s << "---------------";

        for(auto& sql: test_sqls[s]) {
            qDebug() << sql;
            QJsonObject ast_root = p.parse_sql(sql);
            // printJs(ast_root);
            int res = e.execute_ast(ast_root);
            qDebug() << "---> res:" << res;
            qDebug() << "\n\n";
        }
    }
}


int main() {

    DataProcessor::GetInstance().Read(0);
    DataProcessor::GetInstance().CreateUser("root","123456");
    DataProcessor::GetInstance().Login("root","123456");

    qDebug() << "-------------";
    test_sql();
    DataProcessor::GetInstance().Write();

    return 0;
}
