#include "mainwindow.h"

#include "ElaContentDialog.h"
#include "ElaDockWidget.h"
#include "ElaEventBus.h"
#include "ElaLog.h"
#include "ElaMenu.h"
#include "ElaMenuBar.h"
#include "ElaProgressBar.h"
#include "ElaStatusBar.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToolBar.h"
#include "ElaToolButton.h"
#include <ElaMessageBar.h>
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "Pages/t_setting.h"
#include "Pages/t_connectpage.h"
#include "Pages/t_tableview.h"

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent)
{
    initWindow();
    initEdgeLayout();
    initContent();
    setIsDefaultClosed(true);
    moveToCenter();
    click_run_btn();
}

MainWindow::~MainWindow()
{
    delete _aboutPage;
    for (auto client : clients_map_) {
        delete client;
    }
}

void MainWindow::initWindow()
{
    setWindowIcon(QIcon(":/resource/images/icon.png"));
    resize(1200, 740);
    setUserInfoCardPixmap(QPixmap(":/resource/images/icon.png"));
    setUserInfoCardTitle("QueryHub");
    setUserInfoCardSubTitle("https://github.com/winelve/QueryHub.git");
    setWindowTitle("QueryHub");
}

void MainWindow::initEdgeLayout()
{
    ElaToolBar* toolBar = new ElaToolBar("工具栏", this);
    toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    toolBar->setToolBarSpacing(3);
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->setIconSize(QSize(25, 25));

    ElaToolButton* link_btn = new ElaToolButton(this);
    link_btn->setElaIcon(ElaIconType::Link);
    toolBar->addWidget(link_btn);
    connect(link_btn, &ElaToolButton::clicked, this, &MainWindow::click_link_btn);

    run_btn = new ElaToolButton(this);
    run_btn->setElaIcon(ElaIconType::Play);
    toolBar->addWidget(run_btn);

    ElaToolButton* refresh_btn = new ElaToolButton(this);
    refresh_btn->setElaIcon(ElaIconType::ArrowRotateRight);
    toolBar->addWidget(refresh_btn);
    connect(refresh_btn, &ElaToolButton::clicked, this, &MainWindow::refreshAllConnections);

    toolBar->addSeparator();

    ElaProgressBar* progressBar = new ElaProgressBar(this);
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
    progressBar->setFixedWidth(350);
    toolBar->addWidget(progressBar);

    addToolBar(Qt::TopToolBarArea, toolBar);
}

