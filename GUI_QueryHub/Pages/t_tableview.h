#ifndef T_TABLEVIEW_H
#define T_TABLEVIEW_H

#include "t_basepage.h"
#include "ElaTableView.h"
#include "../modle/t_tableviewmodel.h"
#include <vector>

class T_TableView : public T_BasePage
{
    Q_OBJECT
public:
    explicit T_TableView(const QString& database, const QString& table, QWidget* parent = nullptr);
    ~T_TableView();
    void dis_table(const std::vector<std::vector<std::string>>& data);

private:
    ElaTableView* _tableView;
    T_TableViewModel* _model;
    QString _database;
    QString _table;
};

#endif // T_TABLEVIEW_H
