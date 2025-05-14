#ifndef T_DELETEFIELD_H
#define T_DELETEFIELD_H

#include "T_BasePage.h"
#include <QWidget>

class ElaComboBox;
class ElaPushButton;
class ElaText;

class T_DeleteField : public T_BasePage
{
    Q_OBJECT

public:
    explicit T_DeleteField(QWidget* parent = nullptr);
    ~T_DeleteField();

    void setDatabases(const QStringList& databases);
    void setTables(const QStringList& tables);
    void setFields(const QStringList& fields);

signals:
    void fieldDeleted(const QString& dbName, const QString& tableName, const QString& fieldName);

private slots:
    void onConfirmClicked();
    void onCancelClicked();

private:
    void setupUI();

    ElaComboBox* m_dbComboBox;
    ElaComboBox* m_tableComboBox;
    ElaComboBox* m_fieldComboBox;
    ElaPushButton* m_confirmBtn;
    ElaPushButton* m_cancelBtn;
};

#endif // T_DELETEFIELD_H
