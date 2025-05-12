#ifndef T_DELETEDATABASE_H
#define T_DELETEDATABASE_H

#include "T_BasePage.h"
#include <QWidget>

class ElaComboBox;
class ElaPushButton;
class ElaText;

class T_DeleteDataBase : public T_BasePage
{
    Q_OBJECT

public:
    explicit T_DeleteDataBase(QWidget* parent = nullptr);
    ~T_DeleteDataBase();

    void setDatabases(const QStringList& databases); // 设置数据库列表

signals:
    void databaseDeleted(const QString& dbName);

private slots:
    void onConfirmClicked();
    void onCancelClicked();

private:
    void setupUI();

    ElaComboBox* m_dbComboBox;
    ElaPushButton* m_confirmBtn;
    ElaPushButton* m_cancelBtn;
};

#endif // T_DELETEDATABASE_H
