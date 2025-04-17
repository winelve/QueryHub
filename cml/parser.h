#ifndef PARSER_H
#define PARSER_H

#include <QMap>
#include <QString>
#include <QVector>
#include <stdexcept>
#include "tokenizer.h"

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    QString type;
};

// Program node: root of the AST
class ProgramNode : public ASTNode {
public:
    ProgramNode(QVector<ASTNode*> body) : body_(body) { type = "Program"; }
    ~ProgramNode() {
        for (auto* node : body_) delete node;
    }
    QVector<ASTNode*> body_;
};

// Identifier node
class IdentifierNode : public ASTNode {
public:
    IdentifierNode(const QString& name) : name_(name) { type = "Identifier"; }
    QString name_;
};

// Literal nodes
class NumericLiteralNode : public ASTNode {
public:
    NumericLiteralNode(double value) : value_(value) { type = "NumericLiteral"; }
    double value_;
};

class StringLiteralNode : public ASTNode {
public:
    StringLiteralNode(const QString& value) : value_(value) { type = "StringLiteral"; }
    QString value_;
};

// Select statement node
class SelectStatementNode : public ASTNode {
public:
    SelectStatementNode(QVector<ASTNode*> selectList, ASTNode* fromClause, ASTNode* whereClause = nullptr)
        : selectList_(selectList), fromClause_(fromClause), whereClause_(whereClause) {
        type = "SelectStatement";
    }
    ~SelectStatementNode() {
        for (auto* node : selectList_) delete node;
        delete fromClause_;
        delete whereClause_;
    }
    QVector<ASTNode*> selectList_;
    ASTNode* fromClause_;
    ASTNode* whereClause_;
};

// Binary expression node
class BinaryExpressionNode : public ASTNode {
public:
    BinaryExpressionNode(const QString& operator_, ASTNode* left, ASTNode* right)
        : operator_(operator_), left_(left), right_(right) {
        type = "BinaryExpression";
    }
    ~BinaryExpressionNode() {
        delete left_;
        delete right_;
    }
    QString operator_;
    ASTNode* left_;
    ASTNode* right_;
};

// Create Table语句节点
class CreateTableStatementNode : public ASTNode {
public:
    CreateTableStatementNode(ASTNode* tableName, const QVector<QPair<QString, QString>>& columns)
        : tableName_(tableName), columns_(columns) {
        type = "CreateTableStatement";
    }
    ~CreateTableStatementNode() {
        delete tableName_;
    }
    ASTNode* tableName_;
    QVector<QPair<QString, QString>> columns_; // 列名和类型对
};

// Insert语句节点
class InsertStatementNode : public ASTNode {
public:
    InsertStatementNode(ASTNode* tableName, const QVector<QString>& columns, const QVector<ASTNode*>& values)
        : tableName_(tableName), columns_(columns), values_(values) {
        type = "InsertStatement";
    }
    ~InsertStatementNode() {
        delete tableName_;
        for (auto* node : values_) delete node;
    }
    ASTNode* tableName_;
    QVector<QString> columns_;
    QVector<ASTNode*> values_;
};

// Update语句节点
class UpdateStatementNode : public ASTNode {
public:
    UpdateStatementNode(ASTNode* tableName, const QMap<QString, ASTNode*>& assignments, ASTNode* whereClause)
        : tableName_(tableName), assignments_(assignments), whereClause_(whereClause) {
        type = "UpdateStatement";
    }
    ~UpdateStatementNode() {
        delete tableName_;
        for (auto it = assignments_.begin(); it != assignments_.end(); ++it) {
            delete it.value();
        }
        delete whereClause_;
    }
    ASTNode* tableName_;
    QMap<QString, ASTNode*> assignments_;
    ASTNode* whereClause_;
};

// Delete语句节点
class DeleteStatementNode : public ASTNode {
public:
    DeleteStatementNode(ASTNode* tableName, ASTNode* whereClause)
        : tableName_(tableName), whereClause_(whereClause) {
        type = "DeleteStatement";
    }
    ~DeleteStatementNode() {
        delete tableName_;
        delete whereClause_;
    }
    ASTNode* tableName_;
    ASTNode* whereClause_;
};

// Drop Table语句节点
class DropTableStatementNode : public ASTNode {
public:
    DropTableStatementNode(ASTNode* tableName)
        : tableName_(tableName) {
        type = "DropTableStatement";
    }
    ~DropTableStatementNode() {
        delete tableName_;
    }
    ASTNode* tableName_;
};



class Parser {
public:
    Parser();
    ASTNode* parse(const QString& sql);

private:
    QString _string;
    Tokenizer _tokenizer;
    QMap<QString, QString> _currentToken;

    QMap<QString, QString> eat(const QString& tokenType);
    ASTNode* Program();
    QVector<ASTNode*> StatementList();
    ASTNode* Statement();
    SelectStatementNode* SelectStatement();
    QVector<ASTNode*> SelectList();
    ASTNode* Identifier();
    ASTNode* Expression();
    ASTNode* LogicalOrExpression();
    ASTNode* LogicalAndExpression();
    ASTNode* EqualityExpression();
    ASTNode* RelationalExpression();
    ASTNode* AdditiveExpression();
    ASTNode* MultiplicativeExpression();
    ASTNode* PrimaryExpression();
    ASTNode* Literal();
    // 在Parser类的private部分添加以下方法声明
    ASTNode* CreateTableStatement();
    ASTNode* InsertStatement();
    ASTNode* UpdateStatement();
    ASTNode* DeleteStatement();
    ASTNode* DropTableStatement();
    QVector<QPair<QString, QString>> ColumnDefinitionList(); // 用于创建表时的列定义
    QVector<QString> ColumnNameList();     // 用于INSERT语句中的列名列表
    QVector<ASTNode*> ValueList();         // 用于INSERT语句中的值列表
    QMap<QString, ASTNode*> AssignmentList(); // 用于UPDATE语句中的赋值列表


    bool isOperator(const QString& tokenType);
};

#endif // PARSER_H
