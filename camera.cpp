#include "camera.h"

//#include "model.id.h"


double benchmark(SuperRestruction& detector, int loop_num=10, int warm_up = 2)    //测速用
{
    double time_min = DBL_MAX;
    double time_max = -DBL_MAX;
    double time_avg = 0;
    ncnn::Mat input = ncnn::Mat(320, 180, 3);
    ncnn::Mat output;
    input.fill(0.01f);
    for (int i = 0; i < warm_up + loop_num; i++)
    {
        double start = ncnn::get_current_time();
        ncnn::Extractor ex = detector.Net->create_extractor();
        ex.input("in0", input);
        ex.extract("out0", output);

        double end = ncnn::get_current_time();
        double time = end - start;
        if (i >= warm_up)
        {
            time_min = (std::min)(time_min, time);
            time_max = (std::max)(time_max, time);
            time_avg += time;
        }
    }
    time_avg /= loop_num;
//    fprintf(stderr, "%20s  min = %7.2f  max = %7.2f  avg = %7.2f\n", "SuperRestruction", time_min, time_max, time_avg);
    return time_avg;
}

int recode = 0;

void tqdm(int i, int sum) {
    printf("\r");
    int show_num = i * 100 / sum;
    string buf = "+";
    for (int j = 1; j <= show_num; j++)
    {
        buf += "+";
    }
    cout.width(100); //设置宽度
    cout.setf(ios::left); //设置对齐方式为left
    cout.fill(' '); //设置填充，缺省为空格
    cout << buf;
    printf("[%d%%]", int(i * 100.0 / (sum - 1)));
    if (show_num % 10 == 0 && show_num != recode)
    {
        recode = show_num;
        printf("\n");
    }
}

int getFastestDevice()
{
    printf("Selecting the fastest hardware\n");
    int device = -1;
    int threads = std::thread::hardware_concurrency() / 2;
    int gpu_counts = ncnn::get_gpu_count();
    if (gpu_counts > 0)
    {
        double min = 9999999.0;
        for (int i = -1; i < gpu_counts; ++i)
        {
            SuperRestruction detector = SuperRestruction(i, threads);
            double time_avg = benchmark(detector);
            if (time_avg < min) {
                min = time_avg;
                device = i;
            }
            qDebug("device %d time %f\n", i, time_avg);
        }
    }
    return device;
}

Camera::Camera(QObject *parent) : QObject(parent)
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
}

void Camera::online()
{
    openCamera();

    while(1){
        if(!this->openCameraFlag)
        {
           closeCamera();
           break;
        }
        i += 1;
        cv::Mat frame, dest;
        bool ret = cap->read(frame);
        if (!ret) {
            qDebug("Can't receive frame (stream end?). Exiting ...\n");
        }
        frame = detector->detect(frame, bicubic);
        cv::cvtColor(frame, dest, cv::COLOR_BGR2RGB);
        QImage image((uchar*)dest.data, dest.cols, dest.rows,QImage::Format_RGB888);

        emit sendImg(image);
        cv::waitKey(30);
    }

}



void Camera::play(const char* file)
{

    double time_total = 0;

    int i = 0;

    cv::Mat image, dest;
    cv::VideoCapture cap(file);
    int keyvalue = 0;
    while (true)
    {
        if(!this->openCameraFlag)
        {
           closeCamera();
           break;
        }

        cap >> image;
        if (image.empty()) {
            break;
        }
        double start = ncnn::get_current_time();

        cv::Mat result = detector->detect(image, bicubic);

        cv::cvtColor(result, dest, cv::COLOR_BGR2RGB);
        QImage image((uchar*)dest.data, dest.cols, dest.rows,QImage::Format_RGB888);
        emit sendImg(image);

        double time = ncnn::get_current_time() - start;

        keyvalue = cv::waitKey(10);
        if (keyvalue == 113 || keyvalue == 81 || keyvalue == 27)
            break;
        else if (keyvalue == 32)
            waitKey(20);

        time_total += time;
        if (i == 20)
        {
            printf("\rSpeed: %13.4fs/frame", time_total / i / 1000);
            i = 0;
            time_total = 0;
        }
        i += 1;
    }
    cap.release();
}

void Camera::offline(const char* file )
{
    cv::Mat image;
    cv::VideoCapture cap(file);
    int frameRate = cap.get(cv::CAP_PROP_FPS);
    int FramesCnts = cap.get(cv::CAP_PROP_FRAME_COUNT);
    cv::VideoWriter outputVideo("output.mp4", cv::VideoWriter::fourcc('D', 'I', 'V', 'X'), frameRate, Size(1280, 720));
    printf("Start process\n");
    double start = ncnn::get_current_time();
    int i = 0;
    while (true)
    {
        cap >> image;

        if (image.empty()) {
            break;
        }
        tqdm(i, FramesCnts);
        cv::Mat result = detector->detect(image, bicubic);
        cv::Mat out;
        cv::resize(result, out, Size(1280, 720));
        outputVideo.write(out);
        cv::waitKey(1);
        i += 1;
    }
    double end = ncnn::get_current_time();

    printf("\nFinsihed\n");
    printf("Input video :%20s\n", file);
    printf("Total frames:%20d\n", FramesCnts);
    printf("Cost time   :%19.4fs\n", (end - start) / 1000);
    printf("Average time:%13.4fs/frame\n", (end - start) / 1000 / FramesCnts);
    printf("Output video:%20s\n", "output.mp4");

    outputVideo.release();

}

void Camera::doMywork(int mode, QString file)
{

    int device = getFastestDevice();
    int threads = std::thread::hardware_concurrency() / 2;
    if (device == -1) qDebug("Using the CPU with %d threads\n", threads);
    else qDebug("Using the GPU:%d\n", device);
    detector = new SuperRestruction(device, threads);
    switch (mode) {
        case 0:
            online();
            break;
        case 1:
            qDebug(file.toStdString().c_str());
            offline(file.toStdString().c_str());
            break;
        case 2:
            qDebug(file.toStdString().c_str());
            play(file.toStdString().c_str());
        break;
    }
    closeCamera();
}

bool Camera::openCamera()
{
//    qDebug("opening the Camera...");
    cap = new cv::VideoCapture(this->CAM_NUM);
    return true;
}

bool Camera::closeCamera()
{
    cap->release();
    return true;
}

bool Camera::setflag(bool flag)
{
    this->openCameraFlag = flag;
    return true;
}

void Camera::changeStatus()
{
    if (!this->bicubic) bicubic = true;
    else bicubic = false;
    qDebug("isbicubic:%d",bicubic);
}
