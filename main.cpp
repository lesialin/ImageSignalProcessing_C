
#include "isp.h"
#include "opencv2/opencv.hpp"


using namespace cv;
isp_config_t g_isp_config;

int main(int argc, char **argv)
{

    uint16_t* raw_buf;
    uint8_t* rgb_buf;
    uint8_t* yuv_buf;
    uint32_t image_size;
    FILE* f;
    FILE* fyuv_out;

    if (argc != 2)
    {
        printf("usage: ISP_pipeline.out raw_image_path\n");
        return -1;
    }

    //load isp config
    isp_load_config("../config.json", &g_isp_config);
    
    image_size = g_isp_config.image_height*g_isp_config.image_width;
    //allocate raw memory
    raw_buf = (uint16_t*)malloc(sizeof(uint16_t)*image_size);
    //allocate rgb image memory
    rgb_buf = (uint8_t*)malloc(sizeof(uint8_t)*image_size*3);

    //allocate yuv image memory
    yuv_buf = (uint8_t*)malloc(sizeof(uint8_t)*image_size*3);

    // read data
    f = fopen(argv[1], "rb");
    fread(raw_buf, sizeof(uint16_t), image_size, f);

    //init isp parameters
    isp_init();
    
    //run isp raw domain pipe
    isp_raw_run(raw_buf, rgb_buf);

    // run isp yuv domain pipe
    isp_yuv_run(rgb_buf,yuv_buf);

    Mat img(g_isp_config.image_height, g_isp_config.image_width, CV_8UC3, rgb_buf);
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
    imwrite("test.png",img);
    
    fyuv_out = fopen("test.yuv", "wb");  
    fwrite(yuv_buf, sizeof(uint8_t)*image_size*3, 1, fyuv_out); 

    fclose(fyuv_out);
    fclose(f);
    free(raw_buf);
    free(rgb_buf);
    free(yuv_buf);
    
    return 0;
}