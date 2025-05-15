#ifndef T_SELECT_H
#define T_SELECT_H

#include "t_basepage.h"
#include <QPlainTextEdit>
#include <QWidget>
#include <QTabWidget>
#include <vector>

// 自定义行号区域类
class LineNumberArea : public QWidget {
    Q_OBJECT
public:
    LineNumberArea(QPlainTextEdit* editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPlainTextEdit* codeEditor; // 关联的文本编辑器
};

// 自定义文本编辑器类，带行号
class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    CodeEditor(QWidget* parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent* event); // 绘制行号
    int lineNumberAreaWidth(); // 计算行号区域宽度

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount); // 更新行号区域宽度
    void highlightCurrentLine(); // 高亮当前行
    void updateLineNumberArea(const QRect& rect, int dy); // 更新行号区域

private:
    QWidget* lineNumberArea; // 行号区域
};

// 查询界面类
class ElaTableView;
class T_TableViewModel;
class ElaTabWidget;
class T_Select : public T_BasePage {
    Q_OBJECT
public:
    explicit T_Select(QWidget* parent = nullptr);
    ~T_Select();

    QString getQueryText() const; // 获取查询文本
    void resetResults(); // 重置结果
    void executeQuery(const std::vector<std::vector<std::string>>& tableData); // 执行查询

private:
    CodeEditor* _queryEdit; // 自定义编辑器
    ElaTabWidget* _resultTabs; // 结果标签页
    int _resultCount; // 结果计数
};

#endif // T_SELECT_H
