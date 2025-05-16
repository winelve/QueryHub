#pragma once

#include <QMap>
#include <QPair>
#include "client/client.h"
#include "ElaToolButton.h"
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
#include <QJsonObject>
#include <QJsonArray>

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
class ElaPushButton;
class Client;

class MainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void initWindow();
    void initEdgeLayout();
    void initContent();
    void click_run_btn();
    void click_link_btn();
    void init_treeview(const QString& parentKey);
    void refreshConnectionTree(const QString& nodeKey);
    void onlinkNodeClicked(QString nodeKey);


private slots:
    void onNavigationNodeClicked(ElaNavigationType::NavigationNodeType nodeType, QString nodeKey);
    void refreshAllConnections();

    int create_db(const QString& dbName) {
        QJsonArray response = clients_map_.first()->handle_sql(QString("create database %1;").arg(dbName));
        int code = response[0].toObject()["code"].toInt();
        return code;
    }

    int create_tb(const QString& dbName,
                  const QString& tableName,
                  const QList<TableProperty>& properties)
    {
        // clients_map_.first()->handle_sql(QString("use %1;").arg(dbName));
        QJsonArray response = clients_map_.first()->handle_sql(QString("create table %1 (%2 string);").arg(dbName,properties[0].name));
        int code = response[0].toObject()["code"].toInt();
        return code;
    }

    // int add_column(const QString& dbName, const QString& tableName,
    //                const QList<QPair<QString, QString>>& fields);

    // int drop_db(const QString& dbName);
    // int drop_tb(const QString& dbName, const QString& tableName);

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
    ElaToolButton* run_btn{nullptr};

    QString _aboutKey;
    QMap<QString, QPair<QString, QString>> _nodeMap;
    QMap<QString, QString> linkkey_map;
    QVector<QString> _connectionNames;
    QMap<QString, Client*> clients_map_;

    // 新增：记录每个连接节点的子节点
    QMap<QString, QStringList> _childNodesMap; // 映射：linkKey -> 子节点 nodeKey 列表
};
