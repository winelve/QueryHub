#include "T_AddTable.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaMessageBar.h"
#include "ElaComboBox.h"
#include "ElaIconButton.h"
#include "ElaCheckBox.h"
#include "client.h"

T_AddTable::T_AddTable(QWidget* parent)
    : T_BasePage(parent)
{
    createCustomWidget("创建新的数据表");
    // Client client = m_mainWindow->getclients_map_().constBegin().value();
    // 主容器
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("新建表");
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // ==================== 数据库选择 ====================
    QWidget* dbSection = new QWidget(centralWidget);
    QHBoxLayout* dbLayout = new QHBoxLayout(dbSection);
    dbLayout->setContentsMargins(0, 0, 0, 0);
    dbLayout->setSpacing(15);

    ElaIconButton* dbIcon = new ElaIconButton(ElaIconType::Database, dbSection);
    dbIcon->setFixedSize(20, 20);
    dbIcon->setFlat(true);

    ElaText* dbLabel = new ElaText("选择数据库:", dbSection);
    dbLabel->setTextPixelSize(16);

    m_dbComboBox = new ElaComboBox(dbSection);
    m_dbComboBox->setPlaceholderText("请选择数据库");
    m_dbComboBox->setFixedHeight(30);
    m_dbComboBox->setBorderRadius(8);
    // std::vector<std::string> databases = client.get_all_database();
    // // 填充 ComboBox
    //     for (const auto& db : databases) {
    //     m_dbComboBox->addItem(QString::fromStdString(db));
    // }
    // m_dbComboBox->setCurrentIndex(-1); // 默认无选中

    dbLayout->addWidget(dbIcon);
    dbLayout->addWidget(dbLabel);
    dbLayout->addWidget(m_dbComboBox);
    dbLayout->addStretch();

    // ==================== 表名输入 ====================
    QWidget* tableSection = new QWidget(centralWidget);
    QHBoxLayout* tableLayout = new QHBoxLayout(tableSection);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(15);

    ElaIconButton* tableIcon = new ElaIconButton(ElaIconType::Table, tableSection);
    tableIcon->setFixedSize(20, 20);
    tableIcon->setFlat(true);

    ElaText* tableLabel = new ElaText("表名称:", tableSection);
    tableLabel->setTextPixelSize(16);

    m_tableNameEdit = new ElaLineEdit(tableSection);
    m_tableNameEdit->setPlaceholderText("请输入表名称");
    m_tableNameEdit->setFixedHeight(30);
    m_tableNameEdit->setBorderRadius(8);

    tableLayout->addWidget(tableIcon);
    tableLayout->addWidget(tableLabel);
    tableLayout->addWidget(m_tableNameEdit);
    tableLayout->addStretch();

    // ==================== 属性列表 ====================
    QWidget* propertiesHeader = new QWidget(centralWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(propertiesHeader);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(15);

    ElaIconButton* propIcon = new ElaIconButton(ElaIconType::Sparkles, propertiesHeader);
    propIcon->setFixedSize(20, 20);
    propIcon->setFlat(true);

    ElaText* propLabel = new ElaText("表属性:", propertiesHeader);
    propLabel->setTextPixelSize(16);

    ElaPushButton* addBtn = new ElaPushButton("添加属性", propertiesHeader);
    addBtn->setFixedSize(100, 30);
    addBtn->setBorderRadius(8);
    connect(addBtn, &ElaPushButton::clicked, this, &T_AddTable::onAddPropertyClicked);

    headerLayout->addWidget(propIcon);
    headerLayout->addWidget(propLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(addBtn);

    // 属性容器
    m_propertiesContainer = new QWidget(centralWidget);
    m_propertiesLayout = new QVBoxLayout(m_propertiesContainer);
    m_propertiesLayout->setContentsMargins(0, 0, 0, 0);
    m_propertiesLayout->setSpacing(15);

    // ==================== 确认按钮 ====================
    QWidget* buttonSection = new QWidget(centralWidget);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonSection);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    ElaPushButton* confirmBtn = new ElaPushButton("创建", buttonSection);
    confirmBtn->setFixedSize(100, 36);
    confirmBtn->setBorderRadius(8);
    connect(confirmBtn, &ElaPushButton::clicked, this, &T_AddTable::onConfirmClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(confirmBtn);

    // 主布局
    mainLayout->addWidget(dbSection);
    mainLayout->addWidget(tableSection);
    mainLayout->addWidget(propertiesHeader);
    mainLayout->addWidget(m_propertiesContainer);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonSection);

    addCentralWidget(centralWidget);
}

QWidget* T_AddTable::createPropertyRow()
{
    QWidget* row = new QWidget(m_propertiesContainer);
    QVBoxLayout* mainLayout = new QVBoxLayout(row);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    // 第一行：基本属性
    QWidget* basicRow = new QWidget(row);
    QHBoxLayout* basicLayout = new QHBoxLayout(basicRow);
    basicLayout->setContentsMargins(0, 0, 0, 0);
    basicLayout->setSpacing(10);

    // 列名
    ElaLineEdit* nameEdit = new ElaLineEdit(basicRow);
    nameEdit->setPlaceholderText("列名");
    nameEdit->setFixedHeight(30);
    nameEdit->setBorderRadius(8);

    // 类型
    ElaComboBox* typeCombo = new ElaComboBox(basicRow);
    typeCombo->addItems({"INT", "STRING", "FLOAT"});
    typeCombo->setFixedHeight(30);
    typeCombo->setBorderRadius(8);

    // 删除按钮
    ElaPushButton* deleteBtn = new ElaPushButton("删除", basicRow);
    deleteBtn->setFixedSize(60, 30);
    deleteBtn->setBorderRadius(8);
    connect(deleteBtn, &ElaPushButton::clicked, [row]() { row->deleteLater(); });

    basicLayout->addWidget(nameEdit, 2);
    basicLayout->addWidget(typeCombo, 2);
    basicLayout->addStretch(1);
    basicLayout->addWidget(deleteBtn);

    // 第二行：约束条件分组
    QWidget* constraintRow = new QWidget(row);
    QVBoxLayout* constraintLayout = new QVBoxLayout(constraintRow);
    constraintLayout->setContentsMargins(0, 0, 0, 0);
    constraintLayout->setSpacing(10);

    // 基础约束选项
    QWidget* basicConstraints = new QWidget(constraintRow);
    QHBoxLayout* basicLayout_1 = new QHBoxLayout(basicConstraints);
    basicLayout_1->setContentsMargins(0, 0, 0, 0);
    basicLayout_1->setSpacing(15);

    ElaCheckBox* notNullCheck = new ElaCheckBox("NOT NULL", basicConstraints);
    ElaCheckBox* uniqueCheck = new ElaCheckBox("UNIQUE", basicConstraints);
    ElaCheckBox* primaryKeyCheck = new ElaCheckBox("PRIMARY KEY", basicConstraints);

    // 默认值选项
    ElaCheckBox* defaultCheck = new ElaCheckBox("DEFAULT", basicConstraints);
    ElaLineEdit* defaultEdit = new ElaLineEdit(basicConstraints);
    defaultEdit->setPlaceholderText("输入默认值");
    defaultEdit->setFixedHeight(30);
    defaultEdit->setBorderRadius(8);
    defaultEdit->setVisible(false); // 初始隐藏

    basicLayout_1->addWidget(notNullCheck);
    basicLayout_1->addWidget(uniqueCheck);
    basicLayout_1->addWidget(primaryKeyCheck);
    basicLayout_1->addWidget(defaultCheck);
    basicLayout_1->addWidget(defaultEdit);
    basicLayout_1->addStretch();

    // 外键选项（单独一行）
    QWidget* fkOptionRow = new QWidget(constraintRow);
    QHBoxLayout* fkOptionLayout = new QHBoxLayout(fkOptionRow);
    fkOptionLayout->setContentsMargins(0, 0, 0, 0);
    fkOptionLayout->setSpacing(15);

    ElaCheckBox* foreignKeyCheck = new ElaCheckBox("FOREIGN KEY", fkOptionRow);
    fkOptionLayout->addWidget(foreignKeyCheck);
    fkOptionLayout->addStretch();

    // 外键设置区域 (初始隐藏)
    QWidget* fkSettings = new QWidget(constraintRow);
    QVBoxLayout* fkLayout = new QVBoxLayout(fkSettings);
    fkLayout->setContentsMargins(20, 5, 0, 5);
    fkLayout->setSpacing(10);
    fkSettings->setVisible(false);

    // 引用表
    QWidget* refTableRow = new QWidget(fkSettings);
    QHBoxLayout* refTableLayout = new QHBoxLayout(refTableRow);
    refTableLayout->setContentsMargins(0, 0, 0, 0);
    ElaText* refTableLabel = new ElaText("引用表:", refTableRow);
    refTableLabel->setTextPixelSize(15);
    ElaComboBox* refTableCombo = new ElaComboBox(refTableRow);
    refTableCombo->setFixedHeight(30);
    refTableCombo->setPlaceholderText("选择表");
    refTableCombo->setBorderRadius(8);
    refTableLayout->addWidget(refTableLabel);
    refTableLayout->addWidget(refTableCombo);
    refTableLayout->addStretch();

    // 引用字段
    QWidget* refFieldRow = new QWidget(fkSettings);
    QHBoxLayout* refFieldLayout = new QHBoxLayout(refFieldRow);
    refFieldLayout->setContentsMargins(0, 0, 0, 0);
    ElaText* refFieldLabel = new ElaText("引用字段:", refFieldRow);
    refFieldLabel->setTextPixelSize(15);
    ElaComboBox* refFieldCombo = new ElaComboBox(refFieldRow);
    refFieldCombo->setFixedHeight(30);
    refFieldCombo->setPlaceholderText("选择字段");
    refFieldCombo->setBorderRadius(8);
    refFieldLayout->addWidget(refFieldLabel);
    refFieldLayout->addWidget(refFieldCombo);
    refFieldLayout->addStretch();

    // 约束名
    QWidget* constraintNameRow = new QWidget(fkSettings);
    QHBoxLayout* constraintNameLayout = new QHBoxLayout(constraintNameRow);
    constraintNameLayout->setContentsMargins(0, 0, 0, 0);
    ElaText* constraintNameLabel = new ElaText("约束名:", constraintNameRow);
    constraintNameLabel->setTextPixelSize(15);
    ElaLineEdit* constraintNameEdit = new ElaLineEdit(constraintNameRow);
    constraintNameEdit->setFixedHeight(30);
    constraintNameEdit->setPlaceholderText("约束名称(可选)");
    constraintNameEdit->setBorderRadius(8);
    constraintNameLayout->addWidget(constraintNameLabel);
    constraintNameLayout->addWidget(constraintNameEdit);
    constraintNameLayout->addStretch();

    fkLayout->addWidget(refTableRow);
    fkLayout->addWidget(refFieldRow);
    fkLayout->addWidget(constraintNameRow);

    // 连接信号
    connect(defaultCheck, &ElaCheckBox::toggled, defaultEdit, &ElaLineEdit::setVisible);
    connect(foreignKeyCheck, &ElaCheckBox::toggled, fkSettings, &QWidget::setVisible);

    constraintLayout->addWidget(basicConstraints);
    constraintLayout->addWidget(fkOptionRow);
    constraintLayout->addWidget(fkSettings);

    mainLayout->addWidget(basicRow);
    mainLayout->addWidget(constraintRow);

    return row;
}

void T_AddTable::onAddPropertyClicked()
{
    m_propertiesLayout->addWidget(createPropertyRow());
}
void T_AddTable::onConfirmClicked()
{
    QString dbName = m_dbComboBox->currentText();
    QString tableName = m_tableNameEdit->text().trimmed();

    if(dbName.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Bottom, "警告", "请选择数据库!", 2500, this);
        return;
    }

    if(tableName.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Bottom, "警告", "表名称不能为空!", 2500, this);
        return;
    }
    // 收集所有属性
    QList<TableProperty> properties;
    for(int i = 0; i < m_propertiesLayout->count(); ++i) {
        QWidget* row = qobject_cast<QWidget*>(m_propertiesLayout->itemAt(i)->widget());
        if(row) {
            TableProperty prop;

            // 获取基础控件
            QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(row->layout());
            QWidget* basicRow = qobject_cast<QWidget*>(mainLayout->itemAt(0)->widget());
            QHBoxLayout* basicLayout = qobject_cast<QHBoxLayout*>(basicRow->layout());

            // 获取基本属性
            ElaLineEdit* nameEdit = qobject_cast<ElaLineEdit*>(basicLayout->itemAt(0)->widget());
            ElaComboBox* typeCombo = qobject_cast<ElaComboBox*>(basicLayout->itemAt(1)->widget());
            ElaCheckBox* notNullCheck = qobject_cast<ElaCheckBox*>(basicLayout->itemAt(2)->widget());
            ElaCheckBox* uniqueCheck = qobject_cast<ElaCheckBox*>(basicLayout->itemAt(3)->widget());
            ElaCheckBox* primaryKeyCheck = qobject_cast<ElaCheckBox*>(basicLayout->itemAt(4)->widget());
            ElaPushButton* foreignKeyBtn = qobject_cast<ElaPushButton*>(basicLayout->itemAt(5)->widget());

            prop.name = nameEdit->text().trimmed();
            prop.type = typeCombo->currentText();
            prop.isNotNull = notNullCheck->isChecked();
            prop.isUnique = uniqueCheck->isChecked();
            prop.isPrimaryKey = primaryKeyCheck->isChecked();

            // 获取外键信息
            if(foreignKeyBtn->isChecked()) {
                QWidget* fkSettings = qobject_cast<QWidget*>(mainLayout->itemAt(1)->widget());
                QVBoxLayout* fkLayout = qobject_cast<QVBoxLayout*>(fkSettings->layout());

                QWidget* refTableRow = qobject_cast<QWidget*>(fkLayout->itemAt(0)->widget());
                QHBoxLayout* refTableLayout = qobject_cast<QHBoxLayout*>(refTableRow->layout());
                ElaComboBox* refTableCombo = qobject_cast<ElaComboBox*>(refTableLayout->itemAt(1)->widget());

                QWidget* refFieldRow = qobject_cast<QWidget*>(fkLayout->itemAt(1)->widget());
                QHBoxLayout* refFieldLayout = qobject_cast<QHBoxLayout*>(refFieldRow->layout());
                ElaComboBox* refFieldCombo = qobject_cast<ElaComboBox*>(refFieldLayout->itemAt(1)->widget());

                QWidget* constraintRow = qobject_cast<QWidget*>(fkLayout->itemAt(2)->widget());
                QHBoxLayout* constraintLayout = qobject_cast<QHBoxLayout*>(constraintRow->layout());
                ElaLineEdit* constraintEdit = qobject_cast<ElaLineEdit*>(constraintLayout->itemAt(1)->widget());

                prop.foreignKeyRefTable = refTableCombo->currentText();
                prop.foreignKeyRefField = refFieldCombo->currentText();
                prop.constraintName = constraintEdit->text().trimmed();
            }

            if(!prop.name.isEmpty()) {
                properties.append(prop);
            }
        }
    }

    if(properties.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Bottom, "警告", "请至少添加一个属性!", 2500, this);
        return;
    }

    emit tableCreated(dbName, tableName, properties);
}

void T_AddTable::setDatabaseList(const QStringList& databases)
{
    m_dbComboBox->clear();
    m_dbComboBox->addItems(databases);
}
