#include "SuperRestruction.h"
#include <benchmark.h>
#include "utils.h"

cv::Mat bc, result;
bool mouse_down = false;

void SuperRestruction::preprocess(cv::Mat& image, cv::Mat& bc, ncnn::Mat& input)
{

    if (min(image.cols, image.rows) != 180)
    {
        cv::Mat img;
        image.convertTo(img, CV_8U, 1 , 0.00);
        cv::Size size;
        if (image.cols > image.rows)
        {
            size= cv::Size(int(image.cols * 180 / image.rows),180);
        }
        else
        {
            size = cv::Size(180,int(image.rows * 180 / image.cols) );
        }
        cv::resize(img, image, size, cv::INTER_CUBIC);
    }

    cv::Mat img;
    //cv::imwrite("lq.png", image);
    image.convertTo(img, CV_32F, 1 / 255.0, 0.00);
    img = img *2 - 1;
    cv::Size size(image.cols*4, image.rows*4);
    cv::resize(image, bc, size, cv::INTER_LINEAR);

    input = ncnn::Mat(image.cols, image.rows, 3);
    cvMat2NcnnMat(img,input);

}


void SuperRestruction::postprecess(ncnn::Mat& in, cv::Mat& bc, cv::Mat& out)
{

    cv::Mat tmp,a;
    ncnnMat2cvMat(in, a);
    a = (a + 1) / 2;
    a.convertTo(out, CV_8U, 255.0);
}

void onMouse(int event, int x, int y, int flags, void* param) {

    switch (event)
    {
        case cv::EVENT_FLAG_LBUTTON:
            mouse_down = true;
            break;
        case cv::EVENT_LBUTTONUP:
            mouse_down = false;
            break;
        default:
            break;
    }
}
cv::Mat SuperRestruction::detect(cv::Mat image, bool isbicubic)
{
    cv::Size size (640,480);
    ncnn::Mat input(image.cols, image.rows, 3);
    ncnn::Mat output;
    preprocess(image, bc, input);
    process(input, output);
    postprecess(output, bc, result);
    if (isbicubic)
        result = bc;

//    if (!mouse_down)
//    {
//        putText(result, "Enhanced");
////        cv::imshow("VideoEnhancement", result);
//    }
//    else
//    {
//        putText(bc, "Bilinear");
////        cv::imshow("VideoEnhancement", bc);
//    }
//    cv::setMouseCallback("VideoEnhancement", onMouse, 0);

    cv::resize(result, bc, size, cv::INTER_CUBIC);
    return bc;
}




