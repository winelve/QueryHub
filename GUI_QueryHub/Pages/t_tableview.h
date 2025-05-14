#pragma once

#include "T_BasePage.h"
#include <QAbstractTableModel>
#include <vector>

class ElaTableView;
class T_TableViewModel;

class T_TableView : public T_BasePage
{
    Q_OBJECT
public:
    explicit T_TableView(const QString& tableName = "", QWidget* parent = nullptr);
    ~T_TableView() override;

    // 设置表格数据
    void setTableData(const std::vector<std::vector<std::string>>& data);

    // 设置表名作为标题
    void setTableTitle(const QString& title);

private:
    ElaTableView* _tableView{nullptr};
    T_TableViewModel* _tableModel{nullptr};
    QString _tableName; // 存储表名
};