void MainWindow::initContent()
{
    _aboutPage = new T_About();
    _settingPage = new T_Setting(this);
    _addDataBasePage = new T_AddDataBase(this);
    _addTablePage = new T_AddTable(this);
    _addFieldsPage = new T_AddFields(this);
    _delDataBasePage = new T_DeleteDataBase(this);
    _delTablePage = new T_DeleteTable(this);
    _delFieldsPage = new T_DeleteField(this);
    _selectPage = new T_Select(this);

    QString footerKey;
    addFooterNode("Setting", _settingPage, footerKey, 0, ElaIconType::GearComplex);
    addFooterNode("About", nullptr, _aboutKey, 0, ElaIconType::User);
    _aboutPage->hide();

    connect(this, &ElaWindow::navigationNodeClicked, this, [=](ElaNavigationType::NavigationNodeType nodeType, QString nodeKey) {
        qDebug() << "Navigation node clicked, type:" << nodeType << ", key:" << nodeKey;
        if (_aboutKey == nodeKey) {
            _aboutPage->setFixedSize(575, 400);
            _aboutPage->moveToCenter();
            _aboutPage->show();
        } else if (nodeType == ElaNavigationType::PageNode) {
            onNavigationNodeClicked(nodeType, nodeKey);
        } else {
            qDebug() << "Ignoring non-PageNode click, key:" << nodeKey;
        }
    });

    QString fun_1_ddl;
    QString fun_2_add;
    QString fun_2_del;
    QString fun_2_alt;

    addExpanderNode("功能操作", fun_1_ddl, ElaIconType::BullseyeArrow);
    addExpanderNode("添加", fun_2_add, fun_1_ddl, ElaIconType::Plus);
    addExpanderNode("删除", fun_2_del, fun_1_ddl, ElaIconType::Minus);
    // addExpanderNode("修改", fun_2_alt, fun_1_ddl, ElaIconType::Pencil);

    addPageNode("添加数据库", _addDataBasePage, fun_2_add, ElaIconType::Database);
    // addPageNode("添加表", _addTablePage, fun_2_add, ElaIconType::Table);
    // addPageNode("添加字段", _addFieldsPage, fun_2_add, ElaIconType::PenField);
    addPageNode("删除数据库", _delDataBasePage, fun_2_del, ElaIconType::Database);
    // addPageNode("删除表", _delTablePage, fun_2_del, ElaIconType::Table);
    // addPageNode("删除字段", _delFieldsPage, fun_2_del, ElaIconType::PenField);
    addPageNode("查询", _selectPage, ElaIconType::CircleLocationArrow);
    connect(_addDataBasePage, &T_AddDataBase::databaseCreated, this, &MainWindow::handleSQL_function);
    connect(_delDataBasePage, &T_DeleteDataBase::databaseDeleted, this, &MainWindow::handleSQL_function);
}
void MainWindow::handleSQL_function(const QString& sql)
{

    Client* client = clients_map_.constBegin().value();
    try {
        // 执行 SQL
        client->handle_sql(sql);
        qDebug() << "SQL executed successfully:" << sql;
        ElaMessageBar::success(ElaMessageBarType::TopRight, "成功", "执行成功！", 2000);
    } catch (const std::exception& e) {
        qDebug() << "SQL execution failed:" << e.what();
        ElaMessageBar::error(ElaMessageBarType::TopRight, "错误", QString("SQL 执行失败: %1").arg(e.what()), 3000);
    }
}

void MainWindow::click_run_btn()
{
    connect(run_btn, &ElaToolButton::clicked, this, [this]() {
        if (!_selectPage) {
            ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "查询页面未初始化!", 3000);
            return;
        }

        QString sql = _selectPage->getQueryText().trimmed();
        if (sql.isEmpty()) {
            ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", "请输入 SQL 查询语句!", 3000);
            return;
        }

        if (clients_map_.isEmpty()) {
            ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无可用连接!", 3000);
            return;
        }

        _selectPage->resetResults();
        Client* client = clients_map_.begin().value();
        QJsonArray results = client->handle_sql(sql);
        if (results.isEmpty()) {
            ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", "查询结果为空!", 3000);
            return;
        }

        for (const QJsonValue& resultValue : results) {
            QJsonObject resultObj = resultValue.toObject();
            QString code = resultObj["code"].toString();
            QJsonValue dataValue = resultObj["data"];

            if (code != "200" && code != "0") {
                ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", QString("查询失败，状态码: %1").arg(code), 3000);
                continue;
            }

            if (dataValue.isArray()) {
                QJsonArray dataArray = dataValue.toArray();
                if (dataArray.isEmpty()) {
                    // ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", "查询结果为空!", 3000);
                    continue;
                }

                if (dataArray[0].isString()) {
                    std::vector<std::vector<std::string>> tableData;
                    for (const QJsonValue& val : dataArray) {
                        tableData.push_back({val.toString().toStdString()});
                    }
                    _selectPage->executeQuery(tableData);
                } else if (dataArray[0].isArray()) {
                    std::vector<std::vector<std::string>> tableData;
                    qDebug() << "Raw JSON data:" << dataArray;

                    for (const QJsonValue& rowValue : dataArray) {
                        QJsonArray rowArray = rowValue.toArray();
                        std::vector<std::string> row;

                        for (const QJsonValue& cell : rowArray) {
                            if (cell.isNull()) {
                                row.push_back("NULL");
                            } else if (cell.isString()) {
                                row.push_back(cell.toString().toStdString());
                            } else if (cell.isDouble()) {
                                double val = cell.toDouble();
                                if (val == std::floor(val)) {
                                    row.push_back(std::to_string(static_cast<int>(val)));
                                } else {
                                    row.push_back(std::to_string(val));
                                }
                            } else {
                                row.push_back("UNKNOWN_TYPE");
                            }
                        }
                        tableData.push_back(row);
                    }

                    qDebug() << "Processed tableData:";
                    for (const auto& row : tableData) {
                        QStringList rowData;
                        for (const auto& cell : row) {
                            rowData << QString::fromStdString(cell);
                        }
                        qDebug() << rowData.join(" | ");
                    }

                    _selectPage->executeQuery(tableData);
                } else {
                    ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "不支持的数据格式!", 3000);
                    continue;
                }
            } else if (dataValue.isString()) {
                std::vector<std::vector<std::string>> tableData = {{dataValue.toString().toStdString()}};
                _selectPage->executeQuery(tableData);
            } else {
                ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "未知的数据类型!", 3000);
                continue;
            }
        }

        ElaMessageBar::success(ElaMessageBarType::TopRight, "成功", "查询执行成功!", 2000);
    });
}

