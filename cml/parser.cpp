#include "parser.h"
#include <QRegularExpression>

Parser::Parser() {}

ASTNode* Parser::parse(const QString& sql) {
    _string = sql;
    _tokenizer.init(sql);
    _currentToken = _tokenizer.getNextToken();
    return Program();
}

QMap<QString, QString> Parser::eat(const QString& tokenType) {
    if (_currentToken.isEmpty()) {
        throw std::runtime_error("Unexpected end of input, expected: " + tokenType.toStdString());
    }
    if (_currentToken["type"] != tokenType) {
        throw std::runtime_error("Unexpected token: " + _currentToken["value"].toStdString() +
                                 ", expected: " + tokenType.toStdString());
    }
    QMap<QString, QString> token = _currentToken;
    _currentToken = _tokenizer.getNextToken();
    return token;
}

ASTNode* Parser::Program() {
    return new ProgramNode(StatementList());
}

QVector<ASTNode*> Parser::StatementList() {
    QVector<ASTNode*> statements;
    while (!_currentToken.isEmpty()) {
        statements.push_back(Statement());
    }
    return statements;
}

ASTNode* Parser::Statement() {
    if (_currentToken["type"] == "SELECT") {
        return SelectStatement();
    } else if (_currentToken["type"] == "CREATE") {
        return CreateTableStatement();
    } else if (_currentToken["type"] == "INSERT") {
        return InsertStatement();
    } else if (_currentToken["type"] == "UPDATE") {
        return UpdateStatement();
    } else if (_currentToken["type"] == "DELETE") {
        return DeleteStatement();
    } else if (_currentToken["type"] == "DROP") {
        return DropTableStatement();
    }
    throw std::runtime_error("Unsupported statement type: " + _currentToken["value"].toStdString());
}

// 这里需要添加CreateTableStatement, InsertStatement等方法的实现
// CreateTableStatement解析方法
ASTNode* Parser::CreateTableStatement() {
    eat("CREATE");
    eat("TABLE");
    ASTNode* tableName = Identifier();
    eat("(");
    QVector<QPair<QString, QString>> columns = ColumnDefinitionList();
    eat(")");
    eat(";");
    return new CreateTableStatementNode(tableName, columns);
}

// 解析列定义列表
QVector<QPair<QString, QString>> Parser::ColumnDefinitionList() {
    QVector<QPair<QString, QString>> columns;
    do {
        QMap<QString, QString> colNameToken = eat("IDENTIFIER");
        QString colName = colNameToken["value"];
        if (colName.startsWith("`") && colName.endsWith("`")) {
            colName = colName.mid(1, colName.length() - 2); // 移除反引号
        }

        QMap<QString, QString> colTypeToken = eat("IDENTIFIER");
        QString colType = colTypeToken["value"];

        // 可选：处理类型参数，如VARCHAR(50)
        if (_currentToken["type"] == "(") {
            eat("(");
            QMap<QString, QString> sizeToken = eat("NUMBER");
            colType += "(" + sizeToken["value"] + ")";
            eat(")");
        }

        columns.append(QPair<QString, QString>(colName, colType));
    } while (_currentToken["type"] == "," && (eat(","), true));

    return columns;
}

// InsertStatement解析方法
ASTNode* Parser::InsertStatement() {
    eat("INSERT");
    eat("INTO");
    ASTNode* tableName = Identifier();

    QVector<QString> columns;
    if (_currentToken["type"] == "(") {
        eat("(");
        columns = ColumnNameList();
        eat(")");
    }

    eat("VALUES");
    eat("(");
    QVector<ASTNode*> values = ValueList();
    eat(")");
    eat(";");

    return new InsertStatementNode(tableName, columns, values);
}

// 解析列名列表
QVector<QString> Parser::ColumnNameList() {
    QVector<QString> columns;
    do {
        QMap<QString, QString> colNameToken = eat("IDENTIFIER");
        QString colName = colNameToken["value"];
        if (colName.startsWith("`") && colName.endsWith("`")) {
            colName = colName.mid(1, colName.length() - 2); // 移除反引号
        }
        columns.append(colName);
    } while (_currentToken["type"] == "," && (eat(","), true));

    return columns;
}

