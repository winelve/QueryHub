#include "mainwindow.h"

#include <QDebug>
#include <QGraphicsView>
#include <QHBoxLayout>

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
    ElaToolButton* toolButton1 = new ElaToolButton(this);
    toolButton1->setElaIcon(ElaIconType::Link);
    toolBar->addWidget(toolButton1);
    connect(toolButton1, &ElaToolButton::clicked, this, &MainWindow::onToolButton1Clicked);
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

    addExpanderNode("DDL", fun_1_ddl, ElaIconType::BullseyeArrow);
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

void MainWindow::onToolButton1Clicked() {
    //停靠窗口
    ElaDockWidget* connectWidget = new ElaDockWidget("新建连接", this);
    T_ConnectPage *_connectPage = new T_ConnectPage(this);
    connectWidget->setWidget(_connectPage);
    this->addDockWidget(Qt::RightDockWidgetArea, connectWidget);
    resizeDocks({connectWidget}, {200}, Qt::Horizontal);
    if ( _connectPage->setlink_status()) {
        QString linkname = _connectPage->setlinks();
        QString fun1;
        addExpanderNode(linkname, fun1, ElaIconType::Link); //  层级一
    }/*else{
        ElaMessageBar::error(ElaMessageBarType::Bottom, "错误", "创建连接失败!", 2500, this);
    }*/

}
