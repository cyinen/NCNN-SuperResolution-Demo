
#ifndef SuperRestruction_H
#define SuperRestruction_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <net.h>
#include "InferenceEngine.h"

class SuperRestruction :public InferenceEngine
{
public:
    SuperRestruction(int device,int threads) :InferenceEngine(device,threads) {};
    cv::Mat detect(cv::Mat image, bool isbicubic);
private:
    void preprocess(cv::Mat& image, cv::Mat& bc, ncnn::Mat& in);
    void postprecess(ncnn::Mat& in, cv::Mat& bc, cv::Mat& out);
};


#endif //NANODET_H
