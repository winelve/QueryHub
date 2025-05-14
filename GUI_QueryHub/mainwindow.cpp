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

#include "Pages/t_setting.h"
#include "Pages/t_connectpage.h"
#include "Pages/t_tableview.h"

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent)
{
    initWindow();
    initEdgeLayout();
    initContent();
    this->setIsDefaultClosed(true);
    moveToCenter();
}

MainWindow::~MainWindow()
{
    delete _aboutPage;
}

void MainWindow::initWindow()
{
    setWindowIcon(QIcon(":/resource/images/icon.png"));
    resize(1200, 740);
    setUserInfoCardPixmap(QPixmap(":/resource/images/icon.png"));
    setUserInfoCardTitle("QureyHub");
    setUserInfoCardSubTitle("https://github.com/winelve/QueryHub.git");
    setWindowTitle("QureyHub");
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

    ElaToolButton* toolButton2 = new ElaToolButton(this);
    toolButton2->setElaIcon(ElaIconType::ChartUser);
    toolBar->addWidget(toolButton2);
    toolBar->addSeparator();
    ElaToolButton* toolButton3 = new ElaToolButton(this);
    toolButton3->setElaIcon(ElaIconType::Bluetooth);
    toolButton3->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButton3->setText("Bluetooth");
    toolBar->addWidget(toolButton3);
    ElaToolButton* toolButton4 = new ElaToolButton(this);
    toolButton4->setElaIcon(ElaIconType::BringFront);
    toolBar->addWidget(toolButton4);
    toolBar->addSeparator();
    ElaToolButton* toolButton5 = new ElaToolButton(this);
    toolButton5->setElaIcon(ElaIconType::ChartSimple);
    toolBar->addWidget(toolButton5);
    ElaToolButton* toolButton6 = new ElaToolButton(this);
    toolButton6->setElaIcon(ElaIconType::FaceClouds);
    toolBar->addWidget(toolButton6);
    ElaToolButton* toolButton8 = new ElaToolButton(this);
    toolButton8->setElaIcon(ElaIconType::Aperture);
    toolBar->addWidget(toolButton8);
    ElaToolButton* toolButton9 = new ElaToolButton(this);
    toolButton9->setElaIcon(ElaIconType::ChartMixed);
    toolBar->addWidget(toolButton9);
    ElaToolButton* toolButton10 = new ElaToolButton(this);
    toolButton10->setElaIcon(ElaIconType::Coins);
    toolBar->addWidget(toolButton10);
    ElaToolButton* toolButton11 = new ElaToolButton(this);
    toolButton11->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButton11->setElaIcon(ElaIconType::AlarmPlus);
    toolButton11->setText("AlarmPlus");
    toolBar->addWidget(toolButton11);
    ElaToolButton* toolButton12 = new ElaToolButton(this);
    toolButton12->setElaIcon(ElaIconType::Crown);
    toolBar->addWidget(toolButton12);

    ElaProgressBar* progressBar = new ElaProgressBar(this);
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
    progressBar->setFixedWidth(350);
    toolBar->addWidget(progressBar);

    this->addToolBar(Qt::TopToolBarArea, toolBar);
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

    addFooterNode("Setting", _settingPage, _settingKey, 0, ElaIconType::GearComplex);
    addFooterNode("About", nullptr, _aboutKey, 0, ElaIconType::User);
    _aboutPage->hide();
    connect(this, &ElaWindow::navigationNodeClicked, this, [=](ElaNavigationType::NavigationNodeType nodeType, QString nodeKey) {
        if (_aboutKey == nodeKey)
        {
            _aboutPage->setFixedSize(575, 400);
            _aboutPage->moveToCenter();
            _aboutPage->show();
        }
    });

    QString fun_1_dis;
    QString fun_1_ddl;
    QString fun_1_dml;
    QString fun_2_add;
    QString fun_2_del;
    QString fun_2_alt;

    addExpanderNode("功能操作", fun_1_ddl, ElaIconType::BullseyeArrow);
    addExpanderNode("查询", fun_1_dml, ElaIconType::CircleLocationArrow);
    addExpanderNode("添加", fun_2_add, fun_1_ddl, ElaIconType::Plus);
    addExpanderNode("删除", fun_2_del, fun_1_ddl, ElaIconType::Minus);
    addExpanderNode("修改", fun_2_alt, fun_1_ddl, ElaIconType::Pencil);

    addPageNode("添加数据库", _addDataBasePage, fun_2_add, ElaIconType::Database);
    addPageNode("添加表", _addTablePage, fun_2_add, ElaIconType::Table);
    addPageNode("添加字段", _addFieldsPage, fun_2_add, ElaIconType::PenField);
    addPageNode("删除数据库", _delDataBasePage, fun_2_del, ElaIconType::Database);
    addPageNode("删除表", _delTablePage, fun_2_del, ElaIconType::Table);
    addPageNode("删除字段", _delFieldsPage, fun_2_del, ElaIconType::PenField);
}

