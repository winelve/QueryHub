#include <QDebug>

#include "tokenizer.h"
#include "interpreter.h"
#include "utils.h"

void test_tokenizer() {
    QString test_sql ="CREATE TABLE users (id INT, name VARCHAR(50));";
    QString test_sql2 = "INSERT INTO users (id, name, email) VALUES (1, 'John Doe', 'john@example.com');";
    Tokenizer t;
    t.init(test_sql2);
    while (t.hasNextToken()) {
        const auto token = t.getNextToken();
        if (!token.isEmpty())
            qDebug() << "{ type: " << token["type"] << ", value:" << token["value"] << "}";
    }
}

void test_parser() {

    // Example MySQL query
    QString sql = "CREATE TABLE users (id INT, name VARCHAR(50));";
    QString test_sql2 = "INSERT INTO users (id, name, email) VALUES (1, 'John Doe', 'john@example.com');";
    QString sql3 = "CREATE DATABASE a;";
    try {
        Parser parser;
        ASTNode* ast = parser.parse(sql3);
        qDebug() << "AST for query:" << sql;
        printASTNode(ast,4);
        delete ast;
    } catch (const std::runtime_error& e) {
        qDebug() << "Error parsing query:" << e.what();
    }

}

void test_interpreter() {
    // 创建解释器实例
    Interpreter interpreter;

    // 示例1：创建表
    qDebug() << "=== Example 1: Creating a table ===";
    QString createTableSQL = "CREATE TABLE users (id INT, name VARCHAR(50), email VARCHAR(100));";
    interpreter.interpret(createTableSQL);

    // 示例2：插入数据
    qDebug() << "=== Example 2: Inserting data ===";
    QString insertSQL = "INSERT INTO users (id, name, email) VALUES (1, 'John Doe', 'john@example.com');";
    interpreter.interpret(insertSQL);

    // 再插入一条数据
    insertSQL = "INSERT INTO users (id, name, email) VALUES (2, 'Jane Smith', 'jane@example.com');";
    interpreter.interpret(insertSQL);

    // 示例3：查询数据
    qDebug() << "=== Example 3: Querying data ===";
    QString selectSQL = "SELECT * FROM users;";
    interpreter.interpret(selectSQL);

    // 示例4：条件查询
    qDebug() << "=== Example 4: Querying with condition ===";
    QString selectWithWhereSQL = "SELECT name, email FROM users WHERE id = 1;";
    interpreter.interpret(selectWithWhereSQL);

    // 示例5：更新数据
    qDebug() << "=== Example 5: Updating data ===";
    QString updateSQL = "UPDATE users SET email = 'john.doe@example.com' WHERE id = 1;";
    interpreter.interpret(updateSQL);

    // 查询更新后的数据
    qDebug() << "=== Querying after update ===";
    interpreter.interpret(selectSQL);

    // 示例6：删除数据
    qDebug() << "=== Example 6: Deleting data ===";
    QString deleteSQL = "DELETE FROM users WHERE id = 2;";
    interpreter.interpret(deleteSQL);

    // 查询删除后的数据
    qDebug() << "=== Querying after delete ===";
    interpreter.interpret(selectSQL);

    // 示例7：删除表
    qDebug() << "=== Example 7: Dropping table ===";
    QString dropTableSQL = "DROP TABLE users;";
    interpreter.interpret(dropTableSQL);
}

int main() {

    // test_tokenizer();
    // test_parser();
    // test_interpreter();

    // 第一次运行：创建数据
    // {
    //     Interpreter interpreter;
    //     interpreter.interpret("CREATE TABLE users (id INT, name VARCHAR);");
    //     interpreter.interpret("INSERT INTO users (id, name) VALUES (1, 'Alice');");
    //     interpreter.interpret("INSERT INTO users (id, name) VALUES (2, 'Tom');");
    // } // 此处interpreter析构时自动保存

    // 第二次运行：验证数据持久化
    {
        Interpreter interpreter; // 自动加载数据
        interpreter.interpret("SELECT * FROM users;"); // 应能查询到Alice
        // interpreter.interpret("DROP TABLE users;"); // 删除表并自动保存
    }


    return 0;
}
