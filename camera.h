#ifndef CAMERA_H
#define CAMERA_H

#include "QObject"

#include <QMessageBox>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <net.h>
#include "src/superrestruction.h"
#include <benchmark.h>
#include <opencv2/highgui/highgui_c.h>
#include <QFileDialog>
#include <QMessageBox>


using namespace std;
using namespace cv;

class Camera : public QObject
{
Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);
    void online();
    void offline(const char* file );
    void play(const char* file );
    bool openCamera();
    bool closeCamera();
    bool setflag(bool falg);
    bool openCameraFlag = false;
    void doMywork(int mode = 0, QString file = "");
    void changeStatus();
private:
    cv::VideoCapture *cap = NULL;
    int CAM_NUM = 0;
    int i = 0;
    bool bicubic = false;
    SuperRestruction *detector = NULL;

signals:
    void sendImg(QImage);
};

#endif // CAMERA_H