// 解析值列表
QVector<ASTNode*> Parser::ValueList() {
    QVector<ASTNode*> values;
    do {
        if (_currentToken["type"] == "NUMBER") {
            values.append(new NumericLiteralNode(_currentToken["value"].toDouble()));
            eat("NUMBER");
        } else if (_currentToken["type"] == "STRING") {
            QString strValue = _currentToken["value"];
            strValue = strValue.mid(1, strValue.length() - 2); // 移除引号
            values.append(new StringLiteralNode(strValue));
            eat("STRING");
        } else {
            throw std::runtime_error("Expected value in value list");
        }
    } while (_currentToken["type"] == "," && (eat(","), true));

    return values;
}

// UpdateStatement解析方法
ASTNode* Parser::UpdateStatement() {
    eat("UPDATE");
    ASTNode* tableName = Identifier();
    eat("SET");
    QMap<QString, ASTNode*> assignments = AssignmentList();

    ASTNode* whereClause = nullptr;
    if (_currentToken["type"] == "WHERE") {
        eat("WHERE");
        whereClause = Expression();
    }

    eat(";");
    return new UpdateStatementNode(tableName, assignments, whereClause);
}

// 解析赋值列表
QMap<QString, ASTNode*> Parser::AssignmentList() {
    QMap<QString, ASTNode*> assignments;
    do {
        QMap<QString, QString> colNameToken = eat("IDENTIFIER");
        QString colName = colNameToken["value"];
        if (colName.startsWith("`") && colName.endsWith("`")) {
            colName = colName.mid(1, colName.length() - 2); // 移除反引号
        }

        eat("EQUALITY_OPERATOR"); // 吃掉 =

        ASTNode* value;
        if (_currentToken["type"] == "NUMBER") {
            value = new NumericLiteralNode(_currentToken["value"].toDouble());
            eat("NUMBER");
        } else if (_currentToken["type"] == "STRING") {
            QString strValue = _currentToken["value"];
            strValue = strValue.mid(1, strValue.length() - 2); // 移除引号
            value = new StringLiteralNode(strValue);
            eat("STRING");
        } else {
            throw std::runtime_error("Expected value in assignment");
        }

        assignments[colName] = value;
    } while (_currentToken["type"] == "," && (eat(","), true));

    return assignments;
}

// DeleteStatement解析方法
ASTNode* Parser::DeleteStatement() {
    eat("DELETE");
    eat("FROM");
    ASTNode* tableName = Identifier();

    ASTNode* whereClause = nullptr;
    if (_currentToken["type"] == "WHERE") {
        eat("WHERE");
        whereClause = Expression();
    }

    eat(";");
    return new DeleteStatementNode(tableName, whereClause);
}

// DropTableStatement解析方法
ASTNode* Parser::DropTableStatement() {
    eat("DROP");
    eat("TABLE");
    ASTNode* tableName = Identifier();
    eat(";");
    return new DropTableStatementNode(tableName);
}


SelectStatementNode* Parser::SelectStatement() {
    eat("SELECT");
    QVector<ASTNode*> selectList = SelectList();
    eat("FROM");
    ASTNode* fromClause = Identifier();
    ASTNode* whereClause = nullptr;
    if (_currentToken["type"] == "WHERE") {
        eat("WHERE");
        whereClause = Expression();
    }
    eat(";");
    return new SelectStatementNode(selectList, fromClause, whereClause);
}

QVector<ASTNode*> Parser::SelectList() {
    QVector<ASTNode*> items;
    do {
        if (_currentToken["type"] == "*") {
            items.push_back(new IdentifierNode(eat("*")["value"]));
        } else {
            items.push_back(Identifier());
        }
    } while (_currentToken["type"] == "," && (eat(","), true));
    return items;
}

ASTNode* Parser::Identifier() {
    QMap<QString, QString> token = eat("IDENTIFIER");
    QString name = token["value"];
    // Remove backticks for quoted identifiers
    if (name.startsWith("`") && name.endsWith("`")) {
        name = name.mid(1, name.length() - 2);
    }
    return new IdentifierNode(name);
}

ASTNode* Parser::Expression() {
    return LogicalOrExpression();
}

ASTNode* Parser::LogicalOrExpression() {
    ASTNode* left = LogicalAndExpression();
    while (_currentToken["type"] == "OR") {
        QString op = eat("OR")["value"];
        ASTNode* right = LogicalAndExpression();
        left = new BinaryExpressionNode(op, left, right);
    }
    return left;
}

