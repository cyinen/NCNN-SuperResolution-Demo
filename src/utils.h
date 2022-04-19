#pragma once

#ifndef utils
#define utils

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <net.h>

void cvMat2NcnnMat(cv::Mat src1, cv::Mat src2, ncnn::Mat& dst);
void cvMat2NcnnMat(cv::Mat src1, ncnn::Mat& dst);
void ncnnMat2cvMat(ncnn::Mat in,cv::Mat &out);
void pretty_print(const ncnn::Mat& m);

void putText(cv::Mat& image,std::string text);

#endif