// 假设 DataProcessor 接口（保持不变）
class DataProcessor {
public:
    static DataProcessor& GetInstance() { static DataProcessor instance; return instance; }
    int ShowDatabases(std::vector<std::string>& databases) {
        databases = {"db1", "db2"};
        return 0;
    }
    int ShowTables(std::vector<std::string>& tables) {
        tables = {"tb1", "tb2"};
        return 0;
    }
    int UseDatabase(const std::string& database) {
        return 0;
    }
    std::vector<std::vector<std::string>> select_table(const std::string& database, const std::string& table) {
        return {
            {"id", "name", "age", "city", "salary", "department", "join_date", "status"},
            {"1", "Alice", "25", "Beijing", "5000", "HR", "2023-01-15", "Active"},
            {"2", "Bob", "30", "Shanghai", "6000", "IT", "2022-06-10", "Active"},
            {"3", "Charlie", "35", "Guangzhou", "7500", "Finance", "2021-11-20", "Inactive"},
            {"4", "David", "28", "Shenzhen", "5500", "Marketing", "2023-03-05", "Active"},
            {"5", "Eve", "32", "Hangzhou", "7000", "IT", "2022-09-12", "Active"},
            {"6", "Frank", "40", "Chengdu", "8000", "Finance", "2020-08-30", "Inactive"},
            {"7", "Grace", "27", "Nanjing", "5200", "HR", "2023-04-18", "Active"},
            {"8", "Hannah", "33", "Wuhan", "6500", "Marketing", "2022-07-25", "Active"},
            {"9", "Ivy", "29", "Xi'an", "5800", "IT", "2023-02-14", "Inactive"},
            {"10", "Jack", "36", "Chongqing", "7200", "Finance", "2021-12-01", "Active"},
            {"11", "Kelly", "31", "Tianjin", "6300", "HR", "2022-10-05", "Active"},
            {"12", "Liam", "34", "Changsha", "6900", "IT", "2021-05-22", "Inactive"},
            {"13", "Mia", "26", "Fuzhou", "5100", "Marketing", "2023-06-30", "Active"},
            {"14", "Noah", "38", "Qingdao", "7800", "Finance", "2020-09-15", "Active"},
            {"15", "Olivia", "29", "Kunming", "5600", "HR", "2022-11-08", "Inactive"},
            {"16", "Peter", "33", "Harbin", "6700", "IT", "2021-07-19", "Active"},
            {"17", "Quinn", "27", "Xiamen", "5300", "Marketing", "2023-03-22", "Active"},
            {"18", "Rose", "35", "Zhengzhou", "7100", "Finance", "2020-12-10", "Inactive"},
            {"19", "Sam", "30", "Suzhou", "6400", "IT", "2022-04-15", "Active"},
            {"20", "Tina", "32", "Dalian", "6800", "HR", "2021-08-25", "Active"}
        };
    }
};

