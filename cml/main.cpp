#include <QDebug>

#include "parser.h"
#include "executor.h"
#include "dataprocessor.h"
#include "server.h"
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
                    // "ALTER TABLE orders RENAME COLUMN user_iddddddddd TO user_id;"
                    // "INSERT INTO orders (order_id,user_id,amount) VALUES (1,233,900),(2,233,666),(3,234,444);",
                    "SELECT (*) FROM orders ORDER BY (order_id);",
                    "INSERT INTO orders (order_id,user_id,amount) VALUES (4,255,666);",
                    "SELECT (*) FROM orders ORDER BY (order_id);",
                    "DELETE orders WHERE (order_id=4);",
                    "SELECT (*) FROM orders ORDER BY (order_id);",
                    "SHOW TABLES;",
                   }},
    };

    QStringList ss = {"Other"};
    for(const auto&s: ss) {
        qDebug() << "---------------" << s << "---------------";

        for(auto& sql: test_sqls[s]) {
            qDebug() << sql;
            QJsonDocument send_doc;
            QJsonObject ast_root = p.parse_sql(sql);
            // printJs(ast_root);
            int res = e.execute_ast(ast_root,send_doc);
            qDebug() << "send_doc:";
            qDebug() << send_doc;
            qDebug() << "---> res:" << res;
            qDebug() << "\n\n";
        }
    }
}


int main() {

    DataProcessor::GetInstance().Read(0);
    DataProcessor::GetInstance().CreateUser("root","123456");
    DataProcessor::GetInstance().Login("root","123456");

    Server server(8080);
    if (server.start()) {
        server.run();
    }

    DataProcessor::GetInstance().Write();

    return 0;
}
