#include "T_DeleteDataBase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaMessageBar.h"
#include <ElaWindow.h>

T_DeleteDataBase::T_DeleteDataBase(QWidget* parent)
    : T_BasePage(parent)
{
    createCustomWidget("");
    setupUI();
}

void T_DeleteDataBase::setupUI()
{
    // 主容器
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("删除数据库");

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
    m_dbNameEdit->setPlaceholderText("请输入要删除的数据库名称...");
    m_dbNameEdit->setFixedHeight(40);
    m_dbNameEdit->setBorderRadius(8);

    // 警告文本
    ElaText* warningText = new ElaText("警告：删除数据库将永久删除所有数据！", inputContainer);
    warningText->setTextStyle(ElaTextType::Body);

    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(m_dbNameEdit);
    inputLayout->addWidget(warningText);

    // 按钮区域
    QWidget* buttonContainer = new QWidget(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(15);

    // 取消按钮
    m_cancelBtn = new ElaPushButton("取消", buttonContainer);
    m_cancelBtn->setFixedSize(100, 36);
    m_cancelBtn->setBorderRadius(8);

    // 确认按钮（危险样式）
    m_confirmBtn = new ElaPushButton("删除", buttonContainer);
    m_confirmBtn->setFixedSize(100, 36);
    m_confirmBtn->setBorderRadius(8);
    m_confirmBtn->setLightDefaultColor(QColor(220, 80, 80));
    m_confirmBtn->setLightHoverColor(QColor(200, 80, 80));
    m_confirmBtn->setDarkDefaultColor(QColor(180, 50, 50));
    m_confirmBtn->setDarkHoverColor(QColor(220, 50, 50));
    m_confirmBtn->setLightTextColor(Qt::white);
    m_confirmBtn->setDarkTextColor(Qt::white);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelBtn);
    buttonLayout->addWidget(m_confirmBtn);

    // 添加到主布局
    mainLayout->addWidget(inputContainer);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonContainer);

    // 连接信号
    connect(m_confirmBtn, &ElaPushButton::clicked, this, &T_DeleteDataBase::onConfirmClicked);
    connect(m_cancelBtn, &ElaPushButton::clicked, this, &T_DeleteDataBase::onCancelClicked);

    addCentralWidget(centralWidget, true, true, 0);
}

void T_DeleteDataBase::onConfirmClicked()
{
    QString dbName = m_dbNameEdit->text().trimmed();
    if (dbName.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Bottom, "警告", "数据库名称不能为空！", 2500, this);
        return;
    }
    QString sql = "DROP DATABASE " + dbName + ";";
    emit databaseDeleted(sql);
}

void T_DeleteDataBase::onCancelClicked()
{

}

T_DeleteDataBase::~T_DeleteDataBase()
{
}
