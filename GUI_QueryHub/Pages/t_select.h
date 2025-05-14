#ifndef T_SELECT_H
#define T_SELECT_H

#include "ElaPlainTextEdit.h"
#include "ElaTabWidget.h"
#include "t_basepage.h"
#include <QWidget>
#include <vector>

class QPaintEvent;
class LineNumberArea; // 前向声明

class T_Select : public T_BasePage
{
    Q_OBJECT
public:
    explicit T_Select(QWidget* parent = nullptr);
    ~T_Select();

    QString getQueryText() const;
    void resetResults();
    void executeQuery(const std::vector<std::vector<std::string>>& tableData);

protected:
    // 行号区域相关方法
    int lineNumberAreaWidth();
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(QRectF rect, int dy);
    void lineNumberAreaPaintEvent(QPaintEvent* event);

private:
    ElaPlainTextEdit* _queryEdit; // 查询输入框
    ElaTabWidget* _resultTabs;    // 结果展示标签页
    int _resultCount;             // 结果计数器
    LineNumberArea* _lineNumberArea; // 行号区域
};

#endif // T_SELECT_H
