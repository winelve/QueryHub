#include "T_ConnectPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <ElaIcon.h>
#include <ElaMessageBar.h>
T_ConnectPage::T_ConnectPage(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initConnections();
}

void T_ConnectPage::initUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题区域（包含图标和文本）
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);

    // 添加链接图标 - 使用 ElaIcon 单例获取图标
    QLabel *iconLabel = new QLabel(this);
    QIcon linkIcon = ElaIcon::getInstance()->getElaIcon(ElaIconType::Link, 20, 24, 24, QColor("#2c3e50"));
    iconLabel->setPixmap(linkIcon.pixmap(24, 24));
    titleLayout->addWidget(iconLabel);

    // 标题文本
    m_titleLabel = new ElaText("新建连接", this);
    m_titleLabel->setTextStyle(ElaTextType::Title);
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();

    mainLayout->addLayout(titleLayout);

    // 表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->setSpacing(10);
    formLayout->setLabelAlignment(Qt::AlignRight);

    // 连接名称
    m_connectionNameEdit = new ElaLineEdit(this);
    m_connectionNameEdit->setPlaceholderText("输入连接名称");
    formLayout->addRow("连接名称:", m_connectionNameEdit);

    // 主机名
    m_hostEdit = new ElaLineEdit(this);
    m_hostEdit->setPlaceholderText("例如: localhost 或 127.0.0.1");
    formLayout->addRow("主机名/IP:", m_hostEdit);

    // 端口
    m_portEdit = new ElaLineEdit(this);
    m_portEdit->setPlaceholderText("例如: 3306");
    formLayout->addRow("端口:", m_portEdit);

    // 用户名
    m_usernameEdit = new ElaLineEdit(this);
    m_usernameEdit->setPlaceholderText("输入数据库用户名");
    formLayout->addRow("用户名:", m_usernameEdit);

    // 密码
    m_passwordEdit = new ElaLineEdit(this);
    m_passwordEdit->setPlaceholderText("输入数据库密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("密码:", m_passwordEdit);

    mainLayout->addLayout(formLayout);

    // 按钮布局
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 10, 0, 0);
    btnLayout->setSpacing(10);

    m_connectBtn = new ElaPushButton("连接", this);
    m_cancelBtn = new ElaPushButton("取消", this);

    btnLayout->addWidget(m_connectBtn);
    btnLayout->addWidget(m_cancelBtn);

    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch();
}

void T_ConnectPage::initConnections()
{
    // 连接按钮
    connect(m_connectBtn, &QPushButton::clicked, [this]() {
        // 这里添加连接逻辑
        QString connectionName = m_connectionNameEdit->text();
        QString host = m_hostEdit->text();
        QString port = m_portEdit->text();
        QString username = m_usernameEdit->text();
        QString password = m_passwordEdit->text();

        // 验证输入
        if(connectionName.isEmpty()) {
            ElaMessageBar::error(ElaMessageBarType::Bottom, "错误", "🔗连接名称不能为空!", 2500, this);
            return;
        }
        else if(host.isEmpty()) {
            ElaMessageBar::error(ElaMessageBarType::Bottom, "错误", "IP不能为空!", 2500, this);
            return;
        }
        else if(port.isEmpty()) {
            ElaMessageBar::error(ElaMessageBarType::Bottom, "错误", "端口名不能为空!", 2500, this);
            return;
        }
        else if(username.isEmpty()) {
            ElaMessageBar::error(ElaMessageBarType::Bottom, "错误", "用户名不能为空!", 2500, this);
            return;
        }
        else if(password.isEmpty()) {
            ElaMessageBar::error(ElaMessageBarType::Bottom, "错误", "密码不能为空!", 2500, this);
            return;
        }

        //这里可以修改连接成功的判断条件----------------------------------------------------------------------------------------
        if(!connectionName.isEmpty()&&!host.isEmpty()&&!port.isEmpty()&&!username.isEmpty()
            &&!password.isEmpty()){
            link_status = true;
            emit connectionCreated(connectionName);
            ElaMessageBar::success(ElaMessageBarType::Bottom, "成功", "创建连接成功!", 2500, this);
        }
    });

    // 取消按钮
    connect(m_cancelBtn, &QPushButton::clicked, [this]() {
        m_connectionNameEdit->clear();
        m_hostEdit->clear();
        m_portEdit->clear();
        m_usernameEdit->clear();
        m_passwordEdit->clear();
    });
}
