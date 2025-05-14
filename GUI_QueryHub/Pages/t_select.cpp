#include "t_select.h"
#include <QVBoxLayout>
#include <ElaMessageBar.h>
#include "t_tableview.h"
#include <QDebug>

T_Select::T_Select(QWidget* parent)
    : T_BasePage(parent)
    , _queryEdit(new ElaPlainTextEdit(this))
    , _resultTabs(new ElaTabWidget(this))
    , _resultCount(0)
{
    // 设置描述文本
    createCustomWidget("查询界面");

    // 主容器
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("查询");

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(40, 40, 40, 40); // 设置边距
    mainLayout->setSpacing(30); // 设置组件间距

    // 添加输入区域和结果展示区域到主布局
    mainLayout->addWidget(_queryEdit);
    mainLayout->addWidget(_resultTabs);

    // 设置样式和属性
    _queryEdit->setPlaceholderText("请输入 SQL 查询语句...");
    _queryEdit->setMinimumHeight(200); // 设置编辑区域高度
    _resultTabs->setTabPosition(QTabWidget::North);

    // 将主容器添加到页面
    addCentralWidget(centralWidget, true, true, 0);
}

T_Select::~T_Select()
{
}

QString T_Select::getQueryText() const
{
    return _queryEdit->toPlainText();
}

void T_Select::resetResults()
{
    // 清除所有标签页
    while (_resultTabs->count() > 0) {
        QWidget* widget = _resultTabs->widget(0);
        _resultTabs->removeTab(0);
        delete widget;
    }
    // 重置计数器
    _resultCount = 0;
}

void T_Select::executeQuery(const std::vector<std::vector<std::string>>& tableData)
{
    if (tableData.empty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", "查询结果为空!", 3000);
        return;
    }

    // 创建新的表格并填充数据
    // 修正：先递增 _resultCount，确保与标签页编号一致
    _resultCount++;
    T_TableView* tableView = new T_TableView(QString("查询%1").arg(_resultCount), _resultTabs);
    tableView->setTableData(tableData);

    // 添加到标签页，使用一致的编号
    _resultTabs->addTab(tableView, QString("结果%1").arg(_resultCount));
}
