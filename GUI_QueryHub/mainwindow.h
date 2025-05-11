#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ElaWindow.h"
#include "Pages/t_about.h"
#include "Pages/t_setting.h"

class ElaContentDialog;

class MainWindow : public ElaWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void initWindow();
    void initEdgeLayout();
    void initContent();

private:
    ElaContentDialog* _closeDialog{nullptr};
    T_About* _aboutPage{nullptr};
    T_Setting* _settingPage{nullptr};
    QString _aboutKey{""};
    QString _settingKey{""};
};

#endif // MAINWINDOW_H
