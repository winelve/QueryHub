// interpreter.cpp
#include "interpreter.h"

Interpreter::Interpreter() {
    // qDebug() << "Interpreter instance created";
    db.loadFromFile("database.json");
}

Interpreter::~Interpreter() {
    // 销毁时自动保存数据
    db.saveToFile("database.json");
    // qDebug() << "Interpreter instance destroyed";
}

QVariant Interpreter::interpret(const QString& sql) {
    try {
        ASTNode* ast = parser.parse(sql);
        QVariant result = visit(ast);
        delete ast; // 清理AST
        return result;
    } catch (const std::exception& e) {
        qDebug() << "Interpretation error:" << e.what();
        return QVariant();
    }
}

QVariant Interpreter::visit(ASTNode* node) {
    if (!node) return QVariant();

    if (node->type == "Program") {
        return visitProgram(static_cast<ProgramNode*>(node));
    } else if (node->type == "SelectStatement") {
        return visitSelectStatement(static_cast<SelectStatementNode*>(node));
    } else if (node->type == "CreateTableStatement") {
        return visitCreateTableStatement(static_cast<CreateTableStatementNode*>(node));
    } else if (node->type == "InsertStatement") {
        return visitInsertStatement(static_cast<InsertStatementNode*>(node));
    } else if (node->type == "UpdateStatement") {
        return visitUpdateStatement(static_cast<UpdateStatementNode*>(node));
    } else if (node->type == "DeleteStatement") {
        return visitDeleteStatement(static_cast<DeleteStatementNode*>(node));
    } else if (node->type == "DropTableStatement") {
        return visitDropTableStatement(static_cast<DropTableStatementNode*>(node));
    } else if (node->type == "Identifier") {
        return visitIdentifier(static_cast<IdentifierNode*>(node));
    } else if (node->type == "NumericLiteral") {
        return visitNumericLiteral(static_cast<NumericLiteralNode*>(node));
    } else if (node->type == "StringLiteral") {
        return visitStringLiteral(static_cast<StringLiteralNode*>(node));
    } else if (node->type == "BinaryExpression") {
        return visitBinaryExpression(static_cast<BinaryExpressionNode*>(node));
    }

    qDebug() << "Unknown node type:" << node->type;
    return QVariant();
}

QVariant Interpreter::visitProgram(ProgramNode* node) {
    QVariant result;
    for (ASTNode* stmt : node->body_) {
        result = visit(stmt);
    }
    return result;
}

QVariant Interpreter::visitSelectStatement(SelectStatementNode* node) {
    // 获取表名
    QString tableName = visitIdentifier(static_cast<IdentifierNode*>(node->fromClause_)).toString();

    // 获取选择的列
    QStringList columns;
    for (ASTNode* col : node->selectList_) {
        columns.append(visitIdentifier(static_cast<IdentifierNode*>(col)).toString());
    }

    // 处理WHERE子句
    QString whereCondition;
    if (node->whereClause_) {
        whereCondition = evaluateWhereClause(node->whereClause_);
    }

    // 执行SELECT查询
    QVector<QMap<QString, QString>> results = db.selectRecords(tableName, columns, whereCondition);

    // 打印结果（实际应用中应返回给调用者）
    qDebug() << "Query results:";
    // for (const auto& record : results) {
    //     QStringList fields;
    //     for (auto it = record.begin(); it != record.end(); ++it) {
    //         fields.append(it.key() + ": " + it.value());
    //     }
    //     qDebug() << fields.join(", ");
    // }
    db.printTable(tableName);

    return QVariant::fromValue(results);
}

QVariant Interpreter::visitIdentifier(IdentifierNode* node) {
    return QVariant(node->name_);
}

QVariant Interpreter::visitNumericLiteral(NumericLiteralNode* node) {
    return QVariant(node->value_);
}

QVariant Interpreter::visitStringLiteral(StringLiteralNode* node) {
    return QVariant(node->value_);
}

