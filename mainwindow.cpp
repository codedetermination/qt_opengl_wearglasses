#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "MainGlwight.h"
#include "fit_model.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //shoot = ui->shoot_button;
    //reshoot = ui->reshoot_button;
    //connect(shoot,SIGNAL(clicked()),this,)
    ui->reshoot_button->hide();
    ui->confirm_image->hide();
    connect(ui->shoot_button,SIGNAL(clicked(bool)),this,SLOT(shoot1()));
    connect(ui->reshoot_button,SIGNAL(clicked(bool)),this,SLOT(reshoot()));
    connect(ui->confirm_image,SIGNAL(clicked(bool)),this,SLOT(show_cam()));
    //QProgressDialog *progressDlg=new QProgressDialog( QStringLiteral("正在保存......"), QStringLiteral("取消"),0,100,this);
    //progressDlg->hide();



    timer = new QTimer(this);
    timer->setInterval(33);;
    connect(timer, SIGNAL(timeout()), this, SLOT(myopenglupdate()));
    timer->start();
}
void MainWindow::myopenglupdate(){
    ui->openGLWidget->update_scence();
}
void MainWindow::shoot1(){
    ui->openGLWidget->shoot_image();
    ui->shoot_button->hide();
    ui->reshoot_button->show();
    ui->confirm_image->show();
    timer->stop();
}

void MainWindow::reshoot(){
    ui->openGLWidget->reshoot();
    ui->shoot_button->show();
    ui->reshoot_button->hide();
    ui->confirm_image->hide();
    ui->openGLWidget->reshow_camera();
    timer->start();
}

void MainWindow::show_cam(){
    ui->openGLWidget->show_camera();

    QProgressDialog *progressDlg=new QProgressDialog( QStringLiteral("正在处理......"), QStringLiteral("取消"),0,100,this);
    progressDlg->setMinimumDuration(2);
    progressDlg->setWindowModality(Qt::WindowModal);
    //如果进度条运行的时间小于5，进度条就不会显示，不设置默认是4S

    //设置标题，可以不设置默认继承父窗口标题
    progressDlg->setWindowTitle(QStringLiteral("请稍候"));
    progressDlg->setAutoClose(true);

    //keypointface = new points_68(ui->openGLWidget->get_image());
    progressDlg->setValue(1);
    if(ui->openGLWidget->setup_points68()){
        progressDlg->setValue(25);
    }
    else{
        throw EXIT_FAILURE;
    }
    fit_model model(ui->openGLWidget->get_image());
    progressDlg->setValue(50);
    model.generate_obj();
    progressDlg->setValue(75);
    cv::waitKey(300);
    progressDlg->setValue(85);
    cv::waitKey(300);
    progressDlg->setValue(99);
    cv::waitKey(300);
    progressDlg->setValue(100);

    timer->start();

}


MainWindow::~MainWindow()
{
    delete ui;
}

