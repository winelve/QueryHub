// interpreter.h
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QString>
#include <QVariant>
#include "parser.h"
#include "database.h"

class Interpreter {
public:
    Interpreter();
    ~Interpreter();

    QVariant interpret(const QString& sql);

private:
    Parser parser;
    Database db;

    QVariant visit(ASTNode* node);
    QVariant visitProgram(ProgramNode* node);
    QVariant visitSelectStatement(SelectStatementNode* node);
    QVariant visitIdentifier(IdentifierNode* node);
    QVariant visitNumericLiteral(NumericLiteralNode* node);
    QVariant visitStringLiteral(StringLiteralNode* node);
    QVariant visitBinaryExpression(BinaryExpressionNode* node);
    // 在interpreter.h的Interpreter类中添加以下方法
    QVariant visitCreateTableStatement(CreateTableStatementNode* node);
    QVariant visitInsertStatement(InsertStatementNode* node);
    QVariant visitUpdateStatement(UpdateStatementNode* node);
    QVariant visitDeleteStatement(DeleteStatementNode* node);
    QVariant visitDropTableStatement(DropTableStatementNode* node);

    QString evaluateWhereClause(ASTNode* whereClause);
};

#endif // INTERPRETER_H
