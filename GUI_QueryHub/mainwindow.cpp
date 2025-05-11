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

#include "Pages/T_About.h"
#include "Pages/t_setting.h"

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent)
{
    initWindow();

    //额外布局
    initEdgeLayout();

    //中心窗口
    initContent();

    // 拦截默认关闭事件
    _closeDialog = new ElaContentDialog(this);
    connect(_closeDialog, &ElaContentDialog::rightButtonClicked, this, &MainWindow::closeWindow);
    connect(_closeDialog, &ElaContentDialog::middleButtonClicked, this, [=]() {
        _closeDialog->close();
        showMinimized();
    });
    this->setIsDefaultClosed(false);
    connect(this, &MainWindow::closeButtonClicked, this, [=]() {
        _closeDialog->exec();
    });

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
    //菜单栏
    ElaMenuBar* menuBar = new ElaMenuBar(this);
    menuBar->setFixedHeight(30);
    QWidget* customWidget = new QWidget(this);
    QVBoxLayout* customLayout = new QVBoxLayout(customWidget);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->addWidget(menuBar);
    customLayout->addStretch();
    this->setCustomWidget(ElaAppBarType::MiddleArea, customWidget);
    this->setCustomWidgetMaximumWidth(500);

    ElaMenu* uesrMenu = menuBar->addMenu(ElaIconType::UserGroup, "用户");
    menuBar->addMenu(uesrMenu);

    ElaMenu* fileMenu = menuBar->addMenu(ElaIconType::File, "文件");
    fileMenu->setMenuItemHeight(27);
    fileMenu->addElaIconAction(ElaIconType::FileArrowUp, "导入数据");
    fileMenu->addElaIconAction(ElaIconType::FileArrowDown, "导出数据");
    menuBar->addSeparator();


    //工具栏
    ElaToolBar* toolBar = new ElaToolBar("工具栏", this);
    toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    toolBar->setToolBarSpacing(3);
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->setIconSize(QSize(25, 25));
    // toolBar->setFloatable(false);
    // toolBar->setMovable(false);
    ElaToolButton* toolButton1 = new ElaToolButton(this);
    toolButton1->setElaIcon(ElaIconType::BadgeCheck);
    toolBar->addWidget(toolButton1);
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

    // //状态栏
    // ElaStatusBar* statusBar = new ElaStatusBar(this);
    // ElaText* statusText = new ElaText("欢迎来到QueryHub！", this);
    // statusText->setTextPixelSize(10);
    // statusBar->addWidget(statusText);
    // this->setStatusBar(statusBar);
}

void MainWindow::initContent(){
    _settingPage = new T_Setting(this);
    addFooterNode("Setting", _settingPage, _settingKey, 0, ElaIconType::GearComplex);
    QString fun_1;
    // 1.关于界面
    addFooterNode("About", nullptr, _aboutKey, 0, ElaIconType::User);
    _aboutPage = new T_About();

    _aboutPage->hide();
    connect(this, &ElaWindow::navigationNodeClicked, this, [=](ElaNavigationType::NavigationNodeType nodeType, QString nodeKey) {
        if (_aboutKey == nodeKey)
        {
            _aboutPage->setFixedSize(575, 400);
            _aboutPage->moveToCenter();
            _aboutPage->show();
        }
    });
    // 2.数据库
    addExpanderNode("我的连接", fun_1, ElaIconType::Link); //  层级一
    // 3.可视化编辑功能区
    QString fun_2_add;
    QString fun_2_del;
    QString fun_2_alt;
    QString fun_3;
    addExpanderNode("功能操作", fun_1, ElaIconType::BullseyeArrow); //  层级一
    addExpanderNode("添加", fun_2_add, fun_1, ElaIconType::Plus); // 层级二
    addExpanderNode("删除", fun_2_del, fun_1, ElaIconType::Minus);
    // addExpanderNode("修改", fun_2_alt, fun_1, ElaIconType::Pencil);
    addPageNode("查询", new QWidget(this), fun_1, ElaIconType::CircleLocationArrow);
    addPageNode("添加数据库", new QWidget(this), fun_2_add, ElaIconType::Database);   // 层级三
    addPageNode("添加表", new QWidget(this), fun_2_add, ElaIconType::Table);
    addPageNode("添加字段", new QWidget(this), fun_2_add, ElaIconType::PenField);
    addPageNode("添加索引", new QWidget(this), fun_2_add, ElaIconType::Tags);
    addPageNode("删除数据库", new QWidget(this), fun_2_del, ElaIconType::Database);
    addPageNode("删除表", new QWidget(this), fun_2_del, ElaIconType::Table);
    addPageNode("删除字段", new QWidget(this), fun_2_del, ElaIconType::PenField);
    addPageNode("删除索引", new QWidget(this), fun_2_del, ElaIconType::Tags);

}