QVariant Interpreter::visitBinaryExpression(BinaryExpressionNode* node) {
    QVariant left = visit(node->left_);
    QVariant right = visit(node->right_);

    // 简单实现，返回一个字符串表示该表达式
    return QVariant(
        left.toString() + " " + node->operator_ + " " + right.toString()
        );
}

QString Interpreter::evaluateWhereClause(ASTNode* whereClause) {
    // 简化实现，将表达式转换为字符串
    if (whereClause->type == "BinaryExpression") {
        BinaryExpressionNode* binExpr = static_cast<BinaryExpressionNode*>(whereClause);
        return visitBinaryExpression(binExpr).toString();
    }

    return QString();
}

// 实现新添加的访问方法
QVariant Interpreter::visitCreateTableStatement(CreateTableStatementNode* node) {
    QString tableName = visitIdentifier(static_cast<IdentifierNode*>(node->tableName_)).toString();

    QVector<Table::Column> columns;
    for (const auto& colDef : node->columns_) {
        Table::Column column;
        column.name = colDef.first;
        column.type = colDef.second;
        columns.append(column);
    }

    bool success = db.createTable(tableName, columns);
    if (success) db.saveToFile("database.json"); // 新增
    return QVariant(success);
}

QVariant Interpreter::visitInsertStatement(InsertStatementNode* node) {
    QString tableName = visitIdentifier(static_cast<IdentifierNode*>(node->tableName_)).toString();

    QMap<QString, QString> values;
    for (int i = 0; i < node->columns_.size(); i++) {
        QString column = node->columns_[i];
        QString value;

        ASTNode* valueNode = node->values_[i];
        if (valueNode->type == "NumericLiteral") {
            value = QString::number(static_cast<NumericLiteralNode*>(valueNode)->value_);
        } else if (valueNode->type == "StringLiteral") {
            value = static_cast<StringLiteralNode*>(valueNode)->value_;
        }

        values[column] = value;
    }

    bool success = db.insertRecord(tableName, values);
    if (success) db.saveToFile("database.json"); // 新增
    return QVariant(success);
}

QVariant Interpreter::visitUpdateStatement(UpdateStatementNode* node) {
    QString tableName = visitIdentifier(static_cast<IdentifierNode*>(node->tableName_)).toString();

    QMap<QString, QString> assignments;
    for (auto it = node->assignments_.begin(); it != node->assignments_.end(); ++it) {
        QString column = it.key();
        ASTNode* valueNode = it.value();
        QString value;

        if (valueNode->type == "NumericLiteral") {
            value = QString::number(static_cast<NumericLiteralNode*>(valueNode)->value_);
        } else if (valueNode->type == "StringLiteral") {
            value = static_cast<StringLiteralNode*>(valueNode)->value_;
        }

        assignments[column] = value;
    }

    QString whereCondition;
    if (node->whereClause_) {
        whereCondition = evaluateWhereClause(node->whereClause_);
    }

    bool success = db.updateRecords(tableName, assignments, whereCondition);
    if (success) db.saveToFile("database.json"); // 新增
    return QVariant(success);
}

QVariant Interpreter::visitDeleteStatement(DeleteStatementNode* node) {
    QString tableName = visitIdentifier(static_cast<IdentifierNode*>(node->tableName_)).toString();

    QString whereCondition;
    if (node->whereClause_) {
        whereCondition = evaluateWhereClause(node->whereClause_);
    }

    bool success = db.deleteRecords(tableName, whereCondition);
    if (success) db.saveToFile("database.json"); // 新增
    return QVariant(success);
}

QVariant Interpreter::visitDropTableStatement(DropTableStatementNode* node) {
    QString tableName = visitIdentifier(static_cast<IdentifierNode*>(node->tableName_)).toString();

    bool success = db.dropTable(tableName);
    if (success) db.saveToFile("database.json"); // 新增
    return QVariant(success);
}