void MainWindow::click_link_btn()
{
    ElaDockWidget* connectWidget = new ElaDockWidget("新建连接", this);
    T_ConnectPage* _connectPage = new T_ConnectPage(this);
    connectWidget->setWidget(_connectPage);
    addDockWidget(Qt::RightDockWidgetArea, connectWidget);
    resizeDocks({connectWidget}, {200}, Qt::Horizontal);

    connect(_connectPage, &T_ConnectPage::connectionCreated, this, [this, _connectPage](const QString& name) {
        QVector<QString> s = _connectPage->getLinkMember();
        QString link_name = s[0];
        QString host = s[1];
        QString port = s[2];
        QString user_name = s[3];
        QString password = s[4];
        qDebug() << "host:" << host;
        qDebug() << "port:" << port;

        if (clients_map_.contains(link_name)) {
            ElaMessageBar::error(ElaMessageBarType::Bottom, "错误", QString("连接名 %1 已存在!").arg(link_name), 2500, this);
            return;
        }

        Client* client = new Client();
        if (!client->connectToServer(host, port.toInt()) || !client->login(user_name,password)) {
            qDebug() << "连接失败";
            ElaMessageBar::error(ElaMessageBarType::Bottom, "错误", "创建连接失败!", 2500, this);
            delete client;
            return;
        }

        clients_map_[link_name] = client;
        QString linkKey;
        ElaNavigationType::NodeOperateReturnType link_return_page = addExpanderNode(link_name, linkKey, ElaIconType::Link);
        QString parentKey = linkKey;
        if (link_return_page == ElaNavigationType::Success) {
            linkkey_map[parentKey] = link_name;
            qDebug() << "link_name" << link_name;
            qDebug() << "linkkey_map[parentKey]:" << linkkey_map[parentKey];
            _connectionNames.append(link_name);
            init_treeview(linkKey);
            ElaMessageBar::success(ElaMessageBarType::Bottom, "成功", "创建连接成功!", 2500, this);
            qDebug() << "连接成功！";
        } else {
            delete client;
            clients_map_.remove(link_name);
            linkkey_map.remove(parentKey);
            ElaMessageBar::error(ElaMessageBarType::Bottom, "错误", "添加连接节点失败!", 2500, this);
        }
    });
}



