#include "T_About.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QVBoxLayout>

#include "ElaText.h"
T_About::T_About(QWidget* parent)
    : ElaWidget(parent)
{
    setWindowTitle("About");
    setWindowIcon(QIcon(":/resource/images/icon.png"));
    this->setIsFixedSize(true);
    setWindowModality(Qt::ApplicationModal);
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);

    QVBoxLayout* pixCardLayout = new QVBoxLayout();
    pixCardLayout->addStretch();

    ElaText* versionText = new ElaText("QueryHub-2025", this);
    QFont versionTextFont = versionText->font();
    versionTextFont.setWeight(QFont::Bold);
    versionText->setFont(versionTextFont);
    versionText->setWordWrap(false);
    versionText->setTextPixelSize(18);

    ElaText* indText = new ElaText("北京交通大学软件学院 2025年春季 实训课项目 数据库管理系统\n"
                                   "软件名称：QueryHub —— DBMS\n"
                                   "QueryHub支持数据库的创建、数据库表的管理、记录的添加与查询、\n"
                                   "完整性 约束的实现，索引的创建与实现，多用户等功能。", this);
    indText->setWordWrap(false);
    indText->setTextPixelSize(14);
    // ElaText* licenseText = new ElaText("开发人员", this);
    // licenseText->setTextPixelSize(14);
    //licenseText->setWordWrap(false);
    ElaText* supportText = new ElaText("开发人员\n"
                                       "王逸群 ：CML板块\n"
                                       "熊涛    ：FILE板块\n"
                                       "李好    ：GUI板块", this);
    supportText->setWordWrap(false);
    supportText->setTextPixelSize(14);
    ElaText* contactText = new ElaText("GitHub: https://github.com/winelve/QueryHub.git", this);
    contactText->setWordWrap(false);
    contactText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    contactText->setTextPixelSize(14);
    ElaText* helperText = new ElaText("项目完成时间\n"
                                      "20250516", this);
    helperText->setWordWrap(false);
    helperText->setTextPixelSize(14);

    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setSpacing(15);
    textLayout->addWidget(versionText);
    textLayout->addWidget(indText);
    //textLayout->addWidget(licenseText);
    textLayout->addWidget(supportText);
    textLayout->addWidget(contactText);
    textLayout->addWidget(helperText);
    textLayout->addStretch();

    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->addSpacing(30);
    contentLayout->addLayout(pixCardLayout);
    contentLayout->addSpacing(30);
    contentLayout->addLayout(textLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 25, 0, 0);
    mainLayout->addLayout(contentLayout);
}

T_About::~T_About()
{
}
