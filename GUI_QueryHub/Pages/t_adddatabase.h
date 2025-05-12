#ifndef T_ADDDATABASE_H
#define T_ADDDATABASE_H

#include "T_BasePage.h"
#include <QWidget>

class ElaLineEdit;
class ElaPushButton;
class ElaText;

class T_AddDataBase : public T_BasePage
{
    Q_OBJECT
public:
    explicit T_AddDataBase(QWidget* parent = nullptr);
    ~T_AddDataBase();

signals:
    void databaseCreated(const QString& dbName);
    void operationCancelled();

private slots:
    void onConfirmClicked();

private:
    void setupUI();

    ElaLineEdit* m_dbNameEdit;      // 数据库名称输入框
    ElaPushButton* m_confirmBtn;    // 确认按钮
};

#endif // T_ADDDATABASE_H
