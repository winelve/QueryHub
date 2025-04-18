#include <QDebug>

#include "tokenizer.h"
#include "interpreter.h"
#include "utils.h"

void runQuery(Interpreter& interpreter, const QString& sql) {
    qDebug() << "\n=== Executing: " << sql << " ===";
    interpreter.interpret(sql);
}

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
    // QString sql3 = "ALTER students ;";
    try {
        Parser parser;
        ASTNode* ast = parser.parse(sql);
        qDebug() << "AST for query:" << sql;
        printASTNode(ast,4);
        delete ast;
    } catch (const std::runtime_error& e) {
        qDebug() << "Error parsing query:" << e.what();
    }

}


void test_query(){
    Interpreter interpreter;
    // Create a new table
    runQuery(interpreter, "CREATE TABLE students (id INT, name TEXT, grade FLOAT, department TEXT);");

    // Insert data into the table
    runQuery(interpreter, "INSERT INTO students (id, name, grade, department) VALUES (1, 'John', 85.5, 'Computer Science');");
    runQuery(interpreter, "INSERT INTO students (id, name, grade, department) VALUES (2, 'Emma', 92.0, 'Mathematics');");
    runQuery(interpreter, "INSERT INTO students (id, name, grade, department) VALUES (3, 'Michael', 78.3, 'Physics');");
    runQuery(interpreter, "INSERT INTO students (id, name, grade, department) VALUES (4, 'Sophia', 95.7, 'Computer Science');");

    // Select all records from the table
    runQuery(interpreter, "SELECT * FROM students;");


}

void test_query2(){
    Interpreter interpreter;
    // // Update records with WHERE condition
    runQuery(interpreter, "UPDATE students SET grade = 88.0;");

    // Verify the update
    runQuery(interpreter, "SELECT * FROM students;");


}

void test_query3() {
    Interpreter interpreter;

    runQuery(interpreter, "DELETE FROM students WHERE id = 1;");
    runQuery(interpreter, "SELECT * FROM students;");

}
void test_query4() {
    Interpreter interpreter;

    runQuery(interpreter,"DROP TABLE students;");
    runQuery(interpreter, "SELECT * FROM students;");
}

int main() {

    // test_tokenizer();
    // test_parser();
    // test_interpreter();
    // test_query();
    // test_query2();
    // test_query3();
    // test_query4();

    return 0;
}
