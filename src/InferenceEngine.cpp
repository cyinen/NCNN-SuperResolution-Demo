#include "InferenceEngine.h"
#include <benchmark.h>
#include "utils.h"
#include "model.id.h"
#include "model.mem.h"
#include "cpu.h"

bool InferenceEngine::hasGPU = false;
InferenceEngine* InferenceEngine::detector = nullptr;
static ncnn::UnlockedPoolAllocator g_blob_pool_allocator;
static ncnn::PoolAllocator g_workspace_pool_allocator;

#if NCNN_VULKAN
static ncnn::VulkanDevice* g_vkdev = 0;
static ncnn::VkAllocator* g_blob_vkallocator = 0;
static ncnn::VkAllocator* g_staging_vkallocator = 0;
#endif // NCNN_VULKAN

InferenceEngine::InferenceEngine(int device = 0, int threads = 4)
{

    bool use_vulkan_compute = device != -1;
    g_blob_pool_allocator.set_size_compare_ratio(0.0f);
    g_workspace_pool_allocator.set_size_compare_ratio(0.5f);

#if NCNN_VULKAN
    if (use_vulkan_compute)
    {
        g_vkdev = ncnn::get_gpu_device(device);
        g_blob_vkallocator = new ncnn::VkBlobAllocator(g_vkdev);
        g_staging_vkallocator = new ncnn::VkStagingAllocator(g_vkdev);
    }
#endif // NCNN_VULKAN
    // default option
    ncnn::Option opt;
    opt.lightmode = true;
    opt.num_threads = threads;
    opt.blob_allocator = &g_blob_pool_allocator;
    opt.workspace_allocator = &g_workspace_pool_allocator;
#if NCNN_VULKAN
    opt.blob_vkallocator = g_blob_vkallocator;
    opt.workspace_vkallocator = g_blob_vkallocator;
    opt.staging_vkallocator = g_staging_vkallocator;
#endif // NCNN_VULKAN
    opt.use_winograd_convolution = true;
    opt.use_sgemm_convolution = true;
    opt.use_int8_inference = true;
    opt.use_vulkan_compute = use_vulkan_compute;
    opt.use_fp16_packed = true;
    opt.use_fp16_storage = true;
    opt.use_fp16_arithmetic = true;
    opt.use_int8_storage = true;
    opt.use_int8_arithmetic = true;
    opt.use_packing_layout = true;
    opt.use_shader_pack8 = false;
    opt.use_image_storage = false;

    ncnn::set_cpu_powersave(2);

    ncnn::set_omp_dynamic(0);
    ncnn::set_omp_num_threads(threads);
    g_blob_pool_allocator.clear();

    g_workspace_pool_allocator.clear();
#if NCNN_VULKAN
    if (opt.use_vulkan_compute)
    {
        g_blob_vkallocator->clear();
        g_staging_vkallocator->clear();
    }

#endif // NCNN_VULKAN


    this->Net = new ncnn::Net();
    this->Net->opt = opt;

#if NCNN_VULKAN
    if (opt.use_vulkan_compute)
    {
        this->Net->set_vulkan_device(g_vkdev);
    }
#endif // NCNN_VULKAN

    this->Net->load_param("model_best.ncnn.param");
    this->Net->load_model("model_best.ncnn.bin");
    printf("Loading the model\n");
//    this->Net->load_param(_model_opt_param_bin);
//    this->Net->load_model(_model_opt_bin);

}

InferenceEngine::~InferenceEngine()
{
    delete this->Net;
}

void InferenceEngine::process(ncnn::Mat& input, ncnn::Mat& output)
{
    ncnn::Extractor ex = this->Net->create_extractor();
//    ex.input(_model_opt_param_id::BLOB_in0, input);
//    ex.extract(_model_opt_param_id::BLOB_out0, output);
    ex.input("in0", input);
    ex.extract("out0", output);

}

void InferenceEngine::preprocess(cv::Mat& image, ncnn::Mat& input)
{

}

void InferenceEngine::postprecess(ncnn::Mat& in, cv::Mat& out)
{
    const float means[3] = { 0,0,0 };
    const float norms[3] = { 255.0,255.0,255.0 };
    in.substract_mean_normalize(means, norms);
    out = cv::Mat(in.h, in.w, CV_8UC3);
    in.to_pixels(out.data, ncnn::Mat::PIXEL_RGB2BGR);
}
