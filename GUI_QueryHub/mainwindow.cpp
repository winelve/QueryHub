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

#include "Pages/t_setting.h"
#include "Pages/t_connectpage.h"

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent)
{
    initWindow();
    //额外布局
    initEdgeLayout();

    //中心窗口
    initContent();
    this->setIsDefaultClosed(true);

    //移动到中心
    moveToCenter();
}

MainWindow::~MainWindow()
{
    delete this->_aboutPage;
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

void MainWindow::initEdgeLayout(){
    //工具栏
    ElaToolBar* toolBar = new ElaToolBar("工具栏", this);
    toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    toolBar->setToolBarSpacing(3);
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->setIconSize(QSize(25, 25));
    // toolBar->setFloatable(false);
    // toolBar->setMovable(false);
    ElaToolButton* link_btn = new ElaToolButton(this);
    link_btn->setElaIcon(ElaIconType::Link);
    toolBar->addWidget(link_btn);
    connect(link_btn, &ElaToolButton::clicked, this, &MainWindow::click_link_btn);
    //---------------------------------------------------------
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
    QAction* test = new QAction(this);
    test->setMenu(new QMenu(this));

    ElaProgressBar* progressBar = new ElaProgressBar(this);
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
    progressBar->setFixedWidth(350);
    toolBar->addWidget(progressBar);

    this->addToolBar(Qt::TopToolBarArea, toolBar);
}

void MainWindow::initContent(){
    _aboutPage = new T_About();
    _settingPage = new T_Setting(this);
    _addDataBasePage = new T_AddDataBase(this);
    _addTablePage = new T_AddTable(this);
    _addFieldsPage = new T_AddFields(this);
    _delDataBasePage = new T_DeleteDataBase(this);
    _delTablePage = new T_DeleteTable(this);
    _delFieldsPage = new T_DeleteField(this);
    _tabWidget = new ElaTabWidget(this);

    //-----------------------------------------------------------------------------------
    //0.设置界面
    addFooterNode("Setting", _settingPage, _settingKey, 0, ElaIconType::GearComplex);
    // 1.关于界面
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

    // 3.可视化编辑功能区
    QString fun_1_dis;
    QString fun_1_ddl;
    QString fun_1_dml;
    QString fun_2_add;
    QString fun_2_del;
    QString fun_2_alt;
    //QString fun_3;

    addExpanderNode("功能操作", fun_1_ddl, ElaIconType::BullseyeArrow);
    addExpanderNode("查询", fun_1_dml, ElaIconType::CircleLocationArrow);
    //-------------------------------------------------------------------------------------------
    addExpanderNode("添加", fun_2_add, fun_1_ddl, ElaIconType::Plus); // 层级二
    addExpanderNode("删除", fun_2_del, fun_1_ddl, ElaIconType::Minus);
    addExpanderNode("修改", fun_2_alt, fun_1_ddl, ElaIconType::Pencil);
    //--------------------------------------------------------------------------------------------
    addPageNode("添加数据库", _addDataBasePage, fun_2_add, ElaIconType::Database);   // 层级三
    addPageNode("添加表", _addTablePage, fun_2_add, ElaIconType::Table);
    addPageNode("添加字段", _addFieldsPage, fun_2_add, ElaIconType::PenField);
    // addPageNode("添加记录", new QWidget(this), fun_2_add, ElaIconType::ChartGantt);
    // addPageNode("添加索引", new QWidget(this), fun_2_add, ElaIconType::Tags);
    addPageNode("删除数据库", _delDataBasePage, fun_2_del, ElaIconType::Database);
    addPageNode("删除表", _delTablePage, fun_2_del, ElaIconType::Table);
    addPageNode("删除字段", _delFieldsPage, fun_2_del, ElaIconType::PenField);
    // addPageNode("删除记录", new QWidget(this), fun_2_del, ElaIconType::ChartGantt);
    // addPageNode("删除索引", new QWidget(this), fun_2_del, ElaIconType::Tags);

}

// void MainWindow::click_link_btn() {
//     // 连接窗口
//     ElaDockWidget* connectWidget = new ElaDockWidget("新建连接", this);
//     T_ConnectPage *_connectPage = new T_ConnectPage(this);
//     connectWidget->setWidget(_connectPage);
//     this->addDockWidget(Qt::RightDockWidgetArea, connectWidget);
//     resizeDocks({connectWidget}, {200}, Qt::Horizontal);
//     connect(_connectPage, &T_ConnectPage::connectionCreated, this, [this](const QString& name) {
//         QString linkKey = "link" + name;
//         addExpanderNode(name, linkKey, ElaIconType::Link);
//         _connectionNames.append(name); // 保存连接名
//     });

// }

// // -连接（ExpanderNode可展开节点） -数据库（PageNode页面节点） -表（ExpanderNode最深层，无法再展开，调用dis_table（）这个放到T_TableView里面）
// void MainWindow::init_treeview(const QString linkkey)
// {
//     // 获取数据库列表
//     std::vector<std::string> databases;
//     int ret = DataProcessor::GetInstance().ShowDatabases(databases);
//     if (ret != 0) {
//         ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无法加载数据库列表！", 3000);
//         return;
//     }

//     // 遍历数据库
//     for (const auto& db : databases) {
//         QString dbKey;

//         // 添加数据库节点
//         addExpanderNode(QString::fromStdString(db) ,dbKey, ElaIconType::Database);

//         // 切换到该数据库
//         ret = DataProcessor::GetInstance().UseDatabase(db);
//         if (ret != 0) {
//             ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", QString("无法使用数据库 %1").arg(QString::fromStdString(db)), 3000);
//             continue;
//         }

//         // 获取表列表
//         std::vector<std::string> tables;
//         ret = DataProcessor::GetInstance().ShowTables(tables);
//         if (ret != 0) {
//             ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", QString("无法加载数据库 %1 的表").arg(QString::fromStdString(db)), 3000);
//             continue;
//         }

//         _tableview = new T_TableView(this);
//         // 添加表节点（子节点）
//         for (const auto& tb : tables) {
//             QString tbName = QString::fromStdString(tb);
//             addPageNode(tbName, _tableview,dbKey, ElaIconType::Table);
//         }
//     }
// }

// 假设 DataProcessor 接口
class DataProcessor {
public:
    static DataProcessor& GetInstance() { static DataProcessor instance; return instance; }
    int ShowDatabases(std::vector<std::string>& databases) {
        databases = {"db1", "db2"}; // 替换为实际接口
        return 0;
    }
    int ShowTables(std::vector<std::string>& tables) {
        tables = {"tb1", "tb2"}; // 替换为实际接口
        return 0;
    }
    int UseDatabase(const std::string& database) {
        return 0; // 替换为实际接口
    }
    int Select(const std::string& table, const std::vector<std::string>& fields,
               const std::vector<std::tuple<std::string, std::string, int>>& conditions,
               std::vector<std::vector<std::string>>& records) {
        records = {{"c1", "c2", "c3", "c4"}, {"v11", "v12", "v13", "v14"}, {"v21", "v22", "v23", "v24"}};
        return 0; // 替换为实际接口
    }
};

void MainWindow::init_treeview(const QString& linkkey)
{
    qDebug() << "Initializing tree view for connection:" << linkkey;

    // 获取数据库列表
    std::vector<std::string> databases;
    int ret = DataProcessor::GetInstance().ShowDatabases(databases);
    if (ret != 0) {
        qDebug() << "Error loading databases, error code:" << ret;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "无法加载数据库列表！", 3000);
        return;
    }

    // 遍历数据库
    for (const auto& db : databases) {
        QString dbKey;
        QString dbName = QString::fromStdString(db);
        // 添加数据库节点（ExpanderNode）
        addExpanderNode(dbName, dbKey, linkkey, ElaIconType::Database);
        qDebug() << "Added database node:" << db << ", key:" << dbKey;

        // 切换到该数据库
        ret = DataProcessor::GetInstance().UseDatabase(db);
        if (ret != 0) {
            qDebug() << "Error using database:" << db << ", error code:" << ret;
            ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", QString("无法使用数据库 %1").arg(QString::fromStdString(db)), 3000);
            continue;
        }

        // 获取表列表
        std::vector<std::string> tables;
        ret = DataProcessor::GetInstance().ShowTables(tables);
        if (ret != 0) {
            qDebug() << "Error loading tables for database:" << db << ", error code:" << ret;
            ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", QString("无法加载数据库 %1 的表").arg(QString::fromStdString(db)), 3000);
            continue;
        }

        // 添加表节点（PageNode）
        for (const auto& tb : tables) {
            QString tbName = QString::fromStdString(tb);
            QString tbKey = tbName + "_" + dbName; // Create a unique key, e.g., "tb1_db1"
            // Use a placeholder QWidget or nullptr for navigation
            QWidget* tb_widget = new QWidget();
            addPageNode(tbName, tb_widget, dbKey, ElaIconType::Table);
            // Save node information with tbKey
            qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!property:" << tb_widget->property("ElaPageKey").toString();
            // tb_widget->setProperty("ElaPageKey",tbKey);
            // qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!property:" << tb_widget->property("ElaPageKey").toString();
            QString nodeKey = tb_widget->property("ElaPageKey").toString();
            _nodeMap[nodeKey] = {dbName, tbName};
            qDebug() << "nodekey:" << nodeKey;
        }
    }

    // 连接 navigationNodeClicked 信号
    connect(this, &MainWindow::navigationNodeClicked, this, [this](ElaNavigationType::NavigationNodeType nodeType, QString nodeKey) {
        qDebug() << "--------------------------------------------------------hello";

        qDebug() << "type:" << nodeType ;
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
        init_treeview(linkKey); // 初始化树状结构
        qDebug() << "Connection created:" << name << ", linkKey:" << linkKey;
    });
}

