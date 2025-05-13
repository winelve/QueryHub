// #ifndef MAINWINDOW_H
// #define MAINWINDOW_H

// #include <QMainWindow>
// #include "ElaWindow.h"
// #include "Pages/t_about.h"
// #include "Pages/t_setting.h"
// #include "Pages/t_adddatabase.h"
// #include "Pages/t_addtable.h"
// #include "Pages/t_deletedatabase.h"
// #include "Pages/t_deletetable.h"
// #include "Pages/t_addfields.h"
// #include "Pages/t_deletefield.h"
// #include "Pages/t_tableview.h"
// class ElaContentDialog;

// class MainWindow : public ElaWindow
// {
//     Q_OBJECT

// public:
//     MainWindow(QWidget* parent = nullptr);
//     ~MainWindow();

//     void initWindow();
//     void initEdgeLayout();
//     void initContent();

// private slots:
//     // 点击事件
//     void click_link_btn();    //创建连接
//     // 树状结构
//     void init_treeview(const QString linkkey);   // 初始化树状结构

// private:
//     // 界面
//     ElaContentDialog* _closeDialog{nullptr};
//     T_About* _aboutPage{nullptr};
//     T_Setting* _settingPage{nullptr};
//     T_AddDataBase* _addDataBasePage{nullptr};
//     T_AddTable* _addTablePage{nullptr};
//     T_DeleteDataBase* _delDataBasePage{nullptr};
//     T_DeleteTable* _delTablePage{nullptr};
//     T_AddFields* _addFieldsPage{nullptr};
//     T_DeleteField* _delFieldsPage{nullptr};
//     T_TableView* _tableview{nullptr};
//     QString _aboutKey{""};
//     QString _settingKey{""};

//     // 状态
//     std::string current_database = ""; // 当前选用的数据库
//     std::string current_table = "";    // 当前查询表

//     // 储所有连接名
//     QStringList _connectionNames;

// };

// #endif // MAINWINDOW_H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ElaTabWidget.h>
#include "ElaWindow.h"
#include "Pages/t_about.h"
#include "Pages/t_setting.h"
#include "Pages/t_adddatabase.h"
#include "Pages/t_addtable.h"
#include "Pages/t_deletedatabase.h"
#include "Pages/t_deletetable.h"
#include "Pages/t_addfields.h"
#include "Pages/t_deletefield.h"
#include "Pages/t_tableview.h"
class ElaContentDialog;

class MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void initWindow();
    void initEdgeLayout();
    void initContent();
    void init_treeview(const QString& linkkey); // 初始化树状结构
    void click_link_btn(); // 连接按钮点击

    T_Setting* _settingPage{nullptr};
    T_About* _aboutPage{nullptr};
    T_AddDataBase* _addDataBasePage{nullptr};
    T_AddTable* _addTablePage{nullptr};
    T_AddFields* _addFieldsPage{nullptr};
    T_DeleteDataBase* _delDataBasePage{nullptr};
    T_DeleteTable* _delTablePage{nullptr};
    T_DeleteField* _delFieldsPage{nullptr};
    // T_TableView* __tabWidget{nullptr};

    QString _aboutKey;
    QString _settingKey;
    QStringList _connectionNames;
    QMap<QString, QPair<QString, QString>> _nodeMap; // nodeKey -> (database, table)
    ElaTabWidget* _tabWidget{nullptr};

private slots:
    // void onToolButton1Clicked();
    void onNavigationNodeClicked(ElaNavigationType::NavigationNodeType nodeType, QString nodeKey);
};

#endif // MAINWINDOW_H
