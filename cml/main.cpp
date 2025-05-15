#include <QDebug>
#include <QCoreApplication>

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


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv); // Create the application with event loop

    DataProcessor::GetInstance().Read(0);
    DataProcessor::GetInstance().CreateUser("root","123456");
    DataProcessor::GetInstance().Login("root","123456");

    Server server(8080);             // Start server on port 8080
    qDebug() << "Server is running...";

    DataProcessor::GetInstance().Write();
    return app.exec();               // Start the event loop
}
// int main() {
//     DataProcessor::GetInstance().Read(0);
//     DataProcessor::GetInstance().CreateUser("root","123456");
//     DataProcessor::GetInstance().Login("root","123456");

//     QString sql = "create database xt;"
//                   "create database wyq;"
//                   "show databases;"
//                   "drop database wyq;"
//                   "show databases;"
//                   "use xt;"
//                   "create table student(sno string primary_key, sname string, age int, address string);"
//                   "create table course(cno string primary_key, cname string);"
//                   "create table sc(sno string, cno string, cname string);"
//                   "create table teacher(tno string primary_key, tname string);"
//                   "show tables;"
//                   "drop table teacher;"
//                   "show tables;"
//                   "desc sc;"
//                   "alter table sc add grade string;"
//                   "alter table sc modify grade int;"
//                   "alter table sc drop cname;"
//                   "desc sc;"
//                   "alter table sc add constraint sno foregin_key(student, sno);"
//                   "alter table sc add constraint cno foregin_key(course, cno);"
//                   "alter table sc add constraint grade default(66);"
//                   "desc sc;"
//                   "alter table sc delete constraint grade default;"
//                   "desc sc;"
//                   "insert into student (sno, sname, age, address) values (23301082, xt, 19, beijing);"
//                   "insert into student (sno, sname, age, address) values (23301076, wyq, 19, beijing);"
//                   "insert into student (sno, sname, age, address) values (23301000, cat, 200, jiangxi);"
//                   "select (*) from student;"
//                   "insert into course (cno, cname) values (1, sleep);"
//                   "insert into course (cno, cname) values (2, eat);"
//                   "insert into course (cno, cname) values (3, play);"
//                   "select (*) from course;"
//                   "insert into sc (sno, cno, grade) values (23301082, 1, 66);"
//                   "insert into sc (sno, cno, grade) values (23301082, 2, 90);"
//                   "insert into sc (sno, cno, grade) values (23301082, 3, 80);"
//                   "insert into sc (sno, cno, grade) values (23301076, 2, 77);"
//                   "insert into sc (sno, cno, grade) values (23301076, 3, 77);"
//                   "insert into sc (sno, cno, grade) values (23301000, 3, 59);"
//                   "select (*) from sc;"
//                   "insert into student (sno, sname, age, address) values (23301082, xt, 20, jiangxi);"
//                   "select (*) from student;"
//                   "insert into sc (sno, course, grade) values (24301000, 3, 88);"
//                   "select (*) from sc;";
//     Parser p;
//     Executor e;

//     QList<QString> sqlList = p.split_sqls(sql);
//     QJsonArray results;
//     for (const QString& sql : sqlList) {
//         QJsonObject ast = p.parse_sql(sql);
//         QJsonDocument returnData;
//         int code = e.execute_ast(ast, returnData);
//         QJsonObject result = returnData.object();
//         results.append(result);

//         qDebug() << "code: " << code;
//         qDebug() << "returnData: " << returnData;
//         qDebug() << "\n\n";
//     }


//     DataProcessor::GetInstance().Write();
//     return 0;
// }

