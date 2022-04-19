#include "utils.h"

void cvMat2NcnnMat(cv::Mat src1, cv::Mat src2, ncnn::Mat& dst)
{
    std::vector<cv::Mat> channels1, channels2;
    cv::split(src1, channels1);
    cv::split(src2, channels2);
    channels1.insert(channels1.end(), channels2.begin(), channels2.end());

    for (int p = 0; p < dst.c; p++)
    {
        memcpy(dst.channel(p), (const uchar*)channels1[p].data, dst.w * dst.h * sizeof(float));
    }
}

void cvMat2NcnnMat(cv::Mat src1, ncnn::Mat& dst)
{
    std::vector<cv::Mat> channels;
    cv::split(src1, channels);

    for (int p = 0; p < dst.c; p++)
    {
        memcpy(dst.channel(p), (const uchar*)channels[p].data, dst.w * dst.h * sizeof(float));
    }
}

void ncnnMat2cvMat(ncnn::Mat in, cv::Mat& out)
{
    std::vector<cv::Mat> a(in.c);
    for (int p = 0; p < in.c; p++)
    {
        a[p] = cv::Mat(in.h, in.w, CV_32FC1);
        memcpy((uchar*)a[p].data, in.channel(p), in.w * in.h * sizeof(float));
    }

    cv::merge(a, out);
}

void putText(cv::Mat& image, std::string text)
{
    //int font_face = cv::FONT_HERSHEY_COMPLEX;
    int font_face = cv::FONT_HERSHEY_COMPLEX;

    double font_scale = 1;
    int thickness = 1;
    int baseline;
    cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
    cv::Point origin;
    origin.x = 10;
    origin.y = 10 + text_size.height;
    cv::putText(image, text, origin, font_face, font_scale, cv::Scalar(0, 255, 0), thickness, 8, 0);
}
void pretty_print(const ncnn::Mat& m)
{
    for (int q = 0; q < m.c; q++)
    {
        const float* ptr = m.channel(q);
        for (int y = 0; y < m.h; y++)
        {
            for (int x = 0; x < m.w; x++)
            {
                printf("%f ", ptr[x]);
            }
            ptr += m.w;
            printf("\n");
        }
        printf("------------------------\n");
    }
}