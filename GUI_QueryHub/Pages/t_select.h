#ifndef T_SELECT_H
#define T_SELECT_H

#include "t_basepage.h"
#include <ElaTabWidget.h>
#include <ElaPlainTextEdit.h>
#include <ElaTableView.h>
#include <QVector>

class T_Select : public T_BasePage
{
    Q_OBJECT
public:
    explicit T_Select(QWidget* parent = nullptr);
    ~T_Select();

private slots:
    void executeQuery();

private:
    ElaPlainTextEdit* _queryEdit;
    ElaTabWidget* _resultTabs;
    int _resultCount;
};

#endif // T_SELECT_H