void MainWindow::onNavigationNodeClicked(ElaNavigationType::NavigationNodeType nodeType, QString nodeKey)
{
    if (!_nodeMap.contains(nodeKey)) {
        qDebug() << "Node key not found in _nodeMap:" << nodeKey;
        // ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无法找到表信息", 3000);
        return;
    }

    QString database = _nodeMap[nodeKey].first;
    QString table = _nodeMap[nodeKey].second;

    Client* client = nullptr;
    for (const auto& name : _connectionNames) {
        std::vector<std::string> dbs = clients_map_[name]->get_all_database();
        if (std::find(dbs.begin(), dbs.end(), database.toStdString()) != dbs.end()) {
            client = clients_map_[name];
            break;
        }
    }

    if (!client) {
        qDebug() << "Client not found for database:" << database;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无法找到对应的连接", 3000);
        return;
    }

    std::vector<std::vector<std::string>> tableData = client->select_table(database.toStdString(), table.toStdString());
    if (tableData.empty()) {
        qDebug() << "No data returned for table:" << table;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", QString("表 %1 为空").arg(table), 3000);
        return;
    }

    QWidget* targetWidget = nullptr;
    const auto& nodes = findChildren<QWidget*>();
    for (QWidget* widget : nodes) {
        if (widget->property("ElaPageKey").toString() == nodeKey) {
            targetWidget = widget;
            break;
        }
    }

    if (!targetWidget) {
        qDebug() << "Failed to find QWidget for nodeKey:" << nodeKey;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无法找到对应的界面", 3000);
        return;
    }

    if (targetWidget->layout()) {
        QLayout* oldLayout = targetWidget->layout();
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
    }

    T_TableView* tableView = new T_TableView(table, targetWidget);
    tableView->setTableData(tableData);

    QVBoxLayout* layout = new QVBoxLayout(targetWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(tableView);
    targetWidget->setLayout(layout);

    ElaMessageBar::success(ElaMessageBarType::TopRight, "成功", QString("表 %1 已打开").arg(table), 2000);
}

void MainWindow::onlinkNodeClicked(QString nodeKey)
{
    qDebug() << "-----------------------------------------------------------------Link node clicked, key:" << nodeKey;

    if (!linkkey_map.contains(nodeKey)) {
        qDebug() << "Invalid connection node key:" << nodeKey;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无效的连接节点！", 3000);
        return;
    }

    QString linkName = linkkey_map[nodeKey];
    qDebug() << "Refreshing connection:" << linkName;

    if (!clients_map_.contains(linkName)) {
        qDebug() << "Connection not found:" << linkName;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "连接未找到！", 3000);
        return;
    }

    // 清理与该连接相关的 _nodeMap 条目
    QMap<QString, QPair<QString, QString>> newNodeMap;
    for (auto it = _nodeMap.constBegin(); it != _nodeMap.constEnd(); ++it) {
        QString dbName = it.value().first;
        bool belongsToConnection = false;
        for (const auto& name : _connectionNames) {
            if (name == linkName) {
                std::vector<std::string> dbs = clients_map_[name]->get_all_database();
                if (std::find(dbs.begin(), dbs.end(), dbName.toStdString()) != dbs.end()) {
                    belongsToConnection = true;
                    break;
                }
            }
        }
        if (!belongsToConnection) {
            newNodeMap.insert(it.key(), it.value());
        }
    }
    _nodeMap = newNodeMap;

    // 清理 _childNodesMap 中的子节点记录
    if (_childNodesMap.contains(nodeKey)) {
        QStringList childNodes = _childNodesMap[nodeKey];
        qDebug() << "Clearing child nodes for" << nodeKey << ":" << childNodes;
        _childNodesMap[nodeKey].clear();
    } else {
        _childNodesMap[nodeKey] = QStringList();
    }

    // 重建导航树
    init_treeview(nodeKey);

    ElaMessageBar::success(ElaMessageBarType::TopRight, "成功", QString("连接 %1 已刷新").arg(linkName), 2000);
}

