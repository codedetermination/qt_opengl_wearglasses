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
/// @brief  主窗口类
/// 生成主要的窗口，包括摄像头显示，人脸检测，人脸关键点检测，人脸模型拟合
/// @param  Ui::MainWindow *ui  界面指针
/// @param  QPushButton shoot   拍照按钮
/// @param  QPushButton reshoot 重拍按钮
/// @param  QTimer *timer       定时器
/// @param  points_68* keypointface 人脸关键点检测类指针
/// @param  fit_model* fitmodel 人脸模型拟合类指针
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //QPushButton shoot,reshoot ;
    /// @brief  主窗口类构造函数
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
    /// @brief  主窗口的构造函数
    /// 用来初始化主窗口的界面，以及连接信号和槽，以及初始化定时器
    MainWindow(QWidget *parent = nullptr);
    /// @brief  主窗口的析构函数
    /// 用来释放主窗口的界面，以及释放定时器
    ~MainWindow();
public slots:
    /// @brief  定时器槽函数
    /// 用来更新opengl窗口
    void myopenglupdate();
    /// @brief  拍照按钮槽函数
    /// 用来拍照，隐藏拍照按钮，显示重拍和确认按钮
    void shoot1();
    /// @brief  重拍按钮槽函数
    /// 用来重拍，隐藏重拍和确认按钮，显示拍照按钮
    void reshoot();
    /// @brief  确认按钮槽函数
    /// 用来确认拍照，隐藏重拍和确认按钮，显示拍照按钮
    void show_cam();
private:
    /// @brief  主窗口的界面指针
    /// 用来指向主窗口的界面
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
