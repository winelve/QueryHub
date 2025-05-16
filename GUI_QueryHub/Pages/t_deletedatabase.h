#ifndef T_DELETEDATABASE_H
#define T_DELETEDATABASE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "t_basepage.h"
#include "ElaText.h"

class T_DeleteDataBase : public T_BasePage
{
    Q_OBJECT
public:
    explicit T_DeleteDataBase(QWidget* parent = nullptr);
    ~T_DeleteDataBase();

private:
    void setupUI();

    ElaLineEdit* m_dbNameEdit; // 输入框
    ElaPushButton* m_confirmBtn; // 确认按钮
    ElaPushButton* m_cancelBtn; // 取消按钮

signals:
    void databaseDeleted(const QString& sql); // 发出删除数据库的 SQL

private slots:
    void onConfirmClicked();
    void onCancelClicked();
};

#endif // T_DELETEDATABASE_H
