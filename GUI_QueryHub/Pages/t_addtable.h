#ifndef T_ADDTABLE_H
#define T_ADDTABLE_H

#include "T_BasePage.h"
#include <QWidget>
#include <QList>

struct TableProperty {
    QString name;
    QString type;
    bool isNotNull;
    bool isUnique;
    bool isPrimaryKey;
    bool isAutoIncrement;
    QString defaultValue;
    QString foreignKeyRefTable;
    QString foreignKeyRefField;
    QString constraintName;
};

class ElaLineEdit;
class ElaComboBox;
class ElaPushButton;
class ElaCheckBox;
class ElaIconButton;
class QGroupBox;

class T_AddTable : public T_BasePage
{
    Q_OBJECT
public:
    explicit T_AddTable(QWidget* parent = nullptr);
    void setDatabaseList(const QStringList& databases);

signals:
    void tableCreated(const QString& dbName,
                      const QString& tableName,
                      const QList<TableProperty>& properties);

private slots:
    void onAddPropertyClicked();
    void onConfirmClicked();

private:
    QWidget* createPropertyRow();
    void setupConstraintGroup(QHBoxLayout* layout);

    ElaComboBox* m_dbComboBox;
    ElaLineEdit* m_tableNameEdit;
    QVBoxLayout* m_propertiesLayout;
    QWidget* m_propertiesContainer;
};

#endif // T_ADDTABLE_H
