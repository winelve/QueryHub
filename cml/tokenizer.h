#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QRegularExpression>
#include <QPair>
#include <QVector>
#include <QString>

class Tokenizer
{
public:
    explicit Tokenizer();
    void init(const QString& str);
    bool hasNextToken();
    QMap<QString,QString> getNextToken();
private:
    QString string_; // string --> ready to be token
    int cursor_; // position
    static const QList<QPair<QRegularExpression,QString>> Rules; //

    QString match_(const QRegularExpression& reg,const QString& str);

};

#endif // TOKENIZER_H
