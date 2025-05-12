#ifndef T_DELETETABLE_H
#define T_DELETETABLE_H

#include "T_BasePage.h"
#include <QWidget>

class ElaComboBox;
class ElaPushButton;
class ElaText;

class T_DeleteTable : public T_BasePage
{
    Q_OBJECT

public:
    explicit T_DeleteTable(QWidget* parent = nullptr);
    ~T_DeleteTable();

    void setDatabases(const QStringList& databases); // 设置数据库列表
    void setTables(const QStringList& tables);     // 设置表列表

signals:
    void tableDeleted(const QString& dbName, const QString& tableName);

private slots:
    void onConfirmClicked();
    void onCancelClicked();

private:
    void setupUI();

    ElaComboBox* m_dbComboBox;
    ElaComboBox* m_tableComboBox;
    ElaPushButton* m_confirmBtn;
    ElaPushButton* m_cancelBtn;
};

#endif // T_DELETETABLE_H
