#pragma once

#include <QMap>
#include <QPair>
#include "ElaWindow.h"
#include "Pages/t_about.h"
#include "Pages/t_adddatabase.h"
#include "Pages/t_addfields.h"
#include "Pages/t_addtable.h"
#include "Pages/t_basepage.h"
#include "Pages/t_connectpage.h"
#include "Pages/t_deletedatabase.h"
#include "Pages/t_deletetable.h"
#include "Pages/t_deletefield.h"
#include "Pages/t_setting.h"
#include "Pages/t_tableview.h"
#include "Pages/t_select.h"
class T_About;
class T_Setting;
class T_AddDataBase;
class T_AddTable;
class T_AddFields;
class T_DeleteDataBase;
class T_DeleteTable;
class T_DeleteField;
class T_ConnectPage;
class T_TableView;
class T_Select;

class MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void initWindow();
    void initEdgeLayout();
    void initContent();
    void init_treeview(const QString& linkkey);
    void click_link_btn();
    void onNavigationNodeClicked(ElaNavigationType::NavigationNodeType nodeType, QString nodeKey);

private:
    T_About* _aboutPage{nullptr};
    T_Setting* _settingPage{nullptr};
    T_AddDataBase* _addDataBasePage{nullptr};
    T_AddTable* _addTablePage{nullptr};
    T_AddFields* _addFieldsPage{nullptr};
    T_DeleteDataBase* _delDataBasePage{nullptr};
    T_DeleteTable* _delTablePage{nullptr};
    T_DeleteField* _delFieldsPage{nullptr};
    T_Select* _selectPage{nullptr};
    QString _aboutKey;
    QString _settingKey;
    QStringList _connectionNames;

    // 存储节点信息：nodeKey -> (database, table)
    QMap<QString, QPair<QString, QString>> _nodeMap;

    // 当前显示的表格视图
    T_TableView* _currentTableView{nullptr};
};