void MainWindow::init_treeview(const QString& linkkey)
{
    qDebug() << "Initializing tree view for connection:" << linkkey;

    std::vector<std::string> databases;
    int ret = DataProcessor::GetInstance().ShowDatabases(databases);
    if (ret != 0) {
        qDebug() << "Error loading databases, error code:" << ret;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无法加载数据库列表！", 3000);
        return;
    }

    for (const auto& db : databases) {
        QString dbKey;
        QString dbName = QString::fromStdString(db);
        addExpanderNode(dbName, dbKey, linkkey, ElaIconType::Database);
        qDebug() << "Added database node:" << db << ", key:" << dbKey;

        ret = DataProcessor::GetInstance().UseDatabase(db);
        if (ret != 0) {
            qDebug() << "Error using database:" << db << ", error code:" << ret;
            ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", QString("无法使用数据库 %1").arg(QString::fromStdString(db)), 3000);
            continue;
        }

        std::vector<std::string> tables;
        ret = DataProcessor::GetInstance().ShowTables(tables);
        if (ret != 0) {
            qDebug() << "Error loading tables for database:" << db << ", error code:" << ret;
            ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", QString("无法加载数据库 %1 的表").arg(QString::fromStdString(db)), 3000);
            continue;
        }

        for (const auto& tb : tables) {
            QString tbName = QString::fromStdString(tb);
            QString tbKey = tbName + "_" + dbName;
            QWidget* tb_widget = new QWidget();
            addPageNode(tbName, tb_widget, dbKey, ElaIconType::Table);
            QString nodeKey = tb_widget->property("ElaPageKey").toString();
            _nodeMap[nodeKey] = {dbName, tbName};
            qDebug() << "nodekey:" << nodeKey;
        }
    }

    connect(this, &MainWindow::navigationNodeClicked, this, [this](ElaNavigationType::NavigationNodeType nodeType, QString nodeKey) {
        qDebug() << "--------------------------------------------------------hello";
        qDebug() << "type:" << nodeType;
        qDebug() << "key:" << nodeKey;
        onNavigationNodeClicked(nodeType, nodeKey);
    });
    qDebug() << "navigationNodeClicked signal connected";
}

void MainWindow::click_link_btn()
{
    ElaDockWidget* connectWidget = new ElaDockWidget("新建连接", this);
    T_ConnectPage* _connectPage = new T_ConnectPage(this);
    connectWidget->setWidget(_connectPage);
    addDockWidget(Qt::RightDockWidgetArea, connectWidget);
    resizeDocks({connectWidget}, {200}, Qt::Horizontal);
    connect(_connectPage, &T_ConnectPage::connectionCreated, this, [this](const QString& name) {
        QString linkKey;
        addExpanderNode(name, linkKey, ElaIconType::Link);
        _connectionNames.append(name);
        init_treeview(linkKey);
        qDebug() << "Connection created:" << name << ", linkKey:" << linkKey;
    });
}

void MainWindow::onNavigationNodeClicked(ElaNavigationType::NavigationNodeType nodeType, QString nodeKey)
{
    qDebug() << "Navigation node clicked, type:" << nodeType << ", key:" << nodeKey;

    if (nodeType != ElaNavigationType::PageNode) {
        return;
    }

    if (!_nodeMap.contains(nodeKey)) {
        qDebug() << "Node key not found in _nodeMap:" << nodeKey;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无法找到表信息", 3000);
        return;
    }

    QString database = _nodeMap[nodeKey].first;
    QString table = _nodeMap[nodeKey].second;

    std::vector<std::vector<std::string>> tableData;
    try {
        tableData = DataProcessor::GetInstance().select_table(database.toStdString(), table.toStdString());
        if (tableData.empty()) {
            qDebug() << "No data returned for table:" << table;
            ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", QString("表 %1 为空").arg(table), 3000);
            return;
        }
    } catch (const std::exception& e) {
        qDebug() << "Error selecting table:" << table << ", exception:" << e.what();
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", QString("查询表 %1 失败").arg(table), 3000);
        return;
    }

    // 获取与 nodeKey 关联的 QWidget
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

    // 清空目标 QWidget 的现有内容
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

    // 创建新的 T_TableView 并设置数据
    if (_currentTableView) {
        _currentTableView->deleteLater();
    }

    // 传递表名到 T_TableView 构造函数
    _currentTableView = new T_TableView(table, targetWidget);
    _currentTableView->setTableData(tableData);

    // 创建布局并将 T_TableView 添加到目标 QWidget
    QVBoxLayout* layout = new QVBoxLayout(targetWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(_currentTableView);
    targetWidget->setLayout(layout);

    ElaMessageBar::success(ElaMessageBarType::TopRight, "成功", QString("表 %1 已打开").arg(table), 2000);
    qDebug() << "Table opened:" << table;
}
