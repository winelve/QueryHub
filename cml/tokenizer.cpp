#include "tokenizer.h"

Tokenizer::Tokenizer():cursor_(0) {}

void Tokenizer::init(const QString& str) {
    this->string_ = str;
    this->cursor_ = 0;
}

bool Tokenizer::hasNextToken() {
    return cursor_ < string_.length();
}

QString Tokenizer::match_(const QRegularExpression& reg,const QString& str){
    QRegularExpressionMatch match = reg.match(str);
    if(!match.hasMatch()){
        return QString();
    }
    QString matched_str = match.captured();
    this->cursor_ += matched_str.length();
    return matched_str;
}

QMap<QString,QString> Tokenizer::getNextToken() {
    if (!hasNextToken()) return QMap<QString,QString>();

    const QString now_str = string_.mid(this->cursor_);

    for (const auto& [reg,type]: Rules) {
        QString tokenValue = this->match_(reg,now_str);
        if(tokenValue.isEmpty()) continue;

        if(type.isEmpty()) {
            return this->getNextToken();
        }

        // qDebug() << tokenValue << "====" << now_str;
        return {{"type",type},{"value",tokenValue}};
    }


    throw std::runtime_error(QString("SyntaxError.Unexpected token:").arg(now_str[0]).toStdString());
}

const QList<QPair<QRegularExpression,QString>> Tokenizer::Rules = {
    // Whitespace (skipped)
    {QRegularExpression("^\\s+"), ""},

    // Comments (skipped)
    {QRegularExpression("^--.*"), ""},                  // Single-line comment with --
    {QRegularExpression("^#.*"), ""},                   // Single-line comment with #
    {QRegularExpression("^/\\*[\\s\\S]*?\\*/"), ""},   // Multi-line comment /* ... */

    // Symbols and delimiters
    {QRegularExpression("^;"), ";"},                    // Statement terminator
    {QRegularExpression("^,"), ","},                    // Separator
    {QRegularExpression("^\\."), "."},                  // Qualifier (e.g., table.column)
    {QRegularExpression("^\\("), "("},                 // Opening parenthesis
    {QRegularExpression("^\\)"), ")"},                 // Closing parenthesis
    {QRegularExpression("^\\*"), "*"},                  // Asterisk (e.g., SELECT *)

    // Common MySQL Keywords (case-insensitive)
    {QRegularExpression("^\\bselect\\b", QRegularExpression::CaseInsensitiveOption), "SELECT"},
    {QRegularExpression("^\\bfrom\\b", QRegularExpression::CaseInsensitiveOption), "FROM"},
    {QRegularExpression("^\\bwhere\\b", QRegularExpression::CaseInsensitiveOption), "WHERE"},
    {QRegularExpression("^\\binsert\\b", QRegularExpression::CaseInsensitiveOption), "INSERT"},
    {QRegularExpression("^\\binto\\b", QRegularExpression::CaseInsensitiveOption), "INTO"},
    {QRegularExpression("^\\bupdate\\b", QRegularExpression::CaseInsensitiveOption), "UPDATE"},
    {QRegularExpression("^\\bdelete\\b", QRegularExpression::CaseInsensitiveOption), "DELETE"},
    {QRegularExpression("^\\bcreate\\b", QRegularExpression::CaseInsensitiveOption), "CREATE"},
    {QRegularExpression("^\\btable\\b", QRegularExpression::CaseInsensitiveOption), "TABLE"},
    {QRegularExpression("^\\balter\\b", QRegularExpression::CaseInsensitiveOption), "ALTER"},
    {QRegularExpression("^\\bdrop\\b", QRegularExpression::CaseInsensitiveOption), "DROP"},
    {QRegularExpression("^\\band\\b", QRegularExpression::CaseInsensitiveOption), "AND"},
    {QRegularExpression("^\\bor\\b", QRegularExpression::CaseInsensitiveOption), "OR"},
    {QRegularExpression("^\\bnot\\b", QRegularExpression::CaseInsensitiveOption), "NOT"},
    {QRegularExpression("^\\bis\\b", QRegularExpression::CaseInsensitiveOption), "IS"},
    {QRegularExpression("^\\bnull\\b", QRegularExpression::CaseInsensitiveOption), "NULL"},
    {QRegularExpression("^\\blike\\b", QRegularExpression::CaseInsensitiveOption), "LIKE"},
    {QRegularExpression("^\\bin\\b", QRegularExpression::CaseInsensitiveOption), "IN"},
    {QRegularExpression("^\\bbetween\\b", QRegularExpression::CaseInsensitiveOption), "BETWEEN"},
    {QRegularExpression("^\\bvalues\\b", QRegularExpression::CaseInsensitiveOption), "VALUES"},

    // Numbers
    {QRegularExpression("^\\d+\\.\\d+"), "NUMBER"},    // Floating-point numbers
    {QRegularExpression("^\\d+"), "NUMBER"},            // Integers

    // Identifiers
    {QRegularExpression("^`[^`]*`"), "IDENTIFIER"},     // Quoted identifiers (e.g., `table_name`)
    {QRegularExpression("^[a-zA-Z_]\\w*"), "IDENTIFIER"}, // Unquoted identifiers (e.g., table_name)

    // Operators (longer operators before shorter ones)
    {QRegularExpression("^<="), "RELATIONAL_OPERATOR"}, // Less than or equal
    {QRegularExpression("^>="), "RELATIONAL_OPERATOR"}, // Greater than or equal
    {QRegularExpression("^<>"), "INEQUALITY_OPERATOR"}, // Not equal
    {QRegularExpression("^!="), "INEQUALITY_OPERATOR"}, // Not equal (alternative)
    {QRegularExpression("^="), "EQUALITY_OPERATOR"},    // Equal
    {QRegularExpression("^<"), "RELATIONAL_OPERATOR"},  // Less than
    {QRegularExpression("^>"), "RELATIONAL_OPERATOR"},  // Greater than
    {QRegularExpression("^\\+"), "ADDITIVE_OPERATOR"},  // Addition
    {QRegularExpression("^-"), "ADDITIVE_OPERATOR"},    // Subtraction
    {QRegularExpression("^\\*"), "MULTIPLICATIVE_OPERATOR"}, // Multiplication
    {QRegularExpression("^\\/"), "MULTIPLICATIVE_OPERATOR"}, // Division
    {QRegularExpression("^%"), "MODULUS_OPERATOR"},      // Modulus

    // Strings
    {QRegularExpression("^'[^']*'"), "STRING"},         // Single-quoted strings
    {QRegularExpression("^\"[^\"]*\""), "STRING"}       // Double-quoted strings
};

