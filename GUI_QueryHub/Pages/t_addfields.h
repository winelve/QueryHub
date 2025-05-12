#ifndef T_ADDFIELDS_H
#define T_ADDFIELDS_H

#include "T_BasePage.h"
#include <QWidget>

class ElaComboBox;
class ElaLineEdit;
class ElaPushButton;
class ElaText;

class T_AddFields : public T_BasePage
{
    Q_OBJECT

public:
    explicit T_AddFields(QWidget* parent = nullptr);
    ~T_AddFields();

    void setDatabases(const QStringList& databases);
    void setTables(const QStringList& tables);

signals:
    void fieldsAdded(const QString& dbName, const QString& tableName,
                     const QList<QPair<QString, QString>>& fields);

private slots:
    void onAddFieldClicked();
    void onConfirmClicked();
    void onCancelClicked();

private:
    void setupUI();
    QWidget* createFieldRow();

    ElaComboBox* m_dbComboBox;
    ElaComboBox* m_tableComboBox;
    QWidget* m_fieldsContainer;
    QVBoxLayout* m_fieldsLayout;
};

#endif // T_ADDFIELDS_H
