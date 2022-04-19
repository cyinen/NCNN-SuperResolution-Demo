#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <net.h>


class InferenceEngine
{

protected:
    static InferenceEngine* detector;
    static bool hasGPU;
    bool useGPU = false;
    int threads;
public:
    ncnn::Net* Net;
    InferenceEngine(int device,int threads);
    ~InferenceEngine();
    virtual cv::Mat detect(cv::Mat image, bool isbicubic) = 0;
    void preprocess(cv::Mat& image, ncnn::Mat& in);
    void process(ncnn::Mat& input, ncnn::Mat& output);
    void postprecess(ncnn::Mat& in, cv::Mat& out);
    ncnn::Option opt;
};