ASTNode* Parser::LogicalAndExpression() {
    ASTNode* left = EqualityExpression();
    while (_currentToken["type"] == "AND") {
        QString op = eat("AND")["value"];
        ASTNode* right = EqualityExpression();
        left = new BinaryExpressionNode(op, left, right);
    }
    return left;
}

ASTNode* Parser::EqualityExpression() {
    ASTNode* left = RelationalExpression();
    while (_currentToken["type"] == "EQUALITY_OPERATOR" || _currentToken["type"] == "INEQUALITY_OPERATOR") {
        QString op = eat(_currentToken["type"])["value"];
        ASTNode* right = RelationalExpression();
        left = new BinaryExpressionNode(op, left, right);
    }
    return left;
}

ASTNode* Parser::RelationalExpression() {
    ASTNode* left = AdditiveExpression();
    while (_currentToken["type"] == "RELATIONAL_OPERATOR" || _currentToken["type"] == "LIKE") {
        QString op = eat(_currentToken["type"])["value"];
        ASTNode* right = AdditiveExpression();
        left = new BinaryExpressionNode(op, left, right);
    }
    return left;
}

ASTNode* Parser::AdditiveExpression() {
    ASTNode* left = MultiplicativeExpression();
    while (_currentToken["type"] == "ADDITIVE_OPERATOR") {
        QString op = eat("ADDITIVE_OPERATOR")["value"];
        ASTNode* right = MultiplicativeExpression();
        left = new BinaryExpressionNode(op, left, right);
    }
    return left;
}

ASTNode* Parser::MultiplicativeExpression() {
    ASTNode* left = PrimaryExpression();
    while (_currentToken["type"] == "MULTIPLICATIVE_OPERATOR" || _currentToken["type"] == "MODULUS_OPERATOR") {
        QString op = eat(_currentToken["type"])["value"];
        ASTNode* right = PrimaryExpression();
        left = new BinaryExpressionNode(op, left, right);
    }
    return left;
}

ASTNode* Parser::PrimaryExpression() {
    if (_currentToken["type"] == "IDENTIFIER") {
        return Identifier();
    }
    if (_currentToken["type"] == "NUMBER") {
        QMap<QString, QString> token = eat("NUMBER");
        bool ok;
        double value = token["value"].toDouble(&ok);
        if (!ok) throw std::runtime_error("Invalid number: " + token["value"].toStdString());
        return new NumericLiteralNode(value);
    }
    if (_currentToken["type"] == "STRING") {
        QMap<QString, QString> token = eat("STRING");
        QString value = token["value"].mid(1, token["value"].length() - 2); // Remove quotes
        return new StringLiteralNode(value);
    }
    if (_currentToken["type"] == "(") {
        eat("(");
        ASTNode* expr = Expression();
        eat(")");
        return expr;
    }
    throw std::runtime_error("Unexpected token in expression: " + _currentToken["value"].toStdString());
}

ASTNode* Parser::Literal() {
    if (_currentToken["type"] == "NUMBER") {
        QMap<QString, QString> token = eat("NUMBER");
        bool ok;
        double value = token["value"].toDouble(&ok);
        if (!ok) throw std::runtime_error("Invalid number: " + token["value"].toStdString());
        return new NumericLiteralNode(value);
    }
    if (_currentToken["type"] == "STRING") {
        QMap<QString, QString> token = eat("STRING");
        QString value = token["value"].mid(1, token["value"].length() - 2); // Remove quotes
        return new StringLiteralNode(value);
    }
    throw std::runtime_error("Unexpected literal: " + _currentToken["value"].toStdString());
}

bool Parser::isOperator(const QString& tokenType) {
    return tokenType == "EQUALITY_OPERATOR" ||
           tokenType == "INEQUALITY_OPERATOR" ||
           tokenType == "RELATIONAL_OPERATOR" ||
           tokenType == "ADDITIVE_OPERATOR" ||
           tokenType == "MULTIPLICATIVE_OPERATOR" ||
           tokenType == "MODULUS_OPERATOR" ||
           tokenType == "AND" ||
           tokenType == "OR" ||
           tokenType == "LIKE";
}
