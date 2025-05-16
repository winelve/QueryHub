#include "T_AddDataBase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaMessageBar.h"
#include <ElaWindow.h>

T_AddDataBase::T_AddDataBase(QWidget* parent)
    : T_BasePage(parent)
{
    createCustomWidget("");
    setupUI();
}

void T_AddDataBase::setupUI()
{
    // 主容器
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("新建数据库");

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(30);

    // 输入区域
    QWidget* inputContainer = new QWidget(this);
    QVBoxLayout* inputLayout = new QVBoxLayout(inputContainer);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(15);

    // 输入提示
    ElaText* inputLabel = new ElaText("数据库名称", inputContainer);
    inputLabel->setTextStyle(ElaTextType::Subtitle);

    // 输入框
    m_dbNameEdit = new ElaLineEdit(inputContainer);
    m_dbNameEdit->setPlaceholderText("请输入数据库名称...");
    m_dbNameEdit->setFixedHeight(40);
    m_dbNameEdit->setBorderRadius(8);

    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(m_dbNameEdit);

    // 按钮区域
    QWidget* buttonContainer = new QWidget(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(15);

    // 确认按钮
    m_confirmBtn = new ElaPushButton("创建", buttonContainer);
    m_confirmBtn->setFixedSize(100, 36);
    m_confirmBtn->setBorderRadius(8);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_confirmBtn);

    // 添加到主布局
    mainLayout->addWidget(inputContainer);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonContainer);

    // 连接信号
    connect(m_confirmBtn, &ElaPushButton::clicked, this, &T_AddDataBase::onConfirmClicked);

    addCentralWidget(centralWidget, true, true, 0);
}

void T_AddDataBase::onConfirmClicked()
{
    QString dbName = m_dbNameEdit->text().trimmed();
    if(dbName.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Bottom, "警告", "数据库名称不能为空!", 2500, this);
        return;
    }
    QString sql = "create database " + dbName + ";";

    emit databaseCreated(sql);
}

T_AddDataBase::~T_AddDataBase()
{
}
