#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProgressDialog>
//#include "points_68.h"
#include "./fit_model.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //QPushButton shoot,reshoot ;

    QTimer *timer;
    //points_68* keypointface;
    //默认构造函数 参数依次为，对话框正文，取消按钮名称，进度条范围，及所属

    // 0<i<=max   此块代码可以按需要和上面代码分开写在不同位置 如果在不同作用域中注意QProgressDialog *progressDlg应该在哪块声明
//        progressDlg->setValue(i);
//        //如果点击取消按钮就执行的操作
//        if(progressDlg->wasCanceled())
//        {
//              QMessageBox::warning(NULL,QStringLiteral("提示"),QStringLiteral("保存失败"));
//             return;
//        }
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:

    void myopenglupdate();
    void shoot1();
    void reshoot();
    void show_cam();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
