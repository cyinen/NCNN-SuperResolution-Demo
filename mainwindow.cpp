#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camera.h"
#include <QThread>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
//    setWindowState(Qt::WindowMaximized);
    setWindowTitle("video enhancement");

    cap = new Camera();
    pthread = new QThread(this);
    cap->moveToThread(pthread);
    connect(this, &MainWindow::openCamera, cap, &Camera::doMywork);

    void (Camera::*sendImg)(QImage) = &Camera::sendImg;

    connect(cap, sendImg, this, &MainWindow::showImg);

    connect(this, &MainWindow::sendStatus, cap, &Camera::changeStatus);

}


void MainWindow::on_pb_exit_clicked()
{
    this->close();
}

void MainWindow::on_pb_openCamera_clicked()
{
    if(!cap->openCameraFlag)
    {
        if(pthread->isRunning())
            return;
        pthread->start();
        cap->setflag(true);
        ui->pb_openCamera->setText("关闭摄像头 ");
        emit openCamera(0, file);
    }
    else
    {
        if(!pthread->isRunning())
            return;
        cap->setflag(false);
        pthread->quit();
        pthread->wait();
        ui->pb_openCamera->setText("打开摄像头 ");

    }

}

void MainWindow::showImg(QImage img)
{
//    int w = img.width();
//    int h = img.height();
//    ui->label_showImg->resize(w, h);
    ui->label_showImg->setText("");
    ui->label_showImg->setPixmap(QPixmap::fromImage(img));
//    qDebug("w:%d,h:%d",w,h);
}

void MainWindow::on_pb_switch_pressed()
{
    ui->pb_switch->setText("修复后 ");
    qDebug("Bicubic");
    cap->changeStatus();
}

void MainWindow::on_pb_switch_released()
{
    qDebug("Enchence");
    ui->pb_switch->setText("修复前 ");

    cap->changeStatus();

}

void MainWindow::on_pb_playVideo_clicked()
{



    if(!cap->openCameraFlag)
    {
        QString curPath=QDir::currentPath();
        QString dlgTitle="选择一个文件 ";
        QString filter="video (*.mp4 *.gif *.webp *.mkv);;";
        QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
        if(!aFileName.isNull())
        {
//            aFileName.replace("/","\\\\");
            if(pthread->isRunning())
                return;
            pthread->start();
            cap->setflag(true);
//            ui->pb_playVideo->setText("cancel ");
//            qDebug(aFileName.toStdString().c_str());
            emit openCamera(2, aFileName);
        }
        else{
            QMessageBox::information(NULL,
                                      QStringLiteral("Noting"),
                                      QStringLiteral("Please chose a video file"),
                                      QMessageBox::Yes|QMessageBox::No,
                                      QMessageBox::Yes);
        }
    }
    else
    {
        if(!pthread->isRunning())
            return;
        cap->setflag(false);
        pthread->quit();
        pthread->wait();
        ui->pb_playVideo->setText("播放视频");

    }
}
