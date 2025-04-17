#ifndef UTILS_H
#define UTILS_H

#include <QString>

#include "parser.h"

void printASTNode(const ASTNode* node, int indent = 0) {
    QString indentStr(indent * 2, ' ');
    qDebug().noquote() << indentStr << "Type:" << node->type;

    if (const ProgramNode* program = dynamic_cast<const ProgramNode*>(node)) {
        qDebug().noquote() << indentStr << "Body:";
        for (const auto* stmt : program->body_) {
            printASTNode(stmt, indent + 1);
        }
    } else if (const SelectStatementNode* select = dynamic_cast<const SelectStatementNode*>(node)) {
        qDebug().noquote() << indentStr << "SelectList:";
        for (const auto* item : select->selectList_) {
            printASTNode(item, indent + 1);
        }
        qDebug().noquote() << indentStr << "FromClause:";
        printASTNode(select->fromClause_, indent + 1);
        if (select->whereClause_) {
            qDebug().noquote() << indentStr << "WhereClause:";
            printASTNode(select->whereClause_, indent + 1);
        }
    } else if (const IdentifierNode* ident = dynamic_cast<const IdentifierNode*>(node)) {
        qDebug().noquote() << indentStr << "Name:" << ident->name_;
    } else if (const NumericLiteralNode* num = dynamic_cast<const NumericLiteralNode*>(node)) {
        qDebug().noquote() << indentStr << "Value:" << num->value_;
    } else if (const StringLiteralNode* str = dynamic_cast<const StringLiteralNode*>(node)) {
        qDebug().noquote() << indentStr << "Value:" << str->value_;
    } else if (const BinaryExpressionNode* bin = dynamic_cast<const BinaryExpressionNode*>(node)) {
        qDebug().noquote() << indentStr << "Operator:" << bin->operator_;
        qDebug().noquote() << indentStr << "Left:";
        printASTNode(bin->left_, indent + 1);
        qDebug().noquote() << indentStr << "Right:";
        printASTNode(bin->right_, indent + 1);
    }
}


#endif // UTILS_H
