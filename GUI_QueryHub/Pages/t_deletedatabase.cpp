#include "T_DeleteDataBase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "ElaComboBox.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaMessageBar.h"
#include <QDebug>

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

    // 选择区域
    QWidget* selectContainer = new QWidget(this);
    QVBoxLayout* selectLayout = new QVBoxLayout(selectContainer);
    selectLayout->setContentsMargins(0, 0, 0, 0);
    selectLayout->setSpacing(20);

    // 数据库选择
    QWidget* dbSelectRow = new QWidget(selectContainer);
    QHBoxLayout* dbLayout = new QHBoxLayout(dbSelectRow);
    dbLayout->setContentsMargins(0, 0, 0, 0);
    dbLayout->setSpacing(15);

    ElaText* dbLabel = new ElaText("选择数据库:", dbSelectRow);
    dbLabel->setTextStyle(ElaTextType::Subtitle);

    m_dbComboBox = new ElaComboBox(dbSelectRow);
    m_dbComboBox->setPlaceholderText("请选择要删除的数据库");
    m_dbComboBox->setBorderRadius(8);
    m_dbComboBox->setFixedHeight(36);

    dbLayout->addWidget(dbLabel);
    dbLayout->addWidget(m_dbComboBox);
    dbLayout->addStretch();

    // 警告文本
    ElaText* warningText = new ElaText("警告：删除数据库将永久删除所有数据！", selectContainer);
    warningText->setTextStyle(ElaTextType::Body);

    selectLayout->addWidget(dbSelectRow);
    selectLayout->addWidget(warningText);

    // 按钮区域
    QWidget* buttonContainer = new QWidget(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(15);

    // 取消按钮
    m_cancelBtn = new ElaPushButton("取消", buttonContainer);
    m_cancelBtn->setFixedSize(100, 36);
    m_cancelBtn->setBorderRadius(8);

    // 确认按钮（设置为危险样式）
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
    mainLayout->addWidget(selectContainer);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonContainer);

    // 连接信号
    connect(m_confirmBtn, &ElaPushButton::clicked, this, &T_DeleteDataBase::onConfirmClicked);
    connect(m_cancelBtn, &ElaPushButton::clicked, this, &T_DeleteDataBase::onCancelClicked);

    addCentralWidget(centralWidget, true, true, 0);
}

void T_DeleteDataBase::setDatabases(const QStringList& databases)
{
    m_dbComboBox->clear();
    m_dbComboBox->addItems(databases);
}

void T_DeleteDataBase::onConfirmClicked()
{
    QString dbName = m_dbComboBox->currentText();
    if(dbName.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Bottom, "警告", "请选择要删除的数据库!", 2500, this);
        return;
    }

    emit databaseDeleted(dbName);
}

void T_DeleteDataBase::onCancelClicked()
{
    this->close();
}

T_DeleteDataBase::~T_DeleteDataBase()
{
}