//------------------------------------------------------------------------------------------------------------
void MainWindow::init_treeview(const QString& parentKey)
{
    qDebug() << "Initializing tree view for connection under parentKey:" << parentKey;

    QString linkName = linkkey_map.value(parentKey, "");
    if (linkName.isEmpty() || !clients_map_.contains(linkName)) {
        qDebug() << "Connection not found for parentKey:" << parentKey;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "连接未找到！", 3000);
        return;
    }

    Client* client = clients_map_[linkName];
    std::vector<std::string> databases = client->get_all_database();
    if (databases.empty()) {
        qDebug() << "No databases found for connection:" << linkName;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", "无可用数据库！", 3000);
        return;
    }

    for (const auto& db : databases) {
        QString dbKey;
        QString dbName = QString::fromStdString(db);
        ElaNavigationType::NodeOperateReturnType result = addExpanderNode(dbName, dbKey, parentKey, ElaIconType::Database);
        if (result == ElaNavigationType::Success) {
            qDebug() << "Added database node:" << db << ", key:" << dbKey;
            _childNodesMap[parentKey].append(dbKey); // 记录数据库节点

            std::vector<std::string> tables = client->get_all_tables(db);
            if (tables.empty()) {
                qDebug() << db << "-数据库中没找到表！";
                continue;
            }

            for (const auto& tb : tables) {
                QString tbName = QString::fromStdString(tb);
                QWidget* tb_widget = new QWidget();
                ElaNavigationType::NodeOperateReturnType tbResult = addPageNode(tbName, tb_widget, dbKey, ElaIconType::Table);
                if (tbResult == ElaNavigationType::Success) {
                    QString nodeKey = tb_widget->property("ElaPageKey").toString();
                    _nodeMap[nodeKey] = {dbName, tbName};
                    _childNodesMap[parentKey].append(nodeKey); // 记录表节点
                    qDebug() << "Added table node:" << tb << ", nodeKey:" << nodeKey;
                } else {
                    qDebug() << "Failed to add table node:" << tb << ", result:" << tbResult;
                    delete tb_widget; // 防止内存泄漏
                }
            }
        } else {
            qDebug() << "Failed to add database node:" << db << ", result:" << result;
        }
    }

    qDebug() << "Updated child nodes for" << parentKey << ":" << _childNodesMap[parentKey];
}

void MainWindow::refreshConnectionTree(const QString& nodeKey)
{
    if (!linkkey_map.contains(nodeKey)) {
        qDebug() << "Node key is not a connection node:" << nodeKey;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无效的连接节点！", 3000);
        return;
    }

    QString linkName = linkkey_map[nodeKey];
    qDebug() << "Refreshing connection tree for:" << linkName;

    if (!clients_map_.contains(linkName)) {
        qDebug() << "Connection not found:" << linkName;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "连接未找到!", 3000);
        return;
    }

    // 清理与该连接相关的子节点
    if (_childNodesMap.contains(nodeKey)) {
        QStringList childNodes = _childNodesMap[nodeKey];
        for (const QString& childKey : childNodes) {
            // 从 _nodeMap 中移除相关条目
            _nodeMap.remove(childKey);
            // 清理 QWidget（如果存在）
            const auto& nodes = findChildren<QWidget*>();
            for (QWidget* widget : nodes) {
                if (widget->property("ElaPageKey").toString() == childKey) {
                    widget->deleteLater();
                    break;
                }
            }
        }
        _childNodesMap[nodeKey].clear(); // 清空子节点记录
        qDebug() << "Cleared child nodes for" << nodeKey;
    }

    // 重新初始化树状结构
    init_treeview(nodeKey);

    ElaMessageBar::success(ElaMessageBarType::TopRight, "成功", QString("连接 %1 已刷新").arg(linkName), 2000);
}

void MainWindow::refreshAllConnections()
{
    qDebug() << "Refreshing all connections";

    if (linkkey_map.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", "无可用连接！", 3000);
        return;
    }

    for (auto it = linkkey_map.constBegin(); it != linkkey_map.constEnd(); ++it) {
        QString nodeKey = it.key();
        qDebug() << "Processing connection nodeKey:" << nodeKey;
        onlinkNodeClicked(nodeKey);
    }

    ElaMessageBar::success(ElaMessageBarType::TopRight, "成功", "所有连接已刷新！", 2000);
}