void MainWindow::onNavigationNodeClicked(ElaNavigationType::NavigationNodeType nodeType, QString nodeKey)
{
    QString database = _nodeMap[nodeKey].first;
    QString table = _nodeMap[nodeKey].second;
    qDebug() << "Matched node, database:" << database << ", table:" << table;

    // 检查是否已打开
    for (int i = 0; i < _tabWidget->count(); ++i) {
        qDebug() << "Checking tab" << i << ", text:" << _tabWidget->tabText(i);
        if (_tabWidget->tabText(i) == table) {
            _tabWidget->setCurrentIndex(i);
            ElaMessageBar::information(ElaMessageBarType::TopRight, "提示", QString("表 %1 已打开").arg(table), 2000);
            qDebug() << "Table" << table << "already opened, switched to index" << i;
            return;
        }
    }

    // 切换数据库
    int ret = DataProcessor::GetInstance().UseDatabase(database.toStdString());
    if (ret != 0) {
        qDebug() << "Error using database:" << database << ", error code:" << ret;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "使用数据库错误", 3000);
        return;
    }

    // 查询数据
    std::vector<std::vector<std::string>> tableData;
    ret = DataProcessor::GetInstance().Select(table.toStdString(), {"*"}, {}, tableData);
    if (ret != 0) {
        qDebug() << "Error selecting table:" << table << ", error code:" << ret;
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "查询表错误", 3000);
        return;
    }

    // 创建 T_TableView 并显示
    T_TableView* tableView = new T_TableView(database, table, this);
    tableView->dis_table(tableData);

    int index = _tabWidget->addTab(tableView, table);
    _tabWidget->setCurrentIndex(index);
    qDebug() << "Added tab for table:" << table << "at index" << index;

    ElaMessageBar::success(ElaMessageBarType::TopRight, "成功", QString("表 %1 已打开").arg(table), 2000);
    qDebug() << "Table opened:" << table;
}
