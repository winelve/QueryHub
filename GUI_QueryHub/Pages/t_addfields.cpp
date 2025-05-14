#include "T_AddFields.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaMessageBar.h"
#include "ElaIconButton.h"
#include <ElaCheckBox.h>
#include <QDebug>

T_AddFields::T_AddFields(QWidget* parent)
    : T_BasePage(parent)
{
    createCustomWidget("");
    setupUI();
}

void T_AddFields::setupUI()
{
    // 主容器
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("添加字段");
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

    dbLayout->addWidget(dbIcon);
    dbLayout->addWidget(dbLabel);
    dbLayout->addWidget(m_dbComboBox);
    dbLayout->addStretch();

    // ==================== 表选择 ====================
    QWidget* tableSection = new QWidget(centralWidget);
    QHBoxLayout* tableLayout = new QHBoxLayout(tableSection);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(15);

    ElaIconButton* tableIcon = new ElaIconButton(ElaIconType::Table, tableSection);
    tableIcon->setFixedSize(20, 20);
    tableIcon->setFlat(true);

    ElaText* tableLabel = new ElaText("选择数据表:", tableSection);
    tableLabel->setTextPixelSize(16);

    m_tableComboBox = new ElaComboBox(tableSection);
    m_tableComboBox->setPlaceholderText("请选择数据表");
    m_tableComboBox->setFixedHeight(30);
    m_tableComboBox->setBorderRadius(8);

    tableLayout->addWidget(tableIcon);
    tableLayout->addWidget(tableLabel);
    tableLayout->addWidget(m_tableComboBox);
    tableLayout->addStretch();

    // ==================== 字段列表标题 ====================
    QWidget* fieldsHeader = new QWidget(centralWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(fieldsHeader);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(15);

    ElaIconButton* fieldIcon = new ElaIconButton(ElaIconType::Sparkles, fieldsHeader);
    fieldIcon->setFixedSize(20, 20);
    fieldIcon->setFlat(true);

    ElaText* fieldLabel = new ElaText("字段列表:", fieldsHeader);
    fieldLabel->setTextPixelSize(16);

    ElaPushButton* addBtn = new ElaPushButton("添加字段", fieldsHeader);
    addBtn->setFixedSize(100, 30);
    addBtn->setBorderRadius(8);
    connect(addBtn, &ElaPushButton::clicked, this, &T_AddFields::onAddFieldClicked);

    headerLayout->addWidget(fieldIcon);
    headerLayout->addWidget(fieldLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(addBtn);

    // ==================== 字段容器 ====================
    m_fieldsContainer = new QWidget(centralWidget);
    m_fieldsLayout = new QVBoxLayout(m_fieldsContainer);
    m_fieldsLayout->setContentsMargins(0, 0, 0, 0);
    m_fieldsLayout->setSpacing(15);

    // 初始添加一个字段
    onAddFieldClicked();

    // ==================== 确认按钮 ====================
    QWidget* buttonSection = new QWidget(centralWidget);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonSection);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    ElaPushButton* cancelBtn = new ElaPushButton("取消", buttonSection);
    cancelBtn->setFixedSize(100, 36);
    cancelBtn->setBorderRadius(8);
    connect(cancelBtn, &ElaPushButton::clicked, this, &T_AddFields::onCancelClicked);

    ElaPushButton* confirmBtn = new ElaPushButton("添加", buttonSection);
    confirmBtn->setFixedSize(100, 36);
    confirmBtn->setBorderRadius(8);
    confirmBtn->setLightDefaultColor(QColor(0, 120, 212));
    confirmBtn->setDarkDefaultColor(QColor(0, 90, 158));
    confirmBtn->setLightTextColor(Qt::white);
    confirmBtn->setDarkTextColor(Qt::white);
    connect(confirmBtn, &ElaPushButton::clicked, this, &T_AddFields::onConfirmClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(confirmBtn);

    // 主布局
    mainLayout->addWidget(dbSection);
    mainLayout->addWidget(tableSection);
    mainLayout->addWidget(fieldsHeader);
    mainLayout->addWidget(m_fieldsContainer);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonSection);

    addCentralWidget(centralWidget);
}

QWidget* T_AddFields::createFieldRow()
{
    QWidget* row = new QWidget(m_fieldsContainer);
    QVBoxLayout* mainLayout = new QVBoxLayout(row);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    // 第一行：基本属性
    QWidget* basicRow = new QWidget(row);
    QHBoxLayout* basicLayout = new QHBoxLayout(basicRow);
    basicLayout->setContentsMargins(0, 0, 0, 0);
    basicLayout->setSpacing(10);

    // 字段名
    ElaLineEdit* nameEdit = new ElaLineEdit(basicRow);
    nameEdit->setPlaceholderText("字段名");
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

    connect(defaultCheck, &ElaCheckBox::toggled, defaultEdit, &ElaLineEdit::setVisible);
    connect(foreignKeyCheck, &ElaCheckBox::toggled, fkSettings, &QWidget::setVisible);

    constraintLayout->addWidget(basicConstraints);
    constraintLayout->addWidget(fkOptionRow);
    constraintLayout->addWidget(fkSettings);

    mainLayout->addWidget(basicRow);
    mainLayout->addWidget(constraintRow);

    return row;
}

void T_AddFields::setDatabases(const QStringList& databases)
{
    m_dbComboBox->clear();
    m_dbComboBox->addItems(databases);
}

void T_AddFields::setTables(const QStringList& tables)
{
    m_tableComboBox->clear();
    m_tableComboBox->addItems(tables);
}

void T_AddFields::onAddFieldClicked()
{
    m_fieldsLayout->addWidget(createFieldRow());
}

void T_AddFields::onConfirmClicked()
{
    QString dbName = m_dbComboBox->currentText();
    QString tableName = m_tableComboBox->currentText();

    if(dbName.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Bottom, "警告", "请选择数据库!", 2500, this);
        return;
    }

    if(tableName.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Bottom, "警告", "请选择数据表!", 2500, this);
        return;
    }

    // 收集所有字段信息
    QList<QPair<QString, QString>> fields;
    for(int i = 0; i < m_fieldsLayout->count(); ++i) {
        QWidget* row = qobject_cast<QWidget*>(m_fieldsLayout->itemAt(i)->widget());
        if(row) {
            ElaLineEdit* nameEdit = row->findChild<ElaLineEdit*>();
            ElaComboBox* typeCombo = row->findChild<ElaComboBox*>();
            if(nameEdit && typeCombo && !nameEdit->text().isEmpty()) {
                fields.append({nameEdit->text(), typeCombo->currentText()});
            }
        }
    }

    if(fields.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::Bottom, "警告", "请至少添加一个有效字段!", 2500, this);
        return;
    }

    emit fieldsAdded(dbName, tableName, fields);
}

void T_AddFields::onCancelClicked()
{
    this->close();
}

T_AddFields::~T_AddFields()
{
}
