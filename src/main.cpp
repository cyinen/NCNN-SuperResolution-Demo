#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <net.h>
#include "SuperRestruction.h"
#include <benchmark.h>
#include "model.id.h"
using namespace std;
using namespace cv;

int webcam_demo(SuperRestruction& detector, int cam_id)
{
    double time_avg = 0;
    double time_total = 0;

    int i = 0;
    cv::Mat image;
    cv::VideoCapture cap(cam_id);
    int keyvalue = 0;
    while (true)
    {
        cap >> image;
        if (image.empty()) {
            break;
        }
        double start = ncnn::get_current_time();
        detector.detect(image);
        double time = ncnn::get_current_time() - start;
        keyvalue = cv::waitKey(10);
        if (keyvalue == 113 || keyvalue == 81 || keyvalue == 27)
            break;
        else if (keyvalue == 32)
            waitKey(0);
        time_total += time;
        if (i == 20)
        {
            printf("\rSpeed: %13.4fs/frame", time_total / i / 1000);
            i = 0;
            time_total = 0;
        }
        i += 1;
    }
    return 0;
}

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
        ex.input(_model_opt_param_id::BLOB_in0, input);
        ex.extract(_model_opt_param_id::BLOB_out0, output);

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
    //fprintf(stderr, "%20s  min = %7.2f  max = %7.2f  avg = %7.2f\n", "SuperRestruction", time_min, time_max, time_avg);
    return time_avg;
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
            //printf("device %d time %f\n", i, time_avg);
        }
    }
    return device;
}

int main(int argc, char** argv)
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    int device = getFastestDevice();
    int threads = std::thread::hardware_concurrency() / 2;

    if (device == -1) printf("Using the CPU with %d threads\n", threads);
    else printf("Using the GPU:%d\n", device);

    SuperRestruction detector = SuperRestruction(device, threads);

    int cam_id = 0;
    webcam_demo(detector, cam_id);
}
