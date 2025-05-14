#ifndef T_CONNECTPAGE_H
#define T_CONNECTPAGE_H

#include <QWidget>
#include <QString>
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaIcon.h"

class T_ConnectPage : public QWidget
{
    Q_OBJECT
public:
    explicit T_ConnectPage(QWidget *parent = nullptr);

signals:
    void connectionCreated(const QString& connectionName);

private:
    void initUI();
    void initConnections();


    // UI组件
    QWidget *m_titleWidget;
    ElaIcon *m_linkIcon;
    ElaText *m_titleLabel;
    ElaLineEdit *m_connectionNameEdit;
    ElaLineEdit *m_hostEdit;
    ElaLineEdit *m_portEdit;
    ElaLineEdit *m_usernameEdit;
    ElaLineEdit *m_passwordEdit;
    ElaPushButton *m_connectBtn;
    ElaPushButton *m_cancelBtn;

    bool link_status = false;
};

#endif // T_CONNECTPAGE_H
