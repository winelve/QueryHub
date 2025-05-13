#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ElaWindow.h"
#include "Pages/t_about.h"
#include "Pages/t_setting.h"
#include "Pages/t_adddatabase.h"
#include "Pages/t_addtable.h"
#include "Pages/t_deletedatabase.h"
#include "Pages/t_deletetable.h"
#include "Pages/t_addfields.h"
#include "Pages/t_deletefield.h"
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

private slots:
    void onToolButton1Clicked();

private:
    ElaContentDialog* _closeDialog{nullptr};
    T_About* _aboutPage{nullptr};
    T_Setting* _settingPage{nullptr};
    T_AddDataBase* _addDataBasePage{nullptr};
    T_AddTable* _addTablePage{nullptr};
    T_DeleteDataBase* _delDataBasePage{nullptr};
    T_DeleteTable* _delTablePage{nullptr};
    T_AddFields* _addFieldsPage{nullptr};
    T_DeleteField* _delFieldsPage{nullptr};
    QString _aboutKey{""};
    QString _settingKey{""};

    //---------------------------------------------------------------------------


};

#endif // MAINWINDOW_H
