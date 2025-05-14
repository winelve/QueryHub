#include "T_TableView.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include "ElaMessageBar.h"
#include <QDebug>

T_TableView::T_TableView(const QString& database, const QString& table, QWidget* parent)
    : T_BasePage(parent), _database(database), _table(table)
{
    // 预览窗口标题
    setWindowTitle("ElaTableView");

    // 顶部元素
    createCustomWidget("");

    _tableView = new ElaTableView(this);
    _model = new T_TableViewModel(this);
    _tableView->setModel(_model);
    _tableView->setAlternatingRowColors(true);
    _tableView->setIconSize(QSize(38, 38));
    _tableView->verticalHeader()->setHidden(true);
    _tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    _tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableView->horizontalHeader()->setMinimumSectionSize(60);
    _tableView->verticalHeader()->setMinimumSectionSize(46);
    _tableView->setFixedHeight(450);

    connect(_tableView, &ElaTableView::tableViewShow, this, [=]() {
        // 动态调整列宽
        for (int i = 0; i < _model->columnCount(); ++i) {
            _tableView->setColumnWidth(i, i == 0 ? 60 : 150);
        }
    });

    QHBoxLayout* tableViewLayout = new QHBoxLayout();
    tableViewLayout->setContentsMargins(0, 0, 10, 0);
    tableViewLayout->addWidget(_tableView);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle(table);
    QVBoxLayout* centerVLayout = new QVBoxLayout(centralWidget);
    centerVLayout->setContentsMargins(0, 0, 0, 0);
    centerVLayout->addLayout(tableViewLayout);
    addCentralWidget(centralWidget, true, true, 0);
}

T_TableView::~T_TableView()
{
    delete _model;
}

void T_TableView::dis_table(const std::vector<std::vector<std::string>>& data)
{
    qDebug() << "dis_table called, data rows:" << data.size();

    if (data.empty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "错误", "表数据为空", 3000);
        return;
    }

    // 更新模型
    _model->setData(data);

    // 触发列宽调整
    emit _tableView->tableViewShow();
}
