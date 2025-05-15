#include "t_select.h"
#include <QVBoxLayout>
#include <ElaMessageBar.h>
#include <ElaTableView.h>
#include "../modle/t_tableviewmodel.h"
#include <QDebug>
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QHeaderView>
#include <QFontMetrics>
#include <ElaTabWidget.h>
#include "ElaText.h"

// 行号区域实现
LineNumberArea::LineNumberArea(QPlainTextEdit* editor) : QWidget(editor), codeEditor(editor) {
    setStyleSheet("background-color: #F5F5F5; color: #555;"); // 设置行号区域样式
}

QSize LineNumberArea::sizeHint() const {
    return QSize(dynamic_cast<CodeEditor*>(codeEditor)->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    dynamic_cast<CodeEditor*>(codeEditor)->lineNumberAreaPaintEvent(event);
}

// 自定义文本编辑器实现
CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent) {
    lineNumberArea = new LineNumberArea(this);

    // 信号连接
    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    // 初始化行号区域宽度
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    // 设置编辑器样式
    setStyleSheet("QPlainTextEdit { background-color: #FFFFFF; color: #000000; }");
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent* e) {
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width() - 3, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

// 查询界面实现
T_Select::T_Select(QWidget* parent)
    : T_BasePage(parent)
    , _queryEdit(new CodeEditor(this))
    , _resultTabs(new ElaTabWidget(this))
    , _resultCount(0)
{
    createCustomWidget("查询界面");

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("查询");

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(30);

    mainLayout->addWidget(_queryEdit);
    mainLayout->addWidget(_resultTabs);

    _queryEdit->setPlaceholderText("请输入 SQL 查询语句...");
    _queryEdit->setMinimumHeight(200);
    _resultTabs->setTabPosition(QTabWidget::North);

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
    while (_resultTabs->count() > 0) {
        QWidget* widget = _resultTabs->widget(0);
        _resultTabs->removeTab(0);
        delete widget;
    }
    _resultCount = 0;
}

void T_Select::executeQuery(const std::vector<std::vector<std::string>>& tableData)
{
    if (tableData.empty()) {
        ElaMessageBar::warning(ElaMessageBarType::TopRight, "警告", "查询结果为空!", 3000);
        return;
    }

    _resultCount++;
    QWidget* tabWidget = new QWidget(_resultTabs);
    tabWidget->setWindowTitle(QString("查询%1").arg(_resultCount));

    QVBoxLayout* tabLayout = new QVBoxLayout(tabWidget);
    tabLayout->setContentsMargins(15, 15, 15, 15);
    tabLayout->setSpacing(10);

    // 检查数据是否为表格格式（多列）或单列/字符串
    if (tableData[0].size() > 1) { // 假设多列表示表格数据
        // 创建 ElaTableView 和模型
        ElaTableView* tableView = new ElaTableView(tabWidget);
        T_TableViewModel* tableModel = new T_TableViewModel(tabWidget);
        tableView->setModel(tableModel);

        // 配置表格属性
        tableView->setAlternatingRowColors(true);
        tableView->setIconSize(QSize(32, 32));
        tableView->verticalHeader()->setHidden(false);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->setShowGrid(true);
        tableView->setGridStyle(Qt::SolidLine);

        QFont tableHeaderFont = tableView->horizontalHeader()->font();
        tableHeaderFont.setPixelSize(16);
        tableHeaderFont.setBold(true);
        tableView->horizontalHeader()->setFont(tableHeaderFont);
        tableView->verticalHeader()->setFont(tableHeaderFont);

        tableView->horizontalHeader()->setMinimumSectionSize(80);
        tableView->verticalHeader()->setMinimumSectionSize(30);
        tableView->setMinimumHeight(800);

        // 设置表格数据
        tableModel->setData(tableData);

        // 动态调整列宽
        QFontMetrics fontMetrics(tableView->font());
        const int padding = 20;
        for (int col = 0; col < tableModel->columnCount(); ++col) {
            int maxWidth = 0;
            for (size_t row = 0; row < tableData.size(); ++row) {
                QString text = QString::fromStdString(tableData[row][col]);
                int textWidth = fontMetrics.horizontalAdvance(text);
                maxWidth = std::max(maxWidth, textWidth);
            }
            int adjustedWidth = maxWidth + padding;
            tableView->setColumnWidth(col, adjustedWidth);
        }

        if (!tableData.empty() && tableData[0].size() > 6) {
            tableView->setColumnWidth(6, 120);
        }

        QHBoxLayout* tableViewLayout = new QHBoxLayout();
        tableViewLayout->setContentsMargins(10, 10, 10, 10);
        tableViewLayout->addWidget(tableView);
        tabLayout->addLayout(tableViewLayout);
    }
    else { // 单列或字符串，使用 ElaText 显示
        QString text;
        for (const auto& row : tableData) {
            text += QString::fromStdString(row[0]) + "\n";
        }

        ElaText* resultText = new ElaText(tabWidget);
        resultText->setText(text.trimmed());
        resultText->setFont(QFont("Microsoft YaHei", 12));

        QHBoxLayout* textLayout = new QHBoxLayout();
        textLayout->setContentsMargins(10, 10, 10, 10);
        textLayout->addWidget(resultText);
        tabLayout->addLayout(textLayout);
    }

    tabLayout->addStretch();
    _resultTabs->addTab(tabWidget, QString("结果%1").arg(_resultCount));

    // 确保切换到最新 Tab
    _resultTabs->setCurrentIndex(_resultTabs->count() - 1);
}
