#include "t_select.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
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
    _queryEdit->setMinimumHeight(700); // 设置编辑区域高度
    _resultTabs->setTabPosition(QTabWidget::North);

    // 将主容器添加到页面
    addCentralWidget(centralWidget, true, true, 0);
}

T_Select::~T_Select()
{
}

void T_Select::executeQuery()
{
    QString query = _queryEdit->toPlainText().trimmed();
    if (query.isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", "请输入查询语句!", 3000);
        return;
    }

    // 模拟多次查询结果（3次查询）
    std::vector<std::vector<std::string>> mockData1 = {
        {"id", "name", "age", "city"}, // 表头
        {"1", "Alice", "25", "Beijing"},
        {"2", "Bob", "30", "Shanghai"},
        {"3", "Charlie", "35", "Guangzhou"}
    };

    std::vector<std::vector<std::string>> mockData2 = {
        {"product_id", "product_name", "price"}, // 表头
        {"101", "Laptop", "1200"},
        {"102", "Phone", "800"},
        {"103", "Tablet", "500"}
    };

    std::vector<std::vector<std::string>> mockData3 = {
        {"order_id", "customer", "date", "amount"}, // 表头
        {"1001", "David", "2025-01-15", "1500"},
        {"1002", "Eve", "2025-02-20", "900"},
        {"1003", "Frank", "2025-03-10", "2000"}
    };

    // 将所有模拟数据放入一个列表，方便循环处理
    std::vector<std::vector<std::vector<std::string>>> mockQueries = {mockData1, mockData2, mockData3};

    // 循环处理每次查询结果
    for (const auto& mockData : mockQueries) {
        // 创建新的表格并填充数据
        T_TableView* tableView = new T_TableView(QString("查询%1").arg(_resultCount + 1), _resultTabs);
        tableView->setTableData(mockData);

        // 添加到标签页
        _resultCount++;
        _resultTabs->addTab(tableView, QString("结果%1").arg(_resultCount));
    }

    ElaMessageBar::success(ElaMessageBarType::TopRight, "成功", "查询执行成功!", 2000);
}
