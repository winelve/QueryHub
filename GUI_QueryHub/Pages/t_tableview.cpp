#include "T_TableView.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QApplication>
#include <QFontMetrics>

#include "ElaTableView.h"
#include "../modle/t_tableviewmodel.h"

T_TableView::T_TableView(const QString& tableName, QWidget* parent)
    : T_BasePage(parent), _tableName(tableName)
{
    // 设置窗口标题为表名
    setWindowTitle(tableName);

    // ElaTableView
    _tableView = new ElaTableView(this);
    _tableModel = new T_TableViewModel(this);
    _tableView->setModel(_tableModel);

    // 配置表格属性
    _tableView->setAlternatingRowColors(true);
    _tableView->setIconSize(QSize(32, 32));
    _tableView->verticalHeader()->setHidden(false);
    _tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    _tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _tableView->setShowGrid(true);
    _tableView->setGridStyle(Qt::SolidLine);

    // 设置字体
    QFont tableHeaderFont = _tableView->horizontalHeader()->font();
    tableHeaderFont.setPixelSize(16);
    tableHeaderFont.setBold(true);
    _tableView->horizontalHeader()->setFont(tableHeaderFont);
    _tableView->verticalHeader()->setFont(tableHeaderFont);

    // 设置最小尺寸
    _tableView->horizontalHeader()->setMinimumSectionSize(80);
    _tableView->verticalHeader()->setMinimumSectionSize(30);
    _tableView->setMinimumHeight(800);

    // 布局
    QHBoxLayout* tableViewLayout = new QHBoxLayout();
    tableViewLayout->setContentsMargins(10, 10, 10, 10);
    tableViewLayout->addWidget(_tableView);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle(tableName); // 设置 centralWidget 的标题为表名
    QVBoxLayout* centerVLayout = new QVBoxLayout(centralWidget);
    centerVLayout->setContentsMargins(15, 15, 15, 15);
    centerVLayout->setSpacing(10);
    centerVLayout->addLayout(tableViewLayout);
    centerVLayout->addStretch();
    addCentralWidget(centralWidget, true, false, 0);
}

T_TableView::~T_TableView()
{
}

void T_TableView::setTableData(const std::vector<std::vector<std::string>>& data)
{
    _tableModel->setData(data);

    // 动态调整列宽
    QFontMetrics fontMetrics(_tableView->font());
    const int padding = 20; // 额外的内边距
    for (int col = 0; col < _tableModel->columnCount(); ++col) {
        int maxWidth = 0;
        // 遍历每一列，找到最长内容的像素宽度
        for (int row = 0; row < data.size(); ++row) {
            QString text = QString::fromStdString(data[row][col]);
            int textWidth = fontMetrics.horizontalAdvance(text);
            maxWidth = std::max(maxWidth, textWidth);
        }
        // 设置列宽为最长内容的宽度加上内边距
        int adjustedWidth = maxWidth + padding;
        _tableView->setColumnWidth(col, adjustedWidth);
    }

    // 为 join_date 列（假设索引 6）设置固定宽度
    if (data[0].size() > 6) {
        _tableView->setColumnWidth(6, 120);
    }
}

void T_TableView::setTableTitle(const QString& title)
{
    _tableName = title;
    setWindowTitle(title);
    for (QWidget* widget : findChildren<QWidget*>()) {
        if (widget->windowTitle() == _tableName) {
            widget->setWindowTitle(title);
            break;
        }
    }
}
