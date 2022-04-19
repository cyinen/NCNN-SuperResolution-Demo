#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camera.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void init();
    QThread *pthread;
private slots:

    void on_pb_exit_clicked();

    void on_pb_openCamera_clicked();

    void showImg(QImage img);

    void on_pb_switch_pressed();

    void on_pb_switch_released();

    void on_pb_playVideo_clicked();

private:
    Ui::MainWindow *ui;
    Camera *cap=NULL;
    QString file=nullptr;

signals:
    void openCamera(int, QString);
    void sendStatus();
};
#endif // MAINWINDOW_H
