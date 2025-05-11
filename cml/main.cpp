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
                       "CREATE TABLE users (id INT NOT_NULL PRIMARY_KEY, name STRING, email STRING);",
                       "CREATE TABLE orders (order_id INT AUTO_INCREMENT PRIMARY_KEY, user_id INT, amount STRING);"
                   }},/*
        {"ALTER", {
                      "ALTER TABLE users ADD COLUMN age INT;",
                      "ALTER TABLE users MODIFY email STRING;",
                      "ALTER TABLE orders ADD COLUMN status STRING;"
                  }},*/
        // {"DELETE", {
        //                "DROP TABLE users;",
        //                "DROP DATABASE test_db;"
        //            }}
    };

    QStringList ss = {"CREATE"};
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

    // DataProcessor::GetInstance().Read(0);
    DataProcessor::GetInstance().CreateUser("root","123456");
    DataProcessor::GetInstance().Login("root","123456");

    qDebug() << "-------------";
    test_sql();
    DataProcessor::GetInstance().Write();

    return 0;
}
